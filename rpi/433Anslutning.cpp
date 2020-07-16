/*
  RCSwitch - Arduino libary for remote control outlet switches
  Copyright (c) 2011 Suat Özgür.  All right reserved.
  
  Contributors:
  - Andre Koehler / info(at)tomate-online(dot)de
  - Gordeev Andrey Vladimirovich / gordeev(at)openpyro(dot)com
  - Skineffect / http://forum.ardumote.com/viewtopic.php?f=2&t=48
  
  Project home: http://code.google.com/p/rc-switch/

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "433Anslutning.h"

unsigned long RCSwitch::nReceivedValue = NULL;
unsigned int RCSwitch::nReceivedBitlength = 0;
unsigned int RCSwitch::nReceivedDelay = 0;
unsigned int RCSwitch::nReceivedProtocol = 0;
unsigned int RCSwitch::timings[RCSWITCH_MAX_CHANGES];
int RCSwitch::nReceiveTolerance = 60;

RCSwitch::RCSwitch() {
  this->nReceiverInterrupt = -1;
  this->nTransmitterPin = -1;
  RCSwitch::nReceivedValue = NULL;
  this->setPulseLength(350);
  this->setRepeatTransmit(10);
  this->setReceiveTolerance(60);
  this->setProtocol(1);
}

/**
  * Sets the protocol to send.
  */
void RCSwitch::setProtocol(int nProtocol) {
  this->nProtocol = nProtocol;
  if (nProtocol == 1){
	  this->setPulseLength(350);
  }
  else if (nProtocol == 2) {
	  this->setPulseLength(650);
  }
}

/**
  * Sets the protocol to send with pulse length in microseconds.
  */
void RCSwitch::setProtocol(int nProtocol, int nPulseLength) {
  this->nProtocol = nProtocol;
  if (nProtocol == 1){
	  this->setPulseLength(nPulseLength);
  }
  else if (nProtocol == 2) {
	  this->setPulseLength(nPulseLength);
  }
}


/**
  * Sets pulse length in microseconds
  */
void RCSwitch::setPulseLength(int nPulseLength) {
  this->nPulseLength = nPulseLength;
}

/**
 * Sets Repeat Transmits
 */
void RCSwitch::setRepeatTransmit(int nRepeatTransmit) {
  this->nRepeatTransmit = nRepeatTransmit;
}

/**
 * Set Receiving Tolerance
 */
void RCSwitch::setReceiveTolerance(int nPercent) {
  RCSwitch::nReceiveTolerance = nPercent;
}
  

/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Arduino Pin to which the sender is connected to
 */
void RCSwitch::enableTransmit(int nTransmitterPin) {
  this->nTransmitterPin = nTransmitterPin;
  pinMode(this->nTransmitterPin, OUTPUT);
}

/**
  * Disable transmissions
  */
void RCSwitch::disableTransmit() {
  this->nTransmitterPin = -1;
}

/**
 * Switch a remote switch on (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void RCSwitch::switchOn(char* sGroup, int nChannel) {
  this->sendTriState( this->getCodeWordA(sGroup, nChannel, true) );
}

/**
 * Switch a remote switch off (Type A with 10 pole DIP switches)
 *
 * @param sGroup        Code of the switch group (refers to DIP switches 1..5 where "1" = on and "0" = off, if all DIP switches are on it's "11111")
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void RCSwitch::switchOff(char* sGroup, int nChannel) {
  this->sendTriState( this->getCodeWordA(sGroup, nChannel, false) );
}

/**
 * min swtichOn
 *
 */
void RCSwitch::switchOn(int nrOfBreaker){
  
  this->send(dec2binWzerofill(192, 24));
  this->send(dec2binWzerofill(168, 24));
  this->send(dec2binWzerofill(2, 8));
  this->send(dec2binWzerofill(1, 8));
  
  char ipAdress[] = "11000000\0";
  
  //this->send("11000000\0"); //192
  //this->send("10101000\0"); //168
  //this->send("-1-1-1\0"); //0
  //this->send("00000001\0"); //1
  
}


/**
 * Like getCodeWord  (Type A)
 */
