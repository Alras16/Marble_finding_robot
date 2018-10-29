#ifndef DATALOGGIN_H
#define DATALOGGIN_H

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <QCoreApplication>
#include <QDir>

#define FILEPATH_ROOM "/test_files/loggin/rooms/"
#define FILEPATH_STATS "/test_files/loggin/statistics/"
#define FILEPATH_MARBLES "/test_files/loggin/marbles/"

class dataloggin
{
public:
    dataloggin(std::string afileName, int runNumb, char type);

    void write(std::string text);
    std::vector<std::string> read();
    void erase();

private:
    std::string filePath;
    bool firstTimeCall = true;
};

#endif // DATALOGGIN_H
