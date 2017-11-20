#include "mainwindow.h"
#include "loopuptable.h"
#include "g2d_thread.h"

#if 0
static const int bufferCount = 2;
static struct g2d_buf *g_g2d_src_buffer[bufferCount];
static struct g2d_buf *g_g2d_dst_buffer[bufferCount];
#endif

#define topCameraWhich 0
#define bottomCameraWhich 1
#define leftCameraWhich 2
#define rightCameraWhich 3

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //全屏
    this->showFullScreen();

    //初始化查找表
    if (initLoopupTable() == true)
    {
        qDebug() << "initLoopupTable is ok";
    }

    //背景色
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QImage(":/背景1.jpg")));
    this->setPalette(palette);

#if 0
    //清空查找表
    cleanLoopupTable();
    //保存查找表
    saveLoopupTable();

    //this->setBaseSize(420,720);
    //this->setFixedHeight(720);
    //this->setFixedWidth(420);

#if 0
    timer = new QTimer(this);
    timer->start(1);
    connect(timer,SIGNAL(timeout()),this,SLOT(repaint()));
#endif

    //上下左右四个widget，现在的意义在于生成查找表
    topCameraWidget =  new TopCameraWindows();
//    topCameraWidget->hide();
    topCameraWidget->readFrame(0);
    topCameraWidget->close();

    bottomCameraWidget = new BottomCameraWindows();
//    bottomCameraWidget->hide();
    bottomCameraWidget->readFrame(0);
    bottomCameraWidget->close();

    leftCameraWidget = new LeftCameraWindows();
//    leftCameraWidget->hide();
    leftCameraWidget->readFrame(0);
    leftCameraWidget->close();

    rightCameraWidget = new RightCameraWindows();
//    rightCameraWidget->hide();
    rightCameraWidget->readFrame(0);
    rightCameraWidget->close();
#endif




    topButton = new QPushButton(this);
//    topButton->setGeometry(500,623,120,80);
    topButton->setGeometry(0,0,80,80);

    topButton->setText("前");
//    topButton->setAttribute(Qt::WA_TranslucentBackground);
    topButton->show();
    connect(topButton,SIGNAL(clicked(bool)),this,SLOT(TopFisheyeView()));

    bottomButton = new QPushButton(this);
//    bottomButton->setGeometry(650,623,120,80);
    bottomButton->setGeometry(0,120,80,80);
    bottomButton->setText("后");
//    bottomButton->setAttribute(Qt::WA_TranslucentBackground);
    bottomButton->show();
    connect(bottomButton,SIGNAL(clicked(bool)),this,SLOT(BottomFisheyeView()));

    leftButton = new QPushButton(this);
//    leftButton->setGeometry(800,623,120,80);
    leftButton->setGeometry(0,240,80,80);
    leftButton->setText("左");
//    leftButton->setAttribute(Qt::WA_TranslucentBackground);
    leftButton->show();
    connect(leftButton,SIGNAL(clicked(bool)),this,SLOT(LeftFisheyeView()));

    rightButton = new QPushButton(this);
//    rightButton->setGeometry(950,623,120,80);
    rightButton->setGeometry(0,360,80,80);
    rightButton->setText("右");
//    rightButton->setAttribute(Qt::WA_TranslucentBackground);
    rightButton->show();
    connect(rightButton,SIGNAL(clicked(bool)),this,SLOT(RightFisheyeView()));

    exitButton = new QPushButton(this);
//    exitButton->setGeometry(1100,623,180,80);
    exitButton->setGeometry(0,480,80,80);
    exitButton->setText("exit");
//    exitButton->setAttribute(Qt::WA_TranslucentBackground);
    exitButton->show();
    connect(exitButton,SIGNAL(clicked(bool)),this,SLOT(CloseWindows()));



//    repaint();

    //::sleep(3);

#if 0
    int in_buffer_size = countCamera * srcWidth * srcheight * 2;

    for (int i=0;i<bufferCount;i++)
    {
        g_g2d_src_buffer[i] = g2d_alloc(in_buffer_size, 0);

        if (g_g2d_src_buffer[i] == 0)
        {
            qDebug() << "can not malloc memory for g2d source";
            free(cameraBuffer);
            exit(-1);
        }
    }

    int out_buffer_size = countCamera * srcWidth * srcheight *4;

    for (int i=0;i<bufferCount;i++)
    {
        g_g2d_dst_buffer[i] =g2d_alloc(out_buffer_size, 0);

        if (g_g2d_dst_buffer[i] == 0)
        {
            qDebug() << "can not malloc memory for g2d dst";
            free(cameraBuffer);
            g2d_free(g_g2d_src_buffer[i]);
            exit(-1);
        }

        memset(g_g2d_dst_buffer[i]->buf_vaddr,0x0,out_buffer_size);
    }

    stitchingImage = new QImage(imageWidth,imageHeight,QImage::Format_ARGB32);

    cameraFourInThread = new CameraWorkFourin(this);
    cameraFourInThread->setCameraData(&cameraBuffer);
    connect(cameraFourInThread,SIGNAL(whichCameraFrame(unsigned int)),this,SLOT(updateImage(unsigned int)));
    cameraFourInThread->start();
#endif


