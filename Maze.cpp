#include "Cell.h"
#include "Maze.h"
#include "Arduino.h"
#include <StackArray.h> //Library used for stacks (maze creation)
#include <FastLED.h>
#include <ArduinoQueue.h> //Library used for queues (maze solving)
/*
 The LED indices are in a zig zag pattern on the matrix, which means an offset must be provided. The function below map the LEDs row/col to the correct index.
i
0  35 34 33 32 31 30
1  24 25 26 27 28 29
2  23 22 21 20 19 18
3  12 13 14 15 16 17
4  11 10 9  8  7  6
5  0  1  2  3  4  5

j  0  1  2  3  4  5
EVEN ROWS:
ledIndex = 35 - (i * 6) - j
ODD ROWS:
ledIndex = 35 - (i * 6) - (5-j)
*/
Maze::Maze(int prevEndRow, int prevEndCol){
  for (int i = 0; i < 16; i++) { //Initialize cell objects with the correct led index
    for (int j = 0; j < 16; j++) {
      int index = (i % 2 == 0) ? 255 - (i * 16) - j : 255 - (i * 16) - (15 - j); //This is the mapping function defined above
      maze[i][j].setIndex(index);
    }
  } 
  startCell[0] = prevEndRow;
  startCell[1] = prevEndCol;
  endCell[0] = random(1,15);
  if (prevEndCol == 0) { 
    endCell[1] = 15;
  } else {
    endCell[1] = 0;
  }
  maze[startCell[0]][startCell[1]].setWall(false);
  maze[endCell[0]][endCell[1]].setWall(false);
}

int Maze::getEndRow() {
  return endCell[0];
}

int Maze::getEndCol() {
  return endCell[1];
}

void Maze::makeMaze(CRGB leds[]) { //Creates the maze using a stack algorithm and displays to matrix
  
  StackArray <int> rowStack; //Initialize stack
  StackArray <int> colStack;
  rowStack.push(startCell[0]); //Push starting indices onto stack
  colStack.push(startCell[1]);

  while(!rowStack.isEmpty()) {
    int r = rowStack.peek();
    int c = colStack.peek();

    int unvisitedNeighbors[4][2]; //An array to keep a list of all unvisited neighbor indicies
    int size = getUnvisitedNeighbors(r,c,unvisitedNeighbors,2); //Get of unvisited neighbors array and populate the array with neighbor indices

    if (size == 0) { //If there are no unvisitedNeighbors, pop top element
      rowStack.pop();
      colStack.pop();
    } else { //Otherwise randomly choose a neighbor to add to the stack and remove the wall between them and the current cell
        int randomChoice = random(size);
        int chosenIndices[2] = {unvisitedNeighbors[randomChoice][0],unvisitedNeighbors[randomChoice][1]};
        maze[chosenIndices[0]][chosenIndices[1]].setWall(false);
        if (chosenIndices[0] != startCell[0] || chosenIndices[1] != startCell[1]) {
          leds[maze[chosenIndices[0]][chosenIndices[1]].getIndex()] = CRGB(0,0,0); //Turn the LED off on the next cell
        }
        
        if (c+2 == chosenIndices[1]) { //Remove right wall between current and neighbor cell
            maze[r][c+1].setWall(false);
            leds[maze[r][c+1].getIndex()] = CRGB(0,0,0);
        } else if (c-2 == chosenIndices[1]) { //Remove left wall between current and neighbor cell
            maze[r][c-1].setWall(false);
            leds[maze[r][c-1].getIndex()] = CRGB(0,0,0);
        } else if (r+2 == chosenIndices[0]) { //Remove bottom wall between current and neighbor cell
            maze[r+1][c].setWall(false);
            leds[maze[r+1][c].getIndex()] = CRGB(0,0,0);
        } else if (r-2 == chosenIndices[0]) { //Remove top wall between current and neighbor cell
            maze[r-1][c].setWall(false);
            leds[maze[r-1][c].getIndex()] = CRGB(0,0,0);
        }
        FastLED.show();
        delay(50);
        
        maze[chosenIndices[0]][chosenIndices[1]].setVisited(true); //Mark neighbor as visited and add the indices to the stack
        rowStack.push(chosenIndices[0]);
        colStack.push(chosenIndices[1]);
    }
  }
  for (int i = 0; i < 16; i++) { //After generation complete, set all cells visited status back to false
    for (int j = 0; j < 16; j++) {
        maze[i][j].setVisited(false);
    }
  }
}

int Maze::getUnvisitedNeighbors(int r, int c, int unvisitedNeighbors[4][2],int cellMultiplier) { //Helper function to populate the input array with unvisited neighbor cells and return the size
  int neighborIndices[4][2] = {{r,c-1*cellMultiplier},{r,c+1*cellMultiplier},{r-1*cellMultiplier,c},{r+1*cellMultiplier,c}};
  int lastIndex = 0;
  for (int i = 0; i < 4; i++) { //Add neighbor indices to array if they are viable cells and aren't already visited
    int neighborRow = neighborIndices[i][0];
    int neighborCol = neighborIndices[i][1];
    if (neighborRow >= 0 && neighborCol >= 0 && neighborRow < 16 && neighborCol < 16 && !maze[neighborRow][neighborCol].getVisited()) {
      unvisitedNeighbors[lastIndex][0] = neighborIndices[i][0];
      unvisitedNeighbors[lastIndex][1] = neighborIndices[i][1];
      lastIndex++;
    }
  }
  return lastIndex;
}

void Maze::fillMatrix(CRGB leds[]) { //Fills the matrix with solid LED's
  leds[maze[startCell[0]][startCell[1]].getIndex()] = CRGB::Green; //Mark the starting and ending cells
  leds[maze[endCell[0]][endCell[1]].getIndex()] = CRGB::Blue; 
  for (int i = 0; i < 16; i++) { //Turns on all LED's that are walls
      for (int j = 0; j < 16; j++) {
        Cell cell = maze[i][j];
        if (cell.getWall()) {
          leds[cell.getIndex()] = CRGB::Red;
        }
      }
    }
  FastLED.show();
}

void Maze::solveMaze(CRGB leds[]) { //Solve the maze one step at a time using the queue algorithm and display to matrix
  ArduinoQueue<int> rowQ(16); //Create the queues
  ArduinoQueue<int> colQ(16);
  rowQ.enqueue(startCell[0]); //enqueue start row and col
  colQ.enqueue(startCell[1]);

  while(!rowQ.isEmpty()) {
    int Cr = rowQ.dequeue(); //remove head and mark as visited
    int Cc = colQ.dequeue();
    maze[Cr][Cc].setVisited(true);
    leds[maze[Cr][Cc].getIndex()] = CRGB::Green;

    FastLED.show();
    delay(50);
    
    if (Cr == endCell[0] && Cc == endCell[1]) { //If the exit is found, end the search
      return;
    }

    int unvisitedNeighbors[4][2]; //An array to keep a list of all unvisited neighbor indicies
    int size = getUnvisitedNeighbors(Cr,Cc,unvisitedNeighbors,1); //Get of unvisited neighbors array and populate the array with neighbor indices

    for (int i = 0; i < size; i++) { //If the neighbor is unvisited and reachable, add it to queue
      int Nr = unvisitedNeighbors[i][0];
      int Nc = unvisitedNeighbors[i][1];
      //Add the neighbor to the queue if it's reachable from the current cell (not a wall)
      if (!maze[Nr][Nc].getWall()) {
        rowQ.enqueue(Nr);
        colQ.enqueue(Nc);
      }
    }
  }
}
