#include "bst_coordinates.h"
#include "map_class.h"
#include "dataloggin.h"
#include "q_learning.h"

int main(int _argc, char **_argv)
{
    /* This document calculates the average probability of a marble being placed in a specific room */

    map_class map("map_small.png",14,true);
    map.find_center_of_mass();
    q_learning qLearn(map);
    qLearn.setReward(2,10);
    int sweeps = qLearn.doEstimation(0.01);
    std::cout << "Number of sweeps: " << sweeps << std::endl;
    qLearn.paintValueEstimates();
    qLearn.paintPolicy();
    qLearn.showValueEstimates("Value Estimates");
    qLearn.showPolicy("Policy");
    qLearn.saveImage(1,1,sweeps);
    cv::waitKey(0);


    /*int numberOfTests = 2;
    int numberOfRuns = 10;

    // Make matrix of the probabilities
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
    std::vector<float> averageProbability;
    for (unsigned int i = 0; i < probabilityMatrix[0].size(); i++) // rooms
    {
        float sum = 0.0;
        for (unsigned int j = 0; j < probabilityMatrix.size(); j++) // runs
            sum += probabilityMatrix[j][i];

        sum /= probabilityMatrix.size();
        averageProbability.push_back(sum);
        std::cout << "Room" << std::setw(3) << i + 1 << ": " << std::setw(10) << sum << std::endl;
    }*/
}
