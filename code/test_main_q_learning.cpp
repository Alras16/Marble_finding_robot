#include "q_learning.h"

int main(int _argc, char **_argv)
{
    /* This document calculates the average probability of a marble being placed in a specific room */

    std::cout << "hello world" << std::endl;
    /*q_learning QL(2);
    int zero = 0;
    int one = 0;
    int two = 0;
    for (int i = 0; i < 10000; i++)
    {
        //float randomNumb = rand() / double(RAND_MAX);
        int randomNumb = QL.getRandomIndex(3);
        std::cout << "random number: " << randomNumb << std::endl;
        if (randomNumb == 0)
            zero++;
        else if (randomNumb == 1)
            one++;
        else
            two++;
        {
            std::cout << "random action" << std::endl;
            randomAction++;
        }
        else
        {
            std::cout << "policy action" << std::endl;
            policyAction++;
        }
    }
    std::cout << std::endl;
    std::cout << "Number of 0: " << zero << std::endl;
    std::cout << "Number of 1: " << one << std::endl;
    std::cout << "Number of 2: " << two << std::endl;*/

    q_learning QL(2);
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
    QL.makeNewStateMatrix();

    QL.doEpisode(start,0.2,0.9,0.2);
    /*std::cout << "visited rooms: " << start.roomsVisited[0] << ", " << start.roomsVisited[1] << std::endl;
    ct::newState next = QL.qUpdate(start,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;

    next = QL.qUpdate(next,0.5,0.9,0.2);
    std::cout << "state: " << next.RoomNumber << std::endl;
    std::cout << "visited rooms: " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;*/

    QL.printStateMatrix();
    QL.printQMatrix();
    //QL.visitRoom(room1);
    //QL.visitRoom(2);
    //QL.makeNewStateMatrix();

}
