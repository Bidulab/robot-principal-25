#include <math.h>
#include <Servo.h>
#include <Arduino.h>
#include <Wire.h>

#include "Remote.h"
#include <TM1637Display.h>
#include <TMC2209.h>

void setup_2209(TMC2209 stepper_driver);

#define LED 13

// Define the pins for TM1637 module
#define CLK_PIN 23
#define DIO_PIN 22

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

// Stepper configuration
#define R_SENSE 0.11f     // Sense resistor value

Remote myRemote(9600, 10);

HardwareSerial & serial1_stream = MAIN_DRIVERS_SERIAL;
HardwareSerial & serial2_stream = MAIN_DRIVERS_SERIAL;

TMC2209 main_drivers;
TMC2209 clamp_driver;

TM1637Display display(CLK_PIN, DIO_PIN);

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

void setup() {
  delay(1000);

  Serial.begin(9600); //Computer communication

  main_drivers.setup(serial1_stream, 115200, TMC2209::SERIAL_ADDRESS_0);
  clamp_driver.setup(serial2_stream, 115200, TMC2209::SERIAL_ADDRESS_0);
  setup_2209(main_drivers);
  setup_2209(clamp_driver);

  pinMode(LED, OUTPUT);

  pinMode(MAIN_DRIVERS_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_EN_PIN, OUTPUT);
  pinMode(CLAMP_DRIVER_DIAG_PIN, INPUT);

  digitalWrite(MAIN_DRIVERS_EN_PIN, LOW);  // Enabled
  digitalWrite(CLAMP_DRIVER_EN_PIN, LOW);  // Enabled
  
  display.setBrightness(5); // Set the brightness level (0 to 7)

  servo1.attach(68);
  servo2.attach(67);
  servo3.attach(66);
  servo4.attach(65);
  servo5.attach(64);

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
  main_drivers.moveAtVelocity((int32_t)(moteurA * 900));
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

void setup_2209(TMC2209 stepper_driver){
  //stepper_driver.setRMSCurrent(1000, R_SENSE);
  stepper_driver.setRunCurrent(100);
  stepper_driver.useInternalSenseResistors();
  stepper_driver.enableAutomaticCurrentScaling();
  stepper_driver.enableCoolStep();
  stepper_driver.enable();
  stepper_driver.setMicrostepsPerStepPowerOfTwo(8);
  stepper_driver.enableStealthChop();
}