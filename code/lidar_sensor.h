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

#define MARBLE_PERCENTAGE 1.25

class lidar_sensor
{
public:
    lidar_sensor();

    void init_data(std::vector<ct::polarPoint> data);
    void visualize_lidar(std::string name);
    void filter_data();
    void find_marbles();
    void find_lines();
    void merge_lines();
    ct::line get_closest_line();
    ct::marble get_closest_marble();

private:
    float distP2P(ct::polarPoint pointOne, ct::polarPoint pointTwo);
    float angleP2P(ct::polarPoint pointOne, ct::polarPoint pointTwo);
    float calRange(std::vector<ct::polarPoint> points, float alpha);
    float calAlpha(std::vector<ct::polarPoint> points);
    ct::line calOrthoLine(ct::line aLine, ct::polarPoint point);
    ct::point calStartEndPoint(ct::line foundLine, ct::line orthoLine);
    std::vector<float> marbleCoverage(ct::marble marble);

    std::vector<ct::polarPoint> ori_data;
    std::vector<ct::polarPoint> filtered_data;

    std::vector<ct::marble> foundMarbles;
    std::vector<ct::detectedLine*> foundLines;

    int start_point = 0;
    int number_of_points = 1;
    bool lidar_data_updated;

};

#endif // LIDAR_SENSOR_H
