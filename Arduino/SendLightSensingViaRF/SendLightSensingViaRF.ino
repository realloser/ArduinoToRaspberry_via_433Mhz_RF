// Depends on the RCSwitch library
// https://github.com/sui77/rc-switch
#include "RCSwitch.h"

// Light measurement goes to A0 pin
#define LIGHT_IN 0

// A unique ID (4 bits, 0-15) for each device so that the receiver
// understands which device sent what message
#define ARDUINO_ID 3

#define DELAY_BETWEEN_TRANSMITS 5000 // in milliseconds

RCSwitch transmitter = RCSwitch();
 
void setup() 
{  
  Serial.begin(9600);

  pinMode(LIGHT_IN, INPUT);
     
  transmitter.enableTransmit(10); 
  transmitter.setRepeatTransmit(15);
}

// A rolling sequence number for each measurement
// Restarts at 0 after seqNum=15 has been used
unsigned long seqNum=0;

void loop() 
{
    // Get the light measurement (0-1023)
    unsigned int data = analogRead(LIGHT_IN);

    // Calculate the 32 bits to send
    unsigned long checkSum = ARDUINO_ID + seqNum + data;
    unsigned long byte3 = ((0x0F & ARDUINO_ID) << 4) + (0x0F & seqNum);
    unsigned long byte2_and_byte_1 = 0xFFFF & data;
    unsigned long byte0 = 0xFF & checkSum;
    unsigned long dataToSend = (byte3 << 24) + (byte2_and_byte_1 << 8) + byte0;
    
//    Serial.println("byte 3:");
//    Serial.println(byte3);

//    Serial.println(dataToSend);
//    Serial.println(sizeof(unsigned long));
    // Send 32 bits of data
    transmitter.send( dataToSend, 32);
    
    seqNum++;
    if (seqNum > 15)
    {
      seqNum = 0;
    }
    
    delay (DELAY_BETWEEN_TRANSMITS);  
}
