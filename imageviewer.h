#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QDir>

class QAction;
class QLabel;
class QMenu;
class QScrollArea;
class QScrollBar;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

  public:
    ImageViewer();
    bool loadFile(const QString &);

  private slots:
    void open();

    void zoomIn();
    void zoomOut();
    void normalSize();
    void fitToWindow();
    void fitToImage();
    void fitToScreen();

    void rotateClockWise();
    void rotateCounterClockWise();

    void loadNext();
    void loadPrev();

  protected:
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;

  private:
    void createActions();
    void createMenus();
    void scaleContent(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    int getFileIndex();

    QLabel *imageLabel;
    QScrollArea *scrollArea;

    QAction *openAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
    QAction *fitToImageAct;
    QAction *rotateClockWiseAct;
    QAction *rotateCounterClockWiseAct;
    QAction *loadNextAct;
    QAction *loadPrevAct;

    QMenu *fileMenu;
    QMenu *viewMenu;

    QStringList _otherPictures; // Filepaths of all the other pictures in the directory
    QDir _dir;
    QSize _sizeOfGif;
    QString _fileName; // Directory of current image
    int _indexOfCurrent;
    int _screenWidth;
    int _screenHeight;
};

#endif
