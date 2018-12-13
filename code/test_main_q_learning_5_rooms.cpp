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
    std::vector<int> numberOfEpisodes = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,25,30,35,40,45,50,60,70,80,90,100,125,150,175,200,250,300,350,400,450,500,600,700,800,900,1000};
    int numberOfSamples = 100;

    float alpha = 0.025;
    float gamma = 0.9;
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

    std::vector<float> rewards;
    std::vector<float> iterations;
    for (unsigned int variable = 0; variable < numberOfEpisodes.size(); variable++)
    {
        float totalReward = 0.0;
        float totalNumbIterations = 0.0;
        for (int test = 0; test < numberOfSamples; test++)
        {
            q_learning QL(numberOfRooms);
            // Set distance punishments
            QL.setDistancePunishment(start, room3, 0*scalingDistance); // which room to start in
            QL.setDistancePunishment(room1, room2, -1.39*scalingDistance);
            QL.setDistancePunishment(room2, room3, -2.16*scalingDistance);
            QL.setDistancePunishment(room3, room4, -1.94*scalingDistance);
            QL.setDistancePunishment(room3, room5, -2.12*scalingDistance);

            for (int room = 0; room < numberOfRooms; room++)
            {
                std::cout << "Reward for entering room " << room + 1 << ": " << (averageProbability[room] / max)*scalingReward << std::endl;
                QL.setReward(room + 1, (averageProbability[room] / max)*scalingReward);
            }
            std::cout << std::endl;

            // Do estimation episodes
            for (int episode = 0; episode < numberOfEpisodes[variable]; episode++)
            {
                std::cout << "episode number " << episode << std::endl;
                totalNumbIterations += QL.doEpisode(start, alpha, gamma, epsilon);
                std::cout << std::endl;
            }
            //QL.printStateMatrix();
            //QL.printQMatrix();

            std::vector<int> path = QL.getPath(start, alpha, gamma, epsilon);
            //for (unsigned int i = 0; i < path.size(); i++)
                //std::cout << "Path next state: " << path[i] << std::endl;

            std::cout << std::endl;
            float reward = QL.getTotalReward(path);
            totalReward += reward;
            std::cout << "total reward: " << reward << std::endl;

        }

        std::cout << std::endl;
        std::cout << "number of episodes: " << numberOfEpisodes[variable] << std::endl;
        std::cout << "average reward: " << totalReward / numberOfSamples << std::endl;
        rewards.push_back(totalReward / numberOfSamples);
        totalNumbIterations /= (numberOfEpisodes[variable] * numberOfSamples);
        std::cout << "average number of iterations: " << totalNumbIterations << std::endl;
        iterations.push_back(totalNumbIterations);
    }

    iterations[0] = 0;
    for (unsigned int i = 0; i < rewards.size(); i++)
    {
        std::cout << numberOfEpisodes[i] << ";";
        std::cout << rewards[i] << ";";
        std::cout << round(iterations[i]);
        std::cout << std::endl;
    }
}
