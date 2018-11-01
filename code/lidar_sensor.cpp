#include "lidar_sensor.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <iostream>
#include "testdata.h"
#include <math.h>

lidar_sensor::lidar_sensor()
{
    lidar_data_updated = false;
}

void lidar_sensor::init_data(std::vector<ct::polarPoint> data)
{
    ori_data.clear();
    ori_data = data;
    lidar_data_updated = true;
}

void lidar_sensor::visualize_lidar(std::string name)
{
    int width = 450;
    int height = 450;
    int range_max = 10.0;
    float center = 225.0;
    float px_per_m = 200.0 / range_max;

    cv::Mat im(height, width, CV_8UC3);
    im.setTo(0);

    unsigned int nranges = ori_data.size();

    for (unsigned int i = 0; i < nranges; i++)
    {
        float angle = ori_data[i].theta;
        float range = ori_data[i].rho;

        cv::Point2f startpt(center,center);

        cv::Point2f endpt(center + range * px_per_m * std::cos(angle),center - range * px_per_m * std::sin(angle));
        int green;
        int red;
        if (range == 10.0)
        {
            green = 255;
            red = 255;
        }
        else
        {
            green = 25 * range;
            red = 0;
        }
        cv::line(im, startpt * 16, endpt * 16, cv::Scalar(255, green, red), 1, cv::LINE_AA, 4);
    }

    for (unsigned int i = 0; i < filtered_data.size(); i++)
    {

        if ((signed(i) == start_point) || (signed(i) == start_point + number_of_points - 1))
        {
            float angle = filtered_data[i].theta;
            float range = filtered_data[i].rho;

            cv::Point2f startpt(center,center);
            cv::Point2f endpt(center + range * px_per_m * std::cos(angle),center - range * px_per_m * std::sin(angle));

            cv::line(im, startpt * 16, endpt * 16, cv::Scalar(255, 0, 255), 1, cv::LINE_AA, 4);

        }
    }
    cv::circle(im, cv::Point(center, center), 2, cv::Scalar(0, 0, 255));

    cv::Point2f temp_startpt(center + x_start*20,center - y_start*20);
    cv::Point2f temp_endpt(center + x_end*20,center - y_end*20);
    cv::line(im, temp_startpt * 16, temp_endpt * 16, cv::Scalar(255, 255, 255), 2, cv::LINE_AA, 4);

    for (unsigned int i = 0; i < found_marbles_point.size(); i++)
    {
        float xMarble = center + found_marbles_point[i].rho*px_per_m*std::cos(found_marbles_point[i].theta);
        float yMarble = center - found_marbles_point[i].rho*px_per_m*std::sin(found_marbles_point[i].theta);
        float rMarble = px_per_m*found_marbles_radius[i];

        cv::circle(im, cv::Point(xMarble, yMarble), rMarble, cv::Scalar(0, 255, 0));
    }

    cv::imshow(name, im);
}

void lidar_sensor::filter_data()
{
    filtered_data.clear();
    if (lidar_data_updated)
    {
        for (unsigned int i = 0; i < ori_data.size(); i++)
        {
            if (ori_data[i].rho != 10.0)
            {
                ct::polarPoint temp;
                temp.theta = ori_data[i].theta;
                temp.rho = ori_data[i].rho;
                filtered_data.push_back(temp);
            }
        }
    }
}

