
#ifndef __ASM_ARCH_MXCFB_H__
#define __ASM_ARCH_MXCFB_H__

#include <linux/fb.h>

#define FB_SYNC_OE_LOW_ACT	0x80000000
#define FB_SYNC_CLK_LAT_FALL	0x40000000
#define FB_SYNC_DATA_INVERT	0x20000000
#define FB_SYNC_CLK_IDLE_EN	0x10000000
#define FB_SYNC_SHARP_MODE	0x08000000
#define FB_SYNC_SWAP_RGB	0x04000000
#define FB_ACCEL_TRIPLE_FLAG	0x00000000
#define FB_ACCEL_DOUBLE_FLAG	0x00000001

struct mxcfb_gbl_alpha {
    int enable;
    int alpha;
};

struct mxcfb_loc_alpha {
    int enable;
    int alpha_in_pixel;
    unsigned long alpha_phy_addr0;
    unsigned long alpha_phy_addr1;
};

struct mxcfb_color_key {
    int enable;
    __u32 color_key;
};

struct mxcfb_pos {
    __u16 x;
    __u16 y;
};

struct mxcfb_gamma {
    int enable;
    int constk[16];
    int slopek[16];
};

struct mxcfb_rect {
    __u32 top;
    __u32 left;
    __u32 width;
    __u32 height;
};

#define GRAYSCALE_8BIT				0x1
#define GRAYSCALE_8BIT_INVERTED			0x2
#define GRAYSCALE_4BIT                          0x3
#define GRAYSCALE_4BIT_INVERTED                 0x4

#define AUTO_UPDATE_MODE_REGION_MODE		0
#define AUTO_UPDATE_MODE_AUTOMATIC_MODE		1

#define UPDATE_SCHEME_SNAPSHOT			0
#define UPDATE_SCHEME_QUEUE			1
#define UPDATE_SCHEME_QUEUE_AND_MERGE		2

#define UPDATE_MODE_PARTIAL			0x0
#define UPDATE_MODE_FULL			0x1

#define WAVEFORM_MODE_AUTO			257

#define TEMP_USE_AMBIENT			0x1000

#define EPDC_FLAG_ENABLE_INVERSION		0x01
#define EPDC_FLAG_FORCE_MONOCHROME		0x02
#define EPDC_FLAG_USE_CMAP			0x04
#define EPDC_FLAG_USE_ALT_BUFFER		0x100
#define EPDC_FLAG_TEST_COLLISION		0x200
#define EPDC_FLAG_GROUP_UPDATE			0x400
#define EPDC_FLAG_USE_DITHERING_Y1		0x2000
#define EPDC_FLAG_USE_DITHERING_Y4		0x4000

#define FB_POWERDOWN_DISABLE			-1

struct mxcfb_alt_buffer_data {
    __u32 phys_addr;
    __u32 width;	/* width of entire buffer */
    __u32 height;	/* height of entire buffer */
    struct mxcfb_rect alt_update_region;	/* region within buffer to update */
};

struct mxcfb_update_data {
    struct mxcfb_rect update_region;
    __u32 waveform_mode;
    __u32 update_mode;
    __u32 update_marker;
    int temp;
    unsigned int flags;
    struct mxcfb_alt_buffer_data alt_buffer_data;
};

struct mxcfb_update_marker_data {
    __u32 update_marker;
    __u32 collision_test;
};

/*
 * Structure used to define waveform modes for driver
 * Needed for driver to perform auto-waveform selection
 */
struct mxcfb_waveform_modes {
    int mode_init;
    int mode_du;
    int mode_gc4;
    int mode_gc8;
    int mode_gc16;
    int mode_gc32;
};

/*
 * Structure used to define a 5*3 matrix of parameters for
 * setting IPU DP CSC module related to this framebuffer.
 */
struct mxcfb_csc_matrix {
    int param[5][3];
};

#define MXCFB_WAIT_FOR_VSYNC	_IOW('F', 0x20, u_int32_t)
#define MXCFB_SET_GBL_ALPHA     _IOW('F', 0x21, struct mxcfb_gbl_alpha)
#define MXCFB_SET_CLR_KEY       _IOW('F', 0x22, struct mxcfb_color_key)
#define MXCFB_SET_OVERLAY_POS   _IOWR('F', 0x24, struct mxcfb_pos)
#define MXCFB_GET_FB_IPU_CHAN 	_IOR('F', 0x25, u_int32_t)
#define MXCFB_SET_LOC_ALPHA     _IOWR('F', 0x26, struct mxcfb_loc_alpha)
#define MXCFB_SET_LOC_ALP_BUF    _IOW('F', 0x27, unsigned long)
#define MXCFB_SET_GAMMA	       _IOW('F', 0x28, struct mxcfb_gamma)
#define MXCFB_GET_FB_IPU_DI 	_IOR('F', 0x29, u_int32_t)
#define MXCFB_GET_DIFMT	       _IOR('F', 0x2A, u_int32_t)
#define MXCFB_GET_FB_BLANK     _IOR('F', 0x2B, u_int32_t)
#define MXCFB_SET_DIFMT		_IOW('F', 0x2C, u_int32_t)
#define MXCFB_CSC_UPDATE	_IOW('F', 0x2D, struct mxcfb_csc_matrix)

