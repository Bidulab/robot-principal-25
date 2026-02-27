#include <math.h>
#include <Servo.h>
#include <Arduino.h>
//#include <Wire.h>

#include "Remote.h"
#include <TM1637Display.h>
#include <TMC2209.h>
#include "MyStepper.h"
#include <TimerThree.h>

#define LED 13

// Define the pins for TM1637 module
#define TM1637_CLK_PIN 23
#define TM1637_DIO_PIN 22

#define MAIN_DRIVERS_SERIAL Serial1
#define CLAMP_DRIVER_SERIAL Serial2

// Define stepper pins
#define MAIN_DRIVERS_EN_PIN 54 // A0
#define CLAMP_DRIVER_EN_PIN 55 // A1
#define CLAMP_DRIVER_DIAG_PIN 56 // A2


#define MAIN_DRIVER_1_STEP_PIN 9
#define MAIN_DRIVER_1_DIR_PIN 8
#define MAIN_DRIVER_2_STEP_PIN 5
#define MAIN_DRIVER_2_DIR_PIN 4
#define MAIN_DRIVER_3_STEP_PIN 11
#define MAIN_DRIVER_3_DIR_PIN 10
#define MAIN_DRIVER_4_STEP_PIN 3
#define MAIN_DRIVER_4_DIR_PIN 2
#define CLAMP_DRIVER_STEP_PIN 13
#define CLAMP_DRIVER_DIR_PIN 12


Remote myRemote(9600, 30);

MyStepper stepper_1;
MyStepper stepper_2;
MyStepper stepper_3;
MyStepper stepper_4;

TM1637Display display(TM1637_CLK_PIN, TM1637_DIO_PIN);

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

float speed, spin;
const float rotation = .8;
double angle;
float moteurClamp = 0.0;
const int angle_open = 140;
const int angle_closed = 10;
bool servo_closing = false;
bool servo1_closed = false;
bool servo2_closed = false;
bool servo3_closed = false;
bool servo4_closed = false;
bool servo1_oppenning = false;
bool servo2_oppenning = false;
bool servo3_oppenning = false;
bool servo4_oppenning = false;

void stepper_it(){
  stepper_1.loop();
  stepper_2.loop();
  stepper_3.loop();
  stepper_4.loop();  
}

void setup() {
  pinMode(MAIN_DRIVERS_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_DIAG_PIN, INPUT);
  digitalWrite(MAIN_DRIVERS_EN_PIN, LOW);  // Enabled
  digitalWrite(CLAMP_DRIVER_EN_PIN, LOW);  // Enabled

  pinMode(LED, OUTPUT);
 // Serial.begin(115200); //Computer communication

  display.setBrightness(5); // Set the brightness level (0 to 7)

  servo1.attach(6); //68
  servo2.attach(7); //67
  servo3.attach(44);
  servo4.attach(46);

  delay(500);

  stepper_1.begin(MAIN_DRIVER_1_STEP_PIN, MAIN_DRIVER_1_DIR_PIN);
  stepper_2.begin(MAIN_DRIVER_2_STEP_PIN, MAIN_DRIVER_2_DIR_PIN);
  stepper_3.begin(MAIN_DRIVER_3_STEP_PIN, MAIN_DRIVER_3_DIR_PIN);
  stepper_4.begin(MAIN_DRIVER_4_STEP_PIN, MAIN_DRIVER_4_DIR_PIN);
  stepper_1.spin(0.0);
  stepper_2.spin(0.0);
  stepper_3.spin(0.0);
  stepper_4.spin(0.0);

  ouvrir_pinces();

  // Give time to the remote to start
  delay(200);

  Timer3.initialize(2000);
  Timer3.attachInterrupt(stepper_it);
}

void loop() {
  //stepper_it();
  if (myRemote.updateValues()){

  float xVal = (float)myRemote.Joystick1_X;
  float yVal = (float)myRemote.Joystick1_Y;
  float aVal = (float)myRemote.Joystick2_X;
  
  spin = aVal;
  if (abs(aVal) < 30)
    spin = 0;
  else{
    if (aVal > 0)
      spin -= 30;
    else
      spin += 30;
  }
  spin *= rotation;

  float moteur1_target = spin; //Derriere
  float moteur2_target = spin; //Droite
  float moteur3_target = spin; //Gauche
  float moteur4_target = spin; //Avant
  if (abs(xVal) < 30)
    xVal = 0;
  else{
    if (xVal > 0)
      xVal -= 30;
    else
      xVal += 30;
  }
  if (abs(yVal) < 30)
    yVal = 0;
  else{
    if (yVal > 0)
      yVal -= 30;
    else
      yVal += 30;
  }
  if (abs(xVal) > 30 || abs(yVal) > 30){
    if (abs(xVal) < abs(yVal)){
      
      // Avant Arriere
      moteur2_target -= yVal;
      moteur3_target += yVal;
    } else {
      // Gauche Droite
      moteur1_target -= xVal;
      moteur4_target += xVal;    
    }
  }

  stepper_1.spin(moteur1_target * 8.0);
  stepper_2.spin(moteur2_target * 8.0);
  stepper_3.spin(moteur3_target * 8.0);
  stepper_4.spin(moteur4_target * 8.0);

  if ((!servo1_closed && !servo2_closed && !servo3_closed && !servo4_closed) && (myRemote.Button1 || myRemote.Button2 || myRemote.Button3 || myRemote.Button4)){
    fermer_pinces();
    servo_closing = true;
  }
  if (!servo_closing){
    if (myRemote.Button1 && !servo1_oppenning){
      servo1.write(angle_open);
      servo1_oppenning = true;
    } else if (!myRemote.Button1 && servo1_oppenning){
      servo1_closed = false;
      servo1_oppenning = false;
    }
    if (myRemote.Button2 && !servo2_oppenning){
      servo2.write(angle_closed);
      servo2_oppenning = true;
    } else if (!myRemote.Button2 && servo2_oppenning){
      servo2_closed = false;
      servo2_oppenning = false;
    }
    if (myRemote.Button3 && !servo3_oppenning){
      servo3.write(angle_open);
      servo3_oppenning = true;
    } else if (!myRemote.Button3 && servo3_oppenning){
      servo3_closed = false;
      servo3_oppenning = false;
    }
    if (myRemote.Button4 && !servo4_oppenning){
      servo4.write(angle_closed);
      servo4_oppenning = true;
    } else if (!myRemote.Button4 && servo4_oppenning){
      servo4_closed = false;
      servo4_oppenning = false;
    }    
  }
  else if (!(myRemote.Button1 || myRemote.Button2 || myRemote.Button3 || myRemote.Button4)){
    servo_closing = false;
  }
  

  //display.showNumberDec(myRemote.counter);
  digitalWrite(LED, myRemote.Button1 || myRemote.Button2 || myRemote.Button3 || myRemote.Button4 || myRemote.Joystick1_SW || myRemote.Joystick2_SW);

  }
}
void ouvrir_pinces(){
  servo1.write(angle_open);
  servo2.write(angle_closed);
  servo3.write(angle_open);
  servo4.write(angle_closed);
  servo1_closed = false;
  servo2_closed = false;
  servo3_closed = false;
  servo4_closed = false;
}
void fermer_pinces(){
  servo1.write(angle_closed);
  servo2.write(angle_open);
  servo3.write(angle_closed);
  servo4.write(angle_open);  
  servo1_closed = true;
  servo2_closed = true;
  servo3_closed = true;
  servo4_closed = true;
}
