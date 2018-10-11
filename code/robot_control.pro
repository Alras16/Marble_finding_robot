TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    c_vision.cpp \
    testdata.cpp

CONFIG += link_pkgconfig
PKGCONFIG += gazebo
PKGCONFIG += opencv

HEADERS += \
    c_vision.h \
    testdata.h
