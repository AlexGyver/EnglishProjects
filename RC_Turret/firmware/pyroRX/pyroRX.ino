/*  
  Arduino based FPV RC turret
  https://github.com/AlexGyver/EnglishProjects/tree/master/RC_Turret
  Created 2017
  by AlexGyver
  AlexGyver Technologies
  for MadGyver YouTube channel
*/

#define FUSE_TIME 1800  // fuse powering time
int posX0 = 1470;     	// start position of X servo (360 servo doesn't move at ~1500 signal)
int posY = 1500;      	// start position of Y servo 

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>

Servo servoX, servoY;

byte MOSFET[12] = {4, 18, 5, 17, 7, 15, 3, 19, 6, 16, 8, 14}; // mosfet pins in order
boolean FLAGS[12]; 				// state flags {0,0,0,0,0,0,0,0}
unsigned long TIMES[12]; 		// mosfet timers

byte fuse_number;
boolean launch, launch_flag;
int in_data[3] = {0, 512, 512};
byte pipeNo;

RF24 radio(9, 10);

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

int posX;
int errorX, errorY;
unsigned long last;

void setup() {
  //Serial.begin(9600); // debug

  servoX.attach(1);
  servoY.attach(0);

  for (int i = 0; i <= 11; i++) {
    pinMode(MOSFET[i], OUTPUT);
  }

  // ******************* NRF config *******************
  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(6);
  radio.openReadingPipe(1, address[0]);
  
  radio.setChannel(0x60);		// СHANNEL!!!
  
  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);
  radio.powerUp();
  radio.startListening();
}

void loop() {

  if (millis() - last > 3) {	// every 3 ms
    posX = posX0 - errorX / 3; 	// servo speed (X axis)
    posY = posY - errorY / 300; // servo Y axis

    posX = constrain(posX, 900, 2000);
    posY = constrain(posY, 900, 2000);

    last = millis();
  }

  while ( radio.available(&pipeNo)) {
    radio.read( &in_data, sizeof(in_data) );
    
    errorX = in_data[0];	// servo X value
    errorY = in_data[1];	// servo Y value
    
    servoX.writeMicroseconds(posX);
    servoY.writeMicroseconds(posY);
    
    launch = in_data[2];	// fuse value

    if (launch == 1 && launch_flag == 0) {
      launch_flag = 1;

      if (FLAGS[fuse_number] == 0) {
        FLAGS[fuse_number] = 1;                           // flag for this fuse
        TIMES[fuse_number] = millis();                    // remember time for this fuse
        digitalWrite(MOSFET[fuse_number], HIGH);          // activate fuse
        //Serial.print("Fuse #"); Serial.print(fuse_number); Serial.println(" ON");
      }
      fuse_number++;
      if (fuse_number > 11) fuse_number = 0;
    }
    if (launch == 0 && launch_flag == 1) {
      launch_flag = 0;
    }

  }

  for (int i = 0; i <= 11; i++) {                                   // for every fuse
    if (millis() - TIMES[i] > FUSE_TIME && FLAGS[i] == 1) {         // if timer overflow
      digitalWrite(MOSFET[i], LOW);                                 // disable fuse
      FLAGS[i] = 0;
      //Serial.print("Fuse #"); Serial.print(i); Serial.println(" OFF");
    }
  }
}
