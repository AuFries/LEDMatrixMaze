#ifndef Cell_h
#define Cell_h
#include "Arduino.h"

class Cell
{
  public:
    Cell();
    void setIndex(int index);
    void setVisited(boolean visited);
    void setWall(boolean isWall);
    int getIndex();
    boolean getVisited();
    boolean getWall();
  private:
    int ledsIndex; //the index within the leds array used in Main 
    boolean visited;
    boolean isWall;
    boolean isBorder;
};

#endif
