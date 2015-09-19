#include <QtWidgets>
#include <cstdio>
#include <iostream>

// TODO: Load next and move to next image with left/right and scroll
// Idea
// Get all image names in directory and store in vector
// map right/left to get index+1 w/ wraparound and callload with that
// TODO: Preview images by holding shift
// TODO: PNG transparency
// TODO: Change mappings for zoom to up/down and scroll to hjkl

#include "imageviewer.h"

ImageViewer::ImageViewer()
{
    imageLabel = new QLabel;
    imageLabel->setBackgroundRole(QPalette::Background);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Background);
    scrollArea->setWidget(imageLabel);
    scrollArea->verticalScrollBar()->setFocusPolicy(Qt::ClickFocus);
    scrollArea->horizontalScrollBar()->setFocusPolicy(Qt::ClickFocus);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setCentralWidget(scrollArea);

    _dir.cd(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first());

    createActions();
    createMenus();

    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
}

bool ImageViewer::loadFile(const QString &fileName)
{
    QImage image(fileName);
    if (image.isNull())
    {
        QMessageBox::information(
            this, QGuiApplication::applicationDisplayName(),
            tr("Cannot load %1.").arg(QDir::toNativeSeparators(fileName)));
        setWindowFilePath(QString());
        imageLabel->setPixmap(QPixmap());
        imageLabel->adjustSize();
        return false;
    }

    if (QMimeDatabase().mimeTypeForFile(fileName).name().toStdString() == "image/gif")
    {
        QMovie *gif = new QMovie(fileName);
        imageLabel->setMovie(gif);
        _sizeOfGif = image.size();
        imageLabel->movie()->setScaledSize(_sizeOfGif);
        gif->start();

        rotateClockWiseAct->setEnabled(false);
        rotateCounterClockWiseAct->setEnabled(false);
    }
    else
    {
        imageLabel->setPixmap(QPixmap::fromImage(image));
        rotateClockWiseAct->setEnabled(true);
        rotateCounterClockWiseAct->setEnabled(true);
    }

    scaleFactor = 1.0;
    fitToWindow();

    setWindowFilePath(fileName);

    _dir.cd(fileName.left(fileName.lastIndexOf("/") + 1));

    return true;
}

void ImageViewer::open()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();

    const QStringList homeLocations =
        QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    QFileDialog dialog(this, tr("Open File"), _dir.path());

    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/png");

    while (dialog.exec() == QDialog::Accepted &&
           !loadFile(dialog.selectedFiles().first()))
    {
    }
}

void ImageViewer::zoomIn() { scaleContent(1.05); }

void ImageViewer::zoomOut() { scaleContent(0.95); }

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}

void ImageViewer::fitToWindow()
{
    normalSize();
    imageLabel->resize(this->size());
}

void ImageViewer::rotateClockWise()
{
    if (imageLabel->pixmap() != nullptr)
    {
        imageLabel->setPixmap(imageLabel->pixmap()->transformed(QTransform().rotate(90)));
        zoomIn();  // Image dimensions get messed up when rotating
        zoomOut(); // I should probably find a better solution
    }
}

void ImageViewer::rotateCounterClockWise()
{
    if (imageLabel->pixmap() != nullptr)
    {
        imageLabel->setPixmap(
            imageLabel->pixmap()->transformed(QTransform().rotate(-90)));
        zoomIn();  // Image dimensions get messed up when rotating
        zoomOut(); // I should probably find a better solution
    }
}

void ImageViewer::createActions()
{
    openAct = new QAction(tr("Open..."), this);
    openAct->setShortcut(tr("o"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("Exit"), this);
    exitAct->setShortcut(tr("q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    zoomInAct = new QAction(tr("Zoom In 5%"), this);
    zoomInAct->setShortcut(tr("z"));
    zoomInAct->setEnabled(true);
    connect(zoomInAct, SIGNAL(triggered()), this, SLOT(zoomIn()));

    zoomOutAct = new QAction(tr("Zoom Out 5%"), this);
    zoomOutAct->setShortcut(tr("Shift+z"));
    zoomOutAct->setEnabled(true);
    connect(zoomOutAct, SIGNAL(triggered()), this, SLOT(zoomOut()));

    normalSizeAct = new QAction(tr("&Normal Size"), this);
    normalSizeAct->setShortcut(tr("="));
    normalSizeAct->setEnabled(true);
    connect(normalSizeAct, SIGNAL(triggered()), this, SLOT(normalSize()));

    fitToWindowAct = new QAction(tr("&Fit to Window"), this);
    fitToWindowAct->setEnabled(true);
    fitToWindowAct->setShortcut(tr("f"));
    connect(fitToWindowAct, SIGNAL(triggered()), this, SLOT(fitToWindow()));

    rotateClockWiseAct = new QAction(tr("Rotate Clock Wise"), this);
    rotateClockWiseAct->setShortcut(tr("r"));
    connect(rotateClockWiseAct, SIGNAL(triggered()), this, SLOT(rotateClockWise()));

    rotateCounterClockWiseAct = new QAction(tr("Rotate Counter Cock Wise"), this);
    rotateCounterClockWiseAct->setShortcut(tr("Shift+r"));
    connect(rotateCounterClockWiseAct, SIGNAL(triggered()), this,
            SLOT(rotateCounterClockWise()));
}

void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addAction(fitToWindowAct);
    viewMenu->addSeparator();
    viewMenu->addAction(rotateClockWiseAct);
    viewMenu->addAction(rotateCounterClockWiseAct);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(viewMenu);

    scrollArea->addAction(openAct);
    scrollArea->addAction(exitAct);
    scrollArea->addAction(zoomInAct);
    scrollArea->addAction(zoomOutAct);
    scrollArea->addAction(normalSizeAct);
    scrollArea->addAction(fitToWindowAct);
    scrollArea->addAction(rotateClockWiseAct);
    scrollArea->addAction(rotateCounterClockWiseAct);
}

void ImageViewer::scaleContent(double factor)
{
    if (imageLabel->movie() != nullptr)
    {
        Q_ASSERT(imageLabel->movie());
        scaleFactor *= factor;
        imageLabel->resize(scaleFactor * imageLabel->movie()->scaledSize());
    }
    else if (imageLabel->pixmap() != nullptr)
    {
        Q_ASSERT(imageLabel->pixmap());
        scaleFactor *= factor;
        imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());
    }
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(
        int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
}
