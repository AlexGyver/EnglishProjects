/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 
 Update 2014 - TMRh20
 */

/**
 * Example of using interrupts
 *
 * This is an example of how to user interrupts to interact with the radio, and a demonstration
 * of how to use them to sleep when receiving, and not miss any payloads. 
 * The pingpair_sleepy example expands on sleep functionality with a timed sleep option for the transmitter.
 * Sleep functionality is built directly into my fork of the RF24Network library
 */

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"
#include <avr/sleep.h>
#include <avr/power.h>

// Hardware configuration
RF24 radio(7,8);                          // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
                                        
const short role_pin = 5;                 // sets the role of this unit in hardware.  Connect to GND to be the 'pong' receiver
                                          // Leave open to be the 'ping' transmitter

  const uint64_t address[2] = {0xABCDABCD71LL, 0x544d52687CLL};  // Radio pipe addresses for the 2 nodes to communicate.

// Role management

// Set up role.  This sketch uses the same software for all the nodes in this
// system.  Doing so greatly simplifies testing.  The hardware itself specifies
// which node it is.
// This is done through the role_pin
typedef enum { role_sender = 1, role_receiver } role_e;                 // The various roles supported by this sketch
const char* role_friendly_name[] = { "invalid", "Sender", "Receiver"};  // The debug-friendly names of those roles
role_e role;                                                            // The role of the current running sketch

boolean gotMsg = 0;                                                     // So we know when to go to sleep

/********************** Setup *********************/
void setup(){

  pinMode(role_pin, INPUT);                        // set up the role pin                  
  digitalWrite(role_pin,HIGH);                     // Change this to LOW/HIGH instead of using an external pin
  delay(20);                                       // Just to get a solid reading on the role pin

  if ( digitalRead(role_pin) )                    // read the address pin, establish our role
    role = role_sender;
  else
    role = role_receiver;


  Serial.begin(115200);
  printf_begin();
  printf("\n\rRF24/examples/pingpair_irq/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);

  // Setup and configure rf radio
  radio.begin();  
  radio.enableAckPayload();                         // We will be using the Ack Payload feature, so please enable it
                                                    // Open pipes to other node for communication
  if ( role == role_sender ) {                      // This simple sketch opens a pipe on a single address for these two nodes to 
     radio.openWritingPipe(address[0]);             // communicate back and forth.  One listens on it, the other talks to it.
     radio.openReadingPipe(1,address[1]); 
  }else{
    radio.openWritingPipe(address[1]);
    radio.openReadingPipe(1,address[0]);
    radio.startListening();
  }
  radio.printDetails();                              // Dump the configuration of the rf unit for debugging
  delay(50);
  attachInterrupt(0, check_radio, FALLING);          // Attach interrupt handler to interrupt #0 (using pin 2) on BOTH the sender and receiver
}

static uint32_t message_count = 0;


/********************** Main Loop *********************/
void loop() {


  if (role == role_sender)  {                        // Sender role.  Repeatedly send the current time 
    unsigned long time = millis();                   // Take the time, and send it.
    printf("Now sending %lu\n\r",time);
    radio.startWrite( &time, sizeof(unsigned long) ,0);
    delay(2000);                                     // Try again soon
  }


  if(role == role_receiver){                        // Receiver does nothing except in IRQ
  }  
}


/********************** Interrupt *********************/

void check_radio(void)                                // Receiver role: Does nothing!  All the work is in IRQ
{
  
  bool tx,fail,rx;
  radio.whatHappened(tx,fail,rx);                     // What happened?
  
  if ( tx ) {                                         // Have we successfully transmitted?
      if ( role == role_sender ){   printf("Send:OK\n\r"); }
      if ( role == role_receiver ){ printf("Ack Payload:Sent\n\r"); }
  }
  
  if ( fail ) {                                       // Have we failed to transmit?
      if ( role == role_sender ){   printf("Send:Failed\n\r");  }
      if ( role == role_receiver ){ printf("Ack Payload:Failed\n\r");  }
  }
  
  if ( rx || radio.available()){                      // Did we receive a message?
    
    if ( role == role_sender ) {                      // If we're the sender, we've received an ack payload
        radio.read(&message_count,sizeof(message_count));
        printf("Ack:%lu\n\r",message_count);
    }

    
    if ( role == role_receiver ) {                    // If we're the receiver, we've received a time message
      static unsigned long got_time;                  // Get this payload and dump it
      radio.read( &got_time, sizeof(got_time) );
      printf("Got payload %lu\n\r",got_time);
      radio.writeAckPayload( 1, &message_count, sizeof(message_count) );  // Add an ack packet for the next time around.  This is a simple
      ++message_count;                                // packet counter
  
    }
  }
}
