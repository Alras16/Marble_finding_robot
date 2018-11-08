#include "dataloggin.h"
#include <ctime>


int main(int _argc, char **_argv)
{
    std::cout << "Hello World" << std::endl;

    map_class::room *aRoom = new map_class::room;
    aRoom->roomNumber = 1;
    cv::Point aPoint(205,5);
    map_class::foundMarble marble;
    marble.center = aPoint;
    marble.foundInRoom = aRoom;

    dataloggin stats("StatsRun",1,1,'s');
    dataloggin rooms("RoomsCOM",1,1,'r');
    //stats.writeStats(1,0.7746446);
    //rooms.writeRoom(3,cv::Point(2,4),5606);
    std::vector<map_class::room> temp = rooms.readRooms();
    //stats.readStats();
    //std::vector<float> temp = stats.readStats();
    /*for (unsigned int i = 0; i < temp.size(); i++)
    {
        std::cout << "Center: (" << temp[i].center.x << "," << temp[i].center.y << ") Room: " << temp[i].foundInRoom->roomNumber << std::endl;
    }*/
    //for (unsigned int i = 0; i < temp.size(); i++)
        //std::cout << "Room: " << std::setw(2) << i+1 << " Probability: " << temp[i] << std::endl;

    for (unsigned int i = 0; i < temp.size(); i++)
    {
        std::cout << "Room number: " << temp[i].roomNumber;
        std::cout << " Center of mass: (" << temp[i].centerOfMass.x << "," << temp[i].centerOfMass.y << ")";
        std::cout << " Pixels: " << temp[i].numbOfPixels << std::endl;
    }
}
