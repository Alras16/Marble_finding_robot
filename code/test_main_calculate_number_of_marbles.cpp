#include "bst_coordinates.h"
#include "map_class.h"
#include "dataloggin.h"

int main(int _argc, char **_argv)
{
    /* This document calculates the probability of a marble being placed in a specific room */

    int numberOfTests = 5;
    int numberOfRuns = 10;

    map_class map("map_small.png",14,true);

    // Find total number of pixels
    std::vector<ct::room> rooms = map.getRooms();
    float totalNumberOfPixels = 0.0;
    for (unsigned int room = 0; room < rooms.size(); room++)
        totalNumberOfPixels += rooms[room].numbOfPixels;

    // Calculate probability of a marble accuring in a room (size of room taken into account)    
    std::vector<float> marblesInRooms;
    for (int i = 0; i < 14; i++)
        marblesInRooms.push_back(0.0);

    for (int test = 1; test < 1 + numberOfTests; test++)
    {
        for (int run = 1; run < 1 + numberOfRuns; run++)
        {
            dataloggin marblesLog("MarblesRun",test,run,'m');
            std::vector<ct::foundMarble> foundMarbles = marblesLog.readMarbles();

            for (int room = 0; room < 14; room++)
            {
                for (unsigned int marble = 0; marble < foundMarbles.size(); marble++)
                    if (foundMarbles[marble].foundInRoom->roomNumber == rooms[room].roomNumber)
                        marblesInRooms[room]++;
            }
        }
    }

    for (unsigned int i = 0; i < marblesInRooms.size(); i++)
        std::cout << "Room " << i + 1 << ": " << marblesInRooms[i] << std::endl;

}
