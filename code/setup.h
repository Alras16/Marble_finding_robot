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
#include "c_vision.h"
#include "fuzzybugcontroller.h"
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

     LaserScanner laser_scanner;
     c_vision camera;
     FuzzyBugController controller;

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

};

#endif // SETUP_H
