#include "q_learning.h"

q_learning::q_learning(int numberOfRooms)
{
    std::vector<std::vector<float>> tempMatrix;
    for (int i = 0; i < numberOfRooms + 1; i++)
    {
        std::vector<float> tempVector;
        for (int j = 0; j < numberOfRooms + 1; j++)
            tempVector.push_back(0.0);

        tempMatrix.push_back(tempVector);
    }
    baseStateMatrix = tempMatrix;

    for (unsigned int state = 0; state < baseStateMatrix.size(); state++)
    {
        for (unsigned int action = 0; action < baseStateMatrix[state].size(); action++)
        {
            if (state != action)
                baseStateMatrix[state][action] = ACTION_NOT_ABLE;
            else
                baseStateMatrix[state][action] = NO_ACTION;
        }
    }

    qValues.push_back(tempMatrix);

    for (int i = 0; i < numberOfRooms; i++)
    {
        visitedRooms.push_back(false);
        rewards.push_back(0.0);
    }

    std::vector<bool> temp;
    for (int i = 0; i < numberOfRooms; i++)
        temp.push_back(false);
    matrixOrder.push_back(temp);

    uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32)};
    rng.seed(ss);
}

// Public methods
void q_learning::setDistancePunishment(ct::newState stateOne, ct::newState stateTwo, float punishment)
{
    // Set the distance punishment for a given state transition
    baseStateMatrix[stateOne.RoomNumber][stateTwo.RoomNumber] = punishment;
    baseStateMatrix[stateTwo.RoomNumber][stateOne.RoomNumber] = punishment;
}

void q_learning::makeNewStateMatrix()
{
    std::vector<std::vector<float>> tempMatrix = baseStateMatrix;
    for (unsigned int action = 1; action < visitedRooms.size() + 1; action++)
    {
        if (!visitedRooms[action - 1]) // room not visited
        {
            for (unsigned int state = 0; state < visitedRooms.size() + 1; state++)
            {
                if ((tempMatrix[state][action] != ACTION_NOT_ABLE) && (tempMatrix[state][action] != NO_ACTION))
                {
                    // reward will be placed
                    float punishment = tempMatrix[state][action];
                    tempMatrix[state][action] = rewards[action - 1] - punishment;
                }
            }
        }
    }
    stateMatrix.push_back(tempMatrix);
}

void q_learning::makeNewQMatrix()
{
    std::vector<std::vector<float>> tempMatrix = baseStateMatrix;
    for (unsigned int state = 0; state < baseStateMatrix.size(); state++)
        for (unsigned int action = 0; action < baseStateMatrix[state].size(); action++)
            tempMatrix[state][action] = 0.0;

    qValues.push_back(tempMatrix);
}

void q_learning::setReward(int roomNumber, float reward)
{
    rewards[roomNumber - 1] = reward;
}

void q_learning::printStateMatrix()
{
    for (unsigned int i = 0; i < stateMatrix.size(); i++)
    {
        for (unsigned int state = 0; state < stateMatrix[i].size(); state++)
        {
            for (unsigned int action = 0; action < stateMatrix[i][state].size(); action++)
            {
                printf(" %5.2f ",stateMatrix[i][state][action]);
            }
            printf("\n");
        }
        std::cout << std::endl;
    }
}

void q_learning::printQMatrix()
{
    for (unsigned int i = 0; i < qValues.size(); i++)
    {
        for (unsigned int state = 0; state < qValues[i].size(); state++)
        {
            for (unsigned int action = 0; action < qValues[i][state].size(); action++)
            {
                printf(" %5.2f ",qValues[i][state][action]);
            }
            printf("\n");
        }
        std::cout << std::endl;
    }
}

ct::newState q_learning::visitRoom(ct::newState s)
{
    if (s.RoomNumber == 0)
        return s;

    // get the new state
    ct::newState temp = s;
    temp.roomsVisited[s.RoomNumber - 1] = true;
    bool isTerminal = true;
    for (unsigned int room = 0; room < temp.roomsVisited.size(); room++)
        if (temp.roomsVisited[room] == false)
            isTerminal = false;

    temp.isTerminal = isTerminal;

    // check if the room have already been visited
    bool visited = false;

    if (visitedRooms[s.RoomNumber - 1])
        visited = true;

    //for (unsigned int room = 0; room < roomsOrder.size(); room++)
        //if (s.RoomNumber == roomsOrder[room])
            //visited = true;

    // if the room have not been visited previously
    if (!visited)
    {
        visitedRooms[s.RoomNumber - 1] = true;
        if (!isTerminal)
        {
            matrixOrder.push_back(temp.roomsVisited);
            makeNewStateMatrix();
            makeNewQMatrix();
        }
    }

    return temp;
}

ct::newState q_learning::getNextState(ct::newState s, int a)
{
    float reward = getReward(s,a);
    if (reward == ACTION_NOT_ABLE)
        return s;

    ct::newState next = s;
    next.RoomNumber = a;
    return next;
}

