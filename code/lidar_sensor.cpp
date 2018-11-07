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

ct::marble lidar_sensor::get_closest_marble()
{

    ct::marble closest_marble;

    if (foundMarbles.size() != 0)
    {
        closest_marble = foundMarbles[0];
        for (unsigned i = 0; i < foundMarbles.size(); i++)
        {
            if (closest_marble.radius < foundMarbles[i].radius)
            {
                closest_marble = foundMarbles[i];
            }
        }
    }
    else
    {
        closest_marble.radius = 0;
    }

    return closest_marble;
}

ct::line lidar_sensor::get_closest_line()
{
    ct::line closest_line;

    if (foundLines.size() != 0)
    {
        closest_line = foundLines[0]->dLine;
        for(unsigned i = 0; i < foundLines.size(); i++)
        {
            if (closest_line.range > foundLines[i]->dLine.range)
            {
                closest_line = foundLines[i]->dLine;
            }
        }
    }
    else
    {
        closest_line.range = 10;
    }

    return closest_line;
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

    // Visualise original data
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

    cv::circle(im, cv::Point(center, center), 2, cv::Scalar(0, 0, 255));

    // Visualise marbles
    for (unsigned int i = 0; i < foundMarbles.size(); i++)
    {
        float xMarble = center + foundMarbles[i].centerPolar.rho*px_per_m*std::cos(foundMarbles[i].centerPolar.theta);
        float yMarble = center - foundMarbles[i].centerPolar.rho*px_per_m*std::sin(foundMarbles[i].centerPolar.theta);
        float rMarble = px_per_m * foundMarbles[i].radius;

        cv::circle(im, cv::Point(xMarble, yMarble), rMarble, cv::Scalar(0, 255, 0));
    }

    // Visualise lines
    for (unsigned int i = 0; i < foundLines.size(); i++)
    {
        cv::Point2f tsp(center + foundLines[i]->start.x*px_per_m,center - foundLines[i]->start.y*px_per_m);
        cv::Point2f tep(center + foundLines[i]->end.x*px_per_m,center - foundLines[i]->end.y*px_per_m);
        cv::line(im, tsp * 16, tep * 16, cv::Scalar(0, 0, 255), 2, cv::LINE_AA, 4);
    }

    // Visualise lines for marble cutouts
    /*for (unsigned int i = 0; i < foundMarbles.size(); i++)
    {
        std::vector<float> marbleCutout = marbleCoverage(foundMarbles[i]);
        for (unsigned int j = 0; j < marbleCutout.size(); j++)
        {
            cv::Point2f temp_start(center,center);
            cv::Point2f temp_end(center + 10.0 * px_per_m * std::cos(marbleCutout[j]),center - 10.0 * px_per_m * std::sin(marbleCutout[j]));
            cv::line(im, temp_start * 16, temp_end * 16, cv::Scalar(255, 0, 255), 1, cv::LINE_AA, 4);
        }
    }*/


    // Show the finished image
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
        foundMarbles.clear();
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

            ct::marble tempMarble;
            ct::polarPoint temp;
            temp.theta = alpha;
            temp.rho = circleRadius + filtered_data[start_point + ((number_of_points - 1) / 2)].rho;

            float dist_circles = 0.01;
            float prev_radius = 0.0;
            if (foundMarbles.size() != 0)
            {
                dist_circles = distP2P(temp,foundMarbles[foundMarbles.size() - 1].centerPolar);
                prev_radius = foundMarbles[foundMarbles.size() - 1].radius / 2;
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

                    tempMarble.centerPolar = temp;
                    tempMarble.radius = circleRadius;
                    foundMarbles.push_back(tempMarble);
                    //found_marbles_point.push_back(temp);
                    //found_marbles_radius.push_back(circleRadius);
                }
            }
            else
                start_point = start_point + number_of_points - 1;
        }
        //std::cout << "number of circles: " << found_marbles_point.size() << std::endl;
    }
}

