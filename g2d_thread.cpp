#include "g2d_thread.h"
#include "loopuptable.h"
#include <QDebug>
#include <QTime>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <QCoreApplication>

#define TFAIL -1
#define TPASS 0

struct fb_fix_screeninfo FixedInfo;
struct fb_var_screeninfo OrigVarInfo;
static int FrameBufferFD = -1;
void *FrameBuffer = (void *) -1;

#define __ANDROID__





#ifndef __ANDROID__
#define FRAMEBUFFER "/dev/fb0"
#else
#define FRAMEBUFFER "/dev/graphics/fb0"
#endif //__ANDROID__





//static tablePos topFisheyeView[600][800];

//struct g2d_buf *g_g2d_src_cale_buffer;
//struct g2d_buf *g_g2d_desc_cale_buffer;

ushort G2DThread::rgb_24_2_565(int r, int g, int b)
{
    return (ushort)(((unsigned(r) << 8) & 0xF800) |
            ((unsigned(g) << 3) & 0x7E0)  |
            ((unsigned(b) >> 3)));
}

int G2DThread::init_dev(FBDEV *dev)
{
    FBDEV *fr_dev=dev;

    struct mxcfb_gbl_alpha alpha;
    char node[8];
    int retval = TPASS;
    struct fb_fix_screeninfo fb_fix;
    struct mxcfb_pos pos;


//    fr_dev->fdfd=open(FRAMEBUFFER,O_RDWR);

#if 1
    if ((fr_dev->fdfd = open(fb_display_dev, O_RDWR,0)) <= 0) {
        qDebug("Unable to open %s\n", fb_display_dev);
        return TFAIL;
    }
        //qDebug("the framebuffer device was opended successfully.\n");

    /* Get the fixed screen info */
    if (ioctl(fr_dev->fdfd, FBIOGET_FSCREENINFO, &(fr_dev->finfo)))
    {
        qDebug("error: ioctl(FBIOGET_FSCREENINFO) failed\n");
        ::close(fr_dev->fdfd);
        return TFAIL;
    }

    if ((strcmp(fr_dev->finfo.id, "DISP4 FG") == 0) || (strcmp(fr_dev->finfo.id, "DISP3 FG") == 0)) {
        g_display_fg = 1;
        if (g_g2d_render) {
            pos.x = 80;
            pos.y = 0;
        } else {
            pos.x = g_display_left;
            pos.y = g_display_top;
        }
#if 1
        if (ioctl(fr_dev->fdfd, MXCFB_SET_OVERLAY_POS, &pos) < 0) {
            qDebug("fb_display_setup MXCFB_SET_OVERLAY_POS failed\n");
            ::close(fr_dev->fdfd);
            return TFAIL;
        }
#endif

        if (!g_g2d_render) {
            g_screen_info.xres = g_display_width;
            g_screen_info.yres = g_display_height;
            g_screen_info.yres_virtual = g_screen_info.yres * g_display_num_buffers;
            g_screen_info.nonstd = g_display_fmt;
            if (ioctl(fr_dev->fdfd, FBIOPUT_VSCREENINFO, &g_screen_info) < 0) {
                qDebug("fb_display_setup FBIOPUET_VSCREENINFO failed\n");
                ::close(fr_dev->fdfd);
                return TFAIL;
            }

            ioctl(fr_dev->fdfd, FBIOGET_FSCREENINFO, &(fr_dev->finfo));
            ioctl(fr_dev->fdfd, FBIOGET_VSCREENINFO, &g_screen_info);
        }
#if 1
        sprintf(node, "%d", g_display_id - 1);	//for iMX6
#ifdef BUILD_FOR_ANDROID
        strcpy(fb_display_bg_dev, "/dev/graphics/fb");
#else
        strcpy(fb_display_bg_dev, "/dev/fb");
#endif
        strcat(fb_display_bg_dev, node);
        if ((fd_fb_bg = open(fb_display_bg_dev, O_RDWR )) < 0) {
            qDebug("Unable to open bg frame buffer\n");
            qDebug()<<"fb_display_bg_dev is "<<fb_display_bg_dev;
            ::close(fr_dev->fdfd);
            return TFAIL;
        }
#endif

#if 1
        /* Overlay setting */
        alpha.alpha = 0;
        alpha.enable = 1;
        if (ioctl(fd_fb_bg, MXCFB_SET_GBL_ALPHA, &alpha) < 0) {
            qDebug("Set global alpha failed\n");
            ::close(fr_dev->fdfd);
            ::close(fd_fb_bg);
            return TFAIL;
        }
#endif


        if (g_g2d_render) {
#if 1
            ioctl(fd_fb_bg, FBIOGET_VSCREENINFO, &g_screen_info);
#else
            ioctl(fr_dev->fdfd, FBIOGET_VSCREENINFO, &g_screen_info);
#endif

            g_screen_info.yres_virtual = g_screen_info.yres * g_display_num_buffers;

//            g_screen_info.yres_virtual = 640 * g_display_num_buffers;

#if 1
            if (ioctl(fr_dev->fdfd, FBIOPUT_VSCREENINFO, &g_screen_info) < 0) {
                qDebug("fb_display_setup FBIOPUET_VSCREENINFO failed\n");
                ::close(fr_dev->fdfd);
                ::close(fd_fb_bg);
                return TFAIL;
            }
#endif

            ioctl(fr_dev->fdfd, FBIOGET_FSCREENINFO, &fb_fix);
            ioctl(fr_dev->fdfd, FBIOGET_VSCREENINFO, &g_screen_info);
        }
    } else {
        g_display_fg = 0;

        if (!g_g2d_render) {
            qDebug("It is background screen, only full screen default format was supported.\r\n");
            g_display_width = g_screen_info.xres;
            g_display_height = g_screen_info.yres;
            g_display_num_buffers = 1;
            g_screen_info.yres_virtual = g_screen_info.yres * g_display_num_buffers;

            if (ioctl(fr_dev->fdfd, FBIOPUT_VSCREENINFO, &g_screen_info) < 0) {
                qDebug("fb_display_setup FBIOPUET_VSCREENINFO failed\n");
                ::close(fr_dev->fdfd);
                ::close(fd_fb_bg);
                return TFAIL;
            }

            ioctl(fr_dev->fdfd, FBIOGET_FSCREENINFO, &(fr_dev->finfo));
            ioctl(fr_dev->fdfd, FBIOGET_VSCREENINFO, &g_screen_info);

            if (g_screen_info.bits_per_pixel == 16)
                g_display_fmt = V4L2_PIX_FMT_RGB565;
            else if (g_screen_info.bits_per_pixel == 24)
                g_display_fmt = V4L2_PIX_FMT_RGB24;
            else
                g_display_fmt = V4L2_PIX_FMT_RGB32;
        }
    }

    ioctl(fr_dev->fdfd,FBIOGET_VSCREENINFO,&(fr_dev->vinfo)); //获取可变参数


    fr_dev->screensize=fr_dev->vinfo.xres*fr_dev->vinfo.yres*fr_dev->vinfo.bits_per_pixel/8;
//    fr_dev->screensize=fr_dev->vinfo.xres*640*fr_dev->vinfo.bits_per_pixel/8;

    fr_dev->map_fb=(char *)mmap(NULL,fr_dev->finfo.smem_len,PROT_READ|PROT_WRITE,MAP_SHARED,fr_dev->fdfd,0);

    if (fr_dev->map_fb == MAP_FAILED )
    {
        qDebug("fb_display_setup mmap failed\n");
        ::close(fr_dev->fdfd);
        ::close(fd_fb_bg);
        return TFAIL;
    }

#if 1
        ioctl(fr_dev->fdfd, FBIOBLANK, FB_BLANK_UNBLANK);
#endif

#endif


#if 0

    fr_dev->fdfd=open(FRAMEBUFFER,O_RDWR);
//    qDebug("the framebuffer device was opended successfully.\n");

    /* Get the fixed screen info */
    if (ioctl(fr_dev->fdfd, FBIOGET_FSCREENINFO, &(fr_dev->finfo)))
    {
        qDebug("error: ioctl(FBIOGET_FSCREENINFO) failed\n");
        ::close(fr_dev->fdfd);
        return TFAIL;
    }

    ioctl(fr_dev->fdfd,FBIOGET_VSCREENINFO,&(fr_dev->vinfo)); //获取可变参数

    fr_dev->screensize=fr_dev->vinfo.xres*fr_dev->vinfo.yres*fr_dev->vinfo.bits_per_pixel/8;

    fr_dev->map_fb=(char *)mmap(NULL,fr_dev->finfo.smem_len,PROT_READ|PROT_WRITE,MAP_SHARED,fr_dev->fdfd,0);

    //qDebug()<<"fr_dev->vinfo.bits_per_pixel = " <<fr_dev->vinfo.bits_per_pixel;

    //qDebug("init_dev successfully.\n");

#endif


    //qDebug()<<"fr_dev->vinfo.bits_per_pixel = " <<fr_dev->vinfo.bits_per_pixel;

    //qDebug("init_dev successfully.\n");

    return TPASS;
}

