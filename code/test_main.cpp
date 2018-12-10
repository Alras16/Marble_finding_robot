#include "setup.h"
#include <iostream>

setup beginSetup;
static boost::mutex mutex;
int main() {

  beginSetup.initGazebo();
  beginSetup.controller.buildController();
  beginSetup.map_class_object.find_center_of_mass();

  float dir, speed;
  ct::marble closest_marble;
  ct::robot_action robotAction;
  ct::current_position pos1;
  //beginSetup.motion_planning_object.get_room_info(beginSetup.map_class_object.getRooms());
  std::vector<ct::room>rooms1 = beginSetup.map_class_object.getRooms();
 // beginSetup.motion_planning_object.tangent_bug_algoritm(closest_marble,beginSetup.get_robot_position(),beginSetup.map_class_object.getRooms());

   while(true)
   {

      gazebo::common::Time::MSleep(50);

     // beginSetup.lidar_object.filter_data();
     // beginSetup.lidar_object.find_marbles();
     // beginSetup.lidar_object.find_lines();
     // beginSetup.lidar_object.merge_lines();
     // beginSetup.lidar_object.visualize_lidar("test");
     beginSetup.camera.find_marbles();

    //  std::cout << "Marble: " << beginSetup.camera.find_closest_marble().centerPolar.theta << std::endl;

    //  std::cout << "Marble radius: " << beginSetup.lidar_object.get_closest_marble().distance_to_center << std::endl;

      closest_marble = beginSetup.camera.find_closest_marble();
      //closest_marble = beginSetup.lidar_object.get_closest_marble();
      ct::robot_orientation angle = beginSetup.motion_planning_object.get_rotation();
      pos1 = beginSetup.get_robot_position();

    //  beginSetup.motion_planning_object.show_path_of_robot(beginSetup.get_robot_position().robot_pos_pic);
      beginSetup.motion_planning_object.tangent_bug_algoritm(pos1,rooms1);

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

  // Make sure to shut everything down.
  gazebo::client::shutdown();
}
