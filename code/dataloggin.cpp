#include "dataloggin.h"

dataloggin::dataloggin(std::string aBaseName, int testNumb, int runNumb, char type)
{
    QDir path = QDir::current();
    path.cdUp();

    std::string temp;
    switch (type) {
    case 'm': // marbles
        filePath = path.path().toStdString() + FILEPATH_MARBLES + aBaseName;
        filePath += std::to_string(testNumb) + "." + std::to_string(runNumb) + ".txt";
        if (read().size() == 0)
        {
            write("Description: Contains information abount location of found marbles");
            temp = "Test number: " + std::to_string(testNumb);
            write(temp);
            temp = "Run number: " + std::to_string(runNumb);
            write(temp);
        }
        break;
    case 's': // statistics
        filePath = path.path().toStdString() + FILEPATH_STATS + aBaseName;
        filePath += std::to_string(testNumb) + "." + std::to_string(runNumb) + ".txt";
        write("Description: Contains information abount the statistics of the location of found marbles");
        temp = "Test number: " + std::to_string(testNumb);
        write(temp);
        temp = "Run number: " + std::to_string(runNumb);
        write(temp);
        break;
    case 'r': // room center of mass
        filePath = path.path().toStdString() + FILEPATH_ROOM + aBaseName;
        filePath += std::to_string(testNumb) + "." + std::to_string(runNumb) + ".txt";
        write("Description: Contains information abount center of mass of rooms");
        temp = "Test number: " + std::to_string(testNumb);
        write(temp);
        temp = "Run number: " + std::to_string(runNumb);
        write(temp);
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

void dataloggin::writeMarble(map_class::foundMarble marble, int numb)
{
    std::string tempOne;
    std::string tempTwo;
    tempOne = "MARBLE FOUND:: Count: ";
    tempTwo = "0" + std::to_string(numb);
    if (tempTwo.length() != 2)
        tempTwo.erase(tempTwo.begin());
    tempOne += tempTwo + "; Center: (";
    tempOne += std::to_string(marble.center.x) + ",";
    tempOne += std::to_string(marble.center.y) + "); Room: ";
    tempOne += std::to_string(marble.foundInRoom->roomNumber);
    write(tempOne);
}

std::vector<map_class::foundMarble> dataloggin::readMarbles()
{
    std::vector<map_class::foundMarble> marbles;
    std::vector<std::string> input = read();
    input.erase(input.begin(),input.begin() + 3);
    for (unsigned int i = 0; i < input.size(); i++)
    {
        map_class::foundMarble tempMarble;
        map_class::room *tempRoom = new map_class::room;
        tempMarble.foundInRoom = tempRoom;
        std::string tempOne = input[i];
        std::string tempTwo;
        tempOne.erase(0,35);
        unsigned int j = 0;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ',')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempMarble.center.x = std::stoi(tempTwo);
        tempTwo.clear();
        j++;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ')')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempMarble.center.y = std::stoi(tempTwo);
        j += 9;
        tempOne.erase(tempOne.begin(),tempOne.begin() + j);
        tempMarble.foundInRoom->roomNumber = std::stoi(tempOne);
        marbles.push_back(tempMarble);
    }
    return marbles;
}




