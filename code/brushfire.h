#ifndef TRAPEZOIDAL_H
#define TRAPEZOIDAL_H

#define WORLD_WIDTH 84.7
#define WORLD_HEIGHT 56.4

#include <iostream>
#include <stdlib.h>
#include <vector>
#include "ct.h"
#include "map_class.h"
#include <algorithm>

class brushfire
{
public:
    brushfire(std::string fileName);

    void showImage(std::string name);
    void showValues(std::string name);
    void showConnectedPoints (std::string name);
    void showPath(std::string name);
    void paintMap();
    void scaleImage(int factor);
    void brushfireAlgorithm(int iterations);

    void findMedianPoints(int threshold);
    void plotMedianPoints();

    void findCornerPoints();
    void findCenterPoints();
    void findIntersectingPoints();
    void connectPoints();
    void findPathPoints(cv::Point curPos, cv::Point goal);

    ~brushfire();
private:
    int maxValue;
    cv::Mat ori_map;
    cv::Mat point_map;
    cv::Mat connected_map;
    cv::Mat path_map;
    std::vector<std::vector<float>> matrix;
    std::vector<std::vector<cv::Point>> connectedPoints;
    std::vector<cv::Point> imagePoints;
    std::vector<cv::Point> cornerPoints;
    std::vector<cv::Point> centerPoints;
    std::vector<cv::Point> intersectionPoins;
    std::vector<cv::Point> pathPoints;
    std::vector<cv::Point> pathPointsResult;
    std::vector<ct::edge> edges;

    // DisjSets
    int find( int x) const;
    void unionSets(int root1, int root2);

    //DFS
    void DFS(cv::Point start, cv::Point goal);

    //Disjsets
    std::vector<int> s;
};

#endif // TRAPEZOIDAL_DECOMPOSITION_H
