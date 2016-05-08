QT += widgets

QMAKE_CXX = clang++
QMAKE_LINK = clang++

CONFIG += qt debug c++11

HEADERS       = imageviewer.h
SOURCES       = imageviewer.cpp \
                main.cpp

target.path = /usr/bin
INSTALLS += target

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
