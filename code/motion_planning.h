#ifndef MOTION_PLANNING_H
#define MOTION_PLANNING_H

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <opencv2/opencv.hpp>
#include <vector>
#include "ct.h"
#include "math.h"


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
    void get_room_info(std::vector<ct::room> rooms);
    void get_sensor_data(std::vector<ct::polarPoint>);
    float target_location();
    ct::robot_orientation get_rotation();
    float homogeneous_transformation(ct::current_position, cv::Point);
    std::vector<ct::room> get_rooms();
    void show_path_of_robot(cv::Point robot_pos, cv::Point center_mass_room, cv::Point, bool);

    // Plannings algorithm
    void tangent_bug_algoritm(ct::current_position, std::vector<ct::room>);
    void model_based_planner(ct::current_position pos, std::vector<cv::Point> points, float index, float indexRooms, cv::Point);


protected:

    int index = 0;
    int dummy = 0;
    float dist_traveled;
    float path_time;
    bool one_second_ellapes = false;
    float start_orientation;
    float rotation_to_goal;
    float sum_of_angles;
    bool target_location_reached = false;
    bool target_allowed = false;
    bool all_rooms_searched = false;
    bool rotate_to_goal = false;
    std::vector<ct::polarPoint> data_ori;
    std::vector <ct::room> room_info;
    std::vector<float> obstacle_range;
    std::vector<float> distance_traveled;
    std::vector<float> time_steps;
    cv::Mat path_of_robot = cv::imread("/home/kenni/git_workspace/Marble_finding_robot/test_files/maps/map_medium.png",cv::IMREAD_COLOR);
    cv::Mat path_of_robot1 = cv::imread("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/BrushfireTest/brushfireTest13.png",cv::IMREAD_COLOR);
    ct::robot_orientation rotation;
    cv::Point former_position;
    std::vector<cv::Point> pointGoal;
    cv::Point marblePos;
    std::vector<float> marbleCollectTime;
    bool marbleFound = true;

};

#endif // MOTION_PLANNING_H
