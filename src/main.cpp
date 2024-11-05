#include <Arduino.h>
#include <Servo.h>

//some of the following is taken from the previous Arduino challenges.


//*** HARDWARE SETTINGS ****
//variables for the pins
int servopinright = 12;
int servopinleft = 13;

Servo rightservo;
Servo leftservo;

int sensorpinfront = 0; 
int sensorpinright = 1;
int sensorpinleft = 2;

int ledpinfront = 3;
int ledpinright = 2;
int ledpinleft = 4;

int diagpin = 5;


//***** HARDWARE DRIVE TRAIN CALIBRATION ***
//set this to 1 in order to enable the motors to just be at 1500 1500 to tune their 0 points, do this first before
//the software calibration
//how to calibrate:
//(1) set this value to 1 and upload the code.
//(2) use a fine screwdriver on the back of the servo motor (looks like a small screw) to set the 0 point of the motor
//    by slowly turning "the screw" (a potentiometer) on the back until the motor is completely still. This requires 
//    very fine control and it may take a while.
//(3) reset this value to 0 and reupload the code.
int calibrationenabled = 0;


//***** SOFTWARE DRIVE TRAIN CALIBRATION ****
//calibration data software jank 
//how to calibrate:
//(1) start with 1300 and 1700 and make sure that the robot is driving in the right direction, if not, you will have to 
//    switch the 1300 and 1700 around.
//(2) see which way the robot is veering
//(3) if the robot is veering to the left, then you will need to increase the left value proportionally to the right
//    value. This means you can bring the right value closer to 1500 slightly, decreasing the speed on the right motor
//(4) if the robot is verring to the right, then you need to do the opposite, increasing the right value proportionally
//    to the left value. This means you can bring the left value closer to 1500, decreasing the speed on the right motor.
//(5) continue adjusting in increments of 50 at first, then 10, and then down to +-1. Small changes make a large
//    difference here, especially the closer you are to 1500. You'll notice that you've gotten close by the 
//    direction the robot is veering in changing. Once that happens, you can slowly tweak the value back in the other 
//    direction to get the precise value.
//(6) enjoy!
int rightdefault = 1590;
int leftdefault = 1300;



//***** SOFTWARE SENSOR CALIBRATION ****
//how to calibrate:
//(1) start with enabling serial monitoring (set to 1)
int serialmonitoring = 0;
//(2) calibrate the front sensor by placing the robot onto the line, and moving it around, note down these values
//(2a) continue by placing the robot off of the line, and noting down these values
//(2b) find a value, that is approximately in the middle, make sure that throughout the entire length of the line
// the value does not increase above this value 
int frontthreshold = 35;
//(3) calibrate the side sensors in the opposite way, by placing the robot onto the line for one side sensor, noting down these values
//(3a) continue by placing the robot in the middle of the line so that the side sensors are reading off the line, noting down these values
//(3b) find a value, that is approximately in the middle, make sure that throughout moving the robot down the line so that
// the sensors aren't on the line, the value doesn't drop BELOW this value
//(3c) repeat for other side
int rightthreshold = 35;
int leftthreshold = 35;

//for reporting which pins are tripped
int righttripped = 0;
int fronttripped = 0;
int lefttripped = 0;

int failiure = 0;

//right and left default are in charge of making sure that the robot drives straight, this is for compensation and can
//be thought of as "actual"
int right = rightdefault;
int left = leftdefault;


//leave at defuault
int rightadjustmentamount = 30;
int leftadjustmentamount = 30;


//is called by the logic later in the program
//Reset = make robot go straight using default values
//Rotate = make robot rotate
//Stop = make robot stop
//Adjright/left = see individual notes, but increase power to corresponding motor
//in order to straigten the path
void writeServoValue(bool reset, bool rotate, bool stop, bool adjright, bool adjleft){

  if (reset == true){
      right = rightdefault;
      left = leftdefault;

  }

  if (adjright == true){
    //we want to increase the power to the right wheel slightly, which would
    //mean making it turn more CLOCKWISE, or making the right value LARGER
    right = right + rightadjustmentamount;
  }

  if (adjleft == true){
    //we want to increase the power to the left wheel slightly, which would
    //mean making it turn more COUNTERCLOCKWISE, or making the left value
    //SMALLER
    left = left - leftadjustmentamount;
  }

  if (rotate == true){
    leftservo.writeMicroseconds(right);
    rightservo.writeMicroseconds(right);
  }

  if (stop == true){
    leftservo.writeMicroseconds(1500);
    rightservo.writeMicroseconds(1500);    
  }

  else{
    leftservo.writeMicroseconds(left);
    rightservo.writeMicroseconds(right);
  }

  
}


