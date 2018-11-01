#ifndef LIDAR_SENSOR_H
#define LIDAR_SENSOR_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <vector>
#include <iostream>
#include "testdata.h"
#include "ct.h"
#define USE_MATH_DEFINES
#include "math.h"

class lidar_sensor
{
public:
    lidar_sensor();

    void init_data(std::vector<ct::polarPoint> data);
    void visualize_lidar(std::string name);
    void filter_data();
    void find_marbles();
    std::vector<ct::detectedLine> find_lines();

private:
    float distP2P(ct::polarPoint pointOne, ct::polarPoint pointTwo);
    float angleP2P(ct::polarPoint pointOne, ct::polarPoint pointTwo);
    float calRange(std::vector<ct::polarPoint> points, float alpha);
    float calAlpha(std::vector<ct::polarPoint> points);

    std::vector<ct::polarPoint> ori_data;
    std::vector<ct::polarPoint> filtered_data;

    float x_start;
    float y_start;
    float x_end;
    float y_end;

    std::vector<ct::polarPoint> found_marbles_point;
    std::vector<float> found_marbles_radius;

    int start_point = 0;
    int number_of_points = 1;
    bool lidar_data_updated;

};

#endif // LIDAR_SENSOR_H
