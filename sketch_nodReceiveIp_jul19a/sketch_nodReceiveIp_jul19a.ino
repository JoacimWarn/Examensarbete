#include <RCSwitch.h>
#include <ipv4.h>

RCSwitch mySwitch = RCSwitch();
ipv4 ip = ipv4();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
  mySwitch.enableTransmit(8);

  pinMode(8, OUTPUT);
  //Serial.println(ip.);
  
}

void loop() {

  while(!ip.gotAdress){
    
    //attempt to receive an ip adress from the raspberry pi
    //send request signal

    ip.csmaca(mySwitch.available());

    if(!ip.isTransmitting){

      //no one is sending, attempt to get an ip adress
      mySwitch.send(55555, 24);

      //wait for response from the server 2 seconds interval
      
      
    }
    
  }
  while(true){

    ip.csmaca(mySwitch.available());

    if(!ip.isTransmitting){

      //no one is sending, send the data
      
    }
    
  }
  
}
