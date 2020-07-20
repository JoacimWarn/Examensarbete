#include "ipv4.h"
#include <time.h>

/*
ipv4 class definition
*/

ipv4::ipv4(){
	
	for(int i = 0; i < 4; i++){
		
		ipHeader.source[i] = 0;
		ipHeader.destination[i] = 0;
		
	}
	//memset(ipHeader.source, 0, 4);
	//memset(ipHeader.destination, 0, 4);
	gotAdress = false;
	ipHeader.TTL = 0;
	isTransmitting = true;
	
}

/*
CSMA, listens to the channel if no one is sending.
If no one is sending then send.
If not, wait a random interval and listen again.
*/
void ipv4::csmaca(bool listen){
	
	if(listen){
		
		srand(time(NULL));
		
		//random wait between 1 - 3 seconds
		int randomNumber = (rand() % 3 + 1);
		
		isTransmitting = true;
		
		delay(randomNumber);
		
		return;
		
	}
	
	isTransmitting = false;
	
}

/*
Returns the ip adress
*/
int* ipv4::getSourceAdress(){
	
	return ipHeader.source;
	
}

/*
Disconnects the given node and releases the ip adress
*/
void ipv4::disconnectNode(int adress){
	
	for(int i = 0; i < 4; i++){
		
		ipHeader.source[i] = 0;
		
	}
	
	ipHeader.TTL--;
	
}


/*
Attempt to get an adress from the Raspberry Pi
Also set TTL
*/
bool ipv4::setSourceAdress(int* adress){
	
	bool success = false;
	int i;
	
	int* ptrToAdress;
	ptrToAdress = adress;
	i = 0;
	
	while(*ptrToAdress && i < 4){
		
		ipHeader.source[i++] = *ptrToAdress;
		
		ptrToAdress++;
		
	}
	
	if(!success){
		
		for(int i = 0; i < 4; i++){
			
			if(ipHeader.source[i] == 0){
				
				gotAdress = false;
				return false;
				
			}
			
		}
		
		ipHeader.TTL++;
		
		gotAdress = true;
		return true;
		
	}
	
	return false;
	
}