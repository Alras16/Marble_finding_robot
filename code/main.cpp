#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>

#include <opencv2/opencv.hpp>

#include <vector>
#include <string>
#include <ctime>
#include <iostream>

#include "c_vision.h"
#include "testdata.h"

static boost::mutex mutex;
bool lidarCalledOne = true;


void hls_histogram(cv::Mat &image)
{
    //Convert to HLS
    cv::Mat hls_image;
    cv::cvtColor(image, hls_image, CV_BGR2HLS);

    // Generate histogram
    std::vector<int> histogram;
    for (int i = 0; i < 181; i++)
        histogram.push_back(0);

    for (int i = 0; i < hls_image.rows; i++)
        for (int j = 0; j < hls_image.cols; j++)
        {
            cv::Vec3b cur_pixel = *hls_image.ptr<cv::Vec3b>(i,j);
            int H = cur_pixel[0];
            histogram[H]++;
        }

    // Make image for histogram
    int hist_w = 512; // cols
    int hist_h = 400; // rows
    cv::Mat histogram_image(hist_h,hist_w,CV_8UC3,cv::Scalar(0,0,0));

    // Normalize histogram to fit image
    int max = 0;
    for (unsigned int i = 0; i < histogram.size(); i++)
        if (histogram[i] > max)
            max = histogram[i];

    for (unsigned int i = 0; i < histogram.size(); i++)
        histogram[i] = (histogram[i] * (hist_h - 1)) /max;

    // Find top points
    unsigned int numb = 0;
    int offset = 50;
    for (unsigned int i = 0; i < histogram.size() - 1; i++)
    {
        if (i == 0)
        {
            if (histogram[i] > histogram[i+1])
            {
                numb = i;
                std::string text = "Max value at: " + std::to_string(numb);
                cv::putText(histogram_image,text,cv::Point(250,offset), cv::FONT_HERSHEY_COMPLEX_SMALL,1.0,cv::Scalar(255,255,255),1,CV_AA);
                offset+=25;
            }
        }
        else
        {
            if ((histogram[i-1] < histogram[i]) && (histogram[i] > histogram[i+1]))
            {
                numb = i;
                std::string text = "Max value at: " + std::to_string(numb);
                cv::putText(histogram_image,text,cv::Point(250,offset), cv::FONT_HERSHEY_COMPLEX_SMALL,1.0,cv::Scalar(255,255,255),1,CV_AA);
                offset+=25;
            }
        }
    }

    int width = 2;
    // Make plot
    int bin_w = cvRound((double)hist_w / 180);
    for (unsigned int i = 1; i < histogram.size(); i++)
    {
        if (numb == i)
        {
            cv::line(histogram_image, cv::Point(bin_w*(i-width),0),cv::Point(bin_w*(i-width),(hist_h - 1)),cv::Scalar(255,255,0),1,8,0);
            cv::line(histogram_image, cv::Point(bin_w*(i+width),0),cv::Point(bin_w*(i+width),(hist_h - 1)),cv::Scalar(255,255,0),1,8,0);
        }
        cv::line(histogram_image, cv::Point(bin_w*(i-1),hist_h - cvRound(histogram[i-1])),cv::Point(bin_w*(i),hist_h - cvRound(histogram[i])),cv::Scalar(255,0,0),2,8,0);
    }
    cv::imshow("Histogram",histogram_image);
}

void statCallback(ConstWorldStatisticsPtr &_msg) {
  (void)_msg;
  // Dump the message contents to stdout.
  //  std::cout << _msg->DebugString();
  //  std::cout << std::flush;
}

void poseCallback(ConstPosesStampedPtr &_msg) {
  // Dump the message contents to stdout.
  //  std::cout << _msg->DebugString();

  for (int i = 0; i < _msg->pose_size(); i++) {
    if (_msg->pose(i).name() == "pioneer2dx") {

      //std::cout << std::setprecision(2) << std::fixed << std::setw(6)
        //        << _msg->pose(i).position().x() << std::setw(6)
        //        << _msg->pose(i).position().y() << std::setw(6)
        //        << _msg->pose(i).position().z() << std::setw(6)
        //        << _msg->pose(i).orientation().w() << std::setw(6)
        //        << _msg->pose(i).orientation().x() << std::setw(6)
        //        << _msg->pose(i).orientation().y() << std::setw(6)
        //        << _msg->pose(i).orientation().z() << std::endl;
    }
  }
}

c_vision camera;

void cameraCallback(ConstImageStampedPtr &msg) {

  std::size_t width = msg->image().width();
  std::size_t height = msg->image().height();
  const char *data = msg->image().data().c_str();
  cv::Mat im(int(height), int(width), CV_8UC3, const_cast<char *>(data));

  im = im.clone();
  cv::cvtColor(im, im, CV_BGR2RGB);



  mutex.lock();
  cv::imshow("camera", im);
  //hls_histogram(im);
  camera.set_image(im);
  mutex.unlock();
}

