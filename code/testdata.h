#ifndef TESTDATA_H
#define TESTDATA_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <QCoreApplication>
#include <QDir>

class testData
{
public:
    testData(std::string afileName);

    struct lidarData
    {
        std::vector<float> angle;
        std::vector<float> range;
    };

    void write(std::string text);
    std::vector<std::string> read();
    void erase();
    lidarData getLidarData();

private:
    std::string fileName;
    bool firstTimeCall = true;

};

#endif // TESTDATA_H
