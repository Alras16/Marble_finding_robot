#include "dataloggin.h"

dataloggin::dataloggin(std::string aBaseName, int testNumb, int runNumb, char type)
{
    QDir path = QDir::current();
    path.cdUp();

    std::string temp;
    switch (type) {
    case MARBLES: // marbles
        if (aBaseName != "MarblesRunRaw")
            filePath = path.path().toStdString() + FILEPATH_MARBLES + std::to_string(testNumb) + "/" + aBaseName;
        else
            filePath = path.path().toStdString() + FILEPATH_MARBLES + std::to_string(testNumb) + "/" + "raw/" + aBaseName;
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
    case STATS: // statistics
        filePath = path.path().toStdString() + FILEPATH_STATS + aBaseName;
        filePath += std::to_string(testNumb) + "." + std::to_string(runNumb) + ".txt";
        if (read().size() == 0)
        {
            write("Description: Contains information abount the statistics of the location of found marbles");
            temp = "Test number: " + std::to_string(testNumb);
            write(temp);
            temp = "Run number: " + std::to_string(runNumb);
            write(temp);
        }
        break;
    case ROOMS: // room center of mass
        filePath = path.path().toStdString() + FILEPATH_ROOM + aBaseName;
        filePath += std::to_string(testNumb) + "." + std::to_string(runNumb) + ".txt";
        if (read().size() == 0)
        {
            write("Description: Contains information abount center of mass of rooms");
            temp = "Test number: " + std::to_string(testNumb);
            write(temp);
            temp = "Run number: " + std::to_string(runNumb);
            write(temp);
        }
        break;
    default:
        break;
    }
}

/*************** Public methods ***************/
void dataloggin::writeMarble(ct::foundMarble marble, int numb)
{
    std::string tempOne;
    std::string tempTwo;
    tempOne = "MARBLE FOUND:: Count: ";
    tempTwo = "0" + std::to_string(numb);
    if (tempTwo.length() != 2)
        tempTwo.erase(tempTwo.begin());
    tempOne += tempTwo + "; Center: (";
    tempOne += std::to_string(marble.fMarble.center.x) + ",";
    tempOne += std::to_string(marble.fMarble.center.y) + "); Room: ";
    tempOne += std::to_string(marble.foundInRoom->roomNumber);
    write(tempOne);
}

std::vector<ct::foundMarble> dataloggin::readMarbles()
{
    std::vector<ct::foundMarble> marbles;
    std::vector<std::string> input = read();
    input.erase(input.begin(),input.begin() + 3);
    for (unsigned int i = 0; i < input.size(); i++)
    {
        ct::foundMarble tempMarble;
        ct::room *tempRoom = new ct::room;
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
        tempMarble.fMarble.center.x = std::stoi(tempTwo);
        tempTwo.clear();
        j++;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ')')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempMarble.fMarble.center.y = std::stoi(tempTwo);
        j += 9;
        tempOne.erase(tempOne.begin(),tempOne.begin() + j);
        tempMarble.foundInRoom->roomNumber = std::stoi(tempOne);
        marbles.push_back(tempMarble);
    }
    return marbles;
}

std::vector<ct::point> dataloggin::readMarblesRaw()
{
    std::vector<ct::point> marbles;
    std::vector<std::string> input = read();
    input.erase(input.begin(),input.begin() + 3);
    for (unsigned int i = 0; i < input.size(); i++)
    {
        ct::point tempMarble;

        std::string tempOne = input[i];
        unsigned int j = 0;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] == '(')
                break;
        }

        j++;
        std::string tempTwo;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ';')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempMarble.x = std::stof(tempTwo);
        tempTwo.clear();

        j++;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ')')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempMarble.y = std::stoi(tempTwo);
        marbles.push_back(tempMarble);
    }
    return marbles;
}

void dataloggin::writeStats(int roomNumb ,float probability)
{
    std::string tempOne;
    std::string tempTwo;
    tempOne = "ROOM:: Number: ";
    tempTwo = "0" + std::to_string(roomNumb);
    if (tempTwo.length() != 2)
        tempTwo.erase(tempTwo.begin());
    tempOne += tempTwo + "; Probability of marble: ";
    tempOne += std::to_string(probability);
    write(tempOne);
}

std::vector<float> dataloggin::readStats()
{
    std::vector<float> probabilities;
    std::vector<std::string> input = read();
    input.erase(input.begin(),input.begin() + 3);
    for (unsigned int i = 0; i < input.size(); i++)
    {
        float tempProbability;
        std::string tempOne = input[i];
        tempOne.erase(0,42);
        tempProbability = std::stof(tempOne);
        probabilities.push_back(tempProbability);
    }
    return probabilities;
}

void dataloggin::writeRoom(int roomNumb, cv::Point massCenter, int numbOfPixels)
{
    std::string tempOne;
    std::string tempTwo;
    tempOne = "ROOM:: Number: ";
    tempTwo = "0" + std::to_string(roomNumb);
    if (tempTwo.length() != 2)
        tempTwo.erase(tempTwo.begin());
    tempOne += tempTwo + "; Center of mass: (";
    tempOne += std::to_string(massCenter.x) + ",";
    tempOne += std::to_string(massCenter.y) + "); Number of pixels in room: ";
    tempOne += std::to_string(numbOfPixels);
    write(tempOne);
}

std::vector<ct::room> dataloggin::readRooms()
{
    std::vector<ct::room> rooms;
    std::vector<std::string> input = read();
    input.erase(input.begin(),input.begin() + 3);
    for (unsigned int i = 0; i < input.size(); i++)
    {
        ct::room tempRoom;

        std::string tempOne = input[i];
        std::string tempTwo;
        tempOne.erase(0,15);
        unsigned int j = 0;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ';')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempRoom.roomNumber = std::stoi(tempTwo);

        tempTwo.clear();
        j += 19;
        cv::Point tempPoint;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ',')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempPoint.x = std::stoi(tempTwo);

        tempTwo.clear();
        j++;
        for (; j < tempOne.length(); j++)
        {
            if (tempOne[j] != ')')
                tempTwo += tempOne[j];
            else
                break;
        }
        tempPoint.y = std::stoi(tempTwo);
        tempRoom.centerOfMass = tempPoint;

        j += 29;
        tempOne.erase(tempOne.begin(), tempOne.begin() + j);
        tempRoom.numbOfPixels = std::stoi(tempOne);
        rooms.push_back(tempRoom);
    }
    return rooms;
}

/*************** Private methods ***************/
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