void  G2DThread::draw_dot(FBDEV *dev,int x,int y,unsigned char r,unsigned char g,unsigned char b) //(x.y) 是坐标
//void draw_dot(FBDEV *dev,int x,int y) //(x.y) 是坐标
{
    FBDEV *fr_dev=dev;
    int *xx=&x;
    int *yy=&y;

    long int location=0;
    location=(*xx+fr_dev->vinfo.xoffset)*(fr_dev->vinfo.bits_per_pixel/8)+
                 (*yy+fr_dev->vinfo.yoffset)*fr_dev->finfo.line_length;
    //int b=100;
    //int g=100;
    //int r=100;
    unsigned short int t=r<<11|g<<5|b;
    *((unsigned short int *)(fr_dev->map_fb+location))=t;
}



//QImage  *finishImage;

G2DThread::G2DThread(QObject *parent)
{
    strcpy(fb_display_dev,"/dev/graphics/fb1");
    strcpy(fb_display_bg_dev,"/dev/graphics/fb0");

#if 0
    whichCamera = topCameraWhich;
//    fb_dev.fdfd=-1;
//    init_dev(&fb_dev);

    status = false;

#if 0
    QString cutFisheyeViewFileName = "/data/user/loopuptable/cutTopFisheyeView";
    QString loadopenfilemode = "rb+";
    QString saveopenfilemode = "wb+";

    FILE *fp;
    fp = fopen(cutFisheyeViewFileName.toLatin1().data(),loadopenfilemode.toLatin1().data());

    if (fp == NULL)
    {
        return ;
    }

    for (int i=0;i<600;i++)
    {
        for (int j=0;j<800;j++)
        {
            fread((&topFisheyeView[i][j]),sizeof(tablePos),1,fp);
        }
    }

    fclose(fp);
#endif

//    int out_buffer_size = 2 * srcWidth * srcheight;

//      g_g2d_src_cale_buffer = g2d_alloc(out_buffer_size, 0);

//        if (g_g2d_src_cale_buffer == 0)
//        {
//            qDebug("can not malloc memory for g2d dst\n");
//            g2d_free(g_g2d_src_cale_buffer);
//            exit(-1);
//        }

//        memset(g_g2d_src_cale_buffer->buf_vaddr,0x0,out_buffer_size);

//        g_g2d_desc_cale_buffer = g2d_alloc(out_buffer_size, 0);

//          if (g_g2d_desc_cale_buffer == 0)
//          {
//              qDebug("can not malloc memory for g2d dst\n");
//              g2d_free(g_g2d_desc_cale_buffer);
//              exit(-1);
//          }

//    memset(g_g2d_desc_cale_buffer->buf_vaddr,0x0,out_buffer_size);

//    g_g2d_temp_buffer = g2d_alloc(out_buffer_size, 0);

//    if (g_g2d_temp_buffer == 0)
//    {
//        qDebug("can not malloc memory for g2d dst\n");
//        g2d_free(g_g2d_temp_buffer);
//        exit(-1);
//    }

//    memset(g_g2d_temp_buffer->buf_vaddr,0x0,out_buffer_size);


//    timer = new QTimer(this);
//    timer->start(1000);
//    connect(timer,SIGNAL(timeout()),this,SLOT(cleanFrameSpeed()));

//    for (int i=0;i<bufferCount;i++)
//    {
//        stitchingImage[i] = new QImage(imageWidth,imageHeight,QImage::Format_ARGB32);
//    }

//    finishImage = stitchingImage[0];

    cameraFourInThread = new CameraWorkFourin();
//    cameraFourInThread->setCameraData(&cameraBuffer);
    connect(cameraFourInThread,SIGNAL(Frame(unsigned int)),this,SLOT(copyData(unsigned int)));
    cameraFourInThread->start();

#endif

}

