#ifndef MAP_CLASS_H
#define MAP_CLASS_H

#define WORLD_WIDTH 84.7
#define WORLD_HEIGHT 56.4

#include <QCoreApplication>
#include <QDir>
#include <vector>
#include <string>
#include <iostream>
#include "ct.h"

class map_class
{
public:
    map_class(std::string fileName, int numb_of_rooms, bool setAsTree);

    std::vector<ct::room> getRooms();
    void showMap(std::string title);
    cv::Point convertToPixel(ct::point aPoint);
    ct::point convertToCoordinate(cv::Point pixel);
    void setCurrentLocation(cv::Point coordinate);
    cv::Point getCurrentLocation();
    void find_center_of_mass();

    void paintRoom(ct::room* theRoom, cv::Vec3i color);
    void paintMarble(ct::foundMarble marble);

    std::vector<std::vector<float>> genStateMap(float freeSpaceVal, float wallVal);

private:

    void set_image(std::string fileName);
    std::vector<cv::Point> find_color(int color);
    void init_rooms(int numb_of_rooms);
    void paintRoom(ct::room theRoom, cv::Vec3i color);
    void paintMarble(ct::foundMarble marble);

    std::vector<ct::room> listOfRooms;
    std::vector<ct::foundMarble*> foundMarbles;

    cv::Point currLocation;
    cv::Mat ori_map;
    bool asTree;
    
    int image_w; // cols
    int image_h; // rows
};

#endif // MAP_CLASS_H
