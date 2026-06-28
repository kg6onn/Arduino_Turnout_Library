 #include "Turnout.h" // Include turnout library

/*  One Turnout example
 *  Format: Turnout(int buttonPin, int servoPin, long stepDelay, int eepromStart)
 *  Very important: eepromStart value must be different whole digit for each turnout
 *  created.  The library handles allocation of 2 bytes of EEPROM memory per turnout for
 *  storage of servo travel limits.  Recomended approach: Simply increment each eepromStart
 *  value by one for each turnout created. 
*/

// Create Turnout object(s)
Turnout turnout1(10, 2, 15, 1);
// create more turnout objects if desired. 
// Just 3 lines of code per turnout.

void setup() 
{
  // put your setup code here, to run once:
  // This just needs to be run once
  turnout1.turnoutSetup();
}

void loop() 
{
  // put your main code here, to run repeatedly:
  // look for turnout events and button presses, if used.
  turnout1.process();
}
