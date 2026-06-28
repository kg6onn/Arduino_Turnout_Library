/* 
 Turnout.h - Library for controlling a model railroad turnout
 with a servo.  This is the header file for declaring variables
 and class functions.
 Author: John Herbert 
 First version: Februrary 11, 2025
 Last update: June 8, 2026
 Released into the public domain.
 */
 #ifndef Turnout_h
 #define Turnout_h
 
#include "Arduino.h"  // Need Arduino stuff
#include <Servo.h>   // Standard Arduino servo library is used for servo.
#include <EEPROM.h>  // EEPROM is used to store position limit values

// The servo defaults to 90 degrees for first time startup or when user
// puts Arduino into setup mode.  Change to suit your needs.  Travel range of
// servo is 0 to 180 degrees.
#define DEFAULT_SERVO_POS 90
// This is how long the button needs to held down in mS to move to next
// step in setup or save servo limit values to EEPROM. Change to a lower
// or higher value if desired.  Default is 4000 mS or 4 seconds.
#define SETUP_HOLD_TIME 4000   

class Turnout
{
	public:
	
		Turnout(int buttonPin, int servoPin, long stepDelay, int eepromStart);  // constructor
		void turnoutSetup();  // Setup turnout includes initial setup function to set points to desired length of travel
		void process();   // process Turnout events
		int getPosition();  // returns position of turnout in 3 states: divergent, straight or moving
		int getActualPosition();  // returns current angle of servo in degrees
		void setTurnout();   // whenever called, changes current target position of turnout to opposite
		void cmriTurnout(byte state);  // function used in conjunction with C/MRI and CPNode library
		int getCMRIposition();  // function used to return position for a C/MRI CPNode system
		
	private:
	
		bool startTimer();  // Timer used to slow down turnout movement
		void checkButton();  // Function used to check button in main void loop to move turnout points
		void throwTurnout();  // Handles actual turnout movement using Servo library
		void setCMRIposition();
		void setupPoints(unsigned long bootStartTime, bool inSetupMode);  // Used in turnoutSetup() to set point travel in setup mode.
		int handleButtonInput();  // special button input function used only in setup mode, NOT main void loop.
		void startBlinking(int blinkCount, float blinksPerSecond);  // function to flash built-in LED to give user feedback in setup mode.
		bool updateBlinker();  // helper function for startBlinking() 
		
		// hold EEPROM start value
		int eepromStart;
		
		// hold EEPROM addresses
		// setup addresses for EEPROM
		int sAddress;
		int dAddress;
		
		// hold angle
		byte straight;
		byte divergent;
		
		// button and servo pins
		int buttonPin;
		int servoPin;
		
		// servo step delay and timer
		long stepDelay;
		unsigned long timerA = 0;
		
		// holds current position and target position
		int positionNow;
		int targetPosition;
		
		// save button state and last state of button
		int buttonState = 0;
		bool wasPressed = false;
		unsigned long lastDebounceTime = 0;
		unsigned long debounceDelay = 25;
		int lastReading = 0;
		
		// save last CMRI command
		byte lastCMRI;
		
		// save CMRI position of turnout
		int cmriPosition;
		
		// store if moving or not
		bool isMoving;
		
		// create servo object
		Servo servo;
		
		// variables for setup functions
		bool inSetupMode;
		// Required hold time for long press of button during setup.
		// In milliseconds.  Increase or decrease if desired.
		const unsigned long HOLD_DURATION_MS = SETUP_HOLD_TIME;

		// Custom return types for our button function
		const int BTN_NONE = 0;
		const int BTN_CLICK = 1;
		const int BTN_HOLD = 2;

		// Define the LED hardware pin used for setting points.
		// Pin 13 is the built-in LED on Arduino.
		const int LED_PIN = 13; 

		// Global state variables for the non-blocking blinker
		bool isBlinking = false;       // Tracks if a blink sequence is currently active
		int blinksRemaining = 0;       // Keeps track of how many blinks are left to complete
		unsigned long blinkInterval = 0;  // Holds the calculated duration for each half-blink (on or off)
		unsigned long previousMillis = 0; // Stores the timestamp of the last LED state change
		int ledState = LOW;            // Current digital state of the LED used for setting turnout points	
};

#endif
