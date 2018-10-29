#include "dataloggin.h"

dataloggin::dataloggin(std::string afileName, int runNumb, char type)
{
    QDir path = QDir::current();
    path.cdUp();

    switch (type) {
    case 'm': // marbles
        filePath = path.path().toStdString() + FILEPATH_MARBLES + afileName + ".txt";
        if (read().size() == 0)
        {
            write("Description: Contains information abount location of found marbles");
            std::string temp = "Run number: " + std::to_string(runNumb);
            write(temp);
        }
        break;
    case 's': // statistics
        filePath = path.path().toStdString() + FILEPATH_STATS + afileName + ".txt";
        break;
    case 'r': // room center of mass
        filePath = path.path().toStdString() + FILEPATH_ROOM + afileName + ".txt";
        break;
    default:
        break;
    }
}

void dataloggin::write(std::string text)
{
    std::vector<std::string> buffer = read();
    buffer.push_back(text);

    std::ofstream textFileOut(filePath);
    if (textFileOut.is_open())
    {
        for (unsigned int i = 0; i < buffer.size(); i++)
        {
            textFileOut << buffer[i] << "\n";
        }
        textFileOut.close();
    }
    else std::cerr << "Unable to open file" << std::endl;
}

std::vector<std::string> dataloggin::read()
{
    std::vector<std::string> buffer;
    std::string inputLine;
    std::ifstream textFileIn(filePath);
    if (textFileIn.is_open())
    {
        while (std::getline(textFileIn, inputLine))
            buffer.push_back(inputLine);

        textFileIn.close();
    }
    else
    {
        if (firstTimeCall)
        {
            erase();
            firstTimeCall = false;
            read();
        }
        else
            std::cerr << "Unable to open file";
    }

    return buffer;
}

void dataloggin::erase()
{
    std::ofstream textFile;
    textFile.open(filePath);
    textFile.close();
}
