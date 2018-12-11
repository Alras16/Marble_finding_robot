#include "dataloggin.h"
#include "value_iteration.h"

int main(int _argc, char **_argv)
{
    int numberOfTests = 5;
    int numberOfRuns = 10;

    std::vector<float> averageProbability;
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

    // Print probabilities
    for (unsigned int room = 0; room < averageProbability.size(); room++)
    {
        std::cout << "Probability of a marble accuring in room " << room + 1 << ": " << averageProbability[room]*100 << std::endl;
    }
}
