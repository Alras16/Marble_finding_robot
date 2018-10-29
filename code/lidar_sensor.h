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

    void init_data(std::vector<lidarPoint> data);
    void visualize_lidar(std::string name);
    void filter_data();
    void find_marbles();
    void find_obstacles();

private:
    float distP2P(lidarPoint pointOne, lidarPoint pointTwo);
    float angleP2P(lidarPoint pointOne, lidarPoint pointTwo);
    float calRange(std::vector<lidarPoint> points, float alpha);
    float calAlpha(std::vector<lidarPoint> points);

    std::vector<lidarPoint> ori_data;
    std::vector<lidarPoint> filtered_data;

    float x_start = 0;
    float y_start;
    float x_end = 22.5;
    float y_end;

    std::vector<lidarPoint> found_marbles_point;
    std::vector<float> found_marbles_radius;

    std::vector<lidarPoint> temp_list;
    std::vector<float> distance_to_points;
    std::vector<float> found_obstacle_angles;
    std::vector<float> found_obstacle_ranges;

    int start_point = 0;
    int number_of_points = 1;
    bool lidar_data_updated;

};

#endif // LIDAR_SENSOR_H