char* RCSwitch::getCodeWordA(char* sGroup, int nChannelCode, boolean bStatus) {
   int nReturnPos = 0;
   static char sReturn[13];

  char* code[6] = { "FFFFF", "0FFFF", "F0FFF", "FF0FF", "FFF0F", "FFFF0" };

  if (nChannelCode < 1 || nChannelCode > 5) {
      //return '\0';
      return "\0";
  }
  
  for (int i = 0; i<5; i++) {
    if (sGroup[i] == '0') {
      sReturn[nReturnPos++] = 'F';
    } else if (sGroup[i] == '1') {
      sReturn[nReturnPos++] = '0';
    } else {
      //return '\0';
      return "\0";
    }
  }
  
  for (int i = 0; i<5; i++) {
    sReturn[nReturnPos++] = code[ nChannelCode ][i];
  }
  
  if (bStatus) {
    sReturn[nReturnPos++] = '0';
    sReturn[nReturnPos++] = 'F';
  } else {
    sReturn[nReturnPos++] = 'F';
    sReturn[nReturnPos++] = '0';
  }
  sReturn[nReturnPos] = '\0';

  return sReturn;
}

/**
 * Sends a Code Word
 * @param sCodeWord   /^[10FS]*$/  -> see getCodeWord
 */
void RCSwitch::sendTriState(char* sCodeWord) {
  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
          this->sendT0();
        break;
        case 'F':
          this->sendTF();
        break;
        case '1':
          this->sendT1();
        break;
      }
      i++;
    }
    this->sendSync();    
  }
}

void RCSwitch::send(unsigned long Code, unsigned int length) {
  this->send( this->dec2binWzerofill(Code, length) );
}

void RCSwitch::send(char* sCodeWord) {
  for (int nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
        case '0':
          this->send0();
        break;
        case '1':
          this->send1();
        break;
      }
      i++;
    }
    this->sendSync();
  }
}

void RCSwitch::transmit(int nHighPulses, int nLowPulses) {
    boolean disabled_Receive = false;
    int nReceiverInterrupt_backup = nReceiverInterrupt;
    if (this->nTransmitterPin != -1) {
        if (this->nReceiverInterrupt != -1) {
            this->disableReceive();
            disabled_Receive = true;
        }
        digitalWrite(this->nTransmitterPin, HIGH);
        delayMicroseconds( this->nPulseLength * nHighPulses);
        digitalWrite(this->nTransmitterPin, LOW);
        delayMicroseconds( this->nPulseLength * nLowPulses);
        if(disabled_Receive){
            this->enableReceive(nReceiverInterrupt_backup);
        }
    }
}
/**
 * Sends a "0" Bit
 *                       _    
 * Waveform Protocol 1: | |___
 *                       _  
 * Waveform Protocol 2: | |__
 */
void RCSwitch::send0() {
	if (this->nProtocol == 1){
		this->transmit(1,3);
	}
	else if (this->nProtocol == 2) {
		this->transmit(1,2);
	}
}

/**
 * Sends a "1" Bit
 *                       ___  
 * Waveform Protocol 1: |   |_
 *                       __  
 * Waveform Protocol 2: |  |_
 */
void RCSwitch::send1() {
  	if (this->nProtocol == 1){
		this->transmit(3,1);
	}
	else if (this->nProtocol == 2) {
		this->transmit(2,1);
	}
}


/**
 * Sends a Tri-State "0" Bit
 *            _     _
 * Waveform: | |___| |___
 */
void RCSwitch::sendT0() {
  this->transmit(1,3);
  this->transmit(1,3);
}

/**
 * Sends a Tri-State "1" Bit
 *            ___   ___
 * Waveform: |   |_|   |_
 */
void RCSwitch::sendT1() {
  this->transmit(3,1);
  this->transmit(3,1);
}

/**
 * Sends a Tri-State "F" Bit
 *            _     ___
 * Waveform: | |___|   |_
 */
void RCSwitch::sendTF() {
  this->transmit(1,3);
  this->transmit(3,1);
}

/**
 * Sends a "Sync" Bit
 *                       _
 * Waveform Protocol 1: | |_______________________________
 *                       _
 * Waveform Protocol 2: | |__________
 */
void RCSwitch::sendSync() {

    if (this->nProtocol == 1){
		this->transmit(1,31);
	}
	else if (this->nProtocol == 2) {
		this->transmit(1,10);
	}
}

/**
 * Enable receiving data
 */
void RCSwitch::enableReceive(int interrupt) {
  this->nReceiverInterrupt = interrupt;
  this->enableReceive();
}

