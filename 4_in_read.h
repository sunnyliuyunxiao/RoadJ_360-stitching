#ifndef LIBV4L2SHAREMEMORYCLIENTREAD_H
#define LIBV4L2SHAREMEMORYCLIENTREAD_H

int getTopCameraShareMemory(unsigned char *memoryData,int len);
int getBottomCameraShareMemory(unsigned char *memoryData,int len);
int getLeftCameraShareMemory(unsigned char *memoryData,int len);
int getRightCameraShareMemory(unsigned char *memoryData,int len);

#endif
