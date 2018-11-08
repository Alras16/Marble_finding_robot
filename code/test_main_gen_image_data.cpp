#include <gazebo/gazebo_client.hh>
#include <gazebo/msgs/msgs.hh>
#include <gazebo/transport/transport.hh>

#include "ct.h"
#include "lidar_sensor.h"

#include <ctime>

static boost::mutex mutex;
lidar_sensor lidar;

void cameraCallback(ConstImageStampedPtr &msg) {

  std::size_t width = msg->image().width();
  std::size_t height = msg->image().height();
  const char *data = msg->image().data().c_str();
  cv::Mat im(int(height), int(width), CV_8UC3, const_cast<char *>(data));

  im = im.clone();
  cv::cvtColor(im, im, CV_BGR2RGB);

  mutex.lock();
  cv::imshow("camera", im);
  mutex.unlock();
}

void lidarCallback(ConstLaserScanStampedPtr &msg)
{
    float angle_min = float(msg->scan().angle_min());
    float angle_increment = float(msg->scan().angle_step());

    float range_max = float(msg->scan().range_max());

    int nranges = msg->scan().ranges_size();
    int nintensities = msg->scan().intensities_size();

    assert(nranges == nintensities);

    std::vector<ct::polarPoint> data;
    for (int i = 0; i < nranges; i++)
    {
        ct::polarPoint tempPoint;
        tempPoint.theta = angle_min + i * angle_increment;
        tempPoint.rho = std::min(float(msg->scan().ranges(i)), range_max);
        data.push_back(tempPoint);
    }
    lidar.init_data(data);
}

int main(int _argc, char **_argv)
{
    // Load gazebo
    gazebo::client::setup(_argc, _argv);

    // Create our node for communication
    gazebo::transport::NodePtr node(new gazebo::transport::Node());
    node->Init();
    std::cout << "Communication started" << std::endl;

    // Listen to Gazebo topics
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
    while (true)
    {
        gazebo::common::Time::MSleep(50);

        mutex.lock();
        int key = cv::waitKey(1);
        mutex.unlock();

        if (key == key_esc)
            break;

        if ((key == key_up) && (speed <= 1.2f))
            speed += 1;
        else if ((key == key_down) && (speed >= -1.2f))
            speed -= 1;
        else if ((key == key_right) && (dir <= 0.4f))
            dir += 0.05;
        else if ((key == key_left) && (dir >= -0.4f))
            dir -= 0.05;
        else
        {
            // slow down
            //speed *= 0.1;
            //dir *= 0.1;
        }

        //clock_t start;
        //double diff;
        //start = clock();
        lidar.filter_data();
        //std::cout << "filtering finished" << std::endl;
        lidar.find_marbles();
        lidar.filter_marbles();
        //std::cout << "marbles found" << std::endl;
        lidar.find_lines();
        //std::cout << "lines found" << std::endl;
        lidar.merge_lines();

        //std::cout << "lines merged" << std::endl;
        //diff = ((clock() - start) / (double)CLOCKS_PER_SEC)*1000;
        //std::cout << "Execution time: " << diff  << " ms" << std::endl;
        mutex.lock();
        //gazebo::common::Time::MSleep(1);
        lidar.visualize_lidar("lidar");
        //std::cout << "lidar visualised" << std::endl;
        mutex.unlock();

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
