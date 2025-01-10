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
float moteurA;
float moteurB;
float moteurC;
float moteurD;

void config_2209(TMC2209 stepper_driver);

void setup() {
  delay(1000);

  //Serial.begin(9600); //Computer communication

  main_driver_1.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_0); 
  main_driver_2.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_1);
  main_driver_3.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_2);
  main_driver_4.setup(main_drivers_sstream, 115200, TMC2209::SERIAL_ADDRESS_3);
  clamp_driver.setup(clamp_driver_sstream, 115200, TMC2209::SERIAL_ADDRESS_0);
  config_2209(main_driver_1);
  config_2209(main_driver_2);
  config_2209(main_driver_3);
  config_2209(main_driver_4);
  config_2209(clamp_driver);

  pinMode(LED, OUTPUT);

  pinMode(MAIN_DRIVERS_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_DIAG_PIN, INPUT);

  digitalWrite(MAIN_DRIVERS_EN_PIN, LOW);  // Enabled
  digitalWrite(CLAMP_DRIVER_EN_PIN, LOW);  // Enabled
  
  display.setBrightness(5); // Set the brightness level (0 to 7)

  servo1.attach(68); //68
  servo2.attach(67); //67
  servo3.attach(66); //66
  servo4.attach(65); //65
  servo5.attach(64); //64

  // Give time to the remote to start
  delay(1000);
}

void loop() {
  myRemote.updateValues();

  speed = constrain(sqrtf((float)myRemote.Joystick1_Y * (float)myRemote.Joystick1_Y + (long)myRemote.Joystick1_X * (float)myRemote.Joystick1_X), 0, 255);
  angle = atan2(myRemote.Joystick1_Y, myRemote.Joystick1_X); //Angle of the joystick
  spin = rotation * myRemote.Joystick2_X;

  moteurA = speed * cos(1.0  * PI/4.0 - angle) + spin; //Avant Gauche
  moteurB = speed * cos(-1.0 * PI/4.0 - angle) + spin; //Avant Droit
  moteurC = speed * cos(3.0  * PI/4.0 - angle) + spin; //Derriere Gauche
  moteurD = speed * cos(-3.0 * PI/4.0 - angle) + spin; //Derriere Droit

  // driver1 = moteurD
  // driver2 = moteurB
  // driver3 = moteurC
  // driver4 = moteurA
  main_driver_1.moveAtVelocity((int32_t)(moteurD * 900)); // Derriere Droit
  main_driver_2.moveAtVelocity((int32_t)(moteurB * 900)); // Avant Droit
  main_driver_3.moveAtVelocity((int32_t)(moteurC * 900)); // Derriere Gauche
  main_driver_4.moveAtVelocity((int32_t)(moteurA * 900)); // Avant Gauche
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

void config_2209(TMC2209 stepper_driver){
  //stepper_driver.setRMSCurrent(1000, R_SENSE);
  stepper_driver.setRunCurrent(100);
  stepper_driver.useInternalSenseResistors();
  stepper_driver.enableAutomaticCurrentScaling();
  stepper_driver.enableCoolStep();
  stepper_driver.enable();
  stepper_driver.setMicrostepsPerStepPowerOfTwo(8);
  stepper_driver.enableStealthChop();
}