void lidarCallback(ConstLaserScanStampedPtr &msg) {

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

  testData lidar("lidarTestFive");
  //testData::lidarData data = lidar.getLidarData();

  for (int i = 0; i < nranges; i++) {
    float angle = angle_min + i * angle_increment;
    float range = std::min(float(msg->scan().ranges(i)), range_max);
    //float angle = data.angle[i];
    //float range = data.range[i];


    if (!lidarCalledOne)
    {
        std::string temp_string = std::to_string(angle)  + " " + std::to_string(range);
        lidar.write(temp_string);
    }

    //    double intensity = msg->scan().intensities(i);
    cv::Point2f startpt(200.5f + range_min * px_per_m * std::cos(angle),
                        200.5f - range_min * px_per_m * std::sin(angle));
    cv::Point2f endpt(200.5f + range * px_per_m * std::cos(angle),
                      200.5f - range * px_per_m * std::sin(angle));
    cv::line(im, startpt * 16, endpt * 16, cv::Scalar(255, 255, 255, 255), 1,
             cv::LINE_AA, 4);

    //    std::cout << angle << " " << range << " " << intensity << std::endl;
  }
    if (!lidarCalledOne)
        std::cout << "new data generated!" << std::endl;
  lidarCalledOne = true;


  cv::circle(im, cv::Point(200, 200), 2, cv::Scalar(0, 0, 255));
  cv::putText(im, std::to_string(sec) + ":" + std::to_string(nsec),
              cv::Point(10, 20), cv::FONT_HERSHEY_PLAIN, 1.0,
              cv::Scalar(255, 0, 0));

  mutex.lock();
  cv::imshow("lidar", im);
  mutex.unlock();
}

int main(int _argc, char **_argv) {

  // Load gazebo
  gazebo::client::setup(_argc, _argv);
  std::cout << "Gazebo client started" << std::endl;

  // Create our node for communication
  gazebo::transport::NodePtr node(new gazebo::transport::Node());
  node->Init();
  std::cout << "Communication started" << std::endl;

  // Listen to Gazebo topics
  gazebo::transport::SubscriberPtr statSubscriber =
      node->Subscribe("~/world_stats", statCallback);

  gazebo::transport::SubscriberPtr poseSubscriber =
      node->Subscribe("~/pose/info", poseCallback);

  gazebo::transport::SubscriberPtr cameraSubscriber =
      node->Subscribe("~/pioneer2dx/camera/link/camera/image", cameraCallback);

  gazebo::transport::SubscriberPtr lidarSubscriber =
      node->Subscribe("~/pioneer2dx/hokuyo/link/laser/scan", lidarCallback);

  // Publish to the robot vel_cmd topic
  gazebo::transport::PublisherPtr movementPublisher =
      node->Advertise<gazebo::msgs::Pose>("~/pioneer2dx/vel_cmd");

  // Publish a reset of the world
  gazebo::transport::PublisherPtr worldPublisher =
      node->Advertise<gazebo::msgs::WorldControl>("~/world_control");
  gazebo::msgs::WorldControl controlMessage;
  controlMessage.mutable_reset()->set_all(true);
  worldPublisher->WaitForConnection();
  worldPublisher->Publish(controlMessage);

  const int key_left = 81;
  const int key_up = 82;
  const int key_down = 84;
  const int key_right = 83;
  const int key_esc = 27;

  float speed = 0.0;
  float dir = 0.0;

  // Loop
  while (true) {
    gazebo::common::Time::MSleep(10);

    mutex.lock();
    int key = cv::waitKey(1);
    mutex.unlock();

    if (key == key_esc)
      lidarCalledOne = false;
        //break;

    if ((key == key_up) && (speed <= 1.2f))
      speed += 0.5;
        //speed += 0.05;
    else if ((key == key_down) && (speed >= -1.2f))
      speed -= 0.5;
        //speed -= 0.05;
    else if ((key == key_right) && (dir <= 0.4f))
      dir += 0.05;
    else if ((key == key_left) && (dir >= -0.4f))
      dir -= 0.05;
    else {
      // slow down
            //speed = 0;
      //      dir *= 0.1;
    }

    //camera.find_color();
    /*clock_t start;
    double diff;
    start = clock();
    //camera.find_color();
    camera.find_marbles();
    diff = ((clock() - start) / (double)CLOCKS_PER_SEC)*1000;
    std::cout << "Execution time: " << diff << std::endl;*/

    // Generate a pose
    ignition::math::Pose3d pose(double(speed), 0, 0, 0, 0, double(dir));

    // Convert to a pose message
    gazebo::msgs::Pose msg;
    gazebo::msgs::Set(&msg, pose);
    movementPublisher->Publish(msg);
  }

  // Make sure to shut everything down.
  gazebo::client::shutdown();
}
