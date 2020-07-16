/*
 Usage: ./send <systemCode> <unitCode> <command>
 Command is 0 for OFF and 1 for ON
 */

#include "433Anslutning.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    
    /*
     output PIN is hardcoded for testing purposes
     see https://projects.drogon.net/raspberry-pi/wiringpi/pins/
     for pin mapping of the raspberry pi GPIO connector
     */
    int PIN = 0;
    //char* systemCode = argv[1];
    //int unitCode = atoi(argv[2]);
    //int command  = atoi(argv[3]);
    //char* systemCode = "11111";
    
    int unitCode = atoi(argv[1]);
    
    if (wiringPiSetup () == -1) return 1;
	//printf("sending systemCode[%s] unitCode[%i] command[%i]\n", systemCode, unitCode, command);
	RCSwitch mySwitch = RCSwitch();
	mySwitch.enableTransmit(PIN);
    
    delayMicroseconds(1000000);
    
        switch(unitCode) {
            
            case 1:
                mySwitch.switchOn(unitCode);
                //mySwitch.switchOn(systemCode, unitCode);
                break;
            case 0:
                //mySwitch.switchOff(systemCode, unitCode);
                break;
            default:
                printf("unsupported\n");
                return -1;
        
    }
    
	return 0;
}