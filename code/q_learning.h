#ifndef Q_LEARNING_H
#define Q_LEARNING_H

#include <QCoreApplication>
#include <QDir>
#include <vector>
#include <string>
#include <iostream>
#include "ct.h"
#include <time.h>

#include <random>
#include <chrono>
//#include "map_class.h"
//#include "dataloggin.h"

#define ACTION_NOT_ABLE  -100.0
#define NO_ACTION 0.0

class q_learning
{
public:
    q_learning(int numberOfRooms);

    void setDistancePunishment(ct::newState stateOne, ct::newState stateTwo, float punishment);
    void makeNewStateMatrix();
    void makeNewQMatrix();
    void setReward(int roomNumber, float reward);

    void printStateMatrix();
    void printQMatrix();

    ct::newState visitRoom(ct::newState s);

    // Q-learning
    ct::newState getNextState(ct::newState s, int a);
    float getReward(ct::newState s, int a);
    int getNextAction(ct::newState s);
    int eGreedyPolicy(ct::newState s, float epsilon);
    float maxQValue(ct::newState s);
    ct::newState qUpdate(ct::newState s, float alpha, float gamma, float epsilon);
    void doEpisode(ct::newState start, float alpha, float gamma, float epsilon);    
    std::vector<int> getPath(ct::newState start, float alpha, float gamma, float epsilon);
    float getTotalReward(std::vector<int> path);

private:
    int findStateMatrixIndex(ct::newState s);
    int findQMatrixIndex(ct::newState s);

    float getRandom(int min, int max);
    int getRandomIndex(int size);
    std::vector<bool> binaryIncrement(std::vector<bool> numb);

    std::vector<std::vector<float>> baseStateMatrix; // Base environment (matrix)
    std::vector<std::vector<std::vector<float>>> stateMatrix; // Environment (vector of matrices)
    std::vector<std::vector<std::vector<float>>> qValues;

    std::vector<std::vector<bool>> stateMatrixOrder;
    std::vector<std::vector<bool>> qMatrixOrder;
    std::vector<bool> visitedRooms;

    std::vector<float> rewards; // averageProbability

    std::mt19937_64 rng;

    int numbOfRooms;
};

#endif // Q_LEARNING_H
