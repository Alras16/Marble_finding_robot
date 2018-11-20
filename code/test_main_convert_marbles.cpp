#include "bst_coordinates.h"
#include "map_class.h"
#include "dataloggin.h"

int main(int _argc, char **_argv)
{
    std::cout << "Hello World" << std::endl;
    int testNumber = 5;

    map_class map("map_small.png",14,true);
    for (int i = 1; i < 11; i++)
    {
        dataloggin raw("MarblesRunRaw",testNumber,i,'m');
        dataloggin marbles("MarblesRun",testNumber,i,'m');
        std::vector<ct::point> centerRaw = raw.readMarblesRaw();
        for (unsigned int j = 0; j < centerRaw.size(); j++)
        {
            ct::foundMarble tempFoundMarble;
            ct::marble tempMarble;
            tempMarble.center = map.convertToPixel(centerRaw[j]);
            tempMarble.radius = 0.5;
            tempFoundMarble.fMarble = tempMarble;
            for (unsigned int k = 0; k < map.getRooms().size(); k++)
            {
                if (map.getRooms()[k]->coordinatesTree.contains(tempMarble.center))
                {
                    tempFoundMarble.foundInRoom = map.getRooms()[k];
                    break;
                }
            }
            marbles.writeMarble(tempFoundMarble,j);
        }
    }


    //cv::Point t = map.convertToPixel(p);
    //std::cout << t.x << ";" << t.y << std::endl;
    //ct::point t_p = map.convertToCoordinate(t);
    //std::cout << t_p.x << ";" << t_p.y << std::endl;
    //map_class displayMap("map_medium.png",14,false);
    //std::vector<map_class::room*> rooms = map.getRooms();
    //map.paintRoom(rooms[1],cv::Vec3i({0,0,255}));
    //displayMap.showMap("medium map");
    //map.showMap("small map");
    //std::cout << "number of rooms: " << rooms.size() << std::endl;
    //cv::waitKey(0);
}
