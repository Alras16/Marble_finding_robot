#include "testdata.h"

testData::testData(std::string afileName)
{
    fileName = afileName + ".txt";
}


void testData::write(std::string text)
{
    std::vector<std::string> buffer = read();
    buffer.push_back(text);

    std::ofstream textFileOut(fileName);
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
    std::ifstream textFileIn(fileName);
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
    textFile.open(fileName);
    textFile.close();
}

testData::lidarData testData::getLidarData()
{
    std::vector<std::string> temp = read();
    lidarData data;
    std::string::size_type sz;
    for (unsigned int i = 0; i < temp.size(); i++)
    {
        float angle = std::stof(temp[i],&sz);
        float range = std::stof(temp[i].substr(sz));
        data.angle.push_back(angle);
        data.range.push_back(range);
    }
}
