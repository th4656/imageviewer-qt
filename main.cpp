#include <QApplication>
#include <QCommandLineParser>

#include "imageviewer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QGuiApplication::setApplicationDisplayName(ImageViewer::tr("Image Viewer"));

    QCommandLineParser commandLineParser;
    commandLineParser.addHelpOption();
    commandLineParser.addPositionalArgument(ImageViewer::tr("[file]"),
                                            ImageViewer::tr("Image file to open."));
    commandLineParser.process(QCoreApplication::arguments());

    ImageViewer imageViewer;
    imageViewer.resize(150, 150);

    if (!commandLineParser.positionalArguments().isEmpty() &&
        !imageViewer.loadFile(commandLineParser.positionalArguments().front()))
    {
        return -1;
    }

    imageViewer.setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);

	//both needed for transparency
    imageViewer.setStyleSheet("background:transparent;");
    imageViewer.setAttribute(Qt::WA_TranslucentBackground, true);

    imageViewer.show();
    return app.exec();
}
