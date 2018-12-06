#include "q_learning.h"

q_learning::q_learning(int numberOfRooms)
{
    // Initialise the base state matrix to 0
    std::vector<std::vector<float>> tempMatrix;
    for (int i = 0; i < numberOfRooms + 1; i++)
    {
        std::vector<float> tempVector;
        for (int j = 0; j < numberOfRooms + 1; j++)
            tempVector.push_back(0.0);

        tempMatrix.push_back(tempVector);
    }
    baseStateMatrix = tempMatrix;

    // Set the base state matrix to no actions available
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

    // Initialise order of rooms
    std::vector<bool> temp;
    for (int i = 0; i < numberOfRooms; i++)
        temp.push_back(false);
    stateMatrixOrder.push_back(temp);

    // Initialise 2^rooms - 1 rooms+1 x rooms+1 matrices
    for (int i = 0; i < std::pow(2,numberOfRooms) - 1; i++)
        makeNewQMatrix();

    // Initialise order of Q-matrices
    qMatrixOrder.push_back(temp);
    std::vector<bool> qIndex = temp;
    for (int i = 0; i < std::pow(2,numberOfRooms) - 2; i++)
    {
        qIndex = binaryIncrement(qIndex);
        qMatrixOrder.push_back(qIndex);
    }

    // Initialise rewards and visited rooms to 0
    for (int i = 0; i < numberOfRooms; i++)
    {
        visitedRooms.push_back(false);
        rewards.push_back(0.0);
    }

    // Initialise time based seed for random generator
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
        std::cout << "State combinations: ";
        for (unsigned int j = 0; j < qMatrixOrder[i].size(); j++)
            std::cout << qMatrixOrder[i][j];
        std::cout << std::endl;
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
    if (s.RoomNumber == 0) // initial state - no change
        return s;

    // get the new state
    ct::newState temp = s;
    temp.roomsVisited[temp.RoomNumber - 1] = true;
    bool isTerminal = true;
    for (unsigned int room = 0; room < temp.roomsVisited.size(); room++)
        if (temp.roomsVisited[room] == false)
            isTerminal = false;

    temp.isTerminal = isTerminal;

    // check if the room have already been visited
    bool visited = false;

    if (visitedRooms[s.RoomNumber - 1])
        visited = true;

    // if the room have not been visited previously
    if (!visited)
    {
        visitedRooms[s.RoomNumber - 1] = true;
        if (!isTerminal)
        {
            stateMatrixOrder.push_back(temp.roomsVisited);
            makeNewStateMatrix();
            //makeNewQMatrix();
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
    return stateMatrix[findStateMatrixIndex(s)][s.RoomNumber][a];
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
    int matrixIndex = findQMatrixIndex(s);
    for (unsigned int action = 0; action < possibleActions.size(); action++)
    {
        if (qValues[matrixIndex][s.RoomNumber][action] > currentMaxValue && s.RoomNumber != signed(action))
        {
            maxValues.clear();
            actions.clear();
            maxValues.push_back(qValues[matrixIndex][s.RoomNumber][action]);
            actions.push_back(possibleActions[action]);
            currentMaxValue = qValues[matrixIndex][s.RoomNumber][action];
            bestAction = possibleActions[action];
        }
        else if (maxValues.size() != 0 && s.RoomNumber != signed(action))
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
    return bestAction;
}

int q_learning::eGreedyPolicy(ct::newState s, float epsilon)
{
    std::vector<int> possibleActions;
    for (unsigned int action = 0; action < baseStateMatrix.size(); action++)
        possibleActions.push_back(action);

    if (getRandom(0, 1) < epsilon)
        return possibleActions[getRandomIndex(possibleActions.size())];
    else
        return getNextAction(s);
}

float q_learning::maxQValue(ct::newState s)
{
    float currentMaxValue = -std::numeric_limits<float>::min();
    int matrixIndex = findQMatrixIndex(s);

    for (unsigned int action = 0; action < baseStateMatrix.size(); action++)
        if (qValues[matrixIndex][s.RoomNumber][action] > currentMaxValue)
            currentMaxValue = qValues[matrixIndex][s.RoomNumber][action];

    return currentMaxValue;
}

ct::newState q_learning::qUpdate(ct::newState s, float alpha, float gamma, float epsilon)
{
    int a = eGreedyPolicy(s, epsilon);
    //std::cout << "action: " << a << std::endl;
    float reward = getReward(s, a);
    ct::newState next = getNextState(s, a);
    std::cout << "next state: " << next.RoomNumber << "   " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;
    int matrixIndex = findQMatrixIndex(s);
    std::cout << "matrix index qUpdate: " << matrixIndex << std::endl;
    qValues[matrixIndex][s.RoomNumber][a] += alpha * (reward + gamma * maxQValue(next) - qValues[matrixIndex][s.RoomNumber][a]);

    if (next.RoomNumber != 0)
        next = visitRoom(next);

    //std::cout << "next state: " << next.RoomNumber << "   " << next.roomsVisited[0] << ", " << next.roomsVisited[1] << std::endl;
    return next;
}

void q_learning::doEpisode(ct::newState start, float alpha, float gamma, float epsilon)
{
    stateMatrix.clear();
    for (unsigned int room = 0; room < visitedRooms.size(); room++)
        visitedRooms[room] = false;

    stateMatrixOrder.clear();
    stateMatrixOrder.push_back(visitedRooms);
    makeNewStateMatrix();

    int index = 0;
    bool isTerminal = false;
    ct::newState s = start;
    while (!isTerminal)
    {
        s = qUpdate(s, alpha, gamma, epsilon);
        if (s.isTerminal)
            isTerminal = true;
        index++;
    }
    std::cout << "  episode iteration number " << index << std::endl;
}

std::vector<int> q_learning::getPath(ct::newState start)
{
    stateMatrix.clear();
    for (unsigned int room = 0; room < visitedRooms.size(); room++)
        visitedRooms[room] = false;

    stateMatrixOrder.clear();
    stateMatrixOrder.push_back(visitedRooms);
    makeNewStateMatrix();

    bool isTerminal = false;
    ct::newState s = start;

    std::vector<int> path;
    path.push_back(s.RoomNumber);
    while (!isTerminal)
    {
        s = qUpdate(s, 0.0, 1.0, 0.0);
        path.push_back(s.RoomNumber);
        if (s.isTerminal)
            isTerminal = true;
    }
    return path;
}

// Private methods
int q_learning::findStateMatrixIndex(ct::newState s)
{
    int matrixIndex = 0;
    for (unsigned int matrix = 0; matrix < stateMatrixOrder.size(); matrix++)
    {
        bool allEqual = true;
        for (unsigned int room = 0; room < stateMatrixOrder[matrix].size(); room++)
        {
            if (s.roomsVisited[room] != stateMatrixOrder[matrix][room])
                allEqual = false;
        }
        if (allEqual)
            matrixIndex = matrix;
    }
    return matrixIndex;
}

int q_learning::findQMatrixIndex(ct::newState s)
{
    int matrixIndex = 0;
    for (unsigned int matrix = 0; matrix < qMatrixOrder.size(); matrix++)
    {
        bool allEqual = true;
        for (unsigned int room = 0; room < qMatrixOrder[matrix].size(); room++)
        {
            if (s.roomsVisited[room] != qMatrixOrder[matrix][room])
                allEqual = false;
        }
        if (allEqual)
            matrixIndex = matrix;
    }

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

std::vector<bool> q_learning::binaryIncrement(std::vector<bool> numb)
{
    bool carry = false;
    std::vector<bool> temp = numb;

    if (temp[temp.size() - 1])
    {
        carry = true;
        temp[temp.size() - 1] = false;
    }
    else
    {
        carry = false;
        temp[temp.size() - 1] = true;
    }

    for (int i = signed(temp.size()) - 2; i >= 0 && carry; i--)
    {
        if (temp[i])
        {
            carry = true;
            temp[i] = false;
        }
        else
        {
            carry = false;
            temp[i] = true;
        }
    }

    return temp;
}