void setup() {
  //enable serial mon
  Serial.begin(9600);

  //attach pins
  rightservo.attach(servopinright);
  leftservo.attach(servopinleft);

  //set io
  pinMode(ledpinfront, OUTPUT);
  pinMode(ledpinleft,OUTPUT);
  pinMode(ledpinright, OUTPUT);
  pinMode(diagpin, OUTPUT);
}

void loop() {
  //sensor calibration code
  if (serialmonitoring == 1){

    //printing the values of the pins, check threshold, enable LED accordingly

    //hold the values of the analog pins for use later
    int righthold  = analogRead(sensorpinright);
    int lefthold = analogRead (sensorpinleft);
    int fronthold = analogRead(sensorpinfront);

    //start with the right pin, print then pin, set the LED
    Serial.println("Right:");
    Serial.println(righthold);
    
    if (righthold < rightthreshold){
      digitalWrite(ledpinright, HIGH);
    }
    else{
      digitalWrite(ledpinright, LOW);
    }

    //left
    Serial.println("Left:");
    Serial.println(lefthold);

    if (lefthold < leftthreshold){
      digitalWrite(ledpinleft, HIGH);
    }
    else{
      digitalWrite(ledpinleft, LOW);
    }

    //front
    Serial.println("Front:");
    Serial.println(fronthold);

    if (fronthold > frontthreshold){
      digitalWrite(ledpinfront, HIGH);
    }
    else{
      digitalWrite(ledpinfront, LOW);
    }

    //diag pin for failiure.
    if (fronthold == 0 || lefthold == 0 || righthold == 0){
      digitalWrite(diagpin, HIGH);
    }
    else{
      digitalWrite(diagpin, LOW);
    }
  }

  //motor calibration code
  else if (calibrationenabled == 1){

    //this is used in my other code, set the motors to simply mid values so they can be trammed
    leftservo.writeMicroseconds(1500);
    rightservo.writeMicroseconds(1500);
    delay(900000);
  }

  //actual code
  else{

    //hold the values of the analog pins for use later
    int righthold  = analogRead(sensorpinright);
    int lefthold = analogRead (sensorpinleft);
    int fronthold = analogRead(sensorpinfront);

    //check if any of the pins are in a tripped position and change the corresponding value

    //right
    if (righthold < rightthreshold){
      digitalWrite(ledpinright, HIGH);
      righttripped = 1;
    }
    else{
      digitalWrite(ledpinright, LOW);
      righttripped = 0;
    }

    //left

    if (lefthold < leftthreshold){
      digitalWrite(ledpinleft, HIGH);
      lefttripped = 1;
    }
    else{
      digitalWrite(ledpinleft, LOW);
      lefttripped = 0;
    }

    //front

    if (fronthold > frontthreshold){
      digitalWrite(ledpinfront, HIGH);
      fronttripped = 1;
    }
    else{
      digitalWrite(ledpinfront, LOW);
      fronttripped = 0;
    }

    //diag pin for failiure.
    if (fronthold == 0 || lefthold == 0 || righthold == 0){
      digitalWrite(diagpin, HIGH);
      failiure = 1;
    }
    else{
      digitalWrite(diagpin, LOW);
      failiure = 0;
    }

  //THE IMPORTANT STUFF

  // using our new values for tripped and not tripped, we can figure out what state the robot is in

  //robot encounters a (only the front sensor is tripped, this is the most complex case)
  if (fronttripped == 1 && lefttripped == 0 && righttripped == 0 && failiure == 0){
    //rotate until it finds the line again (in this case, to the right)
    while (fronttripped == 0){
      writeServoValue(false,true,false,false,false);
    }
    //once the line is found again, have it go straight by resetting
    writeServoValue(true,false,false,false,false);
  }

  //robot is veering left (right sensor is tripped)
  else if (lefttripped == 0 && righttripped == 1 && failiure == 0){
    //make these values a reality without resetting them also, adjust left
    writeServoValue(false,false,false,false,true);
  }

  //robot is veering right (left sensor is tripped)
  else if (righttripped == 1 && lefttripped == 1 && failiure == 0){
    //make these values a reality without resetting them also, adjust right
    writeServoValue(false,false,false,true,false);
  }

  //robot is okay, drive straight
  else if (fronttripped == 0 && lefttripped == 0 && righttripped == 0 && failiure == 0){
    //reset left and right values to drive straight 
    writeServoValue(true,false,false,false,false);
  }

  //failiure occurs, or robot has all sensors tripped, which makes no sense
  else{
    //stop the robot
    writeServoValue(false,false,true,false,false);
  }

  //only check every 0.1 seconds, I feel like more than that is excessive
  delay(100);

  }
  
}