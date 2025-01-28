#include <math.h>
#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>

#include "Remote.h"
#include <TM1637Display.h>
#include <TMC2209.h>

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

/* Not used
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
*/

Remote myRemote(9600, 10);

HardwareSerial & main_drivers_sstream = MAIN_DRIVERS_SERIAL;
HardwareSerial & clamp_driver_sstream = CLAMP_DRIVER_SERIAL;

TMC2209 main_driver_1;
TMC2209 main_driver_2;
TMC2209 main_driver_3;
TMC2209 main_driver_4;
TMC2209 clamp_driver;

TM1637Display display(TM1637_CLK_PIN, TM1637_DIO_PIN);

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;

float speed, spin;
const float rotation = .8;
double angle;
float moteur1 = 0.0;
float moteur2 = 0.0;
float moteur3 = 0.0;
float moteur4 = 0.0;
float moteurClamp = 0.0;

void config_2209(TMC2209& stepper_driver);
void updateMotorSpeed(float* current, float target, TMC2209& stepper);

void setup() {
  pinMode(MAIN_DRIVERS_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_DIAG_PIN, INPUT);
  digitalWrite(MAIN_DRIVERS_EN_PIN, LOW);  // Enabled
  digitalWrite(CLAMP_DRIVER_EN_PIN, LOW);  // Enabled

  pinMode(LED, OUTPUT);
  Serial.begin(115200); //Computer communication

  display.setBrightness(5); // Set the brightness level (0 to 7)

  servo1.attach(6); //68
  servo2.attach(7); //67
  servo3.attach(66); //66
  servo4.attach(65); //65
  servo5.attach(64); //64
  fermer_pinces_complet();

  delay(500);

  main_driver_1.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_0); 
  config_2209(main_driver_1);
  main_driver_2.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_1);
  config_2209(main_driver_2);
  main_driver_3.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_2);
  config_2209(main_driver_3);
  main_driver_4.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_3);
  config_2209(main_driver_4);
  clamp_driver.setup(clamp_driver_sstream, 115200, TMC2209::SERIAL_ADDRESS_0);
  config_2209(clamp_driver);

  // Give time to the remote to start
  delay(200);

}

void loop() {
  if (!myRemote.updateValues()){
    // Stop the motors if we lots communication with the remote
    updateMotorSpeed(&moteur1, 0.0, main_driver_1); //Avant Gauche
    updateMotorSpeed(&moteur2, 0.0, main_driver_2); //Avant Droit
    updateMotorSpeed(&moteur3, 0.0, main_driver_3); //Derriere Gauche
    updateMotorSpeed(&moteur4, 0.0, main_driver_4); //Derriere Droit
    return;
  }

  speed = constrain(sqrtf((float)myRemote.Joystick1_Y * (float)myRemote.Joystick1_Y + (long)myRemote.Joystick1_X * (float)myRemote.Joystick1_X), 0, 255);
  if (speed < 50)
    speed = 0;
  else
   speed -= 50;
  angle = atan2(myRemote.Joystick1_Y, myRemote.Joystick1_X); //Angle of the joystick
  spin = rotation * myRemote.Joystick2_X;
  if (abs(myRemote.Joystick2_X) < 50)
    spin = 0;

  float moteur1_target = speed * cos(-3.0 * PI/4.0 - angle) + spin; //Derriere Droit
  float moteur2_target = speed * cos(-1.0 * PI/4.0 - angle) + spin; //Avant Droit
  float moteur3_target = speed * cos(3.0  * PI/4.0 - angle) + spin; //Derriere Gauche
  float moteur4_target = speed * cos(1.0  * PI/4.0 - angle) + spin; //Avant Gauche

  // driver1 = moteurD
  // driver2 = moteurB
  // driver3 = moteurC
  // driver4 = moteurA
  updateMotorSpeed(&moteur1, moteur1_target, main_driver_1); //Avant Gauche
  updateMotorSpeed(&moteur2, moteur2_target, main_driver_2); //Avant Droit
  updateMotorSpeed(&moteur3, moteur3_target, main_driver_3); //Derriere Gauche
  updateMotorSpeed(&moteur4, moteur4_target, main_driver_4); //Derriere Droit

  if (myRemote.Button1){
    updateMotorSpeed(&moteurClamp, 255, clamp_driver);
  } else if (myRemote.Button2){
    updateMotorSpeed(&moteurClamp, -255, clamp_driver);
  } else {
    updateMotorSpeed(&moteurClamp, 0.0, clamp_driver);
  }

  if (myRemote.Button4){
    fermer_pinces();
  } else if (myRemote.Button3){
    ouvrir_pinces();
  }

/*
  Serial.print("Joystick1_Y: ");
  Serial.println(myRemote.Joystick1_Y);
  Serial.print("Joystick1_X: ");
  Serial.println(myRemote.Joystick1_X);
  Serial.print("Joystick1_Angle[deg]: ");
  Serial.println(degrees(angle));
  Serial.print("Joystick1_Speed: ");
  Serial.println(speed);
*/

  display.showNumberDec(myRemote.counter);
  digitalWrite(LED, myRemote.Button1 || myRemote.Button2 || myRemote.Button3 || myRemote.Button4 || myRemote.Joystick1_SW || myRemote.Joystick2_SW);

  delayMicroseconds(400);
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
  servo1.write(90);
  servo2.write(90);
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