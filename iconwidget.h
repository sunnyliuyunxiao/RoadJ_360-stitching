#ifndef ICONWIDGET_H
#define ICONWIDGET_H

class ICONWIDGET
{
public:
    ICONWIDGET();
    int getMouseX();
    int getMouseY();
    void setMouseX(int xpos);
    void  setMouseY(int ypos);

public:
    volatile int xpos;
    volatile int ypos;
};


#endif //ICONWIDGET_H
