#ifndef LOOPUPTABLE_H
#define LOOPUPTABLE_H

typedef struct __TablePos{
    unsigned int  x : 12,
                  y : 12,
                  boostCompensation : 8;
}tablePos;



#endif // LOOPUPTABLE_H


bool initLoopupTable();
bool cleanLoopupTable();
bool saveLoopupTable();
bool loadLoopupTable();

tablePos caleLoopupTablePos(int boostCompensation ,int x,int y,int whichCamera);
void writeLoopTablePos(int y,int x,tablePos result);

tablePos readLoopupTablePos(int y,int x);
extern tablePos loopupTable[720][388];
