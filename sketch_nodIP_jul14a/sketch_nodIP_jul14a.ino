
/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

#include <RCSwitch.h>
#include <ipv4.h>

RCSwitch mySwitch = RCSwitch();
ipv4 ip = ipv4();

void setup() {
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2

  pinMode(4, OUTPUT);
  //Serial.println(ip.);
  
}

void loop() {

  if(!ip.gotAdress){

    delay(2000);

    int* ptrToAdress;
    //ptrToAdress = ip.ipHeader.source;
    ptrToAdress = ip.getSourceAdress();
    //ptrToAdress++;
    
   Serial.println("hey");

    Serial.println(ip.ipHeader.source[2]);
    Serial.println(*ptrToAdress);
    //Serial.println(ip.getSourceAdress());
    
    //ask raspberry pi for an ip adress
    if(mySwitch.available()){

      
      mySwitch.resetAvailable();
      
    }
    
  }
  else{

    if(mySwitch.available()){

    int recValue = mySwitch.getReceivedValue();
    unsigned int* recRaw = mySwitch.getReceivedRawdata();
    unsigned int* ptr;

    ptr = &recRaw[0];
    
    Serial.print("Mottaget ");
    Serial.println( recValue);
    //while(*ptr){
      //Serial.println(*ptr++);
    //}
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    if(recValue == 1364){

      Serial.println("Slå av");
      //stäng av
      digitalWrite(4, LOW);
      
    }
    else if(recValue == 1361){

      Serial.println("Slå på");
      //slå på
      digitalWrite(4, HIGH);
      
    }
    
    mySwitch.resetAvailable();

    }
    
  }

}
