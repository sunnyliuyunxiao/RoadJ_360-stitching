#ifndef G2D_YUYV_BGRA_H
#define G2D_YUYV_BGRA_H

#include "g2d.h"

int YUYV2BGRA(struct g2d_buf *buf_yuyv,int img_width,int img_height,
              struct g2d_buf *buf_bgra,int disp_width,int disp_height);

int matrixRotate(struct g2d_buf *buf_yuyv, int offset,int img_width,int img_height,
              struct g2d_buf *buf_bgra,int disp_width,int disp_height,enum g2d_rotation rot);

void mmCopy(struct g2d_buf *buf_src,struct g2d_buf *buf_desc,int size);

#endif // G2D_YUYV_RGB_H
