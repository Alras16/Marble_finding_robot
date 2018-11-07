#include "setup.h"

boost::mutex setup::mutex1;

setup::setup() : controller( &laser_scanner)
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

void setup::lidarCallback(ConstLaserScanStampedPtr &msg){


    bool lidarCalledOne = true;

    laser_scanner.parseLaserScannerMessage(msg);

    //  std::cout << ">> " << msg->DebugString() << std::endl;
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

    //testData lidar("lidarTestFive");
    //testData::lidarData data = lidar.getLidarData();

    for (int i = 0; i < nranges; i++) {
      float angle = angle_min + i * angle_increment;
      float range = std::min(float(msg->scan().ranges(i)), range_max);
      //float angle = data.angle[i];
      //float range = data.range[i];


      if (!lidarCalledOne)
      {
          std::string temp_string = std::to_string(angle)  + " " + std::to_string(range);
         // lidar.write(temp_string);
      }

      //    double intensity = msg->scan().intensities(i);
      cv::Point2f startpt(200.5f + range_min * px_per_m * std::cos(angle),
                          200.5f - range_min * px_per_m * std::sin(angle));
      cv::Point2f endpt(200.5f + range * px_per_m * std::cos(angle),
                        200.5f - range * px_per_m * std::sin(angle));
      cv::line(im, startpt * 16, endpt * 16, cv::Scalar(255, 255, 255, 255), 1,
               cv::LINE_AA, 4);

      //    std::cout << angle << " " << range << std::endl; // << " " << intensity << std::endl;
    }
      if (!lidarCalledOne)
          std::cout << "new data generated!" << std::endl;
    lidarCalledOne = true;


    cv::circle(im, cv::Point(200, 200), 2, cv::Scalar(0, 0, 255));
    cv::putText(im, std::to_string(sec) + ":" + std::to_string(nsec),
                cv::Point(10, 20), cv::FONT_HERSHEY_PLAIN, 1.0,
                cv::Scalar(255, 0, 0));

    mutex1.lock();
    cv::imshow("lidar", im);
    cv::waitKey(1);
    mutex1.unlock();

}

void setup::poseCallback(ConstPosesStampedPtr &_msg){

    // Dump the message contents to stdout.
    //  std::cout << _msg->DebugString();

    for (int i = 0; i < _msg->pose_size(); i++) {
      if (_msg->pose(i).name() == "pioneer2dx") {
        /*
        std::cout << std::setprecision(2) << std::fixed << std::setw(6)
                 << _msg->pose(i).position().x() << std::setw(6)
                << _msg->pose(i).position().y() << std::setw(6)
                 << _msg->pose(i).position().z() << std::setw(6)
                 << _msg->pose(i).orientation().w() << std::setw(6)
                 << _msg->pose(i).orientation().x() << std::setw(6)
                 << _msg->pose(i).orientation().y() << std::setw(6)
                 << _msg->pose(i).orientation().z() << std::endl;
                 */
      }

    }

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