void RCSwitch::enableReceive() {
  if (this->nReceiverInterrupt != -1) {
    RCSwitch::nReceivedValue = NULL;
    RCSwitch::nReceivedBitlength = NULL;
  }
}

/**
 * Disable receiving data
 */
void RCSwitch::disableReceive() {
  this->nReceiverInterrupt = -1;
}

bool RCSwitch::available() {
  return RCSwitch::nReceivedValue != NULL;
}

void RCSwitch::resetAvailable() {
  RCSwitch::nReceivedValue = NULL;
}

unsigned long RCSwitch::getReceivedValue() {
    return RCSwitch::nReceivedValue;
}

unsigned int RCSwitch::getReceivedBitlength() {
  return RCSwitch::nReceivedBitlength;
}

unsigned int RCSwitch::getReceivedDelay() {
  return RCSwitch::nReceivedDelay;
}

unsigned int RCSwitch::getReceivedProtocol() {
  return RCSwitch::nReceivedProtocol;
}

unsigned int* RCSwitch::getReceivedRawdata() {
    return RCSwitch::timings;
}

/**
 *
 */
bool RCSwitch::receiveProtocol1(unsigned int changeCount){
    
	  unsigned long code = 0;
      unsigned long delay = RCSwitch::timings[0] / 31;
      unsigned long delayTolerance = delay * RCSwitch::nReceiveTolerance * 0.01;    

      for (int i = 1; i<changeCount ; i=i+2) {
      
          if (RCSwitch::timings[i] > delay-delayTolerance && RCSwitch::timings[i] < delay+delayTolerance && RCSwitch::timings[i+1] > delay*3-delayTolerance && RCSwitch::timings[i+1] < delay*3+delayTolerance) {
            code = code << 1;
          } else if (RCSwitch::timings[i] > delay*3-delayTolerance && RCSwitch::timings[i] < delay*3+delayTolerance && RCSwitch::timings[i+1] > delay-delayTolerance && RCSwitch::timings[i+1] < delay+delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }      
      code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      RCSwitch::nReceivedValue = code;
      RCSwitch::nReceivedBitlength = changeCount / 2;
      RCSwitch::nReceivedDelay = delay;
	  RCSwitch::nReceivedProtocol = 1;
    }

	if (code == 0){
		return false;
	}else if (code != 0){
		return true;
	}
	

}

bool RCSwitch::receiveProtocol2(unsigned int changeCount){
    
	  unsigned long code = 0;
      unsigned long delay = RCSwitch::timings[0] / 10;
      unsigned long delayTolerance = delay * RCSwitch::nReceiveTolerance * 0.01;    

      for (int i = 1; i<changeCount ; i=i+2) {
      
          if (RCSwitch::timings[i] > delay-delayTolerance && RCSwitch::timings[i] < delay+delayTolerance && RCSwitch::timings[i+1] > delay*2-delayTolerance && RCSwitch::timings[i+1] < delay*2+delayTolerance) {
            code = code << 1;
          } else if (RCSwitch::timings[i] > delay*2-delayTolerance && RCSwitch::timings[i] < delay*2+delayTolerance && RCSwitch::timings[i+1] > delay-delayTolerance && RCSwitch::timings[i+1] < delay+delayTolerance) {
            code+=1;
            code = code << 1;
          } else {
            // Failed
            i = changeCount;
            code = 0;
          }
      }      
      code = code >> 1;
    if (changeCount > 6) {    // ignore < 4bit values as there are no devices sending 4bit values => noise
      RCSwitch::nReceivedValue = code;
      RCSwitch::nReceivedBitlength = changeCount / 2;
      RCSwitch::nReceivedDelay = delay;
	  RCSwitch::nReceivedProtocol = 2;
    }

	if (code == 0){
		return false;
	}else if (code != 0){
		return true;
	}

}

/**
  * Turns a decimal value to its binary representation
  */
char* RCSwitch::dec2binWzerofill(unsigned long Dec, unsigned int bitLength){
  static char bin[64];
  unsigned int i=0;

  while (Dec > 0) {
    bin[32+i++] = ((Dec & 1) > 0) ? '1' : '0';
    Dec = Dec >> 1;
  }

  for (unsigned int j = 0; j< bitLength; j++) {
    if (j >= bitLength - i) {
      bin[j] = bin[ 31 + i - (j - (bitLength - i)) ];
    }else {
      bin[j] = '0';
    }
  }
  bin[bitLength] = '\0';
  
  return bin;
}

