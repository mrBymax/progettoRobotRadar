/*
* ----------------------------------------------------
* Creator: Federico Bertossi
* Title: Robot that detects its nearest object
* Platform: Arduino UNO
* ----------------------------------------------------
*/


#include <Arduino.h>
#include <Servo.h>
#include "robot.h"

// Ultrasonic Sensor
#define triggerPin 8
#define echoPin 7

// Servo Motors
#define servoRightPin 12
#define servoLeftPin 13

Servo servoRight;
Servo servoLeft;

// Compass Variables
#define CLK_pin 9
#define EN_pin 8
#define DIO_pin 7

int X_data = 0;
int Y_data = 0;
int angle;

// LED
#define ledPin 5

// Rotation Precision
#define precision 10

Robot myRobot = Robot();

void setup(){
    Serial.begin(9600);
    pinMode(ledPin, OUTPUT);

    myRobot.attachServos(servoRightPin, servoLeftPin);
    myRobot.initUltrasonic(triggerPin, echoPin);
    myRobot.initCompass(EN_pin, CLK_pin, DIO_pin);
}

void loop(){
    static long minDistance = 90;
    static int minDistanceAngle;

    myRobot.getAngle(); // First reading is always wrong
    int firstAngle = myRobot.getAngle(), nowAngle;
    byte count = 0;

    Serial.print("First angle: ");
    Serial.println(firstAngle);

    // Start searching for the closest object
    while(!((firstAngle - precision) < nowAngle && nowAngle < (firstAngle + precision)) || count < 5){
        nowAngle = myRobot.getAngle();

        Serial.print("New angle: ");
        Serial.println(nowAngle);

        long nowDistance = myRobot.calculateDistance();
        if(nowDistance < minDistance){
            minDistance = nowDistance;
            minDistanceAngle = nowAngle;
        }

        Serial.print("Distance: ");
        Serial.println(nowDistance);

        myRobot.rotateRight();
        delay(100);
        myRobot.stopRotation();

        count++;
    }
    myRobot.stopRotation();

    // Aiming to the closest object
    digitalWrite(ledPin, HIGH);
    delay(2000);

    while(!(minDistanceAngle - precision < nowAngle && nowAngle < minDistanceAngle + precision)){
        myRobot.stopRotation();
        nowAngle = myRobot.getAngle();
        Serial.print("Search ");
        Serial.print(nowAngle);
        Serial.print(" : ");
        Serial.println(minDistanceAngle);
    }
    myRobot.stopRotation();
    Serial.println("Found!");
    
    long nowDistance = myRobot.calculateDistance();
    myRobot.goAhed();
    while(nowDistance > 5){
        nowDistance = myRobot.calculateDistance();
        Serial.println(nowDistance);
        delay(100);
    }

    myRobot.stopRotation();
    for (int i = 0; i < 20; i++)
    {
        digitalWrite(ledPin, LOW);
        delay(250);
        digitalWrite(ledPin, HIGH);
        delay(250);
    }
    delay(10000);
    digitalWrite(ledPin, LOW);
}