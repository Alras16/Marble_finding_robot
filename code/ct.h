#ifndef CT_H
#define CT_H

#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <opencv2/opencv.hpp>

#include "bst_coordinates.h"

class ct
{
public:

    boost::mutex mutex2;

    struct tanget_bug_var
    {
       //current_position q_start;
       cv::Point q_goal;

    };

    struct angle_to_obstacle
    {
        float fSmallestReading;
        float fFurthestReading;

    };

    struct current_position
    {
        cv::Point robot_pos_pic;
        cv::Point robot_pos;
        float angle_yaw_degress;
        float angle_yaw_radi;
        std::vector<cv::Point2f> list_of_points;
        cv::Point obstacle_point;

    };

    struct robot_orientation
    {
        float orientation_to_obstacle;
        float orientation_to_goal;
    };

    struct robot_action
    {
       float speed;
       float dir;
       float dir_rotate;
    };

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
        cv::Point centerOfMassPic;
    };

    struct marble
    {
        cv::Point2f center;
        polarPoint centerPolar;
        float radius;
        float distance_to_center;
    };

    struct foundMarble
    {
        marble fMarble;
        room *foundInRoom;
    };

    struct state
    {
        int x;
        int y;
        bool isOutsideEnvironment;
    };

    enum action { UP, DOWN, LEFT, RIGHT };

    struct newState
    {
        int RoomNumber; // if zero = start state
        bool isTerminal; // is the state a terminal state or not
        std::vector<bool> roomsVisited; // vector containing info on other rooms status
    };

    ct();
};

#endif // CT_H