/* IOCTLs for E-ink panel updates */
#define MXCFB_SET_WAVEFORM_MODES	_IOW('F', 0x2B, struct mxcfb_waveform_modes)
#define MXCFB_SET_TEMPERATURE		_IOW('F', 0x2C, int32_t)
#define MXCFB_SET_AUTO_UPDATE_MODE	_IOW('F', 0x2D, __u32)
#define MXCFB_SEND_UPDATE		_IOW('F', 0x2E, struct mxcfb_update_data)
#define MXCFB_WAIT_FOR_UPDATE_COMPLETE	_IOWR('F', 0x2F, struct mxcfb_update_marker_data)
#define MXCFB_SET_PWRDOWN_DELAY		_IOW('F', 0x30, int32_t)
#define MXCFB_GET_PWRDOWN_DELAY		_IOR('F', 0x31, int32_t)
#define MXCFB_SET_UPDATE_SCHEME		_IOW('F', 0x32, __u32)
#define MXCFB_GET_WORK_BUFFER		_IOWR('F', 0x34, unsigned long)
#define MXCFB_DISABLE_EPDC_ACCESS	_IO('F', 0x35)
#define MXCFB_ENABLE_EPDC_ACCESS	_IO('F', 0x36)
#endif




#ifndef G2D_THREAD_H
#define G2D_THREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QImage>
#include <QTimer>
#include <QPixmap>
#include "g2d_yuyv_bgra.h"
#include "g2d.h"
#include "v4l2_work_fourin.h"

//extern QImage * finishImage;

class G2DThread : public QThread
{
    Q_OBJECT

#define topCameraWhich 0
#define bottomCameraWhich 1
#define leftCameraWhich 2
#define rightCameraWhich 3

#define TFAIL -1
#define TPASS 0

    typedef struct fbdev{
        int fdfd; //open "dev/fb0"
        struct fb_var_screeninfo vinfo;
        struct fb_fix_screeninfo finfo;
        long int screensize;
        char *map_fb;

    }FBDEV;

public:
    G2DThread(QObject *parent = Q_NULLPTR);
    ~G2DThread();

    void setWhichCamera(int whichCamera);
    void init();
//    void run();
    void ClearScreen();

private:
    ushort rgb_24_2_565(int r, int g, int b);
    int init_dev(FBDEV *dev);
    void draw_dot(FBDEV *dev,int x,int y,unsigned char r,unsigned char g,unsigned char b);

signals:void caleFinish(unsigned int);

public slots:
    void copyData(unsigned int frameOffset);
    void cleanFrameSpeed();

public:
//    static const int bufferCount = 4;
//    QImage * finishImage;
//    QImage * stitchingImage[bufferCount];
//    QPixmap fisheyePixmap;

private:
    FBDEV fb_dev;
    CameraWorkFourin *cameraFourInThread;
    unsigned char * cameraBuffer;
    int srcWidth = 1280;
    int srcheight = 800;

    int imageWidth = 388;
    int imageHeight = 720;
    const unsigned int countCamera = 4;
//    struct g2d_buf *g_g2d_temp_buffer;
    struct g2d_buf *g_g2d_src_cale_buffer;
    struct g2d_buf *g_g2d_desc_cale_buffer;

    int countFrameSpeed = 0;

    //QImage * stitchingImage[bufferCount];
    //QImage * finishImage;

    bool status =false;
    bool firstTimeStatus = true;
    QTimer *timer;

    int whichCamera = 0;

    int fd_capture_v4l = 0;
    int fd_fb_display = 0;
    int fd_ipu = 0;
    unsigned char * g_fb_display = NULL;
    int g_input = 1;
    //int g_display_num_buffers = 1;
    int g_display_num_buffers = 3;
    int g_capture_num_buffers = NUMBER_BUFFERS;
    int g_in_width = 0;
    int g_in_height = 0;
    int g_in_fmt = V4L2_PIX_FMT_UYVY;
    int g_display_width = 1200;
    int g_display_height = 720;
    int g_display_top = 0;
    int g_display_left = 0;
    int g_display_fmt = V4L2_PIX_FMT_BGR32;
    int g_display_base_phy;
    int g_display_size;
    int g_display_fg = 1;
    int g_display_id = 1;
    struct fb_var_screeninfo g_screen_info;
    //int g_frame_count = 0x7FFFFFFF;
    int g_frame_count = 0x1;
    int g_frame_size;
    //bool g_g2d_render = true;
    bool g_g2d_render = 1;
    int g_g2d_fmt;
    //int g_mem_type = V4L2_MEMORY_MMAP;
    int g_mem_type = V4L2_MEMORY_USERPTR;
    int fd_fb_bg = 0;
    char fb_display_dev[100];
    char fb_display_bg_dev[100];
    volatile bool myCloseWindows = false;
};

#endif // G2D_THREAD_H
