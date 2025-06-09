#ifndef _MY_STEPPER_H
#define _MY_STEPPER_H

#include <Arduino.h>

class MyStepper
{
private:
    int stepPin;
    int dirPin;
    bool en;
    unsigned long stepInterval = 10000; // Default step interval in microseconds
    unsigned long lastStepTime = 0;
public:
    MyStepper(){
        stepPin = -1;
        dirPin = -1;
        en = false;
    }
    ~MyStepper(){}

    void begin(int stepPin, int dirPin) {
        this->stepPin = stepPin;
        this->dirPin = dirPin;
        pinMode(stepPin, OUTPUT);
        pinMode(dirPin, OUTPUT);
        lastStepTime = micros();
    }

    void spin(float speed) {
        if (stepPin == -1 || dirPin == -1) return; // Not initialized
        if (abs(speed) < 1.0) {
          en = false;
          return;
        }
        digitalWrite(dirPin, speed >= 0 ? HIGH : LOW);
        stepInterval = 1000000 / (int)(abs(speed)); // Adjust step interval based on speed
        lastStepTime = micros(); // Reset last step time
        en = true;
    }

    void loop(){
        if (!en) return;
        while (lastStepTime + stepInterval <= micros()) {
            digitalWrite(stepPin, HIGH);
            lastStepTime += stepInterval;
            digitalWrite(stepPin, LOW);
        }
        lastStepTime = micros();
    }
};

#endif // _MY_STEPPER_H