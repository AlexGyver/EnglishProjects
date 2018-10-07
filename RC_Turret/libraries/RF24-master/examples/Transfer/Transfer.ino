/*
TMRh20 2014

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */

/** General Data Transfer Rate Test
 * This example demonstrates basic data transfer functionality with the 
 updated library. This example will display the transfer rates acheived using
 the slower form of high-speed transfer using blocking-writes.
 */


#include <SPI.h>
#include "RF24.h"
#include "printf.h"

/*************  USER Configuration *****************************/
                                          // Hardware configuration
RF24 radio(8,9);                        // Set up nRF24L01 radio on SPI bus plus pins 8 & 9

/***************************************************************/

const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };   // Radio pipe addresses for the 2 nodes to communicate.

byte data[32];                           //Data buffer for testing data transfer speeds

unsigned long counter, rxTimer;          //Counter and timer for keeping track transfer info
unsigned long startTime, stopTime;  
bool TX=1,RX=0,role=0;

void setup(void) {

  // The UNO Adapter uses pin 10 as Vcc
  pinMode(10,OUTPUT);
  digitalWrite(10,HIGH);
  delay(500);
  
  Serial.begin(57600);
  printf_begin();

  radio.begin();                           // Setup and configure rf radio
  radio.setChannel(1);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF24_1MBPS);
  radio.setAutoAck(1);                     // Ensure autoACK is enabled
  radio.setRetries(2,15);                   // Optionally, increase the delay between retries & # of retries
  radio.setCRCLength(RF24_CRC_8); 
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  
  radio.startListening();                 // Start listening
  radio.printDetails();                   // Dump the configuration of the rf unit for debugging
  
  printf("\n\rRF24/examples/Transfer Rates/\n\r");
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");
  
  randomSeed(analogRead(0));              //Seed for random number generation
  
  for(int i=0; i<32; i++){
     data[i] = random(255);               //Load the buffer with random data
  }
  radio.powerUp();                        //Power up the radio
}

void loop(void){


  if(role == TX){
    
    delay(2000);
    
    printf("Initiating Basic Data Transfer\n\r");
    
    
    unsigned long cycles = 10000; //Change this to a higher or lower number. 
    
    startTime = millis();
    unsigned long pauseTime = millis();
            
    for(int i=0; i<cycles; i++){        //Loop through a number of cycles
      data[0] = i;                      //Change the first byte of the payload for identification
      if(!radio.writeFast(&data,32)){   //Write to the FIFO buffers        
        counter++;                      //Keep count of failed payloads
      }
      
      //This is only required when NO ACK ( enableAutoAck(0) ) payloads are used
//      if(millis() - pauseTime > 3){
//        pauseTime = millis();
//        radio.txStandBy();          // Need to drop out of TX mode every 4ms if sending a steady stream of multicast data
//        //delayMicroseconds(130);     // This gives the PLL time to sync back up   
//      }
      
    }
    
   stopTime = millis();   
                                         //This should be called to wait for completion and put the radio in standby mode after transmission, returns 0 if data still in FIFO (timed out), 1 if success
   if(!radio.txStandBy()){ counter+=3; } //Standby, block only until FIFO empty or auto-retry timeout. Flush TX FIFO if failed
   //radio.txStandBy(1000);              //Standby, using extended timeout period of 1 second
   
   float numBytes = cycles*32;
   float rate = numBytes / (stopTime - startTime);
    
   Serial.print("Transfer complete at "); Serial.print(rate); printf(" KB/s \n\r");
   Serial.print(counter); Serial.print(" of "); Serial.print(cycles); printf(" Packets Failed to Send\n\r");
   counter = 0;   
    
   }
  
  
  
if(role == RX){
     while(radio.available()){       
      radio.read(&data,32);
      counter++;
     }
   if(millis() - rxTimer > 1000){
     rxTimer = millis();     
     float numBytes = (counter*32)/1000.0;
     Serial.print("Rate: ");
     Serial.print(numBytes);
     printf("KB/s \n Payload Count: %d \n\r", counter);
     counter = 0;
   }
  }
  //
  // Change roles
  //

  if ( Serial.available() )
  {
    char c = toupper(Serial.read());
    if ( c == 'T' && role == RX )
    {
      printf("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK\n\r");
      radio.openWritingPipe(pipes[1]);
      radio.openReadingPipe(1,pipes[0]);
      radio.stopListening();
      role = TX;                  // Become the primary transmitter (ping out)
    }
    else if ( c == 'R' && role == TX )
    {
      radio.openWritingPipe(pipes[0]);
      radio.openReadingPipe(1,pipes[1]); 
      radio.startListening();
      printf("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK\n\r");      
      role = RX;                // Become the primary receiver (pong back)
    }
  }
}
