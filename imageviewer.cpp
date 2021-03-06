#include <QtWidgets>
#include <cstdio>
#include <iostream>

#include "imageviewer.h"

ImageViewer::ImageViewer()
{
    imageLabel = new QLabel;
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setScaledContents(true);

    scrollArea = new QScrollArea;
    scrollArea->setWidget(imageLabel);
    scrollArea->verticalScrollBar()->setFocusPolicy(Qt::ClickFocus);
    scrollArea->horizontalScrollBar()->setFocusPolicy(Qt::ClickFocus);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scrollArea->setBackgroundRole(QPalette::Background);
    scrollArea->setStyleSheet("border: 0px;");

    setCentralWidget(scrollArea);

    QStringList filter;
    foreach (const QByteArray &imageformat, QImageReader::supportedImageFormats())
        filter.append("*." + imageformat);

    _dir.setNameFilters(filter);
    _dir.cd(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first());
    _dir.setFilter(QDir::Files);

    createActions();
    createMenus();

    _screenWidth = QGuiApplication::primaryScreen()->availableSize().width();
    _screenHeight = QGuiApplication::primaryScreen()->availableSize().height();

    _indexOfCurrent = -1; // Not yet at any image

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

    _fileName = fileName;
    std::cout << _fileName.toStdString() << std::endl;

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

    fitToImage();
    fitToScreen();

    setWindowFilePath(fileName);

    // Populates _otherPictures with images in new path
    if (QDir::currentPath() != _dir.currentPath() || _otherPictures.isEmpty())
    {
        _dir.cd(fileName.left(fileName.lastIndexOf("/") + 1));
        std::cout << "New Directory" << std::endl;
        _otherPictures = _dir.entryList();
    }
	_indexOfCurrent = getFileIndex();
	std::cout << "set index: " << _indexOfCurrent << std::endl;

    return true;
}

void ImageViewer::open()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();

    QFileDialog dialog(this, tr("Open File"), _dir.path());

    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/png");

    while (dialog.exec() == QDialog::Accepted &&
           !loadFile(dialog.selectedFiles().first()))
    {
    }
}

int ImageViewer::getFileIndex()
{
    for (int i = 0; i < _otherPictures.size(); i++)
    {
        if (_dir.absoluteFilePath(_otherPictures[i]) == _fileName)
        {
			return i;
        }
    }

    return -1;
}

void ImageViewer::zoomIn()
{
    scaleContent(1.05);
}

void ImageViewer::zoomOut()
{
    scaleContent(0.95);
}

void ImageViewer::normalSize()
{
    imageLabel->adjustSize();
}

void ImageViewer::fitToWindow()
{
    QSize imgSize;

    if (imageLabel->movie() != nullptr)
    {
        Q_ASSERT(imageLabel->movie());
        imgSize = imageLabel->movie()->scaledSize();
    }
    else if (imageLabel->pixmap() != nullptr)
    {
        Q_ASSERT(imageLabel->pixmap());
        imgSize = imageLabel->pixmap()->size();
    }

    imgSize.scale(this->size(), Qt::KeepAspectRatio);
    imageLabel->resize(imgSize);
	this->resize(imgSize);
}

void ImageViewer::fitToImage()
{
    int width = 0;
    int height = 0;

    if (imageLabel->movie() != nullptr)
    {
        Q_ASSERT(imageLabel->movie());
        width = imageLabel->movie()->scaledSize().width();
        height = imageLabel->movie()->scaledSize().height();
    }
    else if (imageLabel->pixmap() != nullptr)
    {
        Q_ASSERT(imageLabel->pixmap());
        width = imageLabel->pixmap()->width();
        height = imageLabel->pixmap()->height();
    }

    this->resize(width, height);
    imageLabel->resize(this->size());
}

void ImageViewer::fitToScreen()
{
    int width = qMin(this->width(), _screenWidth);
    int height = qMin(this->height(), _screenHeight);

    this->resize(width, height);
}

void ImageViewer::scaleContent(double factor)
{
    imageLabel->resize(factor * imageLabel->size());
    adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
    adjustScrollBar(scrollArea->verticalScrollBar(), factor);
}