float q_learning::getReward(ct::newState s, int a)
{
    return stateMatrix[findMatrixIndex(s)][s.RoomNumber][a];
}

int q_learning::getNextAction(ct::newState s)
{
    std::vector<int> possibleActions;
    for (unsigned int action = 0; action < baseStateMatrix.size(); action++)
        possibleActions.push_back(action);

    float currentMaxValue = -std::numeric_limits<float>::min();
    std::vector<float> maxValues;
    std::vector<int> actions;

    int bestAction;
    int matrixIndex = findMatrixIndex(s);
    std::cout << "get next action matrix index: " << matrixIndex << std::endl;
    for (unsigned int action = 0; action < possibleActions.size(); action++)
    {
        if (qValues[matrixIndex][s.RoomNumber][action] > currentMaxValue && s.RoomNumber != action)
        {
            maxValues.clear();
            actions.clear();
            maxValues.push_back(qValues[matrixIndex][s.RoomNumber][action]);
            actions.push_back(possibleActions[action]);
            currentMaxValue = qValues[matrixIndex][s.RoomNumber][action];
            bestAction = possibleActions[action];
        }
        else if (maxValues.size() != 0 && s.RoomNumber != action)
        {
            if (qValues[matrixIndex][s.RoomNumber][action] == maxValues[maxValues.size() - 1])
            {
                maxValues.push_back(qValues[matrixIndex][s.RoomNumber][action]);
                actions.push_back(possibleActions[action]);
            }
        }
    }

    if (maxValues.size() > 0) // More than one action with the same value
    {
        int index = getRandomIndex(actions.size());
        bestAction = actions[index];
    }

    std::cout << "action get next action: " << bestAction << std::endl;
    return bestAction;
}

int q_learning::eGreedyPolicy(ct::newState s, float epsilon)
{
    std::vector<int> possibleActions;
    for (unsigned int action = 0; action < baseStateMatrix.size(); action++)
    {
        //if (s.RoomNumber != signed(action))
            possibleActions.push_back(action);
    }

    if (getRandom(0, 1) < epsilon)
    {
        std::cout << "random action" << std::endl;
        return possibleActions[getRandomIndex(possibleActions.size())];
    }
    else
        return getNextAction(s);
}

float q_learning::maxQValue(ct::newState s)
{
    float currentMaxValue = -std::numeric_limits<float>::min();
    int matrixIndex = findMatrixIndex(s);

    for (unsigned int action = 0; action < baseStateMatrix.size(); action++)
    {
        if (qValues[matrixIndex][s.RoomNumber][action] > currentMaxValue)
            currentMaxValue = qValues[matrixIndex][s.RoomNumber][action];
    }

    return currentMaxValue;
}

ct::newState q_learning::qUpdate(ct::newState s, float alpha, float gamma, float epsilon)
{
    int a = eGreedyPolicy(s, epsilon);
    //std::cout << "action: " << a << std::endl;
    float reward = getReward(s, a);
    ct::newState next = getNextState(s, a);
    std::cout << "next state: " << next.RoomNumber << "   " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;
    int matrixIndex = findMatrixIndex(s);
    //std::cout << "matrix index qUpdate: " << matrixIndex << std::endl;
    qValues[matrixIndex][s.RoomNumber][a] += alpha * (reward + gamma * maxQValue(next) - qValues[matrixIndex][s.RoomNumber][a]);

    if (next.roomsVisited[next.RoomNumber] == false)
        next = visitRoom(next);

    std::cout << "next state: " << next.RoomNumber << "   " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;
    return next;
}

void q_learning::doEpisode(ct::newState start, float alpha, float gamma, float epsilon)
{
    int index = 0;
    bool isTerminal = false;
    ct::newState s = start;
    //while (!isTerminal)
    while (index < 200 && !isTerminal)
    {
        //std::cout << "hmm" << std::endl;
        s = qUpdate(s, alpha, gamma, epsilon);
        if (s.isTerminal)
            isTerminal = true;

        //std::cout << "is terminal: " << isTerminal << std::endl;
        std::cout << index++ << std::endl;
    }
}

// Private methods
int q_learning::findMatrixIndex(ct::newState s)
{
    int matrixIndex = 0;
    for (unsigned int matrix = 0; matrix < matrixOrder.size(); matrix++)
    {
        bool allEqual = true;
        for (int room = 0; room < matrixOrder[matrix].size(); room++)
        {
            if (s.roomsVisited[room] != matrixOrder[matrix][room])
                allEqual = false;
        }
        if (allEqual)
            matrixIndex = matrix;
    }
    //std::cout << "matrix index: " << matrixIndex << std::endl;
    return matrixIndex;
}

float q_learning::getRandom(int min, int max)
{
    std::uniform_real_distribution<double> unif(min, max);
    float randomNumb = unif(rng);
    return randomNumb;
}

int q_learning::getRandomIndex(int size)
{
    std::uniform_int_distribution<int> unif(0,size - 1);
    int randomNumb = unif(rng);
    return randomNumb;
}
