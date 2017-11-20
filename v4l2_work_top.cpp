/*
 * main.cpp
 *
 *  Created on: 2016年9月5日
 *      Author: hh
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <getopt.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>
#include <linux/videodev2.h>
#include <pthread.h>

#include "v4l2_work_top.h"

#define CLEAR(x) memset (&(x), 0, sizeof (x))

//static pthread_mutex_t hashlock = PTHREAD_MUTEX_INITIALIZER;


//////////////////////////////////////////////////////
CameraWorkTop::CameraWorkTop(QObject *parent)
{
    memset(camera.dev_name, '\0', sizeof(camera.dev_name));
    this->camera.buf[0] = NULL;
    this->camera.buf[1] = NULL;
    this->camera.buf[2] = NULL;
    this->camera.buf[3] = NULL;
    this->camera.data = NULL;
    this->camera.fd = -1;
    this->camera.n_buffers = 0;
}


//////////////////////////////////////////////////////
//获取一帧数据
//////////////////////////////////////////////////////
int CameraWorkTop::read_frame(CameraStruct *camera)
{

}

int CameraWorkTop::v4l_capture_setup(CameraStruct *camera)
{

        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_control ctl;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
        struct v4l2_requestbuffers req;
    //struct v4l2_dbg_chip_ident chip;
        struct v4l2_streamparm parm;
        v4l2_std_id id;
        unsigned int min;

        if (ioctl (camera->fd, VIDIOC_QUERYCAP, &cap) < 0) {
                if (EINVAL == errno) {
                        qDebug ("%s is no V4L2 device\n",
                                        camera->dev_name);
                        return TFAIL;
                } else {
                        qDebug( "%s isn not V4L device,unknow error\n",
                        camera->dev_name);
                        return TFAIL;
                }
        }

        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                qDebug ( "%s is no video capture device\n",
                        camera->dev_name);
                return TFAIL;
        }

        if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                qDebug ( "%s does not support streaming i/o\n",
                        camera->dev_name);
                return TFAIL;
        }

        if (ioctl(camera->fd, VIDIOC_S_INPUT, &g_input) < 0) {
                qDebug("VIDIOC_S_INPUT failed\n");
                close(camera->fd);
                return TFAIL;
        }

        memset(&cropcap, 0, sizeof(cropcap));
        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl (camera->fd, VIDIOC_CROPCAP, &cropcap) < 0) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (ioctl (camera->fd, VIDIOC_S_CROP, &crop) < 0) {
                        switch (errno) {
                                case EINVAL:
                                        /* Cropping not supported. */
                                        qDebug ( "%s  doesn't support crop\n",
                                                camera->dev_name);
                                        break;
                                default:
                                        /* Errors ignored. */
                                        break;
                        }
                }
        } else {
                /* Errors ignored. */
        }

        parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        parm.parm.capture.timeperframe.numerator = 1;
        parm.parm.capture.timeperframe.denominator = 0;
        parm.parm.capture.capturemode = 0;
        if (ioctl(camera->fd, VIDIOC_S_PARM, &parm) < 0) {
                qDebug("VIDIOC_S_PARM failed\n");
                close(camera->fd);
                return TFAIL;
        }

        memset(&fmt, 0, sizeof(fmt));

        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = 0;
        fmt.fmt.pix.height = 0;
        fmt.fmt.pix.pixelformat = g_in_fmt;
        fmt.fmt.pix.field = V4L2_FIELD_ANY;
        if (ioctl (camera->fd, VIDIOC_S_FMT, &fmt) < 0) {
                qDebug ( "%s iformat not supported \n",
                        camera->dev_name);
                return TFAIL;
        }

        /* Note VIDIOC_S_FMT may change width and height. */

        /* Buggy driver paranoia. */
        min = fmt.fmt.pix.width * 2;
        if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;

        min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
        if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;

        if (ioctl(camera->fd, VIDIOC_G_FMT, &fmt) < 0) {
                qDebug("VIDIOC_G_FMT failed\n");
                close(camera->fd);
                return TFAIL;
        }

        file_length = fmt.fmt.pix.sizeimage;

        g_in_width = fmt.fmt.pix.width;
        g_in_height = fmt.fmt.pix.height;
        qDebug("g_in_width = %d, g_in_height = %d.\r\n", g_in_width, g_in_height);

        if (g_g2d_render) {
                switch (g_in_fmt) {
                        case V4L2_PIX_FMT_RGB565:
                                g_frame_size = g_in_width * g_in_height * 2;
                                //g_g2d_fmt = G2D_RGB565;
                                break;

                        case V4L2_PIX_FMT_UYVY:
                                g_frame_size = g_in_width * g_in_height * 2;
                                //g_g2d_fmt = G2D_UYVY;
                                break;

                        case V4L2_PIX_FMT_YUYV:
                                g_frame_size = g_in_width * g_in_height * 2;
                                //g_g2d_fmt = G2D_YUYV;
                                break;

                        case V4L2_PIX_FMT_YUV420:
                                g_frame_size = g_in_width * g_in_height * 3 / 2;
                                //g_g2d_fmt = G2D_I420;
                                break;

                        case V4L2_PIX_FMT_NV12:
                                g_frame_size = g_in_width * g_in_height * 3 / 2;
                                //g_g2d_fmt = G2D_NV12;
                                break;

                        default:
                                qDebug("Unsupported format.\n");
                                return TFAIL;
                }
        }

        memset(&req, 0, sizeof (req));
        req.count = g_capture_num_buffers;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = (v4l2_memory)g_mem_type;
        if (ioctl (camera->fd, VIDIOC_REQBUFS, &req) < 0) {
                if (EINVAL == errno) {
                        qDebug ( "%s does not support "
                                         "memory mapping\n", camera->dev_name);
                        return TFAIL;
                } else {
                        qDebug ( "%s does not support "
                                         "memory mapping, unknow error\n", camera->dev_name);
                        return TFAIL;
                }
        }

        if (req.count < 2) {
                qDebug ( "Insufficient buffer memory on %s\n",
                         camera->dev_name);
                return TFAIL;
        }

        return TPASS;
}