void ImageViewer::rotateClockWise()
{
    if (imageLabel->pixmap() != nullptr)
    {
        imageLabel->setPixmap(imageLabel->pixmap()->transformed(QTransform().rotate(90)));
        imageLabel->resize(imageLabel->height(), imageLabel->width());
        this->resize(this->height(), this->width());
    }
}

void ImageViewer::rotateCounterClockWise()
{
    if (imageLabel->pixmap() != nullptr)
    {
        imageLabel->setPixmap(
            imageLabel->pixmap()->transformed(QTransform().rotate(-90)));
        imageLabel->resize(imageLabel->height(), imageLabel->width());
        this->resize(this->height(), this->width());
    }
}

void ImageViewer::loadNext()
{
    if (_otherPictures.size() == 0)
        return;
    else if (++_indexOfCurrent >= _otherPictures.size())
        _indexOfCurrent = 0;

    loadFile(_dir.absoluteFilePath(_otherPictures[_indexOfCurrent]));
}

void ImageViewer::loadPrev()
{
    if (_otherPictures.size() == 0)
        return;
    else if (--_indexOfCurrent < 0)
        _indexOfCurrent = _otherPictures.size() - 1;

    loadFile(_dir.absoluteFilePath(_otherPictures[_indexOfCurrent]));
}

void ImageViewer::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(
        int(factor * scrollBar->value() + ((factor - 1) * scrollBar->pageStep() / 2)));
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

    fitToImageAct = new QAction(tr("Fit to Image"), this);
    fitToImageAct->setEnabled(true);
    fitToImageAct->setShortcut(tr("Shift+f"));
    connect(fitToImageAct, SIGNAL(triggered()), this, SLOT(fitToImage()));

    rotateClockWiseAct = new QAction(tr("Rotate Clock Wise"), this);
    rotateClockWiseAct->setShortcut(tr("r"));
    connect(rotateClockWiseAct, SIGNAL(triggered()), this, SLOT(rotateClockWise()));

    rotateCounterClockWiseAct = new QAction(tr("Rotate Counter Cock Wise"), this);
    rotateCounterClockWiseAct->setShortcut(tr("Shift+r"));
    connect(rotateCounterClockWiseAct, SIGNAL(triggered()), this,
            SLOT(rotateCounterClockWise()));

    loadNextAct = new QAction(tr("Next Image"), this);
    loadNextAct->setShortcut(tr("n"));
    connect(loadNextAct, SIGNAL(triggered()), this, SLOT(loadNext()));

    loadPrevAct = new QAction(tr("Previous Image"), this);
    loadPrevAct->setShortcut(tr("Shift+n"));
    connect(loadPrevAct, SIGNAL(triggered()), this, SLOT(loadPrev()));
}

void ImageViewer::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addAction(loadNextAct);
    fileMenu->addAction(loadPrevAct);
    fileMenu->addAction(exitAct);

    viewMenu = new QMenu(tr("&View"), this);
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(normalSizeAct);
    viewMenu->addAction(fitToWindowAct);
    viewMenu->addAction(fitToImageAct);
    viewMenu->addSeparator();
    viewMenu->addAction(rotateClockWiseAct);
    viewMenu->addAction(rotateCounterClockWiseAct);

    scrollArea->addAction(openAct);
    scrollArea->addAction(exitAct);
    scrollArea->addAction(zoomInAct);
    scrollArea->addAction(zoomOutAct);
    scrollArea->addAction(normalSizeAct);
    scrollArea->addAction(fitToWindowAct);
    scrollArea->addAction(fitToImageAct);
    scrollArea->addAction(rotateClockWiseAct);
    scrollArea->addAction(rotateCounterClockWiseAct);
    scrollArea->addAction(loadNextAct);
    scrollArea->addAction(loadPrevAct);
}

void ImageViewer::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addMenu(fileMenu);
    menu.addMenu(viewMenu);
    menu.addAction(openAct);
    menu.addAction(exitAct);
    menu.exec(event->globalPos());
}
