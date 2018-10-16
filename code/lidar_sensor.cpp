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