int CameraWorkTop::start_capturing(CameraStruct *camera)
{
        unsigned int i;
        struct v4l2_buffer buf;
        enum v4l2_buf_type type;

        for (i = 0; i < g_capture_num_buffers; i++) {

         camera->buf[i] = (struct testbuffer*) calloc(1, file_length); //内存中建立对应空间

            if (camera->buf[i] == NULL)
            {
                qDebug("camera.buf is NULL\n");
                close(camera->fd);
                return TFAIL;
            }

                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory =(v4l2_memory) g_mem_type;
                buf.index = i;
                if (g_mem_type == V4L2_MEMORY_USERPTR) {
                        buf.length = camera->buf[i]->length;
                        buf.m.userptr = (unsigned long)camera->buf[i]->offset;
                }
                if (ioctl(camera->fd, VIDIOC_QUERYBUF, &buf) < 0) {
                        qDebug("VIDIOC_QUERYBUF error\n");
                        return TFAIL;
                }

                if (g_mem_type == V4L2_MEMORY_MMAP) {
                        camera->buf[i]->length = buf.length;
                        camera->buf[i]->offset = (size_t) buf.m.offset;
                        camera->buf[i]->start = (unsigned char *) mmap(NULL, camera->buf[i]->length,
                            PROT_READ | PROT_WRITE, MAP_SHARED,
                            camera->fd, camera->buf[i]->offset);
                        memset(camera->buf[i]->start, 0xFF, camera->buf[i]->length);
                }
        }

        for (i = 0; i < g_capture_num_buffers; i++) {
                memset(&buf, 0, sizeof (buf));
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = (v4l2_memory)g_mem_type;
                buf.index = i;
                if (g_mem_type == V4L2_MEMORY_USERPTR)
                        buf.m.offset = (unsigned int)camera->buf[i]->start;
                else
                        buf.m.offset = camera->buf[i]->offset;
                buf.length = camera->buf[i]->length;
                if (ioctl(camera->fd, VIDIOC_QBUF, &buf) < 0) {
                        qDebug("VIDIOC_QBUF error\n");
                        return TFAIL;
                }
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl (camera->fd, VIDIOC_STREAMON, &type) < 0) {
                qDebug("VIDIOC_STREAMON error\n");
                return TFAIL;
        }
        return TPASS;
}



