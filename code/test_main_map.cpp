#include "bst_coordinates.h"
#include "map_class.h"


int main(int _argc, char **_argv)
{
    std::cout << "Hello World" << std::endl;

    map_class map("map_small.png",14,true);
    ct::point p;
    p.x = -4.26351;
    p.y = -20.6245;

    for (int i = 0; i < 14; i++)
    {
        bool result = map.getRooms()[i]->coordinatesTree.contains(map.convertToPixel(p));
        if (result)
            std::cout << "is part of room " << i + 1 << ": yes" << std::endl;
        else
            std::cout << "is part of room " << i + 1 << ": no" << std::endl;
    }


    cv::Point t = map.convertToPixel(p);
    std::cout << t.x << ";" << t.y << std::endl;
    ct::point t_p = map.convertToCoordinate(t);
    std::cout << t_p.x << ";" << t_p.y << std::endl;
    map_class displayMap("map_medium.png",14,false);
    //std::vector<map_class::room*> rooms = map.getRooms();
    //map.paintRoom(rooms[1],cv::Vec3i({0,0,255}));
    displayMap.showMap("medium map");
    //map.showMap("small map");
    //std::cout << "number of rooms: " << rooms.size() << std::endl;
    cv::waitKey(0);
}
