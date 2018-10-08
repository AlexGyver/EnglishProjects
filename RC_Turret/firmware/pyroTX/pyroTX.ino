/*  
  Arduino based FPV RC turret
  https://github.com/AlexGyver/EnglishProjects/tree/master/RC_Turret
  Created 2017
  by AlexGyver
  AlexGyver Technologies
  for MadGyver YouTube channel
*/

#define redLED 2
#define greenLED 3
#define ready_toggle 4
#define launch_butt 8

#define stickX 0
#define stickY 1

#include "Keypad.h"
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

RF24 radio(9, 10);
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"};

int stickX0, stickY0;
int errorX, errorY;

int check = 111;

boolean check_answer;
boolean flag, flag_fuse, launch_flag, toggle_flag;

int out_data[3]; // [fuse flag, x position, y position]

unsigned long last;

void setup() {
  Serial.begin(9600);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, HIGH);

  pinMode(launch_butt, INPUT_PULLUP);
  pinMode(ready_toggle, INPUT_PULLUP);

  delay(50);
  stickX0 = analogRead(stickX);
  delay(50);
  stickY0 = analogRead(stickY);

  radio.begin();
  radio.setAutoAck(1);
  radio.setRetries(0, 15);
  radio.enableAckPayload();
  radio.setPayloadSize(6);

  radio.openWritingPipe(address[0]);
  radio.setChannel(0x60);		// СHANNEL!!!

  radio.setPALevel (RF24_PA_MAX);
  radio.setDataRate (RF24_1MBPS);
  radio.powerUp();
  radio.stopListening();
}

void loop() {
  if (abs(stickX0 - analogRead(stickX)) > 2) errorX = stickX0 - analogRead(0); else errorX = 0;
  if (abs(stickY0 - analogRead(stickY)) > 2) errorY = stickY0 - analogRead(1); else errorY = 0;

  out_data[0] = errorY;
  out_data[1] = errorX;
  radio.write(&out_data, sizeof(out_data));

  boolean launch_butt_state = !digitalRead(launch_butt);
  boolean toggle_state = !digitalRead(ready_toggle);

  if (launch_butt_state == 1 && launch_flag == 0 && flag == 1) {
    out_data[2] = 1;
    radio.write(&out_data, sizeof(out_data));
    out_data[2] = 0;
    launch_flag = 1;
  }
  if (launch_butt_state == 0 && launch_flag == 1) {
    launch_flag = 0;
  }
  if (toggle_state == 1) {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);
    flag = 1;
  }
  if (toggle_state == 0) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    flag = 0;
  }
}