void lidar_sensor::find_marbles()
{
    if (filtered_data.size() != 0)
    {
        found_marbles_point.clear();
        found_marbles_radius.clear();
        float threshold = 0.2;
        float maxRange = 10;
        start_point = 0;
        number_of_points = 1;
        while ((start_point < signed(filtered_data.size() - 1)))
        {
            number_of_points = 1;
            while ((number_of_points < 3) && (start_point < signed(filtered_data.size() - 1)))
            {
                start_point++;
                for (unsigned int i = start_point; i < filtered_data.size(); i++)
                {
                    if (abs(filtered_data[i - 1].rho - filtered_data[i].rho) > threshold)
                    {
                        start_point = i;
                        break;
                    }
                    else if (filtered_data[i].rho < maxRange - threshold)
                    {
                        start_point = i;
                        break;
                    }
                }

                //std::cout << "starting point: " << start_point << std::endl;

                while (abs(filtered_data[start_point].rho - filtered_data[start_point + number_of_points - 1].rho) < threshold)
                    number_of_points += 2;

                number_of_points -= 2;
            }
            //std::cout << "number of points: " << number_of_points << std::endl;

            float circleChord = distP2P(filtered_data[start_point],filtered_data[start_point + number_of_points - 1]);
            //float chordAngle = angleP2P(filtered_data[start_point],filtered_data[start_point + number_of_points - 1]);

            float alpha = filtered_data[start_point + ((number_of_points - 1) / 2)].theta;

            float objectiveOne = filtered_data[start_point].rho*std::cos(filtered_data[start_point].theta - alpha);
            float objectiveTwo = filtered_data[start_point + number_of_points - 1].rho*std::cos(filtered_data[start_point + number_of_points - 1].theta - alpha);

            float rangeLine = (objectiveOne + objectiveTwo) / 2;
            float circleCamber = rangeLine - filtered_data[start_point + ((number_of_points - 1) / 2)].rho;

            float circleRadius = (pow(circleChord, 2.0) + 4*pow(circleCamber, 2.0)) / (8*circleCamber);

            ct::polarPoint temp;
            temp.theta = alpha;
            temp.rho = circleRadius + filtered_data[start_point + ((number_of_points - 1) / 2)].rho;

            float dist_circles = 0.01;
            float prev_radius = 0.0;
            if (found_marbles_point.size() != 0)
            {
                dist_circles = distP2P(temp,found_marbles_point[found_marbles_point.size() - 1]);
                prev_radius = found_marbles_radius[found_marbles_radius.size() - 1] / 2;
            }
            if ((circleCamber > 0.05) && (circleChord > 0.05) && (dist_circles > prev_radius))
            {
                {
                    /*std::cout<< "calculating new marble" << std::endl;
                    std::cout << "  circle chord: " << circleChord << std::endl;
                    std::cout << "  chord angle:  " << chordAngle << std::endl;
                    std::cout << "  alpha: " << alpha << std::endl;
                    std::cout << "  range to line: " << rangeLine << std::endl;
                    std::cout << "  circle camber: " << circleCamber << std::endl;
                    std::cout << "  circle radius: " << circleRadius << std::endl << std::endl;*/

                    found_marbles_point.push_back(temp);
                    found_marbles_radius.push_back(circleRadius);
                }
            }
            else
                start_point = start_point + number_of_points - 1;
        }
        //std::cout << "number of circles: " << found_marbles_point.size() << std::endl;
    }
}

std::vector<ct::detectedLine> lidar_sensor::find_lines()
{
    // Line conditions
    float threshold = 0.05;   // angle
    //float point_dist = 0.625;      // distance between two lines

    // Vectors
    std::vector<ct::detectedLine> found_lines;

    // Define parameters for least square fitting
    /*float theta_start;
    float theta_end;
    float rho_start;
    float rho_end;
    float angle = 0.0;
    float angle1 = 0.0;
    float angle2 = 0.0;*/
    float prev_alpha = 0.0;
    float curr_alpha = 0.0;
    float prev_range = 0.0;
    float curr_range = 0.0;
    unsigned int index_start = 0;
    unsigned int index_end = 0;

    bool condition_satisfied = true;

    std::cout << "total number of points: " << filtered_data.size() << std::endl;
    while ((index_start < filtered_data.size()) || (index_end < filtered_data.size()))
    {
        condition_satisfied = true;
        index_end += 2;
        std::vector<ct::polarPoint> line_points;
        for (unsigned int i = index_start; i < index_end; i++)
            line_points.push_back(filtered_data[i]);

        prev_alpha = curr_alpha;
        prev_range = curr_range;
        curr_alpha = calAlpha(line_points);
        curr_range = calRange(line_points, curr_alpha);
        if (curr_range < 0.0)
        {
            curr_alpha += M_PI;
            curr_range *= -1.0;
        }

        while (condition_satisfied && (index_end < (filtered_data.size() + 1)))
        {
            index_end++;
            line_points.push_back(filtered_data[index_end - 1]);
            prev_alpha = curr_alpha;
            prev_range = curr_range;
            curr_alpha = calAlpha(line_points);
            curr_range = calRange(line_points, curr_alpha);
            if (curr_range < 0.0)
            {
                curr_alpha += M_PI;
                curr_range *= -1.0;
            }

            float delta_angle = abs(curr_alpha - prev_alpha);

            //std::cout << "alpha: " << prev_alpha << " range: " << prev_range << std::endl;
            //std::cout << "prev angle: " << prev_alpha << " curr angle: " << curr_alpha << std::endl;
            //std::cout << "close to 90: " << closeTo90 << std::endl;
            //std::cout << "current end: " << index_end << std::endl;
            if ((delta_angle > threshold) && (line_points.size() < 4))
            {
                //std::cout << "hej" << std::endl;
                index_end--;
                index_start = index_end;
                condition_satisfied = false;
            }
            else if ((delta_angle > threshold) || (index_end > filtered_data.size()))
            {
                std::cout << "prev angle: " << prev_alpha << " curr angle: " << curr_alpha << std::endl;
                std::cout << "start point index: " << index_start << "  end point index: " << index_end - 2 << std::endl;
                std::cout << "delta angle: " << delta_angle << std::endl;
                std::cout << "orthogornal angle: " << std::atan(-1/prev_alpha) << std::endl;
                std::cout << std::endl;
                ct::detectedLine temp_line;
                temp_line.dLine.range = prev_range;
                temp_line.dLine.alpha = prev_alpha;
                // calculate start and end point
                found_lines.push_back(temp_line);
                index_end--;
                index_start = index_end;
                condition_satisfied = false;
            }
        }
    }
    std::cout << "run ended" << std::endl;
    return found_lines;
}

