#ifndef LIDAR_SENSOR_H
#define LIDAR_SENSOR_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <iostream>
#include "testdata.h"

class lidar_sensor
{
public:
    lidar_sensor();

    struct lidarPoint
    {
        float angle;
        float range;
    };

    void init_data(testData::lidarData data);
    void visualize_lidar(std::string name);
    void filter_data();
    void find_marbles();

private:
    float distP2P(lidarPoint pointOne, lidarPoint pointTwo);
    float angleP2P(lidarPoint pointOne, lidarPoint pointTwo);

    std::vector<lidarPoint> ori_data;
    std::vector<lidarPoint> filtered_data;

    std::vector<lidarPoint> found_marbles_point;
    std::vector<float> found_marbles_radius;

    int start_point = 0;
    int number_of_points = 1;

};

#endif // LIDAR_SENSOR_H
