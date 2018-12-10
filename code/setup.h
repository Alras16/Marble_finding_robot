#ifndef SETUP_H
#define SETUP_H

#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <ctime>
#include <iostream>
#include <math.h>

#include "c_vision.h"
#include "fuzzybugcontroller.h"
#include "lidar_sensor.h"
#include "ct.h"
#include "motion_planning.h"
#include "map_class.h"
#include "laserscanner.h"


class setup
{
public:

     setup();

     void statCallback(ConstWorldStatisticsPtr &_msg);
     void poseCallback(ConstPosesStampedPtr &_msg);
     void cameraCallback(ConstImageStampedPtr &msg);
     void lidarCallback(ConstLaserScanStampedPtr &msg);
     void initGazebo();
     ct::current_position get_robot_position();

     c_vision camera;
     lidar_sensor lidar_object;
     FuzzyBugController controller;
     motion_planning motion_planning_object;
     map_class map_class_object;
     LaserScanner laser_scanner;

     gazebo::transport::NodePtr node;
     gazebo::transport::PublisherPtr movementPublisher;
     gazebo::transport::SubscriberPtr statSubscriber;
     gazebo::transport::SubscriberPtr poseSubscriber;
     gazebo::transport::SubscriberPtr cameraSubscriber;
     gazebo::transport::SubscriberPtr lidarSubscriber;
     gazebo::transport::PublisherPtr worldPublisher;
     gazebo::msgs::WorldControl controlMessage;

private:

 static boost::mutex mutex1;
 ct::current_position pos_robot;

};

#endif // SETUP_H
