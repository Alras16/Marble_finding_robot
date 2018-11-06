#ifndef CT_H
#define CT_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "bst_coordinates.h"

class ct
{
public:

    struct polarPoint
    {
        float theta;
        float rho;
    };

    struct line
    {
        float alpha;
        float range;
    };

    struct point
    {
        float x;
        float y;
    };

    struct detectedLine
    {
        line dLine;
        point start;
        point end;
    };

    struct room
    {
        int roomNumber;
        std::vector<cv::Point> coordinates;
        BST_coordinates coordinatesTree;
        float probabilityOfMarbles;
        int numbOfPixels;
        cv::Point centerOfMass;
    };

    struct marble
    {
        cv::Point center;
        polarPoint centerPolar;
        float radius;
    };

    struct foundMarble
    {
        marble fMarble;
        room *foundInRoom;
    };

    ct();
};

#endif // CT_H
