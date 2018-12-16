#include "setup.h"

boost::mutex setup::mutex1;

setup::setup() :  map_class_object("map_small.png",14,false), controller( &laser_scanner)
{

}

void setup::cameraCallback(ConstImageStampedPtr &msg){


    std::size_t width = msg->image().width();
    std::size_t height = msg->image().height();
    const char *data = msg->image().data().c_str();
    cv::Mat im(int(height), int(width), CV_8UC3, const_cast<char *>(data));

    im = im.clone();
    cv::cvtColor(im, im, CV_BGR2RGB);

    camera.set_image(im);

    mutex1.lock();
    cv::imshow("camera", im);
    cv::waitKey(1);
    mutex1.unlock();

}

void setup::lidarCallback(ConstLaserScanStampedPtr &msg)
{

    bool lidarCalledOne = true;
    laser_scanner.parseLaserScannerMessage(msg);
    cv::Point goal;
    goal.x = 35;
    goal.y = 16;
    std::vector<ct::polarPoint> data;
    cv::Point best_point_end, best_point_start, best_point;
    int point_number;
    float result1,result2 = 1000, x_1, x_2, x_3, y_1, y_2, y_3, robot_to_obstacle, obstacle_to_goal, robot_to_goal;

    pos_robot.list_of_points.clear();
    data.clear();

    float angle_min = float(msg->scan().angle_min());
    //  double angle_max = msg->scan().angle_max();
    float angle_increment = float(msg->scan().angle_step());

    float range_min = float(msg->scan().range_min());

    float range_max = float(msg->scan().range_max());

    int sec = msg->time().sec();
    int nsec = msg->time().nsec();

    int nranges = msg->scan().ranges_size();
    int nintensities = msg->scan().intensities_size();

    assert(nranges == nintensities);

    int width = 400;
    int height = 400;
    float px_per_m = 200 / range_max;

    cv::Mat im(height, width, CV_8UC3);
    im.setTo(0);


    for (int i = 0; i < nranges; i++) {
      float angle = angle_min + i * angle_increment;
      float range = std::min(float(msg->scan().ranges(i)), range_max);


      ct::polarPoint tempPoint;
      tempPoint.theta = angle;
      tempPoint.rho = range;
      data.push_back(tempPoint);

      if (!lidarCalledOne)
      {
          std::string temp_string = std::to_string(angle)  + " " + std::to_string(range);

      }

      //    double intensity = msg->scan().intensities(i);
      cv::Point2f startpt(200.5f + range_min * px_per_m * std::cos(angle),
                          200.5f - range_min * px_per_m * std::sin(angle));
      cv::Point2f endpt(200.5f + range * px_per_m * std::cos(angle),
                        200.5f - range * px_per_m * std::sin(angle));
      cv::line(im, startpt * 16, endpt * 16, cv::Scalar(255, 255, 255, 255), 1,
               cv::LINE_AA, 4);

      if (range < 10)
      {


      // Robot point, obstacle point and goal point
      x_1 = pos_robot.robot_pos.x;
      x_2 = pos_robot.robot_pos.x +range*std::cos(pos_robot.angle_yaw_radi+angle);
      x_3 = goal.x;
      y_1 = pos_robot.robot_pos.y;
      y_2 = pos_robot.robot_pos.y + range*std::sin(pos_robot.angle_yaw_radi+angle);
      y_3 = goal.y;

      // Distance from robot to obstacle, distance from obstacle to goal and distance from robot to goal
      robot_to_obstacle = sqrt( pow((x_2-x_1),2)+pow((y_2-y_1),2));
      obstacle_to_goal = sqrt( pow((x_3-x_2),2)+pow((y_3-y_2),2));
      robot_to_goal = sqrt( pow((x_3-x_1),2)+pow((y_3-y_1),2));

      // Calculates distance the shortest path from robot, obstacle and goal
      result1 = obstacle_to_goal;
      // std::cout << "Current pos: " << x_1 << "," << y_1 << "   " << "Obstacle pos: " << x_2 <<","<< y_2 << std::endl;
      if (result1 < result2)
      {
          result2 = result1;
          point_number = i;
          best_point.x = x_2;
          best_point.y = y_2;
          best_point_start = startpt;
          best_point_end = endpt;
      }
      }

    }

   // REMEMBER to uncomment when testing targent bug!!!!!!!!!
   //  cv::line(im, best_point_start * 16, best_point_end * 16, cv::Scalar(255, 255, 100), 1,
   //          cv::LINE_AA, 4);
   // cv::circle(im, best_point_end, 3, cv::Scalar(0, 0, 255),-1);

      if (!lidarCalledOne)
          std::cout << "new data generated!" << std::endl;
    lidarCalledOne = true;

    lidar_object.init_data(data);
    motion_planning_object.get_sensor_data(data);

    cv::circle(im, cv::Point(200, 200), 2, cv::Scalar(0, 0, 255));
    cv::putText(im, std::to_string(sec) + ":" + std::to_string(nsec),
                cv::Point(10, 20), cv::FONT_HERSHEY_PLAIN, 1.0,
                cv::Scalar(255, 0, 0));

   // cv::imwrite("/media/kenni/usb1/linuxUbuntu/Robotics5semesterProject/Tests/TangentbugTest/Analysis5.png", im);
    mutex1.lock();
    cv::imshow("lidar", im);
    cv::waitKey(1);
    mutex1.unlock();

}

