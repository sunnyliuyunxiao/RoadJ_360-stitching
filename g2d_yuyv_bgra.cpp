#include <stdio.h>
#include <QDebug>
#include "g2d.h"
#include "g2d_yuyv_bgra.h"

#define TFAIL -1
#define TPASS 0

//4.1 Color space conversion from YUV to BGRA
int YUYV2BGRA(struct g2d_buf *buf_yuyv,int img_width,int img_height,
              struct g2d_buf *buf_bgra,int disp_width,int disp_height)
{
    struct g2d_surface src,dst;
    void *g2dHandle = NULL;
    if(g2d_open(&g2dHandle) == -1 || g2dHandle == NULL) {
        qDebug("Fail to open g2d device!\n");
        //g2d_free(buf_yuyv);
        return TFAIL;
    }

    src.planes[0] = buf_yuyv->buf_paddr;
    src.left = 0;
    src.top = 0;
    src.right = img_width;
    src.bottom = img_height;
    src.stride = img_width;
    src.width  = img_width;
    src.height = img_height;
    src.rot = G2D_ROTATION_0;
    src.format = G2D_YUYV;

    dst.planes[0] = buf_bgra->buf_paddr;
    dst.left = 0;
    dst.top = 0;
    dst.right = disp_width;
    dst.bottom = disp_height;
    dst.stride = disp_width;
    dst.width = disp_width;
    dst.height = disp_height;
    dst.rot = G2D_ROTATION_0;
    //dst.format = G2D_BGRA8888;
    dst.format = G2D_RGB565;
//        dst.format = G2D_RGBA8888;
    g2d_blit(g2dHandle, &src, &dst);
    g2d_finish(g2dHandle);
    g2d_close(g2dHandle);

    return TPASS;
}


int matrixRotate(struct g2d_buf *buf_yuyv, int offset,int img_width,int img_height,
              struct g2d_buf *buf_bgra,int disp_width,int disp_height,enum g2d_rotation rot)
{
    struct g2d_surface src,dst;
    void *g2dHandle = NULL;
    if(g2d_open(&g2dHandle) == -1 || g2dHandle == NULL) {
        qDebug("Fail to open g2d device!\n");
        //g2d_free(buf_yuyv);
        return TFAIL;
    }

    src.planes[0] = buf_yuyv->buf_paddr+offset;
    src.left = 0;
    src.top = 0;
    src.right = img_width;
    src.bottom = img_height;
    src.stride = img_width;
    src.width  = img_width;
    src.height = img_height;
    src.rot = G2D_ROTATION_0;
    src.format = G2D_RGB565;

    dst.planes[0] = buf_bgra->buf_paddr;
    dst.left = 0;
    dst.top = 0;
    dst.right = disp_width;
    dst.bottom = disp_height;
    dst.stride = disp_width;
    dst.width = disp_width;
    dst.height = disp_height;
    dst.rot = rot;
    //dst.format = G2D_BGRA8888;
    //dst.format = G2D_RGB565;
        dst.format = G2D_RGB565;
    g2d_blit(g2dHandle, &src, &dst);
    g2d_finish(g2dHandle);
    g2d_close(g2dHandle);

    return TPASS;
}

void mmCopy(struct g2d_buf *buf_src,
              struct g2d_buf *buf_desc,int size)
{
    void *g2dHandle = NULL;
    if(g2d_open(&g2dHandle) == -1 || g2dHandle == NULL) {
        printf("Fail to open g2d device!\n");
        return;
    }
    g2d_copy(g2dHandle, buf_desc, buf_src,size);
    g2d_finish(g2dHandle);
    g2d_close(g2dHandle);
}
