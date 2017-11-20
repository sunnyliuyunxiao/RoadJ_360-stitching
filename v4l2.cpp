/*
 * main.cpp
 *
 *  Created on: 2016年8月11日
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

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
    void * start;
    size_t length;
};

static char dev_name[20]; //摄像头设备名
static int fd = -1;
struct buffer * buffers = NULL;
static unsigned int n_buffers = 0;

FILE *file_fd;
static unsigned long file_length;
//////////////////////////////////////////////////////
//获取一帧数据
//////////////////////////////////////////////////////
static int read_frame(void) {
    struct v4l2_buffer buf;

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    /*8.出队列以取得已采集数据的帧缓冲，取得原始采集数据。VIDIOC_DQBUF*/
    int ff = ioctl(fd, VIDIOC_DQBUF, &buf);
    if (ff < 0)
        fprintf(stderr,"read buffer is failture\n"); //出列采集的帧缓冲

    assert(buf.index < n_buffers);
    fprintf(stdout,"buf.index dq is %d,\n", buf.index);

    fwrite(buffers[buf.index].start, buffers[buf.index].length, 1, file_fd); //将其写入文件中
    /*9.将缓冲重新入队列尾,这样可以循环采集。VIDIOC_QBUF*/
    ff = ioctl(fd, VIDIOC_QBUF, &buf); //再将其入列
    if (ff < 0) //把数据从缓存中读取出来
        fprintf(stderr,"failture VIDIOC_QBUF\n");

    fprintf(stdout,"read frame is ok\n");
    return 1;
}

