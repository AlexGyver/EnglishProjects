/*
  Created 2017
  by MadGyver (AlexGyver)

   Wake up every second, get distance
   If there is some obstacle, power on servo, turn it, remember time, sleep sequence again
   If there is no obstacle and passed more than cap_time, power on servo and close the cap
*/
//-------------------- SETTINGS ----------------------
#define maxH 50           // working distance
#define cap_time 3        // time while cap is open, seconds
#define open_angle 10     // open angle
#define close_angle 155   // close angle

#define debug 0           // debug information (0 off, 1 on)
//-------------------- SETTINGS ----------------------

// --- WIRING ---
#define trigPin 3
#define echoPin 2
#define sensorVCC 4
#define MOSFETpin 6
#define servoPin 5
// --- WIRING ---

#include "LowPower.h" // sleep library
#include <Servo.h>    // servo library
Servo servo;

long duration;
byte distance;
unsigned long open_timer;
boolean open_flag;

void setup() {
  Serial.begin(9600);
  if (debug) Serial.println("system start");
  servo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, 0);
  pinMode(MOSFETpin, OUTPUT);
  digitalWrite(MOSFETpin, 0);
  pinMode(echoPin, INPUT);

  pinMode(sensorVCC, OUTPUT);
  digitalWrite(sensorVCC, 0);     // power off the sensor

  open_cap();                     // open cap with system start
}

void open_cap() {
  if (debug) Serial.println("open");
  digitalWrite(MOSFETpin, 1);     // power up the servo
  delay(1);
  servo.write(open_angle);        // open cap
  delay(1000);                    // wait servo
  digitalWrite(MOSFETpin, 0);     // disable servo
  open_flag = 1;                  // remember that cap is open
}
void close_cap() {
  if (debug) Serial.println("close");
  digitalWrite(MOSFETpin, 1);     // power up the servo
  delay(1);
  servo.write(close_angle);       // close cap
  delay(1000);                    // wait servo
  digitalWrite(MOSFETpin, 0);     // disable servo
  open_flag = 0;                  // remember that cap is closed
}

void loop() {
  LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);  // sleep 1 second

  digitalWrite(sensorVCC, 1);           // power up range sensor
  measure();                            // first "idle" measure, there are some glitches without it
  distance = measure();                 // get distance
  digitalWrite(sensorVCC, 0);           // disable range sensor
  if (debug) {
    Serial.println(distance);
    delay(500);
  }

  if (distance > 5 && distance < maxH) { // if distance fit the range
    open_timer = 0;                     // reset timer
    if (!open_flag) {                   // if cap is closed
      open_cap();
    }
  } else {
    if (open_flag) {                    // if cap is open
      open_timer++;                     // timer increment
      if (open_timer > cap_time) {      // if passed more than cap_time after cap was opened
        close_cap();
      }
    }
  }
}

// distance measuring function
byte measure() {
  digitalWrite(trigPin, 0);
  delayMicroseconds(5);
  digitalWrite(trigPin, 1);
  delayMicroseconds(15);
  digitalWrite(trigPin, 0);
  duration = pulseIn(echoPin, HIGH);
  return (duration / 2) / 29.1;
}
