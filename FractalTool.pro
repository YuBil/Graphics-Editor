#-------------------------------------------------
#
# Project created by QtCreator 2016-11-03T14:30:32
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FractalTool
TEMPLATE = app

CONFIG       += c++11

QMAKE_CXXFLAGS += -std=gnu++11

HEADERS += \
    imgconvert.h    \
    mainwindow.h


SOURCES += \
    imgconvert.cpp      \
    mainwindow.cpp      \
    main.cpp

FORMS += \
    /home/keanu/MyProjects/FractalTool/mainwindow.ui

unix {

    # Path to OpenCV
    OPENCV_INCLUDE_PATH +=  /usr/local/lib
    LIBS                +=  -L$$OPENCV_INCLUDE_PATH
    LIBS    +=  -lopencv_core       \
                -lopencv_imgproc    \
                -lopencv_objdetect  \
                -lopencv_highgui
#                -lopencv_videoio

    # Add BOOST libs
 #   LIBS += -L/usr/lib/x86_64-linux-gnu
  #  LIBS += -lboost_system -lboost_filesystem
}

INCLUDEPATH +=  $$OPENCV_INCLUDE_PATH
INCLUDEPATH +=  $$OPENCV_INCLUDE_MODULE_PATH
INCLUDEPATH +=  $$INCLUDE_IMAGEIO_PATH/include
INCLUDEPATH +=  $$INCLUDE_IMAGEIO_PATH/src
INCLUDEPATH +=  $$INCLUDE_LOGGING_PATH/include
INCLUDEPATH +=  $$INCLUDE_LOGGING_PATH/src