void setup::poseCallback(ConstPosesStampedPtr &_msg){

    // Dump the message contents to stdout.
    //  std::cout << _msg->DebugString();


    for (int i = 0; i < _msg->pose_size(); i++)
    {
      if (_msg->pose(i).name() == "pioneer2dx")
      {
         // Original png picture 120*80 and tmp.eps paper size 85 × 56 mm. The scaling factor 5 is
         // used to map to the robot pose to the original picture and 60 and 40 to the center point
          pos_robot.robot_pos_pic.x = (_msg->pose(i).position().x()*1.411764706+60)*5; // (120/2)/(85/2) = 1.411..
          pos_robot.robot_pos_pic.y = (-_msg->pose(i).position().y()*1.428571429+40)*5; // (80/2)/(56/2) = 1.428..
          pos_robot.robot_pos.x = _msg->pose(i).position().x();
          pos_robot.robot_pos.y = _msg->pose(i).position().y();

          float w = _msg->pose(i).orientation().w();
          float x = _msg->pose(i).orientation().x();
          float y = _msg->pose(i).orientation().y();
          float z = _msg->pose(i).orientation().z();

       // roll (x-axis rotation)
           double sinr_cosp = +2.0 * (w * x + y * z);
           double cosr_cosp = +1.0 - 2.0 * (x * x + y * y);
          float roll = atan2(sinr_cosp, cosr_cosp);

       // pitch (y-axis rotation)
            double sinp = +2.0 * (w * y - z * x);
            float pitch;
            if (fabs(sinp) >= 1)
                pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
            else
                pitch = asin(sinp);

       // yaw (z-axis rotation)
            double siny_cosp = +2.0 * (w* z + x * y);
            double cosy_cosp = +1.0 - 2.0 * (y * y + z * z);

            mutex1.lock();

          //  if (siny_cosp<0 && cosy_cosp<0)
          //      pos_robot.angle_yaw_radi = atan2(siny_cosp, cosy_cosp) + 2*M_PI;
          //  else if ( siny_cosp<0 && cosy_cosp>0)
          //      pos_robot.angle_yaw_radi =atan2(siny_cosp, cosy_cosp) + 2*M_PI;
          //  else
                pos_robot.angle_yaw_radi = atan2(siny_cosp, cosy_cosp);

            pos_robot.angle_yaw_degress = (atan2(siny_cosp, cosy_cosp)*180)/M_PI;
            mutex1.unlock();

          //  std::cout << "Radi: " << pos_robot.angle_yaw_radi << "  " << "Degres: " << pos_robot.angle_yaw_degress << std::endl;
          //  std::cout << "x: " << pos_robot.robot_pos.x << "," << "y: " << pos_robot.robot_pos.y << std::endl;

      }

    }

}

ct::current_position setup::get_robot_position()
{
    return pos_robot;
}

void setup::statCallback(ConstWorldStatisticsPtr &_msg){

    (void)_msg;
    // Dump the message contents to stdout.
    //  std::cout << _msg->DebugString();
    //  std::cout << std::flush;

}


void setup::initGazebo(){

    // Load gazebo
    gazebo::client::setup();
    std::cout << "Gazebo client started" << std::endl;

    // Create our node for communication
    //node.swap(new gazebo::transport::Node());
    //gazebo::transport::NodePtr node1(new gazebo::transport::Node());
    //node1->Init();

    node = gazebo::transport::NodePtr(new gazebo::transport::Node());
    node->Init();

    std::cout << "Communication started" << std::endl;

    // Listen to Gazebo topics
    statSubscriber = node->Subscribe("~/world_stats", &setup::statCallback, this);

    poseSubscriber = node->Subscribe("~/pose/info", &setup::poseCallback, this);

    cameraSubscriber = node->Subscribe("~/pioneer2dx/camera/link/camera/image", &setup::cameraCallback, this);

    lidarSubscriber = node->Subscribe("~/pioneer2dx/hokuyo/link/laser/scan", &setup::lidarCallback, this);

    // Publish to the robot vel_cmd topic
    movementPublisher = node->Advertise<gazebo::msgs::Pose>("~/pioneer2dx/vel_cmd");

    // Publish a reset of the world
    worldPublisher = node->Advertise<gazebo::msgs::WorldControl>("~/world_control");
    controlMessage.mutable_reset()->set_all(true);
    worldPublisher->WaitForConnection();
    worldPublisher->Publish(controlMessage);

}
