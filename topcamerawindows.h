#ifndef TOPCAMERAWINDOWS_H
#define TOPCAMERAWINDOWS_H

#include "opencv2/opencv.hpp"
#include <QThread>
#include <QWidget>
#include <QVector>
//#include <QTimer>
#include <QPushButton>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <QTime>
#include "4_in_read.h"
#include "iconwidget.h"
#include "loopuptable.h"
#include "v4l2_work_top.h"

class TopCameraWindows : public QWidget
{
    Q_OBJECT


public:
    TopCameraWindows(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~TopCameraWindows();
    QImage cvMat2QImage(const cv::Mat& mat);

 public:
    QImage *finishImg;
    QImage *image;

private:
    void run();
    void my_show_fun();
    void initUTL1Array();
    void testLoopupTable();
    void use_fisheye_show();

signals:
    void updateTopImage();

private slots:
    void CaptureView(int frameOffset);
    void Start_warpPerspective();

private:

    CameraWorkTop * camera;
    std::vector<uchar> data_encode;
    cv::Mat intrinsic_matrix;    /*****    摄像机内参数矩阵    ****/
    cv::Mat distortion_coeffs;     /* 摄像机的4个畸变系数：k1,k2,k3,k4*/

    cv::Mat mapx;
    cv::Mat mapy;

    cv::Mat sourcePicture;
    cv::Mat warp_dst;

    cv::Mat rgb_source;
    cv::Mat rgb;
    cv::Mat my_mat;
    cv::Point *UTL1Array;

    unsigned int startX[165];
    unsigned int endX[165];
    cv::Mat warp_matrix;
    cv::Mat warp_matrix_inv;
    cv::Point *array;

    int srcWidth = 1280;
    int srcheight = 800;
    unsigned char * cameraBuffer;

    int frameCount = 0;

    double zoomDouble = 1;
    cv::Point2f srcTri[4];
    cv::Point2f dstTri[4];
    bool firstview = false;
    bool initWarpPerspective = true;


//    cv::Mat MscalFig;
//    cv::Mat mimg_fish2dist;

//    unsigned short calib_x0,calib_y0;
//    unsigned short calib_x1,calib_y1;
//    unsigned short calib_x2,calib_y2;
//    unsigned short calib_x3,calib_y3;

//    cv::Mat transform;
//    std::vector<cv::Point2f> ponits, points_trans;

public slots:
    void readFrame(int frameOffset);
};

#endif // TOPCAMERAWINDOWS_H
