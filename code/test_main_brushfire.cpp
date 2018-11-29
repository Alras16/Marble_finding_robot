//#include <gazebo/gazebo_client.hh>
//#include <gazebo/msgs/msgs.hh>
//#include <gazebo/transport/transport.hh>

#include <opencv2/opencv.hpp>

#include <vector>
#include <string>
#include <ctime>
#include <iostream>

#include "ct.h"
#include "map_class.h"
#include "brushfire.h"

int main()
{
    std::string fileName = "map_small.png";
    brushfire map(fileName);
    map.brushfireAlgorithm();
    map.paintMap();
    map.showImage(fileName);

    cv::waitKey(0);
}
