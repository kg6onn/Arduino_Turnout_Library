# Arduino_Turnout_Library
A Library for Arduino for controlling a model railroad turnout with a servo

This Arduino library is designed to create a slow motion turnout controller with a servo 
for a model railroad.  It uses Arduino millis() function for slow motion control so there is no delay when
utilizing the library for multiple turnouts.

The library contains an overloaded constructor so you have the option of controling a turnout with the library's 
built-in push button control or using your own system to determine when the turnout will be thrown.  It is also possible to use the built-in pushbutton and other methods of conrol as well.  This will all be explained in this documentation.

Installing Library:

Place the "Turnout" directory in your Arduino "libraries" directory.  The turnout library includes the Arduino
Servo library so there is no need to import Servo into your main sketch.

Constructor:

The constructor is overloaded with an option for using a built-in push button or not.

The constructor with a push button is the easiest method for using the library since most of the work
is done for you.  It is as follows:

Turnout(int buttonPin, int servoPin, int straight, int divergent, long stepDelay)

buttonPin: The pin used for a pushbutton.  It is set up in the library as INPUT_PULLUP so no 
           pull up resistor is necessary.

servoPin:  The Arduino pin that connects to the control line of the servo.

straight:  The angle of servo with your turnout in the straight postion.

divergent:  The angle of servo with your turnout in the divergent position.

stepDelay:  delay in milliseconds of turnout movement per degree.  You can try different times, the basic example
            uses 70.

More Documentation to follow on using available library functions and the other constructor.
