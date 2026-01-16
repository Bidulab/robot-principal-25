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

float speed, spin;
const float rotation = .8;
double angle;
float moteurClamp = 0.0;

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
  fermer_pinces_complet();

  delay(500);

  stepper_1.begin(MAIN_DRIVER_1_STEP_PIN, MAIN_DRIVER_1_DIR_PIN);
  stepper_2.begin(MAIN_DRIVER_2_STEP_PIN, MAIN_DRIVER_2_DIR_PIN);
  stepper_3.begin(MAIN_DRIVER_3_STEP_PIN, MAIN_DRIVER_3_DIR_PIN);
  stepper_4.begin(MAIN_DRIVER_4_STEP_PIN, MAIN_DRIVER_4_DIR_PIN);
  stepper_1.spin(0.0);
  stepper_2.spin(0.0);
  stepper_3.spin(0.0);
  stepper_4.spin(0.0);

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
  if (abs(aVal) < 50)
    spin = 0;
  else{
    if (aVal > 0)
      spin -= 50;
    else
      spin += 50;
  }
  spin *= rotation;

  float moteur1_target = spin; //Derriere
  float moteur2_target = spin; //Droite
  float moteur3_target = spin; //Gauche
  float moteur4_target = spin; //Avant
  if (abs(xVal) > 50 || abs(yVal) > 50){
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

  if (myRemote.Button4){
    fermer_pinces();
  } else if (myRemote.Button3){
    ouvrir_pinces();
  }

  display.showNumberDec(myRemote.counter);
  digitalWrite(LED, myRemote.Button1 || myRemote.Button2 || myRemote.Button3 || myRemote.Button4 || myRemote.Joystick1_SW || myRemote.Joystick2_SW);

  }
}

void fermer_pinces(){
  const int angle = 10;
  servo1.write(90 + angle);
  servo2.write(90 - angle);
}

void ouvrir_pinces(){
  const int angle = 30;
  servo1.write(90 - angle);
  servo2.write(90 + angle);
}

void fermer_pinces_complet(){
  const int angle = -37;
  servo1.write(90 - angle);
  servo2.write(90 + angle);
}
