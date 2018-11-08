#include "lidar_sensor.h"
#include "testdata.h"

int main(int _argc, char **_argv)
{
    std::cout << "Hello World" << std::endl;

    lidar_sensor lidar;
    testData lidarD("lidarTestThree");
    testData::lidarData data = lidarD.getLidarData();
    std::vector<ct::polarPoint> temp_vec;
    for (unsigned int i = 0; i < data.angle.size(); i++)
    {
        ct::polarPoint temp_point;
        temp_point.theta = data.angle[i];
        temp_point.rho = data.range[i];
        temp_vec.push_back(temp_point);
    }
    lidar.init_data(temp_vec);
    lidar.filter_data();
    lidar.find_marbles();
    lidar.find_lines();
    lidar.merge_lines();
    lidar.visualize_lidar("lidar test");

    cv::waitKey(0);
}