int main(int argc, char ** argv) {
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    unsigned int i;
    int oc; /*选项字符 */
    struct v4l2_input input;
    int setVideoInput = 0;
    unsigned int min;
    int ret;
    struct v4l2_standard standard;
    v4l2_std_id std_id;

    memset(dev_name, '\0', sizeof(dev_name));

    while ((oc = getopt(argc, argv, "d:c:")) != -1) {
        switch (oc) {
            case 'd':
                strcpy(dev_name, optarg);
                break;

            case 'c':
                setVideoInput = atoi(optarg);
                break;
        }
    }

    enum v4l2_buf_type type;

    file_fd = fopen("test-mmap.jpg", "w+"); //图片文件名

    if (file_fd == NULL) {
        fprintf(stderr,"you can not new jpg\n");
        exit(EXIT_FAILURE);
    }

    /*1.打开设备文件。 int fd=open(”/dev/video0″,O_RDWR);*********/

    if (*dev_name == '\0') {
        fprintf(stderr,"you must use the i argment for command line\n");
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }

    fd = open(dev_name, O_RDWR /* required */  | O_NONBLOCK , 0); //打开设备

    if (fd == -1) {
        fprintf(stderr,"you can not open %s\n", dev_name);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"open this device is ok\n");

    /*2.取得设备的capability，看看设备具有什么功能，比如是否具有视频输入,或者音频输入输出等。VIDIOC_QUERYCAP,struct v4l2_capability*/
    int ff = ioctl(fd, VIDIOC_QUERYCAP, &cap); //获取摄像头参数
    if (ff < 0) {
        fprintf(stderr,"failture VIDIOC_QUERYCAP\n");
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    //打印设备的capability
    fprintf(stdout,"DriverName:%s\nCard Name:%s\nBus info:%s\nDriverVersion:%u.%u.%u\n",cap.driver,cap.card,cap.bus_info,(cap.version>>16)&0xff,(cap.version>>8)&0xff,cap.version&0xff);

    //判断是不是摄像头
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        fprintf (stderr, "%s is no video capture device\n",dev_name);
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"this device is a capture\n");

    //判断是不是流设备
    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        fprintf (stderr, "%s does not support streaming i/o\n",dev_name);
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"this capture is stream device\n");


    //读取当前的视频输入格式
    if (ioctl(fd, VIDIOC_G_STD, &std_id) < 0)
    {
        fprintf(stderr,"VIDIOC_G_STD failed\n");
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"getting VIDIOC_G_STD = %llx is ok \n",std_id);

    //枚举当前的视频输入格式，得名字
    memset(&standard, 0, sizeof(standard));
    standard.index = 0;

    while (0 == ioctl(fd, VIDIOC_ENUMSTD, &standard)) {
        if (standard.id & std_id) {
               printf("Current video standard: %s\n", standard.name);
               break;
        }

        standard.index++;
    }

    //设置输入制式
    std_id = V4L2_STD_NTSC;
    if (ioctl(fd, VIDIOC_S_STD, &std_id) < 0)
    {
	    fprintf(stderr,"VIDIOC_S_STD failed\n");
	    close(fd);
	    fclose(file_fd);
	    exit(EXIT_FAILURE);
    }
    fprintf(stdout,"setting VIDIOC_S_STD = V4L2_STD_NTSC is ok\n");

    //取得当前驱动的格式
    CLEAR(fmt);
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd,VIDIOC_G_FMT,&fmt);
    if (ret == -1)
    {
        fprintf(stderr,"ioctl VIDIOC_G_FMT fail\n");
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"Currentdata format information:\n\twidth:%d\n\theight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);

    /*3.设置视频的制式和帧格式，制式包括PAL，NTSC，帧的格式个包括宽度和高度等。*/
    struct v4l2_fmtdesc fmt1;
    memset(&fmt1, 0, sizeof(fmt1));
    fmt1.index = 0;
    fmt1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //获取当前驱动支持的视频格式
    while(ioctl(fd,VIDIOC_ENUM_FMT,&fmt1)!=-1)
    {
        if(fmt1.pixelformat & fmt.fmt.pix.pixelformat)
        {
            fprintf(stdout,"\tformat:%s = %d \n",fmt1.description,fmt1.pixelformat);
            break;
        }

        fprintf(stdout,"\tformat:%s = %d \n",fmt1.description,fmt1.pixelformat);

        fmt1.index++;
    }    //帧的格式，比如宽度，高度等

    CLEAR(fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //数据流类型，必须永远是V4L2_BUF_TYPE_VIDEO_CAPTURE
    fmt.fmt.pix.width = 720; //宽，必须是16的倍数
    fmt.fmt.pix.height = 480; //高，必须是16的倍数
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; //视频数据存储类型//V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_YVU420;//V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    fprintf(stdout,"\n\n i want setting ");
    fprintf(stdout,"fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE\n");
    fprintf(stdout,"fmt.fmt.pix.width = 720\n");
    fprintf(stdout,"fmt.fmt.pix.height = 480\n");
    fprintf(stdout,"fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV\n");
    fprintf(stdout,"fmt.fmt.pix.field = V4L2_FIELD_INTERLACED\n");

    //设置当前驱动的频捕获格式
    ff = ioctl(fd, VIDIOC_S_FMT, &fmt);
    if (ff < 0)
    {
        fprintf(stderr,"failture VIDIOC_S_FMT\n");
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"\n\nVIDIOC_S_FMT is ok\n");

    //重新读取当前投置的值
    CLEAR(fmt);
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd,VIDIOC_G_FMT,&fmt);
    if (ret == -1)
    {
        fprintf(stderr,"ioctl VIDIOC_G_FMT fail\n");
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"Currentdata format information:\n\twidth:%d\n\theight:%d\n",fmt.fmt.pix.width,fmt.fmt.pix.height);


    //计算图片大小
    min = fmt.fmt.pix.width * 2;
    if (fmt.fmt.pix.bytesperline < min)
	fmt.fmt.pix.bytesperline = min;
    min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
    if (fmt.fmt.pix.sizeimage < min)
	fmt.fmt.pix.sizeimage = min;

    file_length = fmt.fmt.pix.sizeimage;

    /*4.向驱动申请帧缓冲，一般不超过5个。struct v4l2_requestbuffers*/
    struct v4l2_requestbuffers req;
    CLEAR(req);
    req.count = 1; //缓存数量，也就是说在缓存队列里保持多少张照片
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP; //或V4L2_MEMORY_USERPTR

    ff = ioctl(fd, VIDIOC_REQBUFS, &req); //申请缓冲，count是申请的数量
    if (ff < 0)
    {
        fprintf(stderr,"failture VIDIOC_REQBUFS\n");
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    if (req.count < 1)
    {
        fprintf(stderr,"Insufficient buffer memory\n");
        close(fd);
        fclose(file_fd);
        exit(EXIT_FAILURE);
    }
    fprintf(stdout,"VIDIOC_REQBUFS is ok\n");


    buffers = (struct buffer*) calloc(req.count, sizeof(*buffers)); //内存中建立对应空间
    /*5.将申请到的帧缓冲映射到用户空间，这样就可以直接操作采集到的帧了，而不必去复制。mmap*/
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
        struct v4l2_buffer buf; //驱动中的一帧
        CLEAR(buf);
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;
        //把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
        if (-1 == ioctl(fd, VIDIOC_QUERYBUF, &buf)) //映射用户空间
        {
            fprintf(stderr,"VIDIOC_QUERYBUF error\n");
            close(fd);
            fclose(file_fd);
            exit(EXIT_FAILURE);
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start = mmap(NULL /* start anywhere */, buf.length,
        PROT_READ | PROT_WRITE /* required */,
        MAP_SHARED /* recommended */, fd, buf.m.offset); //通过mmap建立映射关系,返回映射区的起始地址

        if (MAP_FAILED == buffers[n_buffers].start)
        {
            fprintf(stderr,"mmap failed\n");
            close(fd);
            fclose(file_fd);
            exit(EXIT_FAILURE);
        }
    }
    /*6.将申请到的帧缓冲全部入队列，以便存放采集到的数据.VIDIOC_QBUF,struct v4l2_buffer*/
    for (i = 0; i < n_buffers; ++i) {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
        //把数据从缓存中读取出来
        if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) //申请到的缓冲进入列队
            fprintf(stderr,"VIDIOC_QBUF failed\n");
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*7.开始视频的采集。VIDIOC_STREAMON*/
    if (-1 == ioctl(fd, VIDIOC_STREAMON, &type)) //开始捕捉图像数据
        fprintf(stderr,"VIDIOC_STREAMON failed\n");

    for (;;) //这一段涉及到异步IO
    {
        fd_set fds;
        struct timeval tv;
        int r;

        FD_ZERO(&fds); //将指定的?件描述符集清空
        FD_SET(fd, &fds); //在文件描述符集合中增鍔????个新的文件描述符

        /* Timeout. */
        tv.tv_sec = 2;
        tv.tv_usec = 0;

        r = select(fd + 1, &fds, NULL, NULL, &tv); //判断是否可读（即摄像头是否准备好），tv是定时

        if (-1 == r) {
            if (EINTR == errno)
                continue;
            printf("select err\n");
        }
        if (0 == r) {
            fprintf(stderr, "select timeout from %s\n", dev_name);
            close(fd);
            fclose(file_fd);
            exit(EXIT_FAILURE);
        }

        if (read_frame()) //如果可读，执行read_frame ()函数，并跳出循环
            break;
    }

    for (i = 0; i < n_buffers; ++i)
        if (-1 == munmap(buffers->start, buffers->length))
            printf("munmap error");
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    /*10.停止视频的采集。VIDIOC_STREAMOFF*/
    if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
        printf("VIDIOC_STREAMOFF");

    /*11.关闭视频设备。close(fd);*/
    close(fd);
    fclose(file_fd);

    exit(EXIT_SUCCESS);
    return 0;
}

