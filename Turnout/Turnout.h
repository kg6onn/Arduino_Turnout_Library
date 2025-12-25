/* 
 Turnout.h - Library for controling a model railroad turnout
 with a servo.
 Created by John Herbert, Februrary 11, 2025
 Last update September 8, 2025
 Released into the public domain.
 */
 #ifndef Turnout_h
 #define Turnout_h
 
#include "Arduino.h"
#include <Servo.h>

class Turnout
{
	public:
		Turnout(int servoPin, int straight, int divergent, long stepDelay);  // Default constructor
		Turnout(int buttonPin, int servoPin, int straight, int divergent, long stepDelay);  // Overloaded constructor
		void turnoutSetup();
		void update();
		int getPosition();
		int getActualPosition();
		void setTurnout();
		void cmriTurnout(byte state);
		int getCMRIposition();
		
	private:
		boolean startTimer();
		void checkButton();
		void throwTurnout();
		void setCMRIposition();
		
		// hold angle
		int straight;
		int divergent;
		
		// button and servo pins
		int buttonPin;
		int servoPin;
		
		// servo step delay and timer
		long stepDelay;
		unsigned long timerA;
		
		// holds current position and target position
		int positionNow;
		int targetPosition;
		
		// save button state and last state of button
		int buttonState;
		int lastButtonState;
		
		// save last CMRI command
		byte lastCMRI;
		
		// save CMRI position of turnout
		int cmriPosition;
		
		// store if moving or not
		boolean isMoving;
		
		// create servo object
		Servo servo;
};

#endif