void G2DThread::init()
{
    ::sleep(3);

//        fb_dev.fdfd=-1;
//        init_dev(&fb_dev);

        int out_buffer_size = 2 * srcWidth * srcheight;

          g_g2d_src_cale_buffer = g2d_alloc(out_buffer_size, 0);

            if (g_g2d_src_cale_buffer == 0)
            {
                qDebug("can not malloc memory for g2d dst\n");
                g2d_free(g_g2d_src_cale_buffer);
                exit(-1);
            }

            memset(g_g2d_src_cale_buffer->buf_vaddr,0x0,out_buffer_size);

            g_g2d_desc_cale_buffer = g2d_alloc(out_buffer_size, 0);

              if (g_g2d_desc_cale_buffer == 0)
              {
                  qDebug("can not malloc memory for g2d dst\n");
                  g2d_free(g_g2d_desc_cale_buffer);
                  exit(-1);
              }

        memset(g_g2d_desc_cale_buffer->buf_vaddr,0x0,out_buffer_size);


    whichCamera = topCameraWhich;
    status = false;
    cameraFourInThread = new CameraWorkFourin();
    if (cameraFourInThread->video_init() ==  TPASS)
    {
        qDebug()<<"CameraWorkFourin video_init is pass";
        cameraFourInThread->start();
        ::usleep(500000);
        connect(cameraFourInThread,SIGNAL(Frame(unsigned int)),this,SLOT(copyData(unsigned int)));
    }
    else
    {
        qDebug()<<"CameraWorkFourin video_init is fail";
    }

    qDebug()<<"G2DThread init pass";
}

