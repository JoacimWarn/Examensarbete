#include "ipv4.h"

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
	this->gotAdress = false;
	ipHeader.TTL = 0;
	
}


int* ipv4::getSourceAdress(){
	
	return ipHeader.source;
	
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
				
				this->gotAdress = false;
				return false;
				
			}
			
		}
		
		this->gotAdress = true;
		return true;
		
	}
	
	return false;
	
}