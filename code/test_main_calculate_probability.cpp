#include "bst_coordinates.h"
#include "map_class.h"
#include "dataloggin.h"

int main(int _argc, char **_argv)
{
    /* This document calculates the probability of a marble being placed in a specific room */

    int testNumber = 2;
    int numberOfRuns = 10;

    map_class map("map_small.png",14,true);

    // Find total number of pixels
    std::vector<ct::room*> rooms = map.getRooms();
    float totalNumberOfPixels = 0.0;
    for (unsigned int room = 0; room < rooms.size(); room++)
        totalNumberOfPixels += rooms[room]->numbOfPixels;

    // Calculate probability of a marble accuring in a room (size of room taken into account)
    std::vector<std::vector<float>> marbleMatrix;
    for (int run = 1; run < 1 + numberOfRuns; run++)
    {
        dataloggin marblesLog("MarblesRun",testNumber,run,'m');

        std::vector<ct::foundMarble> foundMarbles = marblesLog.readMarbles();
        std::vector<float> marblesInRooms;

        for (int room = 0; room < 14; room++)
        {
            float marblesInRoom = 0.0;
            for (unsigned int marble = 0; marble < foundMarbles.size(); marble++)
                if (foundMarbles[marble].foundInRoom->roomNumber == rooms[room]->roomNumber)
                    marblesInRoom++;
            marblesInRooms.push_back((marblesInRoom / 20.0) * (rooms[room]->numbOfPixels / totalNumberOfPixels));
        }
        marbleMatrix.push_back(marblesInRooms);
    }

    // Normalise the probability
    for (unsigned int run = 0; run < marbleMatrix.size(); run++)
    {
        float sum = 0.0;
        for (unsigned int room = 0; room < marbleMatrix[run].size(); room++)
            sum += marbleMatrix[run][room];

        for (unsigned int room = 0; room < marbleMatrix[run].size(); room++)
            marbleMatrix[run][room] /= sum;
    }

    // Generate loggin files
    for (unsigned int run = 0; run < marbleMatrix.size(); run++)
    {
        dataloggin statsLog("StatsRun",testNumber,run + 1,'s');
        for (unsigned int room = 0; room < marbleMatrix[run].size(); room++)
            statsLog.writeStats(room + 1,marbleMatrix[run][room]);
    }
}