void G2DThread::setWhichCamera(int whichCamera)
{
    this->whichCamera = whichCamera;
//    QCoreApplication::processEvents();
}

void G2DThread::cleanFrameSpeed()
{
    qDebug() << "G2DThread::countFrameSpeed = " << countFrameSpeed;
    countFrameSpeed = 0;
}

#if 0
void G2DThread::copyData(unsigned int frameOffset)
{
    int nr= imageHeight; // number of rows
    int nc= imageWidth; // number of columns
    int lenght = 4;
    char* destdata;
    int j =0;
    int i =0;
    int x = 0;
    int y = 0;
    uchar* srcdata;
    tablePos tempTablePos;
    int widthOfLine = srcWidth*lenght;

    //if (mutex.tryLock(1))
    {
        //QTime time;
        //time.start();

        //qDebug()<<time.elapsed()/1000.0<<"s";

        //QTime time;
        //time.start();
            for (j=0; j<nr; j++)
            {
                    destdata = (char*)stitchingImage[frameOffset]->scanLine(j);

                    for (i=0; i<nc; i++)
                    {
                            tempTablePos = loopupTable[j][i];

                            x = tempTablePos.x;
                            y = tempTablePos.y;

                            if (!( (x == 0) && (y == 0) ))
                            {
                                srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr))+(((y))*widthOfLine)+((x)*lenght);
                                *((int*)destdata) = *((int*)srcdata) ;
                            }

                            destdata += lenght;

                    } // end of row
            }
            //qDebug()<<time.elapsed()/1000.0<<"s";
            //QTime time;
            //time.start();
            finishImage = stitchingImage[frameOffset];
            //fisheyePixmap= QPixmap::fromImage(*finishImage);
            emit caleFinish(frameOffset);
            //update();
            //qDebug()<<time.elapsed()/1000.0<<"s";
        countFrameSpeed++;
        //mutex.unlock();
        //qDebug()<<time.elapsed()/1000.0<<"s";
    }

}
#endif

