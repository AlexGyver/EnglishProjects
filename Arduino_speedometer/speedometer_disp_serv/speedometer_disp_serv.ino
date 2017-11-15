/*
  Created 2016
  by AlexGyver
  AlexGyver Technologies.
*/

#include <EEPROM.h>   // EEPROM library
#include <Servo.h>    // Servo library
Servo servo;
unsigned int min_speed = 0;  // minimal speed to display on scale, km/h
unsigned int max_speed = 40; // maximum speed to display on scale, km/h

//-----------display-----------
#include <TM74HC595Display-old.h>
int SCLK = 7;
int RCLK = 6;
int DIO = 5;
TM74HC595Display disp(SCLK, RCLK, DIO);
unsigned char LED_0F[29];
//-----------display-----------

volatile unsigned long lastturn, time_press;
volatile float SPEED;
volatile float DIST;
volatile boolean eeprom_flag;
float w_length = 2.050;
boolean flag;
boolean state, button;

void setup() {
  servo.write(0);
  Serial.begin(9600);                // configure serial for debug
  attachInterrupt(0, sens, RISING);  // hall sensor interrupt
  pinMode(3, OUTPUT);                // D3 as power source
  digitalWrite(3, HIGH);
  pinMode(8, INPUT);                 // some buttuon pin
  servo.attach(4);                   // servo pin
  // some digs for display
  LED_0F[0] = 0xC0; //0
  LED_0F[1] = 0xF9; //1
  LED_0F[2] = 0xA4; //2
  LED_0F[3] = 0xB0; //3
  LED_0F[4] = 0x99; //4
  LED_0F[5] = 0x92; //5
  LED_0F[6] = 0x82; //6
  LED_0F[7] = 0xF8; //7
  LED_0F[8] = 0x80; //8
  LED_0F[9] = 0x90; //9

  DIST = (float)EEPROM.read(0) / 10.0; // remember some distance
}

void sens() {
  if (millis() - lastturn > 80) {    // simple noise cut filter (based on fact that you will not be ride your bike more than 120 km/h =)
    SPEED = w_length / ((float)(millis() - lastturn) / 1000) * 3.6;   // calculate speed
    lastturn = millis();                                              // remember time of last revolution
    DIST = DIST + w_length / 1000;                                    // calculate distance
  }
  eeprom_flag = 1;
}

void loop() {
  // convert speed value to servo angle
  int pos = map(SPEED, min_speed, max_speed, 180, 0);
  servo.write(pos);         // turn servo
  disp.digit4(DIST, 50);    // display odo

  if ((millis() - lastturn) > 2000) {       // if there is no signal more than 2 seconds
    SPEED = 0;                              // so, speed is 0
    if (eeprom_flag) {                      // if eeprom flag is true
      EEPROM.write(0, (float)DIST * 10.0);  // write ODO in EEPROM
      eeprom_flag = 0;                      // flag down. To prevent rewritind
    }
  }

  if (digitalRead(8) == 1) {  // if button pressed
    DIST = 0;                 // clear distance
  }
}
