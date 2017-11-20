#ifndef RIGHTCAMERAWINDOWS_H
#define RIGHTCAMERAWINDOWS_H

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
#include "v4l2_work_right.h"

class RightCameraWindows : public QWidget
{
    Q_OBJECT


public:
    RightCameraWindows(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~RightCameraWindows();
    QImage cvMat2QImage(const cv::Mat& mat);
    QImage * getFisheyeImage();
    QImage * getFinishImage();
    //void run();

 public:
    QImage *finishImg;
    QImage *image;

private:
    void run();
    void my_show_fun();
    void initUTL1Array();
    void testLoopupTable();
    void use_fisheye_show();
    cv::Mat & ScanImageAndReduceC(cv::Mat& I, const uchar* const table);
    QImage mat_to_qimage_ref(cv::Mat &mat, QImage::Format format);

signals:
    void updateRightImage();

private slots:
    void CaptureView(int frameOffset);
    void Start_warpPerspective();

private:

    CameraWorkRight * camera;
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

    unsigned int startX[720];
    unsigned int endX[720];
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

public slots:
    void readFrame(int frameOffset);
};



#endif // RIGHTCAMERAWINDOWS_H
