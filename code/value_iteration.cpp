#include "value_iteration.h"

value_iteration::value_iteration(map_class map)
{
    stateMatrix = map.genStateMap(FREE_SPACE,WALL);
    centerOfMassRooms = map.getRooms();

    stateValueEstimates = stateMatrix;
    for (unsigned int row = 0; row < stateValueEstimates.size(); row++)
        for (unsigned int col = 0; col < stateValueEstimates[row].size(); col++)
            stateValueEstimates[row][col] = 0.0;

    width = stateMatrix[0].size();
    height = stateMatrix.size();

    imageValues = cv::Mat(height, width, CV_8UC3);
    imageValues.setTo(0);

    imagePolicy = cv::Mat(height, width, CV_8UC3);
    imagePolicy.setTo(0);
}

void value_iteration::setReward(int numberOfTests, int numberOfRuns)
{
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
    for (unsigned int i = 0; i < probabilityMatrix[0].size(); i++) // rooms
    {
        float sum = 0.0;
        for (unsigned int j = 0; j < probabilityMatrix.size(); j++) // runs
            sum += probabilityMatrix[j][i];

        sum /= probabilityMatrix.size();
        averageProbability.push_back(sum);
        std::cout << "Room" << std::setw(3) << i + 1 << ": " << std::setw(10) << sum << std::endl;
    }

    // Find max value
    float max = 0.0;
    for (unsigned int room = 0; room < averageProbability.size(); room++)
        if (averageProbability[room] > max)
            max = averageProbability[room];

    // Normalise with max
    for (unsigned int room = 0; room < averageProbability.size(); room++)
    {
        cv::Point point = centerOfMassRooms[room].centerOfMass;
        stateMatrix[point.y][point.x] = averageProbability[room] / max;
    }
}

ct::state value_iteration::getNextState(ct::state s, ct::action a)
{
    const ct::state TERMINAL_STATE = { -1, -1, true };
    if (stateMatrix[s.y][s.x] != FREE_SPACE)
        return TERMINAL_STATE;

    switch (a)
    {
    case ct::UP:     s.y -= 1; break;
    case ct::DOWN:   s.y += 1; break;
    case ct::LEFT:   s.x -= 1; break;
    case ct::RIGHT:  s.x += 1; break;
    }

    if (s.x < 0 || s.y < 0 || s.x >= width || s.y >= height)
        return TERMINAL_STATE;

    s.isOutsideEnvironment = false;
    return s;
}

float value_iteration::getReward(ct::state s, ct::action a)
{
    ct::state next = getNextState(s, a);
    if (next.isOutsideEnvironment)
        return 0.0;
    else
    {
        if ((stateMatrix[next.y][next.x] != FREE_SPACE) && (stateMatrix[next.y][next.x] != WALL))
            return stateMatrix[next.y][next.x];

        return 0.0;
    }
}

ct::action value_iteration::getNextAction(ct::state s)
{
    std::vector<ct::action> possibleActions = { ct::UP, ct::DOWN, ct::LEFT, ct::RIGHT };
    float currentMaxValue = std::numeric_limits<float>::min();
    ct::action bestAction = possibleActions[0]; // Make sure to have a default action

    for (const auto a : possibleActions)
    {
        ct::state next = getNextState(s, a);
        float reward = getReward(s, a);
        if (!next.isOutsideEnvironment)
        {
            if (stateValueEstimates[next.y][next.x] + reward > currentMaxValue)
            {
                bestAction = a;
                currentMaxValue = stateValueEstimates[next.y][next.x] + reward;
            }
        }
    }
    return bestAction;

}

void value_iteration::resetReward(ct::state s)
{
    // Reset the reward
    for (unsigned int room = 0; room < averageProbability.size(); room++)
    {
        cv::Point point = centerOfMassRooms[room].centerOfMass;
        if ((point.x == s.x) && (point.y == s.y))
            averageProbability[room] = 0.0;
    }

    // Find the new max value
    float max = 0.0;
    for (unsigned int room = 0; room < averageProbability.size(); room++)
        if (averageProbability[room] > max)
            max = averageProbability[room];

    // Normalise
    for (unsigned int room = 0; room < averageProbability.size(); room++)
    {
        cv::Point point = centerOfMassRooms[room].centerOfMass;
        if (averageProbability[room] != 0.0)
            stateMatrix[point.y][point.x] = averageProbability[room] / max;
        else
            stateMatrix[point.y][point.x] = FREE_SPACE;
    }
}