//工作线程
void CameraWorkTop::run()
{
//    memset(this->camera.dev_name, '\0', sizeof(this->Camera.dev_name));

//    setCameraName(&(this->camera),"/dev/video0");

    if ((camera.fd = open(camera.dev_name, O_RDWR, 0)) < 0) {
    qDebug("Unable to open %s\n", camera.dev_name);
            return;
    }

    if (v4l_capture_setup(&(this->camera)) < 0) {
            qDebug("Setup v4l capture failed.\n");
            return ;
    }

    if (start_capturing(&(this->camera)) < 0) {
            qDebug("start_capturing failed\n");
            return;
    }

    struct v4l2_buffer capture_buf;

    for (int i = 0; i < g_frame_count; i++) {
            memset(&capture_buf, 0, sizeof(capture_buf));
            capture_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            capture_buf.memory = (v4l2_memory)g_mem_type;

            if (ioctl(camera.fd, VIDIOC_DQBUF, &capture_buf) < 0) {
                    qDebug("VIDIOC_DQBUF failed.\n");
                    return ;
            }

//            FILE *file_fd;
//            file_fd = fopen(save_file, "w+"); //图片文件名
//            fwrite(capture_buffers[capture_buf.index].start, capture_buffers[capture_buf.index].length, 1, file_fd); //将其写入文件中
//            fclose(file_fd);


//            memcpy(((uchar*)(g_g2d_src_buffer[offsetCount]->buf_vaddr) + (theCamera * 2 *width *height)),capture_buffers[capture_buf.index].start, capture_buffers[capture_buf.index].length);
//            memcpy(((uchar*)(g_g2d_src_buffer[offsetCount]->buf_vaddr) + (theCamera * 2 *width *height)),((camera->buf)[capture_buf.index])->start, ((camera->buf)[capture_buf.index])->length);


            //写入内存文件
            memcpy(((camera.data) + ((offsetCount) * 2 *width *height)),((camera.buf)[capture_buf.index])->start, ((camera.buf)[capture_buf.index])->length);
            //memcpy(((camera->data)),((camera->buf)[buf.index]).start, ((camera->buf)[buf.index]).length);
            emit whichTopFrame(offsetCount);
            offsetCount++;

            if (offsetCount % count == 0)
            {
                offsetCount = 0;
            }


            if (ioctl(camera.fd, VIDIOC_QBUF, &capture_buf) < 0) {
                    qDebug("VIDIOC_QBUF failed\n");
                    return ;
            }
    }

    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(camera.fd, VIDIOC_STREAMOFF, &type);

    for (int i = 0; i < g_capture_num_buffers; i++) {
        munmap(camera.buf[i]->start, camera.buf[i]->length);
    }

    close(camera.fd);

    return ;
}

void CameraWorkTop::setCameraName(CameraStruct *camera,QString name)
{
    this->cameraName = name;
    memcpy(&(camera->dev_name),cameraName.toLatin1().data(),sizeof(camera->dev_name));
}


void CameraWorkTop::setCameraName(QString name)
{
    this->cameraName = name;
    memcpy(&camera.dev_name,cameraName.toLatin1().data(),sizeof(camera.dev_name));
}

unsigned char * CameraWorkTop::setCameraData(unsigned char **data)
{
    *data = new unsigned char [count * 2 * width *height];
    camera.data = *data;
    return *data;
}

CameraWorkTop::~CameraWorkTop()
{

}
