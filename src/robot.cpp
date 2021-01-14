#include <Arduino.h>
#include "robot.h"

#define MAX_TIME 1700
#define MIN_TIME 1300
#define IDLE_TIME 1500

Robot::Robot() {}

/*
* ------------------
* ULTRASONIC SENSOR
* ------------------
*/

void Robot::initUltrasonic(byte trig, byte echo)
{
  this->trigPin = trig;
  this->echoPin = echo;
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
}

long Robot::ping()
{
  long duration;
  digitalWrite(this->trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(this->trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(this->trigPin, LOW);
  delayMicroseconds(100);

  duration = pulseIn(this->echoPin, HIGH, 25000); // pin | status to read | 25.000 us = 25 ms | after timeout returns 0
  return duration;
}

long Robot::calculateDistance()
{
  long distance = this->ping() / 58; // Returns cm
  if (distance <= 0)
    return 90;

  return distance;
}

/*
* ------------------
* SERVO MOTOR
* ------------------
*/

void Robot::attachServos(byte leftPin, byte rightPin)
{
  this->servoRight.attach(rightPin);
  this->servoLeft.attach(leftPin);
}

void Robot::detachServos()
{
  this->servoRight.detach();
  this->servoLeft.detach();
}

void Robot::rotateRight()
{
  this->servoRight.writeMicroseconds(MAX_TIME);
  this->servoLeft.writeMicroseconds(MAX_TIME);
}

void Robot::rotateRightSlow()
{
  this->servoRight.writeMicroseconds(MAX_TIME - 180);
  this->servoLeft.writeMicroseconds(MAX_TIME - 180);
}

void Robot::stopRotation()
{
  this->servoRight.writeMicroseconds(IDLE_TIME);
  this->servoLeft.writeMicroseconds(IDLE_TIME);
}

void Robot::goAhed()
{
  this->servoRight.writeMicroseconds(MAX_TIME - 100);
  this->servoLeft.writeMicroseconds(MAX_TIME + 100);
}

/*
* ------------------
* COMPASS
* ------------------
*/

void Robot::initCompass(byte EN_pin, byte CLK_pin, byte DIO_pin)
{
  this->EN_pin = EN_pin;
  this->CLK_pin = CLK_pin;
  this->DIO_pin = DIO_pin;

  pinMode(EN_pin, OUTPUT);
  pinMode(CLK_pin, OUTPUT);
  pinMode(DIO_pin, INPUT);
}

void Robot::shiftOut(int value, int bitsCount)
{
  for (int i = bitsCount; i >= 0; i--)
  {
    digitalWrite(CLK_pin, LOW);
    if ((value & 1 << i) == (1 << i))
    {
      digitalWrite(DIO_pin, HIGH);
    }
    else
    {
      digitalWrite(DIO_pin, LOW);
    }
    digitalWrite(CLK_pin, HIGH);
    delayMicroseconds(1);
  }
}

int Robot::shiftIn(int bitsCount){
  int shiftIn_result;
  shiftIn_result = 0;

  pinMode(DIO_pin, INPUT);
  for (int i = bitsCount; i >= 0; i--)
  {
    digitalWrite(CLK_pin, HIGH);
    delayMicroseconds(1);

    if(digitalRead(DIO_pin) == HIGH){
      shiftIn_result = (shiftIn_result << 1) + 1;
    }
    else {
      shiftIn_result = (shiftIn_result << 1) + 0;
    }
    digitalWrite(CLK_pin, LOW);
    delayMicroseconds(1);
  }
  if((shiftIn_result & 1 <<11) == 1 << 11){
    shiftIn_result = (B11111000 < 8) | shiftIn_result;
  }

  return shiftIn_result;
}

void Robot::HM55B_Reset(){
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  shiftOut(B0000, 3);
  digitalWrite(EN_pin, HIGH);
}

void Robot::HM55B_StartMeasurementCommand(){
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  shiftOut(B1000, 3);
  digitalWrite(EN_pin, HIGH);
}

int Robot::HM55B_ReadCommand(){
  int result = 0;
  pinMode(DIO_pin, OUTPUT);
  digitalWrite(EN_pin, LOW);
  shiftOut(B1100, 3);
  result = shiftIn(3);
  return result;
}

int Robot::getAngle() {
  int angle = 0;
  for(int i = 0; i < 10; i++){
    this->HM55B_StartMeasurementCommand(); // necessary!!
    delay(40); // the data is 40ms later ready
    HM55B_ReadCommand();
    X_data = shiftIn(11); // Field strength in X
    Y_data = shiftIn(11); // and Y direction
    digitalWrite(EN_pin, HIGH); // ok deselect chip
    angle += 180 * (atan2(-1 * Y_data , X_data) / M_PI); // angle is atan( -y/x) !!!
  }
  angle /= 10;
  return angle;
}