void value_iteration::deleteMaxReward()
{
    // Find the max value
    float max = 0.0;
    for (unsigned int room = 0; room < averageProbability.size(); room++)
        if (averageProbability[room] > max)
            max = averageProbability[room];

    // Reset the max value
    for (unsigned int room = 0; room < averageProbability.size(); room++)
        if (averageProbability[room] == max)
            averageProbability[room] = 0.0;

    // Find the new max value
    max = 0.0;
    for (unsigned int room = 0; room < averageProbability.size(); room++)
        if (averageProbability[room] > max)
            max = averageProbability[room];

    // Normalise
    for (unsigned int room = 0; room < averageProbability.size(); room++)
    {
        cv::Point point = centerOfMassRooms[room].centerOfMass;
        if (averageProbability[room] != 0.0)
            stateMatrix[point.y][point.x] = averageProbability[room] / max;
        else
            stateMatrix[point.y][point.x] = FREE_SPACE;
    }
}

float value_iteration::performFullSweep()
{
    float delta = 0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            ct::state s = { x, y, false };
            if (stateMatrix[y][x] == FREE_SPACE)
            {
                float value = stateValueEstimates[y][x];
                ct::action a = getNextAction(s);
                float reward = getReward(s, a);
                ct::state next = getNextState(s, a);
                if (!next.isOutsideEnvironment)
                    stateValueEstimates[y][x] = reward + discountRate * stateValueEstimates[next.y][next.x];

                delta = std::max(delta, (float)fabs(value - stateValueEstimates[y][x]));
            }
        }
    }
    return delta;
}

float value_iteration::doEstimation(float theta)
{
    int sweep = 0;
    float delta = 0;
    do
    {
        delta = performFullSweep();
        sweep++;
    } while (delta > theta);
    std::cout << "last delta: "<< delta << std::endl;
    return sweep;
}

std::vector<ct::state> value_iteration::getPath(ct::state startState)
{
    std::vector<ct::state> path;
    ct::state curr = startState;
    path.push_back(startState);
    while(!curr.isOutsideEnvironment)
    {
        ct::action a = getNextAction(curr);
        ct::state next = getNextState(curr,a);
        if (!next.isOutsideEnvironment)
            path.push_back(next);

        curr = next;
    }
    return path;
}

void value_iteration::paintValueEstimates()
{
    // Paint the state value estimates
    for (unsigned int row = 0; row < stateValueEstimates.size(); row++)
    {
        for (unsigned int col = 0; col < stateValueEstimates[row].size(); col++)
        {
            if (stateValueEstimates[row][col] < 0.5) // red to yellow
            {
                int green = stateValueEstimates[row][col] * (255 / 0.5);
                *imageValues.ptr<cv::Vec3b>(row,col) = {0, green, 255};
            }
            else // yellow to green
            {
                int red = (1 - stateValueEstimates[row][col]) * (255 / 0.5);
                *imageValues.ptr<cv::Vec3b>(row,col) = {0, 255, red};
            }
        }
    }

    // Paint the walls
    paintWalls();

    // Paint center of mass
    paintCenterOfMass();

    // Paint the outside the environment
    paintOutsideEnvironment();
}

void value_iteration::paintPolicy()
{
    // Colours for painting the states
    cv::Vec3b up({ 0, 0, 255 }); // red
    cv::Vec3b down({ 0, 255, 0 }); // green
    cv::Vec3b right({ 255, 255, 0 }); // cyan
    cv::Vec3b left({ 255, 0, 255 }); // magenta

    // Paint the policy
    for (unsigned int row = 0; row < stateMatrix.size(); row++)
    {
        for (unsigned int col = 0; col < stateMatrix[row].size(); col++)
        {
            if (stateMatrix[row][col] == FREE_SPACE)
            {
                ct::action a = getNextAction(ct::state({col, row, false}));

                switch (a) {
                case ct::UP:
                    *imagePolicy.ptr<cv::Vec3b>(row,col) = up;
                    break;
                case ct::DOWN:
                    *imagePolicy.ptr<cv::Vec3b>(row,col) = down;
                    break;
                case ct::LEFT:
                    *imagePolicy.ptr<cv::Vec3b>(row,col) = left;
                    break;
                case ct::RIGHT:
                    *imagePolicy.ptr<cv::Vec3b>(row,col) = right;
                    break;
                default:
                    *imagePolicy.ptr<cv::Vec3b>(row,col) = {255, 255, 255};
                    break;
                }
            }
        }
    }

    // Paint the walls
    paintWalls();

    // Paint center of mass
    paintCenterOfMass();

    // Paint the outside the environment
    paintOutsideEnvironment();
}

void value_iteration::showValueEstimates(std::string name)
{
    cv::imshow(name,imageValues);
}

void value_iteration::showPolicy(std::string name)
{
    cv::imshow(name,imagePolicy);
}

