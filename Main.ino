#include <FastLED.h>
#include "Maze.h"

#define LED_PIN     5
#define NUM_LEDS    256
#define BRIGHTNESS  64
#define LED_TYPE    WS2811 
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

int prevEndRow = 1; //The end cell of the previous maze will be the start cell of the new one. The end cell row is always random.
int prevEndCol = 0;

void setup() {
    Serial.begin(9600);
    delay(3000); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    randomSeed(analogRead(0));
}

void loop()
{     
  FastLED.clear(); //Clear matrix
  Maze maze = Maze(prevEndRow,prevEndCol); //Create maze object
  prevEndRow = maze.getEndRow();
  prevEndCol = maze.getEndCol();
  maze.fillMatrix(leds); //Fill matrix with colored LEDs + (start and end position)
  maze.makeMaze(leds); //Remove walls step by step and create maze with stack algorithm
  delay(100);
  maze.solveMaze(leds); //Solve the maze step by step using a queue algorithm
} 
