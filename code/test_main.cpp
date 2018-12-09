#include "setup.h"
#include <iostream>
#include <brushfire.h>

setup beginSetup;
static boost::mutex mutex;
int main() {

  beginSetup.initGazebo();
  beginSetup.controller.buildController();
  beginSetup.map_class_object.find_center_of_mass();
  brushfire brush("map_small.png");

  float dir, speed, index = 1, indexRooms = 0;
  ct::marble closest_marble;
  ct::robot_action robotAction;
  ct::current_position pos1;
 // beginSetup.motion_planning_object.get_room_info(beginSetup.map_class_object.getRooms());
  std::vector<ct::room> rooms1 = beginSetup.map_class_object.getRooms();
  std::vector<cv::Point> pathPoints;

  brush.brushfireAlgorithm(13);
  brush.paintMap();
  brush.findCornerPoints();
  brush.findCenterPoints();
  brush.findIntersectingPoints();
  brush.scaleImage(5);
  brush.connectPoints();
 // brush.showImage("Brushfire algorithm");
 // brush.showValues("Voronoi diagram");

   while(indexRooms <= rooms1.size())
   {

      if (beginSetup.motion_planning_object.target_location())
           index++;

      if (index == pathPoints.size() || indexRooms == 0)
      {
         pathPoints.clear();
         brush.findPathPoints(beginSetup.get_robot_position().robot_pos, rooms1[indexRooms++].centerOfMass);
         brush.showConnectedPoints("Kruskal's algorithm and DFS");
         pathPoints = brush.getRoadPath();
         index = 1;
      }

      gazebo::common::Time::MSleep(50);
      beginSetup.camera.find_marbles();

      closest_marble = beginSetup.camera.find_closest_marble();
      ct::robot_orientation angle = beginSetup.motion_planning_object.get_rotation();
      pos1 = beginSetup.get_robot_position();

    //  beginSetup.motion_planning_object.show_path_of_robot(beginSetup.get_robot_position().robot_pos_pic);
    //  beginSetup.motion_planning_object.tangent_bug_algoritm(pos1,rooms1);

      beginSetup.motion_planning_object.model_based_planner(pos1,pathPoints,index);

      robotAction = beginSetup.controller.getControlOutput(closest_marble,angle);
      speed = robotAction.speed;
      dir = robotAction.dir;

      // Generate a pose
      ignition::math::Pose3d pose(double(speed), 0, 0, 0, 0, double(dir));

      // Convert to a pose message
      gazebo::msgs::Pose msg;
      gazebo::msgs::Set(&msg, pose);
      beginSetup.movementPublisher->Publish(msg);

  }

  std::cout << "Mission complete! All rooms searched.." << std::endl;

  // Make sure to shut everything down.
  gazebo::client::shutdown();
}