#if 0
    fisheyeScene  = new QGraphicsScene(this);

    fisheyeView = new QGraphicsView(this);
    fisheyeScene->setSceneRect(0,0,180,420);
    fisheyeScene->setBackgroundBrush(QBrush(QImage(":/car.jpeg")));
    //pixmapittem = new QGraphicsPixmapItem();
    //pixmapittem->setPos(0,0);

    //fisheyeScene->addItem(pixmapittem);

    fisheyeView->setScene(fisheyeScene);
    fisheyeView->setGeometry(120,135,180,420);
    fisheyeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    fisheyeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    fisheyeView->show();
#endif


#if 0
    isImageReady = false;

    if (isImageReady==false)
    {
        //pixmapittem->setActive(true);
        fisheyeScene->setSceneRect(0,0,420,720);
        fisheyeView->setGeometry(0,0,420,720);
        fisheyeView->setScene(fisheyeScene);
        fisheyeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        fisheyeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        //fisheyeView->hide();
        fisheyeView->show();
        //setCentralWidget(fisheyeView);
        isImageReady = true;
    }
#endif

    caleThread = new G2DThread();
    caleThread->setWhichCamera(topCameraWhich);
    caleThread->init();
    caleThread->start();

//    caleThread = new G2DThread();
//    caleThread->setWhichCamera(topCameraWhich);
//    caleThread->start();
    //QObject::connect(caleThread,SIGNAL(caleFinish(unsigned int)),this,SLOT(updateImage(unsigned int)));
#if 0
    updateTimer = new QTimer(this);
    updateTimer->start(1000);
    connect(updateTimer,SIGNAL(timeout()),this,SLOT(cleanFrameSpeed()));
#endif

//    setUpdatesEnabled(false);
}
#if 1
void MainWindow::cleanFrameSpeed()
{
    //mutex.lock();
    qDebug() << "MainWindow::countFrameSpeed = " << countFrameSpeed;
    countFrameSpeed = 0;
    //mutex.unlock();
}
#endif

#if 1
void MainWindow::updateImage(unsigned int frameOffset)
{
#if 0

    //repaint();

    //QTime time;
    //time.start();

    if (countCameraUpate>=2)
    //mutex.lock();
    //if ((mutex.tryLock(1)))
    {

        //pixmapittem->setPixmap(fisheyePixmap);
        //fisheyeScene->setSceneRect(0, 0, caleThread->finishImage->width(), caleThread->finishImage->height());
        //QTime time;
        //time.start();
        //fisheyePixmap= QPixmap::fromImage(*(caleThread->finishImage));
       // fisheyeScene->setBackgroundBrush(QBrush(fisheyePixmap));
        //qDebug()<< "paintEvent = "<<time.elapsed()/1000.0<<"s";

        //fisheyeScene->update();
        //fisheyeView->update();
        //qimageToPixmap();
        //updateFisheyeView();
        //update();
        countCameraUpate = 0;
        //mutex.unlock();
        countFrameSpeed++;
        //countCameraUpate++;
    }
    else
    {
        //qimageToPixmap();
        //updateFisheyeView();
        //countFrameSpeed++;
        countCameraUpate++;
    }
    //mutex.unlock();
    //qDebug()<< "paintEvent = "<<time.elapsed()/1000.0<<"s";
#endif

    //isImageReady = true;
    //repaint();
    //repaint(0,0,caleThread->finishImage->width(),caleThread->finishImage->height());
}
#endif
#if 0
void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    //QTime time;
    //time.start();
    QPainter painter;
    painter.begin(this);

    qDebug() << "is update ...............................................................";

    if (isImageReady)
    {
        //painter.drawImage(0,0,(*caleThread->finishImage));
        //isImageReady = false;
    }
    painter.end();
    countFrameSpeed++;
    //qDebug()<< "paintEvent = "<<time.elapsed()/1000.0<<"s";
}
#endif
void MainWindow::updateFisheyeView()
{
    //pixmapittem->setPixmap(fisheyePixmap);
    //fisheyeView->update();
}

void MainWindow::qimageToPixmap()
{
    //QTime time;
    //time.start();
    //fisheyePixmap= QPixmap::fromImage((*finishImage));
    //qDebug()<< "paintEvent = "<<time.elapsed()/1000.0<<"s";
}

void MainWindow::TopFisheyeView()
{
    caleThread->setWhichCamera(topCameraWhich);
}

void MainWindow::BottomFisheyeView()
{
    caleThread->setWhichCamera(bottomCameraWhich);
}

void MainWindow::LeftFisheyeView()
{
    caleThread->setWhichCamera(leftCameraWhich);
}

void MainWindow::RightFisheyeView()
{
    caleThread->setWhichCamera(rightCameraWhich);
}

void MainWindow::CloseWindows()
{
    //cameraFourInThread->exit(0);
    caleThread->ClearScreen();
    ::usleep(100000);
    caleThread->quit();
    caleThread->exit(0);
//    caleThread->ClearScreen();

//    caleThread->quit();
//    caleThread->exit(0);
//    caleThread->~G2DThread();
    this->close();
    exit(0);
//   this->~MainWindow();
}

MainWindow::~MainWindow()
{
    caleThread->ClearScreen();
    ::usleep(100000);
    caleThread->quit();
    caleThread->exit(0);
//    caleThread->ClearScreen();
//    caleThread->~G2DThread();
    this->close();
    exit(0);
}
