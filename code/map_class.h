#ifndef MAP_CLASS_H
#define MAP_CLASS_H

#define WORLD_WIDTH 8.47
#define WORLD_HEIGHT 5.64

#include <QCoreApplication>
#include <QDir>
#include <vector>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "bst_coordinates.h"

class map_class
{
public:

    struct point
    {
        float x;
        float y;
    };

    struct room
    {
        int roomNumber;
        std::vector<cv::Point> coordinates;
        BST_coordinates coordinatesTree;
        float probabilityOfMarbles;
        int numbOfPixels;
        cv::Point centerOfMass;
    };
    
    struct foundMarble
    {
        cv::Point center;
        room *foundInRoom;
    };

    map_class(std::string fileName, int numb_of_rooms, bool setAsTree);

    std::vector<room*> getRooms();
    void showMap(std::string title);
    cv::Point convertToPixel(point aPoint);
    point convertToCoordinate(cv::Point pixel);
    void setCurrentLocation(cv::Point coordinate);
    cv::Point getCurrentLocation();

private:

    void set_image(std::string fileName);
    std::vector<cv::Point> find_color(int color);
    void init_rooms(int numb_of_rooms);
    void paintRoom(room* theRoom, cv::Vec3i color);
    void paintMarble(foundMarble marble);

    std::vector<room*> listOfRooms;
    std::vector<foundMarble*> foundMarbles;

    cv::Point currLocation;
    cv::Mat ori_map;
    bool asTree;
    
    int image_w; // cols
    int image_h; // rows
};

#endif // MAP_CLASS_H
