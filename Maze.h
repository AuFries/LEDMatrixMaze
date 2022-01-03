#ifndef Maze_h
#define Maze_h
#include "Cell.h"
#include "Arduino.h"
#include <FastLED.h>
#include <ArduinoQueue.h>


class Maze
{
  public:
    Maze(int startRow, int startCol);
    int getEndRow();
    int getEndCol();
    void makeMaze(CRGB leds[]);
    void fillMatrix(CRGB leds[]); 
    void solveMaze(CRGB leds[]);
  private:
    int getUnvisitedNeighbors(int r, int c, int unvisitedNeighbors[][2], int cellMultiplier); //Modifies unvisitedNeigbhbors and returns the size
    Cell maze[16][16]; //Array to hold cell objects
    int startCell[2]; //Row and col of starting and ending cell
    int endCell[2];
};

#endif
