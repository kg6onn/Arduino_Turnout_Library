
#include "Turnout.h"

// This example will show how to operate a turnout
// with a serial connection.
// Create a Turnout object with no built-in button
Turnout turnout1 = Turnout(9, 90, 180, 70);

void setup() {
  // put your setup code here, to run once:
  // Set up your turnout
  turnout1.TurnoutSetup();
  // Start serial connection
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Look for turnout event.
  turnout1.Update();

  if(Serial.available())
  {
    if(Serial.read() == 'a')
    {
      // set turnout for throw.
      turnout1.setTurnout();
    }
  }
}
