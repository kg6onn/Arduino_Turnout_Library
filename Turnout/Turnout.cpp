/* 
Turnout.cpp - Library for controlling a model railroad turnout with a servo.
Author: John Herbert 
First version: Februrary 11, 2025
Last update: June 8, 2026
Released into the public domain.
*/
#include "Turnout.h"

Turnout::Turnout(int buttonPin, int servoPin, long stepDelay, int eepromStart)
{
      // variables passed to class
      this->buttonPin = buttonPin;
      this->servoPin = servoPin;
      this->stepDelay = stepDelay;
      this->eepromStart = eepromStart; // Important! eepromStart must
      // be different whole number for each Turnout object created.
      // Recommendation: 0, 1, 2, 3, etc.
      
      // Initialize other variables to safe defaults
      this->straight = DEFAULT_SERVO_POS;
      this->divergent = DEFAULT_SERVO_POS;
      this->positionNow = DEFAULT_SERVO_POS; 
      this->targetPosition = DEFAULT_SERVO_POS;
      this->isMoving = false;
      this->lastCMRI = 0x00;
      this->cmriPosition = 0;
      this->inSetupMode = false;
}

void Turnout::turnoutSetup()
{
    // setup addresses for EEPROM
    // Allocate 2 bytes for each turnout.
    sAddress = eepromStart*2;
    dAddress = sAddress + 1;
    
    // Read current EEPROM state into temp variable
    byte sTemp = EEPROM.read(sAddress);
    byte dTemp = EEPROM.read(dAddress);
    
    // Unwritten EEPROM values default to 255 in Arduino.
    // If they are 255, the straight and divergent values have
    // not been written yet, so leave turnout servo limits at the default value
    if(sTemp == 255)
    {
      straight = DEFAULT_SERVO_POS;
    }
    else
    {
      straight = sTemp;
    }
    
    if(dTemp == 255)
    {
      divergent = DEFAULT_SERVO_POS;
    }
    else
    {
      divergent = dTemp;
    }
    
    // Minimum step delay if less than 5
    // Tests have shown a step delay of less than 5 mS is unstable.
    if(stepDelay < 5)
    {
      stepDelay = 5;
    }

    pinMode(buttonPin, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT); // only used for setup mode
    servo.attach(servoPin);
    // startup time only used in setup mode
    unsigned long bootStartTime = millis();
    setupPoints(bootStartTime, inSetupMode);
    targetPosition = straight;  // default startup target position
    positionNow = (divergent + straight)/2; // startup position
    servo.write(positionNow); // put the turnout into a known state (target position)
    digitalWrite(LED_PIN, LOW); // Turn off built-in LED
}
// process turnout events.  This is the only function that needs to be called in main void loop
void Turnout::process()  
{
	checkButton();
	throwTurnout();
}

int Turnout::getPosition()
{
  // This function would be good to use for standalone Arduino 
  // program.  It returns 3 posible values (moving, straight or divergent)
  // depending on state.
	int retVal = 0;
    if(isMoving)
    {
      retVal = 1;
    }
    else if(positionNow == straight)
    {
      retVal = 2;
    }
    else
    {
      retVal = 3;
    }
    return retVal;
}

int Turnout::getActualPosition()
{
  // simply returns the current position of the turnout.  May be
  // useful for standalone Arduino program
	return positionNow;
}

int Turnout::getCMRIposition()
{
  // C/MRI position only has two states since we only have one bit.
	return cmriPosition;
}

void Turnout::setCMRIposition()
{
  // Sets C/MRI state depending on current position.
	if(positionNow == straight)
	{	
	  cmriPosition = 0;
	}
	else if(positionNow == divergent)
	{
	  cmriPosition = 1;
	}
}

void Turnout::setTurnout()
{
  // This function does not throw turnout(write servo). Instead,
  // it changes the target position to opposite value.
	servo.attach(servoPin);
	isMoving = true;
	if(positionNow == straight)
	{
	  targetPosition = divergent;
	}
	else if(positionNow == divergent)
	{
	  targetPosition = straight;
	}
}

void Turnout::cmriTurnout(byte state)
{
  // This function for C/MRI sets the turnout if there is a change 
  // state of the control bit.
	byte changedState = state ^ lastCMRI;  // XOR current state to last
	if(changedState)
	{
		setTurnout();
    // save the current state for next call.
		lastCMRI = state;
	}
}

bool Turnout::startTimer()
{
  // This is a non-blocking function that slows down servo movement 
	unsigned long CurrentMillis = millis();
  
    if(CurrentMillis - timerA >= stepDelay)
    {
      timerA = CurrentMillis;
      return true;
    }
    else
    {
      return false;
    }
}

void Turnout::checkButton()
{
    // Get the current raw reading
    int reading = digitalRead(buttonPin);

    // Debounce logic: check if the signal just flickered/changed
    if (reading != lastReading) 
    {
        lastDebounceTime = millis(); // Reset the timer
    }

    //  Only process the button if the signal has been stable for long enough
    if ((millis() - lastDebounceTime) > debounceDelay) 
    {
        
        // If the stable state has changed...
        if (reading != buttonState) 
        {
            buttonState = reading;

            //  State Logic: Detect the Press and the Release
            if (buttonState == LOW) 
            {
                // The button is now officially "Pressed"
                wasPressed = true;
            } 
            else if (buttonState == HIGH) 
            {
                // The button was just released. 
                // Only trigger if it was previously in a "Pressed" state.
                if (wasPressed) 
                {
                    setTurnout();
                    wasPressed = false; // Reset the flag for the next cycle
                }
            }
        }
    }

    // Save the raw reading for the next comparison
    lastReading = reading;
}

