#ifndef LIDAR_SENSOR_H
#define LIDAR_SENSOR_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <iostream>
#include "testdata.h"
#define USE_MATH_DEFINES
#include "math.h"

class lidar_sensor
{
public:
    lidar_sensor();

    struct lidarPoint
    {
        float angle;
        float range;
    };

    struct detectedLine
    {
        float alpha;
        float range;
        lidarPoint start;
        lidarPoint end;
        unsigned int lengthOfLine;
    };

    void init_data(std::vector<lidarPoint> data);
    void visualize_lidar(std::string name);
    void filter_data();
    void find_marbles();
    std::vector<detectedLine> find_lines();

private:
    float distP2P(lidarPoint pointOne, lidarPoint pointTwo);
    float angleP2P(lidarPoint pointOne, lidarPoint pointTwo);
    float calRange(std::vector<lidarPoint> points, float alpha);
    float calAlpha(std::vector<lidarPoint> points);

    std::vector<lidarPoint> ori_data;
    std::vector<lidarPoint> filtered_data;

    float x_start;
    float y_start;
    float x_end;
    float y_end;

    std::vector<lidarPoint> found_marbles_point;
    std::vector<float> found_marbles_radius;

    int start_point = 0;
    int number_of_points = 1;
    bool lidar_data_updated;

};

#endif // LIDAR_SENSOR_H
