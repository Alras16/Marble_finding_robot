#include "q_learning.h"
#include "dataloggin.h"

int main(int _argc, char **_argv)
{
    std::cout << "hello world" << std::endl;

    int numberOfRooms = 5;
    float scalingReward = 20.0;
    float scalingDistance = 1.2;
    int numberOfTests = 5;
    int numberOfRuns = 10;

    float alpha = 0.025;
    float gamma = 0.99;
    float epsilon = 0.05;

    // Init base states
    ct::newState start;
    start.isTerminal = false;
    start.RoomNumber = 0;
    std::vector<bool> temp;
    for (int i = 0; i < numberOfRooms; i++)
        temp.push_back(false);
    start.roomsVisited = temp;
    ct::newState room1 = start;
    ct::newState room2 = start;
    ct::newState room3 = start;
    ct::newState room4 = start;
    ct::newState room5 = start;

    room1.RoomNumber = 1;
    room2.RoomNumber = 2;
    room3.RoomNumber = 3;
    room4.RoomNumber = 4;
    room5.RoomNumber = 5;

    // Set rewards
    // Make matrix of the probabilities
    std::vector<float> averageProbability;
    std::vector<std::vector<float>> probabilityMatrix;
    for (int test = 1; test < 1 + numberOfTests; test++)
    {
        for (int run = 1; run < 1 + numberOfRuns; run++)
        {
            dataloggin statsLog("StatsRun",test,run,'s');
            probabilityMatrix.push_back(statsLog.readStats());
        }
    }

    // Calculate average probability
    for (unsigned int i = 0; i < probabilityMatrix[0].size(); i++) // rooms
    {
        float sum = 0.0;
        for (unsigned int j = 0; j < probabilityMatrix.size(); j++) // runs
            sum += probabilityMatrix[j][i];

        sum /= probabilityMatrix.size();
        averageProbability.push_back(sum);
        std::cout << "Room" << std::setw(3) << i + 1 << ": " << std::setw(10) << sum << std::endl;
    }
    std::cout << std::endl;

    // Find max value
    float max = 0.0;
    for (int room = 0; room < numberOfRooms; room++)
        if (averageProbability[room] > max)
            max = averageProbability[room];

    q_learning QL(numberOfRooms);
    // Set distance punishments
    QL.setDistancePunishment(start, room5, 0*scalingDistance); // which room to start in
    QL.setDistancePunishment(room1, room2, -1.39*scalingDistance);
    QL.setDistancePunishment(room2, room3, -2.16*scalingDistance);
    QL.setDistancePunishment(room3, room4, -1.94*scalingDistance);
    QL.setDistancePunishment(room3, room5, -2.12*scalingDistance);

    for (int room = 0; room < numberOfRooms; room++)
    {
        std::cout << "Reward for entering room " << room + 1 << ": " << (averageProbability[room] / max)*scalingReward << std::endl;
        QL.setReward(room + 1, (averageProbability[room] / max)*scalingReward);
    }

    std::vector<std::vector<int>> path = {
        {0,5,3,2,1,2,3,4},
        {0,5,3,2,1,2,3,4},
        {0,5,3,5,5,3,4,3,2,1},
        {0,5,3,2,1,2,3,4},
        {0,5,3,2,1,2,3,4},
        {0,5,3,2,1,1,2,3,2,3,4},
        {0,5,3,2,1,2,3,4},
        {0,5,3,4,3,2,1},
        {0,5,3,4,3,2,1},
        {0,5,3,4,3,2,1}
    };
    float average_reward = 0.0;

    for (unsigned int i = 0; i < path.size(); i++)
    {
        float reward = QL.getTotalReward(path[i]);
        average_reward += reward;
        std::cout << reward << std::endl;
    }
    std::cout << average_reward / path.size() << std::endl;

    while (true);
}