void Turnout::throwTurnout()
{
	bool itsTime = startTimer();
    if(itsTime)
    {
      if(positionNow < targetPosition)
      {
        servo.write(++positionNow);
      }
      else if(positionNow > targetPosition)
      {
          servo.write(--positionNow);
      }
    }
    if(positionNow == targetPosition)
    {
      servo.detach();
      setCMRIposition();
      isMoving = false;
    }
}

// The rest of the Turnout library functions are used in setup mode
// to set the points of a newly installed servo motor.
void Turnout::setupPoints(unsigned long bootStartTime, bool inSetupMode)
{
    // Go into setup points mode if local control button is
    // held down at startup or reset.
    if (digitalRead(buttonPin) == LOW) 
    {
      delay(50); // Debounce press
      startBlinking(4, 4.0);

      while(updateBlinker());
      while (digitalRead(buttonPin) == LOW); // Wait for release
      delay(50); // Debounce release
      // Go back to default values for turnout points.
      straight = DEFAULT_SERVO_POS;
      divergent = DEFAULT_SERVO_POS;
      inSetupMode = true;             
    }
  // If in setup mode, set turnout points
  if (inSetupMode) 
  {
    int currentPhase = 1;
    bool processingSetup = true;

    while (processingSetup) 
    {
      bool phaseComplete = false;
      // Set turnout back to default postion for setup mode
      servo.write(DEFAULT_SERVO_POS);

      // Loop for the specific configuration phase
      while (!phaseComplete) {
        updateBlinker();
        int buttonAction = handleButtonInput();

        if (buttonAction == BTN_CLICK) 
        {
          startBlinking(1, 16.0);
          
          if (currentPhase == 1)
          {
            if(straight < 180)  // Don't go above 180, the max servo value
            {
              servo.write(straight++);
            }
          }
          else
          {
            if(divergent > 0)  // Don't go below 0, the min servo value
            {
              servo.write(divergent--);
            }
          }
          
        } 
        else if (buttonAction == BTN_HOLD) 
        {
          phaseComplete = true; // break out of the phase
        }
      }

      // Increment the phase counter.
      currentPhase++;
     // Serial.println("Reached update blinker in between phases.");
      startBlinking(4, 4.0);
      while (updateBlinker());
      while (digitalRead(buttonPin) == LOW);  // just in case user keeps holding button down.
      
      // Currently only 2 phases. One for each servo direction.
      if (currentPhase > 2) 
      {
        
        //store point values to eeprom
        // Only write final values if both phases are complete.
        EEPROM.update(sAddress, straight);
        EEPROM.update(dAddress, divergent);
        
        processingSetup = false; // Turn off flag to exit the setup mode entirely
      }
    }
  }
}

// Dedicated function to process clicks vs long holds
// Currently, this button function is only used in setup.  A seperate 
// button function is used in the main void loop.
int Turnout::handleButtonInput() 
{
  // If the button isn't pressed, do nothing
  if (digitalRead(buttonPin) == HIGH) 
  {
    return BTN_NONE;
  }

  // Button was just pressed (LOW)
  // Note: the delay() function is only called in setup. Never in the
  // main loop.
  delay(50); // Debounce physical press
  unsigned long pressStartTime = millis();
  bool holdTriggered = false;

  // Track the button while it remains held down
  while (digitalRead(buttonPin) == LOW) 
  {
    // Check if the hold time has crossed the long hold mark
    // In setup mode. This will either move to the next step or
    // save turnout point values to EEPROM and exit setup.
    if (!holdTriggered && (millis() - pressStartTime >= HOLD_DURATION_MS)) 
    {
      holdTriggered = true;
      break; 
    }
  }

  // Button has now been released
  delay(50); // Debounce physical release

  if (holdTriggered) 
  {
    return BTN_HOLD;
  } 
  else 
  {
    return BTN_CLICK;
  }
}

// Function to trigger a new blink sequence
void Turnout::startBlinking(int blinkCount, float blinksPerSecond) 
{
  if (blinkCount <= 0 || blinksPerSecond <= 0.0) return;

  // Calculate duration of one full blink cycle (on + off period) in milliseconds
  float totalCycleTime = 1000.0 / blinksPerSecond;
  
  // The interval for just ONE state (half of a full blink cycle)
  blinkInterval = (unsigned long)(totalCycleTime / 2.0);
  
  blinksRemaining = blinkCount;
  isBlinking = true;
  
  // Turn the LED on immediately to start the first blink
  ledState = HIGH;
  digitalWrite(LED_PIN, ledState);
  previousMillis = millis(); 
}

// Non-blocking helper function that monitors time and updates the LED state
bool Turnout::updateBlinker() 
{
  bool returnVal = true;
  // Do nothing if there are no blinks scheduled
  if (!isBlinking) returnVal = false;

  unsigned long currentMillis = millis();

  // Check if enough time has passed to change the LED state
  if (currentMillis - previousMillis >= blinkInterval) 
  {
    previousMillis = currentMillis; // Reset the interval timer

    if (ledState == HIGH) 
    {
      // LED was on, so turn it off. One full blink finishes when it turns back off.
      ledState = LOW;
      blinksRemaining--; 
    } 
    else 
    {
      // LED was off, turn it back on for the next blink
      ledState = HIGH;
    }

    // Apply the new state to the hardware pin
    digitalWrite(LED_PIN, ledState);

    // If all requested blinks are finished, stop the sequence
    if (blinksRemaining <= 0) 
    {
      isBlinking = false;
      ledState = LOW;
      digitalWrite(LED_PIN, ledState); // Ensure LED ends in the safe OFF position
      returnVal = false;
    }
    
  }
  return returnVal;
}
