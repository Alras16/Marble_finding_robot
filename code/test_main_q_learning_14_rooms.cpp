#include "q_learning.h"
#include "dataloggin.h"

int main(int _argc, char **_argv)
{
    std::cout << "hello world" << std::endl;

    int numberOfRooms = 14;
    float scalingReward = 45.0;
    float scalingDistance = 2.0;
    int numberOfTests = 5;
    int numberOfRuns = 10;
    int numberOfEpisodes = 10000;
    int numberOfSamples = 1;

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
    ct::newState room6 = start;
    ct::newState room7 = start;
    ct::newState room8 = start;
    ct::newState room9 = start;
    ct::newState room10 = start;
    ct::newState room11 = start;
    ct::newState room12 = start;
    ct::newState room13 = start;
    ct::newState room14 = start;

    room1.RoomNumber = 1;
    room2.RoomNumber = 2;
    room3.RoomNumber = 3;
    room4.RoomNumber = 4;
    room5.RoomNumber = 5;
    room6.RoomNumber = 6;
    room7.RoomNumber = 7;
    room8.RoomNumber = 8;
    room9.RoomNumber = 9;
    room10.RoomNumber = 10;
    room11.RoomNumber = 11;
    room12.RoomNumber = 12;
    room13.RoomNumber = 13;
    room14.RoomNumber = 14;

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

    float succeses = 0.0;
    float failiures = 0.0;
    float totalReward = 0.0;
    for (int test = 0; test < numberOfSamples; test++)
    {
        q_learning QL(numberOfRooms);
        // Set distance punishments
        QL.setDistancePunishment(start, room6, -1.17*scalingDistance);
        QL.setDistancePunishment(start, room9, -2.84*scalingDistance);
        QL.setDistancePunishment(room1, room2, -1.39*scalingDistance);
        QL.setDistancePunishment(room2, room3, -2.16*scalingDistance);
        QL.setDistancePunishment(room2, room6, -2.29*scalingDistance);
        QL.setDistancePunishment(room3, room4, -1.94*scalingDistance);
        QL.setDistancePunishment(room3, room5, -2.12*scalingDistance);
        QL.setDistancePunishment(room3, room6, -1.73*scalingDistance);
        QL.setDistancePunishment(room6, room7, -3.27*scalingDistance);
        QL.setDistancePunishment(room6, room8, -1.88*scalingDistance);
        QL.setDistancePunishment(room6, room9, -4.01*scalingDistance);
        QL.setDistancePunishment(room9, room10, -1.47*scalingDistance);
        QL.setDistancePunishment(room9, room11, -2.57*scalingDistance);
        QL.setDistancePunishment(room10, room11, -2.82*scalingDistance);
        QL.setDistancePunishment(room11, room12, -2.51*scalingDistance);
        QL.setDistancePunishment(room11, room13, -2.36*scalingDistance);
        QL.setDistancePunishment(room13, room14, -1.78*scalingDistance);

        for (int room = 0; room < numberOfRooms; room++)
        {
            std::cout << "Reward for entering room " << room + 1 << ": " << (averageProbability[room] / max)*scalingReward << std::endl;
            QL.setReward(room + 1, (averageProbability[room] / max)*scalingReward);
        }
        std::cout << std::endl;

        //QL.makeNewStateMatrix();
        //QL.printStateMatrix();

        // Do estimation episodes
        for (int episode = 0; episode < numberOfEpisodes; episode++)
        {
            std::cout << "episode number " << episode << std::endl;
            QL.doEpisode(start,0.1,0.9,0.5);
            std::cout << std::endl;
        }

        //QL.printStateMatrix();
        //QL.printQMatrix();

        for (int k = 0; k < 10; k++)
        {
            std::vector<int> path = QL.getPath(start);

            if (path.size() == 1)
            {
                std::cout << "no solution found" << std::endl;
                failiures++;
                totalReward -= 10000;
            }
            else
            {
                for (unsigned int i = 0; i < path.size(); i++)
                    std::cout << "Path next state: " << path[i] << std::endl;

                std::cout << std::endl;
                float reward = QL.getTotalReward(start);
                totalReward += reward;
                std::cout << "total reward: " << reward << std::endl;
                succeses++;
            }
        }
    }

    std::cout << std::endl;
    std::cout << "number of episodes: " << numberOfEpisodes << std::endl;
    std::cout << "success rate: " << succeses << std::endl;
    std::cout << "failiure rate: " << failiures << std::endl;
    std::cout << "average reward: " << totalReward / numberOfSamples << std::endl;
}
