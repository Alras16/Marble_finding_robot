TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += main.cpp \
    bst_coordinates.cpp \
    map_class.cpp \
    c_vision.cpp \
    testdata.cpp \
    lidar_sensor.cpp

CONFIG += link_pkgconfig
PKGCONFIG += gazebo
PKGCONFIG += opencv

HEADERS += \
    bst_coordinates.h \
    map_class.h \
    c_vision.h \
    testdata.h \
    lidar_sensor.h
