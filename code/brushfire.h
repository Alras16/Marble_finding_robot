#ifndef TRAPEZOIDAL_H
#define TRAPEZOIDAL_H

#define WORLD_WIDTH 84.7
#define WORLD_HEIGHT 56.4

#include <iostream>
#include <vector>
#include "ct.h"
#include "map_class.h"

class brushfire
{
public:
    brushfire(std::string fileName);

    void showImage(std::string name);
    void showValues(std::string name);
    void paintMap();
    void scaleImage(int factor);
    void brushfireAlgorithm(int iterations);

    void findMedianPoints(int threshold);
    void plotMedianPoints();

    void findCornerPoints();

    ~brushfire();
private:
    int maxValue;
    cv::Mat ori_map;
    cv::Mat point_map;
    std::vector<std::vector<float>> matrix;
    std::vector<cv::Point> imagePoints;
    std::vector<cv::Point> cornerPoints;
};

#endif // TRAPEZOIDAL_DECOMPOSITION_H
