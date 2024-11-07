//Written by Maeve Baksa for Union College MER-312 on November 7th, 2024.

#include <Arduino.h>

// defines pins numbers
const int moveStep = 2;
const int moveDir  = 5;

const int cardStep = 3;
const int cardDir  = 6;

const int enPin = 8;
const int homingPin = 11;

//set delay time aka: "speed"
const int delayTime = 500;
const int homingDelayTime = 500;

//for testing
const bool ignoreHoming = false;

//set to (360/stepangle)*microstepping or ((360/1.8)*32)
const int stepsPerRotation = 200*32;

//how far to rotate to dispense card in degrees
int cardDispenserTheta = 180;

//set initial positions to zero, it will be homed
int currentMoveTheta = 0;
int currentCardTheta = 0;

void moveStepper(int motor, int direction, int theta, bool absolute){
  int stepsNeeded = 0;

  //calculate steps to take, check if it is absolute or not
  if (absolute){
    if (motor == 1){
      //float hell, you have to stipulate floating division
      float rotFrac = (float)(theta-currentMoveTheta)/360;
      stepsNeeded = abs(rotFrac * stepsPerRotation);

      currentMoveTheta = theta;
      Serial.println("ABSOLUTE");
      Serial.println("Rotation Fraction: " + String(rotFrac));


    }
    else if (motor == 2){
      float rotFrac = (float)(theta-currentCardTheta)/360;
      stepsNeeded = abs(rotFrac * stepsPerRotation);
      currentCardTheta = theta;
      Serial.println("ABSOLUTE");
      Serial.println("Rotation Fraction: " + String(rotFrac));
    }
  }

  else{
    if (motor == 1){
      float rotFrac = (float)theta/360;
      stepsNeeded = abs(rotFrac * stepsPerRotation);
      currentMoveTheta += theta%360;
      Serial.println("NONABSOLUTE");
      Serial.println("Rotation Fraction: " + String(rotFrac));


    }
    else if (motor == 2){
      float rotFrac = (float)theta/360;
      stepsNeeded = abs(rotFrac * stepsPerRotation);
      currentMoveTheta += theta%360;
      Serial.println("NONABSOLUTE");
      Serial.println("Rotation Fraction: " + String(rotFrac));

    }
  }
  Serial.println("Current Card Motor Pos: " + String(currentCardTheta));
  Serial.println("Current Move Motor Pos: " + String(currentMoveTheta));
  Serial.println("Steps Needed: "+ String(stepsNeeded));
  Serial.println("");
  //source: 
  //https://www.makerguides.com/a4988-stepper-motor-driver-arduino-tutorial/
  //left motor
  if (motor == 1){
    //checks the direction that is desired, and set Dir accordingly
    if (direction == 1){
      digitalWrite(moveDir, LOW);
    }
    else if (direction == 2){
      digitalWrite(moveDir, HIGH);
    }
    //repeat this for the amount of pulses needed (mm*steps / mm = steps)
    for (int i = 0; i < int(stepsNeeded); i++) {
    // Pulsing the pin like with PWM will change the speed.
      digitalWrite(moveStep, HIGH);
      delayMicroseconds(delayTime);
      digitalWrite(moveStep, LOW);
      delayMicroseconds(delayTime);
    }
  }



    if (motor == 2){
    //checks the direction that is desired, and set Dir accordingly
    if (direction == 1){
      digitalWrite(cardDir, LOW);
    }
    else if (direction == 2){
      digitalWrite(cardDir, HIGH);
    }
    //repeat this for the amount of pulses needed (mm*steps / mm = steps)
    for (int i = 0; i < int(stepsNeeded); i++) {
    // Pulsing the pin like with PWM will change the speed.
      digitalWrite(cardStep, HIGH);
      delayMicroseconds(delayTime);
      digitalWrite(cardStep, LOW);
      delayMicroseconds(delayTime);
    }
  }
}


void setup() {

  //start serial monitor
  Serial.begin(9600);

  // Sets the two pins as Outputs
  pinMode(moveStep,OUTPUT);
  pinMode(moveDir,OUTPUT);
  pinMode(cardStep,OUTPUT);
  pinMode(cardDir,OUTPUT);
  pinMode(homingPin,INPUT_PULLUP);


  pinMode(enPin,OUTPUT);
  digitalWrite(enPin,LOW);


  //set the direction to clockwise
  digitalWrite(moveDir,HIGH);
  digitalWrite(cardDir,HIGH);

  Serial.write("Ready");

  //homing
  //TODO

  if (!ignoreHoming){
    while(digitalRead(homingPin) == HIGH){
      digitalWrite(moveStep, HIGH);
      delayMicroseconds(homingDelayTime);
      digitalWrite(moveStep, LOW);
      delayMicroseconds(homingDelayTime);
    }
    Serial.println("Homed");
  }

}

void loop() {
  //first position
  moveStepper(1,1,75,false);
  delay(1000);
  moveStepper(2,1,cardDispenserTheta,false);
  delay(1000);

  //second position
  moveStepper(1,1,105,false);
  delay(1000);
  moveStepper(2,1,cardDispenserTheta,false);
  delay(1000);

  //final position
  moveStepper(1,1,180,false);
  delay(1000);
  moveStepper(2,1,cardDispenserTheta,false);
  delay(1000);

}