void value_iteration::scaleImage(int factor)
{
    int newWidth = width * factor;
    int newHeight = height * factor;

    cv::Mat tempImageOne = cv::Mat(newHeight, newWidth, CV_8UC3);
    tempImageOne.setTo(0);

    cv::Mat tempImageTwo = cv::Mat(newHeight, newWidth, CV_8UC3);
    tempImageTwo.setTo(0);

    for (int row = 0; row < height; row++)
    {
        for (int col = 0; col < width; col++)
        {
            cv::Vec3b colorOne = *imageValues.ptr<cv::Vec3b>(row,col);
            cv::Vec3b colorTwo = *imagePolicy.ptr<cv::Vec3b>(row,col);
            for (int newRow = row * factor; newRow < row * factor + factor; newRow++)
            {
                for (int newCol = col * factor; newCol < col * factor + factor; newCol++)
                {
                    *tempImageOne.ptr<cv::Vec3b>(newRow,newCol) = colorOne;
                    *tempImageTwo.ptr<cv::Vec3b>(newRow,newCol) = colorTwo;
                }
            }
        }
    }
    imageValues = tempImageOne.clone();
    imagePolicy = tempImageTwo.clone();
}

void value_iteration::paintWalls()
{
    // Paint the walls
    for (unsigned int row = 0; row < stateMatrix.size(); row++)
    {
        for (unsigned int col = 0; col < stateMatrix[row].size(); col++)
        {
            if (stateMatrix[row][col] == WALL)
            {
                *imageValues.ptr<cv::Vec3b>(row,col) = {0, 0, 0};
                *imagePolicy.ptr<cv::Vec3b>(row,col) = {0, 0, 0};
            }
        }
    }
}

void value_iteration::paintCenterOfMass()
{
    for (unsigned int room = 0; room < centerOfMassRooms.size(); room++)
    {
        if (averageProbability[room] != 0.0)
        {
            cv::Point point = centerOfMassRooms[room].centerOfMass;
            *imageValues.ptr<cv::Vec3b>(point.y,point.x) = {255, 0, 0};
            *imagePolicy.ptr<cv::Vec3b>(point.y,point.x) = {255, 0, 0};
        }
    }
}

void value_iteration::paintOutsideEnvironment()
{
    std::vector<cv::Point> outsideEnvironment;
    //= {cv::Point(17,0), cv::Point(18,0), cv::Point(19,0),
      //  cv::Point(20,0), cv::Point(21,0), cv::Point(22,0), cv::Point(23,0), cv::Point(31,0)};
    for (int i = 17; i < 24; i++)
        outsideEnvironment.push_back(cv::Point(i,0));

    for (int i = 31; i < 53; i++)
        outsideEnvironment.push_back(cv::Point(i,0));

    for (int i = 85; i < 101; i++)
    {
        outsideEnvironment.push_back(cv::Point(i,0));
        outsideEnvironment.push_back(cv::Point(i,1));
        outsideEnvironment.push_back(cv::Point(i,2));
        outsideEnvironment.push_back(cv::Point(i,3));
    }

    for (int i = 27; i < 41; i++)
        outsideEnvironment.push_back(cv::Point(i,79));

    for (int i = 65; i < 77; i++)
        outsideEnvironment.push_back(cv::Point(i,79));

    for (int i = 117; i < 120; i++)
        outsideEnvironment.push_back(cv::Point(i,79));

    for (int i = 35; i < 48; i++)
        outsideEnvironment.push_back(cv::Point(0,i));

    for (int i = 32; i < 64; i++)
        outsideEnvironment.push_back(cv::Point(119,i));

    for (unsigned int i = 0; i < outsideEnvironment.size(); i++)
    {
        *imageValues.ptr<cv::Vec3b>(outsideEnvironment[i].y,outsideEnvironment[i].x) = {255, 255, 255};
        *imagePolicy.ptr<cv::Vec3b>(outsideEnvironment[i].y,outsideEnvironment[i].x) = {255, 255, 255};
    }
}

void value_iteration::saveImage(int testNumb, int runNumb, int sweepNumb)
{
    QDir path = QDir::current();
    path.cdUp();
    std::string fileNameOne = std::to_string(testNumb) + "/Policy" + std::to_string(testNumb) + "." + std::to_string(runNumb) + "." + std::to_string(sweepNumb) + ".png";
    std::string fileNameTwo = std::to_string(testNumb) + "/Value_estimates" + std::to_string(testNumb) + "." + std::to_string(runNumb) + "." + std::to_string(sweepNumb) + ".png";
    std::string filePathOne = path.path().toStdString() + "/test_files/q_learning/test" + fileNameOne;
    std::string filePathTwo = path.path().toStdString() + "/test_files/q_learning/test" + fileNameTwo;
    cv::imwrite(filePathOne, imagePolicy);
    cv::imwrite(filePathTwo, imageValues);
}
