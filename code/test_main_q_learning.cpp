#include "q_learning.h"

int main(int _argc, char **_argv)
{
    /* This document calculates the average probability of a marble being placed in a specific room */

    std::cout << "hello world" << std::endl;

    q_learning QL(2);

    /*std::vector<bool> temp = {false, false, false, false};
    for (int j = 0; j < std::pow(2,temp.size())-2; j++)
    {
        temp = QL.binaryIncrement(temp);
        for (unsigned int i = 0; i < temp.size(); i++)
            std::cout << temp[i];

        std::cout << std::endl;
    }*/


    ct::newState start;
    start.isTerminal = false;
    start.RoomNumber = 0;
    start.roomsVisited = {false,false};
    ct::newState room1 = start;
    room1.RoomNumber = 1;
    ct::newState room2 = start;
    room2.RoomNumber = 2;
    QL.setDistancePunishment(start, room1, 6.0);
    QL.setDistancePunishment(start, room2, 6.0);

    QL.setReward(1,100.0);
    QL.setReward(2,75.0);
    //QL.makeNewStateMatrix();

    for (int episode = 0; episode < 100; episode++)
    {
        QL.doEpisode(start,0.1,0.9,0.2);
        std::cout << "episode number " << episode << std::endl << std::endl;
    }

    QL.printStateMatrix();
    QL.printQMatrix();
}
