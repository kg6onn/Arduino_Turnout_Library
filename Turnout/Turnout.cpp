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
	_buttonPin = 99;
	_servoPin = servoPin;
	_straight = straight;
	_divergent = divergent;
	_stepDelay = stepDelay;
	_buttonState = 0;
	_lastButtonState = 0;
	_timerA = 0;
	_positionNow = _divergent; 
	_targetPosition = _straight;
	_isMoving = false;
}

Turnout::Turnout(int buttonPin, int servoPin, int straight, int divergent, long stepDelay)
{
	_buttonPin = buttonPin;
	_servoPin = servoPin;
	_straight = straight;
	_divergent = divergent;
	_stepDelay = stepDelay;
	_buttonState = 0;
	_lastButtonState = 0;
	_timerA = 0;
	_positionNow = _divergent; 
	_targetPosition = _straight;
	_isMoving = false;
}

void Turnout::TurnoutSetup()
{
	if(_buttonPin != 99)
	{
		pinMode(_buttonPin, INPUT_PULLUP);
	}
	
    _servo.attach(_servoPin);
    _positionNow = (_divergent + _straight)/2;
    _servo.write(_positionNow);
}

void Turnout::Update()
{
	if(_buttonPin != 99)
	{
		checkButton();
	}
	throwTurnout();
}

int Turnout::getPosition()
{
	int retVal = 0;
    if(_isMoving)
    {
      retVal = 1;
    }
    else if(_positionNow == _straight)
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
	return _positionNow;
}

void Turnout::setTurnout()
{
	_servo.attach(_servoPin);
    _isMoving = true;
    if(_positionNow == _straight)
     {
      _targetPosition = _divergent;
     }
     else if(_positionNow == _divergent)
     {
      _targetPosition = _straight;
     }
}

boolean Turnout::StartTimer(unsigned long &_timer, long interval)
{
	unsigned long CurrentMillis = millis();
  
    if(CurrentMillis - _timer >= interval)
    {
      _timer = CurrentMillis;
      return true;
    }
    else
    {
      return false;
    }
}

void Turnout::checkButton()
{
	_buttonState = digitalRead(_buttonPin);
    if(_buttonState != _lastButtonState)
    {
      if(_buttonState == LOW)
      {
        setTurnout();
      }
    }
    _lastButtonState = _buttonState;
}

void Turnout::throwTurnout()
{
	boolean itsTime = StartTimer(_timerA, _stepDelay);
    if(itsTime)
    {
      if(_positionNow < _targetPosition)
      {
        _servo.write(++_positionNow);
      }
      else if(_positionNow != _targetPosition)
      {
          _servo.write(--_positionNow);
      }
    }
    if(_positionNow == _targetPosition)
    {
      _servo.detach();
      _isMoving = false;
    }
}
