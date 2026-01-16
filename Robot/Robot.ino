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

HardwareSerial & clamp_driver_sstream = CLAMP_DRIVER_SERIAL;

MyStepper stepper_1;
MyStepper stepper_2;
MyStepper stepper_3;
MyStepper stepper_4;
TMC2209 clamp_driver;

TM1637Display display(TM1637_CLK_PIN, TM1637_DIO_PIN);

Servo servo1;
Servo servo2;

float speed, spin;
const float rotation = .8;
double angle;
float moteurClamp = 0.0;

void config_2209(TMC2209& stepper_driver);
void updateMotorSpeed(float* current, float target, TMC2209& stepper);

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


  clamp_driver.setup(clamp_driver_sstream, 115200, TMC2209::SERIAL_ADDRESS_0);
  config_2209(clamp_driver);

  // Give time to the remote to start
  delay(200);

  Timer3.initialize(2000);
  Timer3.attachInterrupt(stepper_it);
}

void loop() {
  //stepper_it();
  if (myRemote.updateValues()){

  speed = constrain(sqrtf((float)myRemote.Joystick1_Y * (float)myRemote.Joystick1_Y + (float)myRemote.Joystick1_X * (float)myRemote.Joystick1_X), 0, 255);
  if (speed < 50)
    speed = 0;
  else
   speed -= 50;
  angle = atan2(myRemote.Joystick1_Y, myRemote.Joystick1_X); //Angle of the joystick
  spin = myRemote.Joystick2_X;
  if (abs(myRemote.Joystick2_X) < 50)
    spin = 0;
  else{
    if (myRemote.Joystick2_X > 0)
      spin -= 50;
    else
      spin += 50;
  }
  spin *= rotation;

  float moteur1_target = speed * -cos(angle) + spin; //Derriere
  float moteur2_target = speed * -sin(angle) + spin; //Droite
  float moteur3_target = speed *  sin(angle) + spin; //Gauche
  float moteur4_target = speed *  cos(angle) + spin; //Avant

  stepper_1.spin(moteur1_target * 10.0);
  stepper_2.spin(moteur2_target * 10.0);
  stepper_3.spin(moteur3_target * 10.0);
  stepper_4.spin(moteur4_target * 10.0);

  if (myRemote.Button1){
    updateMotorSpeed(&moteurClamp, 200, clamp_driver);
  } else if (myRemote.Button2){
    updateMotorSpeed(&moteurClamp, -200, clamp_driver);
  } else {
    updateMotorSpeed(&moteurClamp, 0.0, clamp_driver);
  }

  if (myRemote.Button4){
    fermer_pinces();
  } else if (myRemote.Button3){
    ouvrir_pinces();
  }



  display.showNumberDec(myRemote.counter);
  digitalWrite(LED, myRemote.Button1 || myRemote.Button2 || myRemote.Button3 || myRemote.Button4 || myRemote.Joystick1_SW || myRemote.Joystick2_SW);

  }
}

void config_2209(TMC2209& stepper_driver){
  delay(10);
  //stepper_driver.setRMSCurrent(1000, R_SENSE);
  stepper_driver.setRunCurrent(100);
  delay(10);
  //stepper_driver.useInternalSenseResistors();
  stepper_driver.useExternalSenseResistors();
  //stepper_driver.enableAutomaticCurrentScaling();
  stepper_driver.enableCoolStep();
  stepper_driver.setMicrostepsPerStepPowerOfTwo(6);
  delay(10);
  //stepper_driver.enableStealthChop();¸
  //stepper_driver.disableStealthChop();
  //stepper_driver.setStandstillMode(1); //Freewheel
  delay(10);
  stepper_driver.enable();
}

void updateMotorSpeed(float* current, float target, TMC2209& stepper) {
  const float acceleration = 15.0;
  if (target == 0.0){
    *current = 0.0;
  } else if (*current < target) {
    *current += acceleration;
    if (*current > target) *current = target; // Avoid overshoot
  } else if (*current > target) {
    *current -= acceleration;
    if (*current < target) *current = target; // Avoid undershoot
  }
  stepper.moveAtVelocity((int32_t)((*current) * 160));
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