void lidar_sensor::find_lines()
{
    if (filtered_data.size() != 0)
    {
        // Line conditions
        float threshold = 0.0025; // angle
        float angleIncrement = abs(ori_data[0].theta - ori_data[1].theta) * 1.25;

        // Define parameters for least square fitting
        float prev_alpha = 0.0;
        float curr_alpha = 0.0;
        float prev_range = 0.0;
        float curr_range = 0.0;
        unsigned int index_start = 0;
        unsigned int index_end = 0;

        bool condition_satisfied = true;

        foundLines.clear();

        while ((index_start < filtered_data.size()) && (index_end < filtered_data.size()))
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

                if ((delta_angle > threshold) && (line_points.size() < 4))
                {
                    index_end--;
                    index_start = index_end;
                    condition_satisfied = false;
                }
                else if ((delta_angle > threshold) || (index_end > filtered_data.size()))
                {
                    // check for breaks inside the line
                    bool angle_to_large = false;;
                    for (unsigned int i = 0; i < line_points.size() - 2; i++)
                    {
                        float delta_theta = abs(line_points[i].theta - line_points[i + 1].theta);
                        if (delta_theta > angleIncrement)
                        {
                            angle_to_large = true;
                            break;
                        }
                    }

                    if (!angle_to_large) // if no breaks
                    {
                        ct::detectedLine* temp_line = new ct::detectedLine;
                        temp_line->dLine.range = prev_range;
                        temp_line->dLine.alpha = prev_alpha;
                        // calculate start and end point
                        ct::line startLine = calOrthoLine(temp_line->dLine,line_points[0]);
                        ct::line endLine = calOrthoLine(temp_line->dLine,line_points[line_points.size() - 2]);
                        ct::point startP = calStartEndPoint(temp_line->dLine,startLine);
                        ct::point endP = calStartEndPoint(temp_line->dLine,endLine);
                        temp_line->start = startP;
                        temp_line->end = endP;

                        foundLines.push_back(temp_line);
                    }

                    index_end--;
                    index_start = index_end;
                    condition_satisfied = false;
                }
            }
        }
    }
}

void lidar_sensor::merge_lines()
{
    float threshold = 0.1;
    float thresholdAlpha = 0.1;
    std::vector<ct::detectedLine*> tempLines;
    std::vector<std::vector<float>> tempAngles;
    for (unsigned int i = 0; i < foundMarbles.size(); i++)
    {
        std::vector<float> temp = marbleCoverage(foundMarbles[i]);
        tempAngles.push_back(temp);
    }

    bool merge = false;
    for (unsigned int i = 0; i < foundLines.size() - 1; i++)
    {
        merge = false;
        float endTheta = std::atan2(foundLines[i]->end.y,foundLines[i]->end.x); // end off first line
        float startTheta = std::atan2(foundLines[i + 1]->start.y,foundLines[i + 1]->start.x); // start of next line

        for (unsigned int j = 0; j < tempAngles.size(); j++)
        {
            float deltaAngleOne = abs(endTheta - tempAngles[j][0]);
            float deltaAngleTwo = abs(startTheta - tempAngles[j][1]);
            float deltaAngleThree = abs(foundLines[i]->dLine.alpha - foundLines[i + 1]->dLine.alpha);
            if ((deltaAngleOne < threshold) && (deltaAngleTwo < threshold) && (deltaAngleThree < thresholdAlpha))
            {
                merge = true;
                break;
            }
        }

        if (merge)
        {
            //merge lines
            ct::detectedLine *tempLine = new ct::detectedLine;
            tempLine->start = foundLines[i]->start;
            tempLine->end = foundLines[i + 1]->end;
            tempLine->dLine.alpha = (foundLines[i]->dLine.alpha + foundLines[i + 1]->dLine.alpha) / 2;
            tempLine->dLine.range = (foundLines[i]->dLine.range + foundLines[i + 1]->dLine.range) / 2;
            tempLines.push_back(tempLine);
        }
        else if ((i == foundLines.size() - 2))
        {
            // dublicate line
            tempLines.push_back(foundLines[i]);
            tempLines.push_back(foundLines[i + 1]);
        }
        else
        {
            //dublicate line
            tempLines.push_back(foundLines[i]);
        }
    }
    foundLines = tempLines;
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

    return 0.5 * std::atan2(num, den);
}

ct::line lidar_sensor::calOrthoLine(ct::line aLine, ct::polarPoint point)
{
    ct::line tempLine;
    float orthoAlpha = std::atan(-1/aLine.alpha);
    float range = point.rho * std::cos(point.theta - orthoAlpha);

    if (range < 0.0)
    {
        range *= -1.0;
        orthoAlpha += M_PI;
    }

    tempLine.alpha = orthoAlpha;
    tempLine.range = range;

    return tempLine;
}

ct::point lidar_sensor::calStartEndPoint(ct::line foundLine, ct::line orthoLine)
{
    float x = orthoLine.range * std::sin(foundLine.alpha) - foundLine.range * std::sin(orthoLine.alpha);
    float y = orthoLine.range * std::cos(foundLine.alpha) - foundLine.range * std::cos(orthoLine.alpha);
    float den = std::cos(foundLine.alpha) * std::sin(orthoLine.alpha) - std::sin(foundLine.alpha) * std::cos(orthoLine.alpha);

    x /= den;
    x *= -1.0;
    y /= den;

    ct::point tempPoint;
    tempPoint.x = x;
    tempPoint.y = y;

    return tempPoint;
}

std::vector<float> lidar_sensor::marbleCoverage(ct::marble marble)
{
    std::vector<float> tempVec;
    float deltaTheta = std::asin(marble.radius / marble.centerPolar.rho) * MARBLE_PERCENTAGE;
    tempVec.push_back(marble.centerPolar.theta - deltaTheta);
    tempVec.push_back(marble.centerPolar.theta + deltaTheta);
    return tempVec;
}
