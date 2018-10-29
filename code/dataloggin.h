#ifndef DATALOGGIN_H
#define DATALOGGIN_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <QCoreApplication>
#include <QDir>
#include "map_class.h"

#define FILEPATH_ROOM "/test_files/loggin/rooms/"
#define FILEPATH_STATS "/test_files/loggin/statistics/"
#define FILEPATH_MARBLES "/test_files/loggin/marbles/"

class dataloggin
{
public:
    dataloggin(std::string aBaseName,int testNumb, int runNumb, char type);

    void write(std::string text);
    std::vector<std::string> read();
    void erase();

    void writeMarble(map_class::foundMarble marble, int numb);
    std::vector<map_class::foundMarble> readMarbles();


private:
    std::string filePath;
    bool firstTimeCall = true;
};

#endif // DATALOGGIN_H
