// *************************************************
// CMRI/Net example Sketch: ProMini 8OUT 8IN
// Onboard I/O:   8 bits of OUTPUT
//                8 bits of INPUT
// IOX I/O:      1x IOX-16 (1 port IN, 1 OUT)
// *************************************************
//
//  Reserved I/O pins are:
//   D0 - RX  CMRI RS485 Receive
//   D1 - TX  CMRI RS485 Transmit
//   A4 - SDA I2C Data
//   A5 - SCL I2C Clock

#include "cpNode.h"
#include "Turnout.h"  // Include Turnout Library
#include <Wire.h>  // for the I/O expander

cpNode cmri;    // Processing logic for handling CMRINet packets
IOX  iox;

// Create a turnout object with local control
Turnout turnout1(11, 8, 90, 180, 70);

const int  nodeID = 0;                            // 0...63 (nodeID + ord('A') => 'A'..chr(127))
const long CMRINET_SPEED = 19200;                 // 9600, 19200 ...

// the following need to match the code in setup(), pack() and unpack()...
const int  InputBytes  = 2 + 1;                    // 2x onboard plus 1 IOX port (A)
const int  OutputBytes = 2 + 1;                    // 2x onboard plus 1 IOX port (B)

void setup(void) {
    // *************************************************
    // *******          Setup CMRI            **********
    // *************************************************

    Serial.begin(CMRINET_SPEED);          // Set up and Open the CMRInet port
    while(!Serial) { };

    cmri.setCMRIPort(&Serial);
    cmri.setNodeAddress(nodeID);          // Set the node address
    cmri.setNumInputBytes(InputBytes);    // reflected in pack(),   2x onboard plus IOX expander
    cmri.setNumOutputBytes(OutputBytes);  // reflected in unpack(), 2x onboard plus IOX expander

    cmri.invertInputs(  true );           // invert all bits?
    cmri.invertOutputs( true );

    // *************************************************
    // *******   Setup  Onboard I/O           **********
    // *************************************************
    pinMode( 2, OUTPUT);        // D2
    pinMode( 3, OUTPUT);        // D3
    pinMode( 4, OUTPUT);        // D4
    pinMode( 5, OUTPUT);        // D5
    pinMode( 6, OUTPUT);        // D6
    pinMode( 7, OUTPUT);        // D7
    //pinMode( 8, OUTPUT);        // D8  // Used by Turnout library for servo.
    pinMode( 9, OUTPUT);        // D9

    digitalWrite( 2, 0 );       // if desired, set initial output state
    digitalWrite( 3, 0 );
    digitalWrite( 4, 0 );
    digitalWrite( 5, 0 );
    digitalWrite( 6, 0 );
    digitalWrite( 7, 0 );
    digitalWrite( 8, 0 );
    digitalWrite( 9, 0 );

    pinMode(10, INPUT_PULLUP);  // D10
    //pinMode(11, INPUT_PULLUP);  // D11  // Used by Turnout library for local control
    pinMode(12, INPUT_PULLUP);  // D12
    pinMode(13, INPUT_PULLUP);  // D13
    pinMode(A0, INPUT_PULLUP);  // A0
    pinMode(A1, INPUT_PULLUP);  // A1
    pinMode(A2, INPUT_PULLUP);  // A2
    pinMode(A3, INPUT_PULLUP);  // A3

    // Setup turnout
    turnout1.turnoutSetup();

    // *************************************************
    // *******   Setup  I/O Expander (IOX)    **********
    // *************************************************

    Wire.begin();
    iox.init( 0x20, IOX::PORT_A, IOX::IN);
    iox.init( 0x20, IOX::PORT_B, IOX::OUT);
    iox.write(0x20, IOX::PORT_B, 0x00);  // if desired, set initial output state
}

// ---------------------------------------------------------------------------
// pack() is called whenever there is a need to read bits from the layout
//        in response to a poll request
//
//  The pack input routine collects the bits from the onboard IO ports and
//  external I/O expanders and puts the them into the correct IB array bytes
//  for transmission back to the control host.
//
//  len bytes (as set by setNumInputBytes above) need to be read:
//  Onboard I/O goes in the first two bytes, IB[0] and IB[1]
//  The rest of the bytes are used by the optional IO expanders
// ---------------------------------------------------------------------------

void pack(byte *IB, int len) {
    IB[0] = 0;  // shadowed by Output bits...

    IB[1] = 0;
    IB[1] |= (!digitalRead(10) << 0);
    //IB[1] |= (!digitalRead(11) << 1);  // local control uses pin 11 now.
    IB[1] |= (!turnout1.getCMRIposition() << 1);  // now this bit is used for C/MRI feedback.
    IB[1] |= (!digitalRead(12) << 2);
    IB[1] |= (!digitalRead(13) << 3);
    IB[1] |= (!digitalRead(A0) << 4);
    IB[1] |= (!digitalRead(A1) << 5);
    IB[1] |= (!digitalRead(A2) << 6);
    IB[1] |= (!digitalRead(A3) << 7);

    IB[2] = iox.read(0x20, IOX::PORT_A);
}

// ---------------------------------------------------------------------------
// unpack() is called whenever there is a need to write bits out to the layout
//
//  The unpack output routine needs to take the received bits from the OB
//  ouput buffer and write them to the correct output ports using either
//  digitalWrite() or the IO expanders
//
//  len bytes (as set by setNumOutputBytes above) are available to be written
//  Onboard I/O comes from the first two bytes, followed by IO expander bytes
//----------------------------------------------------------------------------

void unpack(byte *OB, int len) {
    digitalWrite( 2, (( OB[0] >> 0) &  0x01) );
    digitalWrite( 3, (( OB[0] >> 1) &  0x01) );
    digitalWrite( 4, (( OB[0] >> 2) &  0x01) );
    digitalWrite( 5, (( OB[0] >> 3) &  0x01) );
    digitalWrite( 6, (( OB[0] >> 4) &  0x01) );
    digitalWrite( 7, (( OB[0] >> 5) &  0x01) );
    //digitalWrite( 8, (( OB[0] >> 6) &  0x01) );  // Used for Servo now.
    turnout1.cmriTurnout(  (( OB[0] >> 6) &  0x01) ); // bit sent to Turnout library 
    digitalWrite( 9, (( OB[0] >> 7) &  0x01) );

    // OB[1] is shadowed by the Input bits...

    iox.write(0x20, IOX::PORT_B, OB[2]);
}

void loop(void) {
    cmri.proceess();   // process any C/MRI packets
    turnout1.update();  // process any turnout1 events
}