float lidar_sensor::distP2P(ct::polarPoint pointOne, ct::polarPoint pointTwo)
{
    float xOne = pointOne.rho*std::cos(pointOne.theta);
    float yOne = pointOne.rho*std::sin(pointOne.theta);
    float xTwo = pointTwo.rho*std::cos(pointTwo.theta);
    float yTwo = pointTwo.rho*std::sin(pointTwo.theta);

    float dist_x = abs(xTwo - xOne);
    float dist_y = abs(yTwo - yOne);

    return sqrt(pow(dist_x, 2.0) + pow(dist_y, 2.0));
}

float lidar_sensor::angleP2P(ct::polarPoint pointOne, ct::polarPoint pointTwo)
{
    float xOne = pointOne.rho*std::cos(pointOne.theta);
    float yOne = pointOne.rho*std::sin(pointOne.theta);
    float xTwo = pointTwo.rho*std::cos(pointTwo.theta);
    float yTwo = pointTwo.rho*std::sin(pointTwo.theta);

    float dist_x = abs(xTwo - xOne);
    float dist_y = abs(yTwo - yOne);

    return atan(dist_y / dist_x);
}

float lidar_sensor::calRange(std::vector<ct::polarPoint> points, float alpha)
{
    float range = 0.0;
    float weight = 1.0;
    float num = 0.0;
    float den = 0.0;

    for(unsigned int i = 0; i < points.size(); i++)
    {
        num += weight * points[i].rho * std::cos(points[i].theta - alpha);
        den += weight;
    }
    return range = num/den;
}

float lidar_sensor::calAlpha(std::vector<ct::polarPoint> points)
{
    //float weight = 1.0;
    float num = 0.0;
    float den = 0.0;
    float centroidX = 0.0;
    float centroidY = 0.0;

    for (unsigned int i = 0; i < points.size(); i++)
    {
        centroidX += points[i].rho*std::cos(points[i].theta);
        centroidY += points[i].rho*std::sin(points[i].theta);
    }

    centroidX /= points.size();
    centroidY /= points.size();

    for (unsigned int i = 0; i < points.size(); i++)
    {
        num += (points[i].rho * std::cos(points[i].theta) - centroidX) * (points[i].rho * std::sin(points[i].theta) - centroidY);
        den += std::pow((points[i].rho * std::sin(points[i].theta) - centroidY),2.0) - std::pow((points[i].rho * std::cos(points[i].theta) - centroidX),2.0);
    }
    num *= -2.0;


    /*float sum11 = 0;
    float sum21 = 0;
    float sum12 = 0;
    float sum22 = 0;
    for (unsigned int i = 0; i < points.size(); i++)
    {
        sum11 += std::pow(points[i].range, 2.0) * std::sin(2*points[i].angle);
        sum21 += std::pow(points[i].range, 2.0) * std::cos(2*points[i].angle);
        for (unsigned int j = 0; j < points.size(); j++)
        {
            sum12 += 2 * points[i].range * points[j].range * std::cos(points[i].angle) * std::sin(points[j].angle);
            sum22 += 2 * points[i].range * points[j].range * std::cos(points[i].angle + points[j].angle);
        }
    }
    sum12 *= 2/points.size();
    sum22 *= 1/points.size();

    num = sum11 - sum12;
    den = sum21 - sum22;*/
    /*for(unsigned int i = 0; i < points.size() - 1; i++)
    {
        for(unsigned int j = 1; j < points.size(); j++)
        {
            num = weight * std::pow(points[i].range, 2.0) * std::sin(2*points[i].angle) - ((2 / weight) * weight * weight* points[i].range * points[j].range * std::cos(points[i].angle) * std::sin(points[j].angle));
            den = weight * std::pow(points[i].range, 2.0) * std::cos(2*points[i].angle) - ((1 / weight) * weight * weight * points[i].range * points[j].range * std::cos(points[i].angle + points[j].angle));
        }
    }*/
    //std::cout << "num: " << num << "  den: " << den << std::endl;
    return 0.5 * std::atan2(num, den);
}
