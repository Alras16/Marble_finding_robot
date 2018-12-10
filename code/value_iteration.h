#ifndef VALUE_ITERATION_H
#define VALUE_ITERATION_H

#include <QCoreApplication>
#include <QDir>
#include <vector>
#include <string>
#include <iostream>
#include "ct.h"
#include "map_class.h"
#include "dataloggin.h"

#define FREE_SPACE  2.0
#define WALL        3.0

class value_iteration
{
public:
    value_iteration(map_class map);

    void setReward(int numberOfTests, int numberOfRuns);

    ct::state getNextState(ct::state s, ct::action a);
    float getReward(ct::state s, ct::action a);
    ct::action getNextAction(ct::state s);
    void resetReward(ct::state s);
    void deleteMaxReward();

    float performFullSweep();
    float doEstimation(float theta);
    std::vector<ct::state> getPath(ct::state startState);


    void paintValueEstimates();
    void paintPolicy();
    void showValueEstimates(std::string name);
    void showPolicy(std::string name);
    void scaleImage(int factor);
    void saveImage(int testNumb, int runNumb, int sweepNumb);

private:
    void paintWalls();
    void paintCenterOfMass();
    void paintOutsideEnvironment();

    int width = 0;
    int height = 0;

    std::vector<std::vector<float>> stateMatrix; // Environment
    std::vector<std::vector<float>> stateValueEstimates;
    cv::Mat imageValues;
    cv::Mat imagePolicy;

    float discountRate = 0.9;

    std::vector<ct::room> centerOfMassRooms;
    std::vector<float> averageProbability;
};

#endif // VAULUE_ITERATION_H
