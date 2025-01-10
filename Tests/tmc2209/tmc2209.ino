#include <TMC2209.h>

// Define stepper pins
#define MAIN_DRIVERS_EN_PIN 54 // Enable pin
//#define STEP_PIN 9        // Step pin
//#define DIR_PIN 10        // Direction pin

// Stepper configuration
#define R_SENSE 0.11f     // Sense resistor value (typically 0.11 ohms)

#define MAIN_DRIVERS_SERIAL Serial1
HardwareSerial & serial_stream = MAIN_DRIVERS_SERIAL;

TMC2209 stepper_driver;
long speed = 0;
void setup() {
  pinMode(MAIN_DRIVERS_EN_PIN, OUTPUT);
  digitalWrite(MAIN_DRIVERS_EN_PIN, LOW); // Enable the driver

  stepper_driver.setup(serial_stream, 115200, TMC2209::SERIAL_ADDRESS_0);
  delay(100);

  //stepper_driver.setRMSCurrent(1000, R_SENSE);
  stepper_driver.setRunCurrent(100);
  stepper_driver.useInternalSenseResistors();
  stepper_driver.enableAutomaticCurrentScaling();
  stepper_driver.enableCoolStep();
  stepper_driver.enable();
  stepper_driver.setMicrostepsPerStepPowerOfTwo(8);
  stepper_driver.enableStealthChop();
}

void loop() {
  stepper_driver.moveAtVelocity(speed);
  if (speed < 240000)
    speed += 4000;
  delay(100);

}
