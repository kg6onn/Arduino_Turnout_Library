# Arduino_Turnout_Library
A Library for Arduino for controlling a model railroad turnout with a servo.

This Arduino library is designed to create a slow motion turnout controller with a servo 
for a model railroad.  It uses Arduino millis() function for slow motion control so there is no delay when
utilizing the library for multiple turnouts.

The library contains an overloaded constructor so you have the option of controling a turnout with the library's 
built-in push button control or using your own system to determine when the turnout will be thrown.  It is also possible to use the built-in pushbutton and other methods of control as well.  It is also possible to use the library with Model Railroad Control Systems CPNode library for use with their CPNode C/MRI on Arduino Nodes.

Installing Library:

Place the "Turnout" directory in your Arduino "libraries" directory.

Including Library:

Place #include "Turnout.h" at the top of your sketch.  There is no need to import Arduino's Servo library as this
is handled by the Turnout library.

Constructor:

The constructor is overloaded with an option for using a built-in push button or not.

The constructor with a push button is the easiest method for using the library since most of the work
is done for you.  It is as follows:

Turnout(int buttonPin, int servoPin, int straight, int divergent, long stepDelay)

buttonPin: Optional pin used for a pushbutton.  It is set up in the library as INPUT_PULLUP so no 
           pull up resistor is necessary.

servoPin:  The Arduino pin that connects to the control line of the servo.

straight:  The angle of servo with your turnout in the straight postion.

divergent:  The angle of servo with your turnout in the divergent position.

stepDelay:  delay in milliseconds of turnout movement per degree.  You can try different times, the basic example
            uses 70.

The other constructor is the same except the button pin is not included.  It is as follows:

Turnout(int servoPin, int straight, int divergent, long stepDelay)

With this constructor, the optional button is not used and it is never setup as an input.  This constructor is 
reccomended if you want to control a turnout with C/MRI or some other serial Communications method.

Public Functions:

turnoutSetup():

This function needs to be called in the Void setup() method in your Arduino sketch.  It will attach the servo to
the pin and will move the servo to half the distance between your straight and divergent angles before slowing moving into the straight position.  The idea with this was to put the turnout into a known postion at startup.


