#include "bst_coordinates.h"
#include "map_class.h"
#include "dataloggin.h"
#include "q_learning.h"

int main(int _argc, char **_argv)
{
    /* This document calculates the average probability of a marble being placed in a specific room */

    std::cout << "hmm" << std::endl;
    map_class map("map_small.png",14,true);
    map.find_center_of_mass();
    q_learning qLearn(map);
    qLearn.setReward(5,10);

    qLearn.resetReward(ct::state({43, 39, false}));
    qLearn.resetReward(ct::state({41, 15, false}));
    qLearn.resetReward(ct::state({68, 25, false}));
    qLearn.resetReward(ct::state({68, 9, false}));
    qLearn.resetReward(ct::state({17, 20, false}));
    qLearn.resetReward(ct::state({9, 7, false}));
    qLearn.resetReward(ct::state({31, 62, false}));
    qLearn.resetReward(ct::state({9, 62, false}));
    qLearn.resetReward(ct::state({92, 21, false}));
    qLearn.resetReward(ct::state({110, 15, false}));
    qLearn.resetReward(ct::state({106, 55, false}));
    qLearn.resetReward(ct::state({70, 53, false}));
    qLearn.resetReward(ct::state({78, 70, false}));

    int sweeps = qLearn.doEstimation(0.0);
    std::cout << "number of sweeps: " << sweeps << std::endl;
    qLearn.paintValueEstimates();
    qLearn.paintPolicy();

    std::vector<ct::state> path = qLearn.getPath(ct::state({78, 70, false}));
    std::cout << path[path.size() - 1].x  << "," << path[path.size() - 1].y << std::endl;
    //qLearn.resetReward(path[path.size() - 1]);

    qLearn.scaleImage(5);
    qLearn.showPolicy("Policy");
    qLearn.showValueEstimates("Value Estimates");

    qLearn.saveImage(2,14,sweeps);










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
