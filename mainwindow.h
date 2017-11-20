#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QPicture>
#include <QPaintEvent>
#include <QDebug>
#include <QMutex>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

#include "topcamerawindows.h"
#include "bottomcamerawindows.h"
#include "leftcamerawindows.h"
#include "rightcamerawindows.h"
#include "v4l2_work_fourin.h"

#include "g2d.h"
#include "g2d_yuyv_bgra.h"

#include "g2d_thread.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
//    void paintEvent(QPaintEvent *event);
    void updateFisheyeView();
    void qimageToPixmap();

private slots:
    void updateImage(unsigned int frameOffset);
    void cleanFrameSpeed();
    void CloseWindows();
    void TopFisheyeView();
    void BottomFisheyeView();
    void LeftFisheyeView();
    void RightFisheyeView();

private:

    //QMutex mutex;
    QGraphicsView *fisheyeView;
    QGraphicsScene *fisheyeScene;
    QGraphicsPixmapItem * pixmapittem;
    QPixmap fisheyePixmap;
    int countFrameSpeed = 0;

    int srcWidth = 1280;
    int srcheight = 800;

    int imageWidth = 420;
    int imageHeight = 720;
    unsigned int countCameraUpate = 0;
    bool isImageReady = false;

    TopCameraWindows *topCameraWidget;
    BottomCameraWindows *bottomCameraWidget;
    LeftCameraWindows *leftCameraWidget;
    RightCameraWindows *rightCameraWidget;

    CameraWorkFourin *cameraFourInThread;
    G2DThread *caleThread;
    unsigned char * cameraBuffer;

    QImage * stitchingImage;

//    QTimer *timer;
//    QTimer *updateTimer;
    QPushButton *exitButton;

    QPushButton * topButton;
    QPushButton * bottomButton;
    QPushButton * leftButton;
    QPushButton * rightButton;
};

#endif // MAINWINDOW_H
