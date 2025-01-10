#include <TMCStepper.h>

// Define stepper pins
#define MAIN_DRIVERS_EN_PIN 54 // Enable pin
//#define STEP_PIN 9        // Step pin
//#define DIR_PIN 10        // Direction pin

// Stepper configuration
#define R_SENSE 0.11f     // Sense resistor value (typically 0.11 ohms)

#define MAIN_DRIVERS_SERIAL Serial1

// Create a TMC2209 driver instance
TMC2209Stepper driver(&MAIN_DRIVERS_SERIAL, R_SENSE, 0);

// Stepper motor parameters
const int stepsPerRevolution = 200;  // Adjust to your stepper motor
const int microsteps = 1;           // Microstepping setting (1, 2, 4, ..., 256)

void setup() {
  // Initialize Serial1 for UART communication
  MAIN_DRIVERS_SERIAL.begin(115200); // Default baud rate for TMC2209
  Serial.begin(9600);    // For debugging via Serial Monitor

  // Initialize the driver
  driver.begin();
  driver.toff(4);                // Enable driver by setting toff (0 disables driver)
  driver.rms_current(1000);       // Set motor current in mA (adjust as needed)
  driver.microsteps(microsteps); // Set microstepping resolution
  driver.en_spreadCycle(false);  // Enable stealthChop for quieter operation
  driver.pwm_autoscale(true);    // Enable automatic current scaling

  // Enable CoolStep (optional, as configured previously)
  driver.TCOOLTHRS(0xFFFF);     // Threshold for switching to CoolStep
  driver.SGTHRS(10);            // StallGuard threshold
  driver.COOLCONF(0x0D0F);      // Enable CoolStep with adaptive scaling
  
  digitalWrite(MAIN_DRIVERS_EN_PIN, LOW); // Enable the driver

  Serial.println("TMC2209 Initialized.");
}

void loop() {

  // Example motor movement
  driver.VACTUAL(56000); // Run motor
  delay(2000);
  driver.VACTUAL(0);     // Stop motor (will enter freewheeling after TPOWERDOWN)
  delay(2000);
  driver.VACTUAL(-56000); // Run motor
  delay(2000);

  /*
  uint32_t rpm = 0;
  while (true) {
    // Motor is running at the desired speed
    delay(10); // Adjust as needed
    driver.VACTUAL(rpm);
    if (rpm < 200000)
      rpm+=500;
  }
  */
}