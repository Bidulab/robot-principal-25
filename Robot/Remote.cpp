#include "HardwareSerial.h"
#include "Arduino.h"
#include "Remote.h"

#define REMOTE_SERIAL Serial3

Remote::Remote(unsigned long baud, short int initCounter)
{
  REMOTE_SERIAL.begin(baud);
  counter = initCounter;
}

// Returns true if valid
bool Remote::updateValues()
{
  REMOTE_SERIAL.flush();      // Clear incomming message
  REMOTE_SERIAL.write('A');                               // Send get message
  unsigned short i = 0;
  while(REMOTE_SERIAL.available() < 13){
    i++;
    delayMicroseconds(50);
    if (i == 400){                                  // If delay > 20 ms
      return false;
    }
  }

  // read the incoming bytes
  REMOTE_SERIAL.readBytes(Mymessage, 13);

  Joystick1_X = Mymessage[1];
  Joystick1_X = -2 * Joystick1_X + 255;
  Joystick1_Y = Mymessage[2];
  Joystick1_Y = -2 * Joystick1_Y + 255;

  Joystick2_X = Mymessage[3];
  Joystick2_X = -2 * Joystick2_X + 255;
  Joystick2_Y = Mymessage[4];
  Joystick2_Y = -2 * Joystick2_Y + 255;

  Button1 = !Mymessage[5];
  Button2 = !Mymessage[6];
  Button3 = !Mymessage[7];
  Button4 = !Mymessage[8];

  Joystick1_SW = !Mymessage[9]; 
  Joystick2_SW = !Mymessage[10];
  
  counter += (signed char)Mymessage[11];
  
  REMOTE_SERIAL.flush();

  return true;
}