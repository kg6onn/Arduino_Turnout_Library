 #include "Turnout.h"

/*  Three Turnout example
 *  Format: Turnout(int buttonPin, int servoPin, long stepDelay, int eepromStart)
 *  Very important: eepromStart value must be different whole digit for each turnout
 *  created.  The library handles allocation of 2 bytes of EEPROM memory per turnout for
 *  storage of servo travel limits.  Recomended approach: Simply increment each eepromStart
 *  value by one for each turnout created. 
*/
Turnout turnout0(12, 4, 15, 3);
Turnout turnout1(10, 2, 15, 1);
Turnout turnout2(11, 3, 15, 2);
// create more turnout objects if desired. 
// Just 3 lines of code per turnout.

void setup() 
{
  // put your setup code here, to run once:
  // This just needs to be run once
  turnout0.turnoutSetup();
  turnout1.turnoutSetup();
  turnout2.turnoutSetup();

}

void loop() 
{
  // put your main code here, to run repeatedly:
  // look for turnout events and button presses, if used.
  turnout0.process(); 
  turnout1.process();
  turnout2.process();
}
