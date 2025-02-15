/* 
 Turnout.h - Library for controling a model railroad turnout
 with a servo.
 Created by John Herbert, Februrary 11, 2025
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
		void TurnoutSetup();
		void Update();
		int getPosition();
		int getActualPosition();
		void setTurnout();
		
	private:
		boolean StartTimer(unsigned long &_timer, long interval);
		void checkButton();
		void throwTurnout();
		
		// hold angle
		int _straight;
		int _divergent;
		
		// button and servo pins
		int _buttonPin;
		int _servoPin;
		
		// servo step delay and timer
		long _stepDelay;
		unsigned long _timerA;
		
		// holds current position and target position
		int _positionNow;
		int _targetPosition;
		
		// save button state and last state
		int _buttonState;
		int _lastButtonState;
		
		// store if moving or not
		boolean _isMoving;
		
		// create servo object
		Servo _servo;
};

#endif
