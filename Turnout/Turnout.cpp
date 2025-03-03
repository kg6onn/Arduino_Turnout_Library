/* 
 Turnout.cpp - Library for controling a model railroad turnout
 with a servo.
 Created by John Herbert, Februrary 11, 2025
 Released into the public domain.
 */

#include "Arduino.h"
#include "Turnout.h"
#include <Servo.h>

Turnout::Turnout(int servoPin, int straight, int divergent, long stepDelay)
{
	buttonPin = 99;
	this->servoPin = servoPin;
	this->straight = straight;
	this->divergent = divergent;
	this->stepDelay = stepDelay;
	buttonState = 0;
	lastButtonState = 0;
	timerA = 0;
	positionNow = divergent; 
	targetPosition = straight;
	isMoving = false;
	lastState = 0;
}

Turnout::Turnout(int buttonPin, int servoPin, int straight, int divergent, long stepDelay)
{
	this->buttonPin = buttonPin;
	this->servoPin = servoPin;
	this->straight = straight;
	this->divergent = divergent;
	this->stepDelay = stepDelay;
	buttonState = 0;
	lastButtonState = 0;
	timerA = 0;
	positionNow = divergent; 
	targetPosition = straight;
	isMoving = false;
	lastState = 0;
}

void Turnout::turnoutSetup()
{
	if(buttonPin != 99)
	{
		pinMode(buttonPin, INPUT_PULLUP);
	}
	
    servo.attach(servoPin);
    positionNow = (divergent + straight)/2;
    servo.write(positionNow);
}

void Turnout::update()
{
	if(buttonPin != 99)
	{
		checkButton();
	}
	throwTurnout();
}

int Turnout::getPosition()
{
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
	return positionNow;
}

int Turnout::getCMRIposition()
{
	
	if(positionNow == straight)
	{	
		return 0;
		
	}
	else if(positionNow == divergent)
	{
		return 1;
	}
}

void Turnout::setTurnout()
{
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
	if(state == 0x01)
	{
	  servo.attach(servoPin);
	  isMoving = true;
	  targetPosition = straight;
		  
	}
	else if(state == 0x00)
	{
	  servo.attach(servoPin);
	  isMoving = true;	
	  targetPosition = divergent;
	}
}

boolean Turnout::startTimer(unsigned long &timer, long interval)
{
	unsigned long CurrentMillis = millis();
  
    if(CurrentMillis - timer >= interval)
    {
      timer = CurrentMillis;
      return true;
    }
    else
    {
      return false;
    }
}

void Turnout::checkButton()
{
	buttonState = digitalRead(buttonPin);
    if(buttonState != lastButtonState)
    {
      if(buttonState == LOW)
      {
        setTurnout();
      }
    }
    lastButtonState = buttonState;
}

void Turnout::throwTurnout()
{
	boolean itsTime = startTimer(timerA, stepDelay);
    if(itsTime)
    {
      if(positionNow < targetPosition)
      {
        servo.write(++positionNow);
      }
      else if(positionNow != targetPosition)
      {
          servo.write(--positionNow);
      }
    }
    if(positionNow == targetPosition)
    {
      servo.detach();
      isMoving = false;
    }
}
