#ifndef DATALOGGIN_H
#define DATALOGGIN_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <QCoreApplication>
#include <QDir>
#include "ct.h"

#define FILEPATH_ROOM "/test_files/loggin/rooms/"
#define FILEPATH_STATS "/test_files/loggin/statistics/"
#define FILEPATH_MARBLES "/test_files/loggin/marbles/"

#define MARBLES 'm'
#define STATS 's'
#define ROOMS 'r'

class dataloggin
{
public:
    dataloggin(std::string aBaseName,int testNumb, int runNumb, char type);

    // ONLY USE WHEN type == 'm'
    void writeMarble(ct::foundMarble marble, int numb);
    std::vector<ct::foundMarble> readMarbles();

    // ONLY USE WHEN type == 's'
    void writeStats(int roomNumb ,float probability);
    std::vector<float> readStats();

    // ONLY USE WHEN type == 'r'
    void writeRoom(int roomNumb, cv::Point massCenter, int numbOfPixels);
    std::vector<ct::room> readRooms();

private:
    void write(std::string text);
    std::vector<std::string> read();
    void erase();

    std::string filePath;
    bool firstTimeCall = true;
};

#endif // DATALOGGIN_H
