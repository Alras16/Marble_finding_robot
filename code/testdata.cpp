#include "testdata.h"

testData::testData(std::string afileName)
{
    fileName = afileName + ".txt";
}


void testData::write(std::string text)
{
    std::vector<std::string> buffer = read();
    buffer.push_back(text);

    QDir path = QDir::current();
    path.cdUp();
    std::string filePath = path.path().toStdString() + "/test_files/lidar_test_files/" + fileName;
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

std::vector<std::string> testData::read()
{
    std::vector<std::string> buffer;
    std::string inputLine;
    QDir path = QDir::current();
    path.cdUp();
    std::string filePath = path.path().toStdString() + "/test_files/lidar_test_files/" + fileName;
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

void testData::erase()
{
    std::ofstream textFile;
    QDir path = QDir::current();
    path.cdUp();
    std::string filePath = path.path().toStdString() + "/test_files/lidar_test_files/" + fileName;
    textFile.open(filePath);
    textFile.close();
}

testData::lidarData testData::getLidarData()
{
    std::vector<std::string> temp = read();
    lidarData data;
    for (unsigned int i = 0; i < temp.size(); i++)
    {
        unsigned int index = 0;
        std::string angle_string;
        for (unsigned int j = index; j < temp[i].length(); j++)
        {
            if (temp[i][j] != ' ')
                break;
            else
                index++;
        }

        for (unsigned int j = index; j < temp[i].length(); j++)
        {
            if (temp[i][j] == ' ')
                break;
            else
            {
                std::string ch(1,temp[i][j]);
                angle_string += ch;
                index++;
            }
        }
        float angle = std::strtof(angle_string.c_str(),0);
        data.angle.push_back(angle);

        std::string range_string;
        for (unsigned int j = index; j < temp[i].length(); j++)
        {
            if (temp[i][j] != ' ')
                break;
            else
                index++;
        }

        for (unsigned int j = index; j < temp[i].length(); j++)
        {
            if (temp[i][j] == ' ')
                break;
            else
            {
                std::string ch(1,temp[i][j]);
                range_string += ch;
                index++;
            }
        }
        float range = std::strtof(range_string.c_str(),0);
        data.range.push_back(range);
    }

    return data;
}
