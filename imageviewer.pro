QT += widgets

QMAKE_CXX = clang++
QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
QMAKE_LINK = clang++
QMAKE_LFLAGS += -std=c++11 -stdlib=libc++ -lc++abi

HEADERS       = imageviewer.h
SOURCES       = imageviewer.cpp \
                main.cpp

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
