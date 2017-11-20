#include "iconwidget.h"

ICONWIDGET::ICONWIDGET()
{
}

int ICONWIDGET::getMouseX()
{
    return xpos;
}

int ICONWIDGET::getMouseY()
{
    return ypos;
}

void ICONWIDGET::setMouseX(int xpos)
{
    this->xpos = xpos;
}

void ICONWIDGET::setMouseY(int ypos)
{
    this->ypos = ypos;
}
