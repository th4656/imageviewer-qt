QT += widgets

QMAKE_CXX = clang++
QMAKE_CXXFLAGS += -std=c++11 -stdlib=libc++
QMAKE_LINK = clang++
QMAKE_LFLAGS += -std=c++11 -stdlib=libc++ -lc++abi

HEADERS       = imageviewer.h
SOURCES       = imageviewer.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/widgets/imageviewer
INSTALLS += target

wince*: {
   DEPLOYMENT_PLUGIN += qjpeg qgif
}
