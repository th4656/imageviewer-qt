#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>

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

    void rotateClockWise();
    void rotateCounterClockWise();

  private:
    void createActions();
    void createMenus();
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);

    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;

    QAction *openAct;
    QAction *exitAct;
    QAction *zoomInAct;
    QAction *zoomOutAct;
    QAction *normalSizeAct;
    QAction *fitToWindowAct;
	QAction *rotateClockWiseAct;
	QAction *rotateCounterClockWiseAct;

	double _imageWindowRatio;

    QMenu *fileMenu;
    QMenu *viewMenu;
};

#endif
