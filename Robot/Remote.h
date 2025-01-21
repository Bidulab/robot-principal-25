/** \addtogroup Remote 
 *  @{
 */

#ifndef REMOTE_H
#define REMOTE_H

#include "Arduino.h"
#include "Remote.h"

class Remote 
{
	public:
    //*** SENSOR VALUES VARIABLES ***
    short int Joystick1_X = 0; //[-255;255]
    short int Joystick1_Y = 0; //[-255;255]

    short int Joystick2_X = 0; //[-255;255]
    short int Joystick2_Y = 0; //[-255;255]

    bool Button1 = false;
    bool Button2 = false;
    bool Button3 = false;
    bool Button4 = false;

    bool Joystick1_SW = false; 
    bool Joystick2_SW = false;

    short int counter;

		Remote(unsigned long baud, short int initCounter);

		bool updateValues();
		
	private:
    unsigned char Mymessage[13]; //Initialized variable to store recieved data  
};
#endif //REMOTE
/** @}*/