void G2DThread::copyData(unsigned int frameOffset)
{
//     QTime time;
//     time.start();



    int nr= imageHeight; // number of rows
    int nc= imageWidth; // number of columns
    int lenght = 2;
    char* destdata;
    int j =0;
    int i =0;
    int x = 0;
    int y = 0;
    uchar* srcdata;
    tablePos tempTablePos;
    int b;
    int g;
    int r;



    unsigned short int t;
    long int location=0;
    int widthOfLine = srcWidth*lenght;

    int out_buffer_size = 4 * srcWidth * srcheight;

//    int out_buffer_size = srcWidth * srcheight *4;

//      g_g2d_src_cale_buffer = g2d_alloc(out_buffer_size, 0);

//        if (g_g2d_src_cale_buffer == 0)
//        {
//            qDebug("can not malloc memory for g2d dst\n");
//            g2d_free(g_g2d_src_cale_buffer);
//            exit(-1);
//        }

//        memset(g_g2d_src_cale_buffer->buf_vaddr,0x0,out_buffer_size);

//        g_g2d_desc_cale_buffer = g2d_alloc(out_buffer_size, 0);

//          if (g_g2d_desc_cale_buffer == 0)
//          {
//              qDebug("can not malloc memory for g2d dst\n");
//              g2d_free(g_g2d_desc_cale_buffer);
//              exit(-1);
//          }

//    memset(g_g2d_desc_cale_buffer->buf_vaddr,0x0,out_buffer_size);


    if (status == false)
    {
        status = true;

        QCoreApplication::processEvents();



        if (fb_dev.map_fb != MAP_FAILED)
        {
            munmap(fb_dev.map_fb,fb_dev.finfo.smem_len);
        }

        if (fb_dev.fdfd > 0)
        {
            ::close(fb_dev.fdfd);
        }
        fb_dev.fdfd=-1;


        if (fd_fb_bg > 0)
        {
            ::close(fd_fb_bg);
        }
        fd_fb_bg = 0;

        if (init_dev(&fb_dev) == TPASS)
        {
//            qDebug()<<"G2DThread init_dev is pass";
//                ::usleep(50000);
//            memset(fb_dev.map_fb, 0xff, fb_dev.finfo.smem_len);
//                QCoreApplication::processEvents();
         }
        else
        {
            status = false;
            return;
        }

//        memcpy(g_g2d_temp_buffer->buf_vaddr,cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr,out_buffer_size);


        if (firstTimeStatus == true)
        {
//            fb_dev.fdfd=-1;
//            ::usleep(500000);
//            if (init_dev(&fb_dev) == TPASS)
//            {
//                qDebug()<<"G2DThread init_dev is pass";
//                ::usleep(50000);
//                memset(fb_dev.map_fb, 0xff, fb_dev.finfo.smem_len);
//                QCoreApplication::processEvents();
//             }
//            else
//            {
//                qDebug()<<"G2DThread init_dev is fail";
//                ::usleep(50000);
//                firstTimeStatus = true;
//                status = false;
//                return;
//            }

//                int out_buffer_size = 2 * srcWidth * srcheight;

//                  g_g2d_src_cale_buffer = g2d_alloc(out_buffer_size, 0);

//                    if (!g_g2d_src_cale_buffer)
//                    {
//                        qDebug("can not malloc memory for g2d src\n");
//                        return ;
//                    }

//                    memset(g_g2d_src_cale_buffer->buf_vaddr,0xff,out_buffer_size);

//                    g_g2d_desc_cale_buffer = g2d_alloc(out_buffer_size, 0);

//                      if (!g_g2d_desc_cale_buffer)
//                      {
//                          qDebug("can not malloc memory for g2d dst\n");
//                          g2d_free(g_g2d_src_cale_buffer);
//                          return;
//                      }

//                memset(g_g2d_desc_cale_buffer->buf_vaddr,0xff,out_buffer_size);

                 firstTimeStatus = false;
        }
//        QTime time;
//        time.start();

        //qDebug()<<time.elapsed()/1000.0<<"s";

        //QTime time;
        //time.start();
#if 1
            //拼接完后的图像。
            for (j=0; j<nr; j++)
            {

//                 QCoreApplication::processEvents();

                    unsigned int offsetLine = (j+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                    for (i=0; i<nc; i++)
                    {
                            tempTablePos = loopupTable[j][i];

                            x = tempTablePos.x;
                            y = tempTablePos.y;

                            if (!( (x == 0) && (y == 0) ))
                            {
                                srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr))+((y)*widthOfLine)+((x)*lenght);
//                                srcdata = ((uchar *)(g_g2d_temp_buffer->buf_vaddr))+((y)*widthOfLine)+((x)*lenght);

//                                location=(i+(fb_dev.vinfo.xoffset))*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
                                location=(0+i+(fb_dev.vinfo.xoffset))*lenght+offsetLine;
                                *((unsigned short int *)(fb_dev.map_fb+location))=*((unsigned short int *)(srcdata));//rgb_24_2_565(r,g,b);
//                                memcpy(fb_dev.map_fb+location,srcdata,4);
                            }
                    } // end of row
            }
#endif

#if 0
                    //鱼眼效果图
                    //srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr))+(((j))*widthOfLine)+((i)*lenght);
                    srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;

                    for (j=0; j<srcheight; j++)
                    {
                        if (j<720)
                        {
                            int offsetLine = (j+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;

                            //i = 500;
                            location=(0+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
                            memcpy(fb_dev.map_fb+location,srcdata,srcWidth*4);
                            srcdata += srcWidth * 4;
                        }
                    }

 #endif

//                    QTime time;
//                    time.start();
#if 1
//                    QCoreApplication::processEvents();
                    //朝向向左
                    if (whichCamera == topCameraWhich)
                    {
                        int tmpCounti = 0;
                        int heightLimit = srcheight - 600;
                        int tmpOffset;
                        tmpOffset = (800);
                        int tmpLenght = tmpOffset *lenght;
                        int basePoint =  srcWidth/2;
                        int widthLimit = tmpOffset / 2;

                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
//                        srcdata = ((uchar *)(g_g2d_temp_buffer->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
                        for(int i=0;i<srcheight;i++)
                        {
//                            QCoreApplication::processEvents();


                            if (i > heightLimit)
                            {
                                uchar * tmpdata;
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                for(int j=0;j<srcWidth;j++)
                                {   
                                    if ( (j>(basePoint-(800/2)))&& (j<(basePoint+(800/2))))
//                                    if ( (j>(240))&& (j<(1040)))
                                    {

                                        tmpdata =  srcdata + (j * lenght);
                                        location=((0+400+tmpCountj+(fb_dev.vinfo.xoffset))*lenght)+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                        memcpy(fb_dev.map_fb+location,tmpdata,lenght*tmpOffset);
                                        j+=tmpOffset;
//                                        tmpCountj++;
//                                        j++;
                                        break;
                                    }
                                }
//                                qDebug()<<"tmpCountj = "<<tmpCountj;
                                tmpCounti++;
                            }
                            srcdata += srcWidth*lenght;
                        }
                    }

//                    QCoreApplication::processEvents();
                    //朝向向右
                    if (whichCamera == bottomCameraWhich)
                    {
                        int tmpCounti = 0;
                        int heightLimit = 600;
                        int tmpOffset = 800;
                        int basePoint =  srcWidth/2;
                        int tmpLenght = tmpOffset*lenght;
//                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
//                        srcdata = ((uchar *)(g_g2d_temp_buffer->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
//                        memcpy( g_g2d_src_cale_buffer->buf_vaddr, srcdata,  srcheight*widthOfLine);

//                        QTime time;
//                         time.start();

                         if (matrixRotate(cameraFourInThread->g_g2d_dst_buffer[frameOffset], (whichCamera)*srcheight*widthOfLine,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcWidth,srcheight,G2D_ROTATION_180) !=TPASS)
                        {
                             status = false;
                             return ;
                         }
//                         qDebug()<<time.elapsed()/1000.0<<"s";

                        srcdata = ((uchar *)(g_g2d_desc_cale_buffer->buf_vaddr));
                        for(int i=0;i<srcheight;i++)
                        {
//                            QCoreApplication::processEvents();

                            if (i < heightLimit)
                            {
                                uchar * tmpdata;
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                for(int j=0;j<srcWidth;j++)
                                {
                                    if ( (j>(basePoint-(800/2)))&& (j<(basePoint+(800/2))))
                                    {

                                        tmpdata =  srcdata + j * lenght;
                                        location=(0+400+tmpCountj+fb_dev.vinfo.xoffset)*lenght+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                        memcpy(fb_dev.map_fb+location,tmpdata,tmpLenght);
                                        j+=tmpOffset;
                                        tmpCountj++;
                                    }
                                }
                                tmpCounti++;
                            }
                            srcdata += srcWidth*lenght;
                        }
                    }

//                    QCoreApplication::processEvents();
                    //朝向向前
                    if (whichCamera == leftCameraWhich)
                    {
//                                            QTime time;
//                                            time.start();

//                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
//                        srcdata = ((uchar *)(g_g2d_temp_buffer->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
//                        memcpy( g_g2d_src_cale_buffer->buf_vaddr, srcdata,  srcheight*widthOfLine);

//                        qDebug()<<time.elapsed()/1000.0<<"s";

//                        QTime time;
//                        time.start();

                         if ( matrixRotate(cameraFourInThread->g_g2d_dst_buffer[frameOffset], (whichCamera)*srcheight*widthOfLine,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcheight,srcWidth,G2D_ROTATION_270) !=TPASS)
                         {
                             qDebug()<<" matrixRotate for left fail";
                              status = false;
                              return ;
                          }
//                        matrixRotate(g_g2d_src_cale_buffer,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcheight,srcWidth,G2D_ROTATION_270);
                        srcdata = ((uchar *)(g_g2d_desc_cale_buffer->buf_vaddr));


//                        qDebug()<<time.elapsed()/1000.0<<"s";

                        int tmpCounti = 0;
                        int heightLimit = 800-800;
                        int basePoint =  1280/2;
                        for(int i=0;i<1280;i++)
                        {
                //            int basePoint =  imgRatate->width()/2;

                            if ( (i > (basePoint - (600/2)))&& (i < (basePoint + (600/2))) )
//                            if ( i< heightLimit)
                            {
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                uchar * tmpdata;
                                for(int j=0;j<800;j++)
                                {
                                    if ( j> heightLimit)
//                                       if ( (j > (basePoint - (600/2)))&& (j < (basePoint + (600/2))) )
                                    {
                                        tmpdata =  srcdata + j * lenght;
                                        location=(0+400+tmpCountj+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                        memcpy(fb_dev.map_fb+location,tmpdata,800*lenght);
                                        j=800;
                                        tmpCountj++;
                                       break;
                                    }
                                }

                                tmpCounti++;
                            }
                            srcdata += srcheight * lenght;
                        }
                    }

                    //朝向向后
                    if (whichCamera == rightCameraWhich)
                    {
//                        srcdata = ((uchar *)(cameraFourInThread->g_g2d_dst_buffer[frameOffset]->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
//                        srcdata = ((uchar *)(g_g2d_temp_buffer->buf_vaddr)) + (whichCamera)*srcheight*widthOfLine;
//                        memcpy( g_g2d_src_cale_buffer->buf_vaddr, srcdata,  srcheight*widthOfLine);
//                        matrixRotate(g_g2d_src_cale_buffer,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcheight,srcWidth,G2D_ROTATION_90);
//                        QTime time;
//                        time.start();

                        if (matrixRotate(cameraFourInThread->g_g2d_dst_buffer[frameOffset], (whichCamera)*srcheight*widthOfLine,srcWidth,srcheight,g_g2d_desc_cale_buffer,srcheight,srcWidth,G2D_ROTATION_90) != TPASS)
                        {
                            qDebug()<<" matrixRotate for right fail";
                             status = false;
                             return ;
                         }

//                        qDebug()<<time.elapsed()/1000.0<<"s";

                        srcdata = ((uchar *)(g_g2d_desc_cale_buffer->buf_vaddr));

                        int tmpCounti = 0;
                        int heightLimit = 0;
                        int basePoint =  1280/2;
                        for(int i=0;i<1280;i++)
                        {
                //            int basePoint =  imgRatate->width()/2;

                            if ( (i > (basePoint - (600/2)))&& (i < (basePoint + (600/2))) )
//                            if ( i< heightLimit)
                            {
                                int offsetLine = (tmpCounti+fb_dev.vinfo.yoffset)*fb_dev.finfo.line_length;
                                int tmpCountj = 0;
                                uchar * tmpdata;
                                for(int j=0;j<800;j++)
                                {
                                    if ( j > heightLimit)
//                                       if ( (j > (basePoint - (600/2)))&& (j < (basePoint + (600/2))) )
                                    {
                                        tmpdata =  srcdata + j * lenght;
                                        location=(400+tmpCountj+fb_dev.vinfo.xoffset)*(fb_dev.vinfo.bits_per_pixel/8)+offsetLine;
//                                        *((unsigned int *)(fb_dev.map_fb+location))=*((unsigned int *)(tmpdata));//rgb_24_2_565(r,g,b);
                                         memcpy(fb_dev.map_fb+location,tmpdata,800*lenght);
                                        tmpCountj++;
                                        j=800;
                                        break;
                                    }
                                }

                                tmpCounti++;
                            }
                            srcdata += srcheight * lenght;
                        }
                    }

#endif


        countFrameSpeed++;

//        QCoreApplication::processEvents();
//        if (countFrameSpeed == 1)
        {
//            qDebug()<<time.elapsed()/1000.0<<"s";
//            countFrameSpeed = 0;
        }

        if (myCloseWindows == true)
        {
            qDebug()<<"set FB_BLANK_NORMAL";
            ioctl(fb_dev.fdfd, FBIOBLANK, FB_BLANK_NORMAL);
        }

//        munmap(fb_dev.map_fb,fb_dev.finfo.smem_len);
//        ::close(fb_dev.fdfd);
//        ::close(fd_fb_bg);
        status = false;
    }

    if (myCloseWindows ==true)
    {
        qDebug()<<"exit for G2DThread";
        this->quit();
        this->exit(0);
    }

//    qDebug()<<time.elapsed()/1000.0<<"s";

//    g2d_free(g_g2d_desc_cale_buffer);
//    g2d_free(g_g2d_src_cale_buffer);
}

//void G2DThread::run()
//{
//    while(1)
//    {
//        usleep(1000);
//    }
//}

void G2DThread::ClearScreen()
{
    ioctl(fb_dev.fdfd, FBIOBLANK, FB_BLANK_NORMAL);
    myCloseWindows = true;
    QCoreApplication::processEvents(); 
}

G2DThread::~G2DThread()
{

#if 0
    struct mxcfb_gbl_alpha alpha;
        /* Overlay setting */
        alpha.alpha = 0;
        alpha.enable = 0;
        if (ioctl(fd_fb_bg, MXCFB_SET_GBL_ALPHA, &alpha) < 0) {
            qDebug("Set global alpha failed\n");
            ::close(fd_fb_bg);
            return;
        }

         ioctl(fd_fb_bg, FBIOGET_VSCREENINFO, &g_screen_info);
#endif

         qDebug("G2DThread::~G2DThread() function");

         ioctl(fb_dev.fdfd, FBIOBLANK, FB_BLANK_NORMAL);

//            munmap(fb_dev.map_fb,fb_dev.finfo.smem_len);
//           ::close(fb_dev.fdfd);
//           ::close(fd_fb_bg);

                g2d_free(g_g2d_desc_cale_buffer);
                g2d_free(g_g2d_src_cale_buffer);
//                g2d_free(g_g2d_temp_buffer);

//    cameraFourInThread->quit();
//    cameraFourInThread->~CameraWorkFourin();
}
