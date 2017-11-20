#include "loopuptable.h"
#include <stdio.h>
#include <unistd.h>
#include <QDebug>

static const int width = 388;
static const int height = 720;

static const int fisheye_width = 1280;
static const int fisheye_height = 800;

QString loopupTableFileName = "/data/user/loopuptable/loopuptablefile";
QString loadopenfilemode = "rb+";
QString saveopenfilemode = "wb+";

tablePos loopupTable[height][width];

//初始化查找表
bool initLoopupTable()
{
    if (loadLoopupTable() == true)
    {
        qDebug() << "loadLoopupTable is ok";
        return true;
    }

    cleanLoopupTable();
    saveLoopupTable();


    //qDebug() << "initLoopupTable is false";
    //return false;
    return true;
}

//
bool cleanLoopupTable()
{
    for (int i=0;i<height;i++) 
    {
        for (int j=0;j<width;j++) 
        {
            (loopupTable[i][j]).boostCompensation = 0;
            (loopupTable[i][j]).x = 0;
            (loopupTable[i][j]).y = 0;
        }
    }

    return true;
}

//保存查找表
bool saveLoopupTable()
{
    FILE *fp;
    fp = fopen(loopupTableFileName.toLatin1().data(),saveopenfilemode.toLatin1().data());
    
    if (fp == NULL) 
    {
        return false;
    }

    for (int i=0;i<height;i++) 
    {
        for (int j=0;j<width;j++) 
        {
            fwrite((&loopupTable[i][j]),sizeof(tablePos),1,fp);
        }
    }

    fflush(fp);
    fclose(fp);

    return true;
}

//讀取查找表
bool loadLoopupTable()
{
    FILE *fp;
    fp = fopen(loopupTableFileName.toLatin1().data(),loadopenfilemode.toLatin1().data());

    if (fp == NULL) 
    {
        return false;
    }

    for (int i=0;i<height;i++) 
    {
        for (int j=0;j<width;j++) 
        {
            fread((&loopupTable[i][j]),sizeof(tablePos),1,fp);
        }
    }

    fclose(fp);
    return true;
}

//caleLoopupTablePos函數是計算出要寫入查找表的數值
//whichCamera代表那個攝像頭
//x代表x坐標
//y代表y坐標
tablePos caleLoopupTablePos(int boostCompensation ,int x,int y,int whichCamera)
{
    tablePos value;
    value.x = x;
    value.y = y + (whichCamera * fisheye_height);
    value.boostCompensation = boostCompensation;
    return value;
}

tablePos readLoopupTablePos(int y,int x)
{
    return loopupTable[y][x];
}

//writeLoopTablePos函數是把數值寫入攝像頭
//value是代表我要寫入的數值
void writeLoopTablePos(int y,int x,tablePos value)
{
    loopupTable[y][x] = value;
}


