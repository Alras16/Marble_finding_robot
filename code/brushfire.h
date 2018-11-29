#ifndef TRAPEZOIDAL_H
#define TRAPEZOIDAL_H

#include <iostream>
#include <vector>
#include "ct.h"
#include "map_class.h"

class brushfire
{
public:
    brushfire(std::string fileName);

    void showImage(std::string name);
    void paintMap();
    void brushfireAlgorithm();

    ~brushfire();
private:
    int maxValue;
    cv::Mat ori_map;
    std::vector<std::vector<int>> matrix;
};

#endif // TRAPEZOIDAL_DECOMPOSITION_H
