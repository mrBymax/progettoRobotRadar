#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include <Servo.h>
#include <math.h>

class Robot
{
public:
    Robot();

    // Ultrasonic sensor
    void initUltrasonic(byte, byte);
    long ping(); //return us
    long calculateDistance(); //returns mm

    // Servo motors
    void attachServos(byte, byte);
    void detachServos();
    void rotateRight();
    void rotateRightSlow();
    void stopRotation();
    void goAhed();

    // Compass
    void initCompass(byte, byte, byte);
    int getAngle();

private:
    byte trigPin, echoPin; // Returns rotation angle
    Servo servoRight, servoLeft;

    byte CLK_pin, EN_pin, DIO_pin; // for compass
    int X_data, Y_data, angle; // For compass reading

    void shiftOut(int, int);
    int shiftIn(int);
    void HM55B_Reset();
    void HM55B_StartMeasurementCommand();
    int HM55B_ReadCommand();
};

#endif