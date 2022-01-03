#include "Cell.h"

Cell::Cell(){ 
  ledsIndex = -1;
  visited = false;
  isWall = true;
}

void Cell::setIndex(int index) {
  ledsIndex = index;
}

void Cell::setVisited(boolean visited) {
  this->visited = visited;
}

void Cell::setWall(boolean isWall) {
  this->isWall = isWall;
}

int Cell::getIndex() {
  return ledsIndex;
}

boolean Cell::getVisited() {
  return visited;
}

boolean Cell::getWall() {
  return isWall;
}
