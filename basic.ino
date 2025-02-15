#include "Turnout.h"

// Create a Turnout object with push button control.
Turnout turnout1(3, 9, 90, 180, 70);
// create more turnout objects if desired. 
// Just 3 lines of code per turnout.

void setup() 
{
  // put your setup code here, to run once:
  // This just needs to be run once
  turnout1.TurnoutSetup();

}

void loop() 
{
  // put your main code here, to run repeatedly:
  // look for turnout events and button presses, if used.
  turnout1.Update();
}
