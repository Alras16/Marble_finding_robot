TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += test_main_grove.cpp \
    #fuzzybugcontroller.cpp \
    bst_coordinates.cpp \
    map_class.cpp \
    c_vision.cpp \
    testdata.cpp \
    lidar_sensor.cpp \
    dataloggin.cpp \
    #motion_planning.cpp \

CONFIG += link_pkgconfig
PKGCONFIG += gazebo
PKGCONFIG += opencv

#INCLUDEPATH += /media/kenni/usb1/linuxUbuntu/fuzzylite-6.0-linux64/fuzzylite-6.0/fuzzylite
#LIBS += -L/media/kenni/usb1/linuxUbuntu/fuzzylite-6.0-linux64/fuzzylite-6.0/release/bin -lfuzzylite-static
#DEPENDPATH += /media/kenni/usb1/linuxUbuntu/fuzzylite-6.0-linux64/fuzzylite-6.0

HEADERS += \
    #fuzzybugcontroller.h \
    bst_coordinates.h \
    map_class.h \
    c_vision.h \
    testdata.h \
    lidar_sensor.h \
    dataloggin.h \
    #motion_planning.h \

DISTFILES += \
    #fuzzybugcontroller.fll
