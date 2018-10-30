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

void lidar_sensor::init_data(std::vector<lidarPoint> data)
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
        float angle = ori_data[i].angle;
        float range = ori_data[i].range;

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
            float angle = filtered_data[i].angle;
            float range = filtered_data[i].range;

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
        float xMarble = center + found_marbles_point[i].range*px_per_m*std::cos(found_marbles_point[i].angle);
        float yMarble = center - found_marbles_point[i].range*px_per_m*std::sin(found_marbles_point[i].angle);
        float rMarble = px_per_m*found_marbles_radius[i];

        cv::circle(im, cv::Point(xMarble, yMarble), rMarble, cv::Scalar(0, 255, 0));
    }
    //found_marbles_point.clear();
    //found_marbles_radius.clear();

    cv::imshow(name, im);
}

void lidar_sensor::filter_data()
{
    filtered_data.clear();
    if (lidar_data_updated)
    {
        for (unsigned int i = 0; i < ori_data.size(); i++)
        {
            if (ori_data[i].range != 10.0)
            {
                lidarPoint temp;
                temp.angle = ori_data[i].angle;
                temp.range = ori_data[i].range;
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
                    if (abs(filtered_data[i - 1].range - filtered_data[i].range) > threshold)
                    {
                        start_point = i;
                        break;
                    }
                    else if (filtered_data[i].range < maxRange - threshold)
                    {
                        start_point = i;
                        break;
                    }
                }

                //std::cout << "starting point: " << start_point << std::endl;

                while (abs(filtered_data[start_point].range - filtered_data[start_point + number_of_points - 1].range) < threshold)
                    number_of_points += 2;

                number_of_points -= 2;
            }
            //std::cout << "number of points: " << number_of_points << std::endl;

            float circleChord = distP2P(filtered_data[start_point],filtered_data[start_point + number_of_points - 1]);
            //float chordAngle = angleP2P(filtered_data[start_point],filtered_data[start_point + number_of_points - 1]);

            float alpha = filtered_data[start_point + ((number_of_points - 1) / 2)].angle;

            float objectiveOne = filtered_data[start_point].range*std::cos(filtered_data[start_point].angle - alpha);
            float objectiveTwo = filtered_data[start_point + number_of_points - 1].range*std::cos(filtered_data[start_point + number_of_points - 1].angle - alpha);

            float rangeLine = (objectiveOne + objectiveTwo) / 2;
            float circleCamber = rangeLine - filtered_data[start_point + ((number_of_points - 1) / 2)].range;

            float circleRadius = (pow(circleChord, 2.0) + 4*pow(circleCamber, 2.0)) / (8*circleCamber);

            lidarPoint temp;
            temp.angle = alpha;
            temp.range = circleRadius + filtered_data[start_point + ((number_of_points - 1) / 2)].range;

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

void lidar_sensor::find_obstacles()
{
    // Define parameters for least square fitting
    float threshold_upper = 0.3;
    float threshold_lower = -0.3;
    float theta_start;
    float theta_end;
    float rho_start;
    float rho_end;
    float angle1 = 0.0;
    float angle2 = 0.0;
    float angle = 0.0;
    int N = 1;

    for (int i = 0; i < filtered_data.size(); i++)
    {
        lidarPoint temp;
        temp.range = filtered_data[i].range;
        temp.angle = filtered_data[i].angle;
        temp_filtered_data.push_back(temp);
    }

    while(temp_filtered_data.size() != 0)
    {
        if ((angle < threshold_upper) && (angle > threshold_lower))
        {
            angle1 = angleP2P(temp_filtered_data[0], temp_filtered_data[N]);
            angle2 = angleP2P(temp_filtered_data[0], temp_filtered_data[N + 1]);
            angle = angle1 - angle2;
            //std::cout << angle1 << "  " << angle2 << "  " << angle << std::endl;
            N++;
        }
        else
        {
            //if (N > 2)
            //{
            //    std::cout << "Filtered data size: " << temp_filtered_data.size() << std::endl;
                for (int i = 0; i < N; i++)
                {
                    lidarPoint temp;
                    temp.range = temp_filtered_data[i].range;
                    temp.angle = temp_filtered_data[i].angle;
                    temp_list.push_back(temp);
                }
                //std::cout << "For-loop ended" << std::endl;
                temp_list_obstacles.push_back(temp_list);
                temp_list.clear();
            //}
            temp_filtered_data.erase(temp_filtered_data.begin(), temp_filtered_data.begin() + N);
            N = 1;
        }
    }

    for (int i = 0; i  < temp_list_obstacles.size(); i++){
        for (int j = 0; j < temp_list_obstacles[i].size(); j++){
            std::cout << temp_list_obstacles[i][j].angle << " , ";
        }
        std::cout << std::endl;
    }
   // std::cout << temp_list_obstacles.size() << std::endl;

    std::vector<float> closest_distance;
    std::vector<float> distAngle;
    float buffer_angle;
    float buffer_range;

    for (unsigned int j = 0; j < temp_list_obstacles.size(); j++)
    {
        float alpha = calAlpha(temp_list_obstacles[j]);
        float range = calRange(temp_list_obstacles[j], alpha);

        closest_distance.push_back(range);
        distAngle.push_back(alpha);
    }

    buffer_range = closest_distance[0];

    for (unsigned int k = 0; k < closest_distance.size(); k++)
    {
        if (buffer_range < closest_distance[k])
        {
                buffer_range = closest_distance[k];
                buffer_angle = distAngle[k];
        }
    }

    lidarObstacle obstacle;
    obstacle.range = buffer_range;
    obstacle.angle = buffer_angle;
    //std::cout<<obstacle.range << std::endl;

    //theta_start = temp_list.front().angle;
    //theta_end = temp_list.back().angle;
    //rho_start = range/(std::cos(theta_start) * std::cos(alpha) + std::sin(theta_start) * std::sin(alpha));
    //rho_end = range/(std::cos(theta_end) * std::cos(alpha) + std::sin(theta_end) * std::sin(alpha));

    //x_start = rho_start * std::cos(theta_start);
    //y_start = rho_start * std::sin(theta_start);
    //x_end = rho_end * std::cos(theta_end);
    //y_end = rho_end * std::sin(theta_end);

    //std::cout << "range: " << range << "   alpha: " << alpha * (180/M_PI) << std::endl;
    //std::cout << "startpoint: " << x_start*20 << ", " << y_start*20 << std::endl;
    //std::cout << "startpoint: " << x_end*20 << ", " << y_end*20 << std::endl;
}

float lidar_sensor::distP2P(lidarPoint pointOne, lidarPoint pointTwo)
{
    float xOne = pointOne.range*std::cos(pointOne.angle);
    float yOne = pointOne.range*std::sin(pointOne.angle);
    float xTwo = pointTwo.range*std::cos(pointTwo.angle);
    float yTwo = pointTwo.range*std::sin(pointTwo.angle);

    float dist_x = abs(xTwo - xOne);
    float dist_y = abs(yTwo - yOne);

    return sqrt(pow(dist_x, 2.0) + pow(dist_y, 2.0));
}

float lidar_sensor::angleP2P(lidarPoint pointOne, lidarPoint pointTwo)
{
    float xOne = pointOne.range*std::cos(pointOne.angle);
    float yOne = pointOne.range*std::sin(pointOne.angle);
    float xTwo = pointTwo.range*std::cos(pointTwo.angle);
    float yTwo = pointTwo.range*std::sin(pointTwo.angle);

    float dist_x = abs(xTwo - xOne);
    float dist_y = abs(yTwo - yOne);

    return atan(dist_y / dist_x);
}

float lidar_sensor::calRange(std::vector<lidarPoint> points, float alpha)
{
    float range = 0.0;
    float weight = 1.0;
    float num = 0.0;
    float den = 0.0;

    for(unsigned int i = 0; i < points.size(); i++)
    {
        num += weight * points[i].range * std::cos(points[i].angle - alpha);
        den += weight;
    }
    return range = num/den;
}

float lidar_sensor::calAlpha(std::vector<lidarPoint> points)
{
    float alpha = 0.0;
    float weight = 1.0;
    float num = 0;
    float den = 0;
    for(unsigned int i = 0; i < points.size() - 1; i++)
    {
        for(unsigned int j = 1; j < points.size(); j++)
        {
            num = weight * std::pow(points[i].range, 2.0) * std::sin(2*points[i].angle) - ((2 / weight) * weight * weight* points[i].range * points[j].range * std::cos(points[i].angle) * std::sin(points[j].angle));
            den = weight * std::pow(points[i].range, 2.0) * std::cos(2*points[i].angle) - ((1 / weight) * weight * weight * points[i].range * points[j].range * std::cos(points[i].angle + points[j].angle));
        }
    }
    return alpha = 0.5 * std::atan2(num, den);
    /*float temp_angle = angleP2P(points[0],points[points.size()-1]);
    alpha = -std::atan(-1/std::tan(temp_angle));
    return alpha;*/
}
