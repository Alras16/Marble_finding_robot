#ifndef MOTION_PLANNING_H
#define MOTION_PLANNING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include "fl/Headers.h"
#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <opencv2/opencv.hpp>
#include "fl/Headers.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "ct.h"
#include "map_class.h"

struct goal_position
{
    float goal_distance;
    float goal_angle;
};

struct current_position
{
    float current_position_x;
    float current_position_y;
};

class motion_planning
{
public:
    motion_planning();
    virtual goal_position tangent_bug_algoritm();

protected:

    bool target_location_reached = false;
    bool all_rooms_searched = false;

};

#endif // MOTION_PLANNING_H
