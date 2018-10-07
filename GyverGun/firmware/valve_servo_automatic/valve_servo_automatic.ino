/*
  Created 2016
  by AlexGyver
  AlexGyver Technologies
  for MadGyver YouTube channel
*/

#define MODE 5
#define TRIGGER 3
#define VALVE 2
#define SERVO 4

#define STARTPOS 145  	// bolt is open
#define STOPPOS 51    	// bolt is closed
#define VALVE_DELAY 20	// valve open delay in ms

#include <Servo.h>
Servo servo;

byte flag = 0;
byte trigger = 0;
byte set = 0;

void setup()
{
  pinMode(VALVE, OUTPUT);
  pinMode(TRIGGER, INPUT_PULLUP);
  pinMode(MODE, INPUT_PULLUP);

  servo.attach(SERVO);
  servo.write(STOPPOS);
}

void shot() {
  digitalWrite(VALVE, HIGH);  // open valve
  delay(VALVE_DELAY);         // wait
  digitalWrite(VALVE, LOW);   // close valve
  delay(20);                  // wait for ball to go away
  servo.write(STOPPOS);       // open bolt
  delay(272);                 // wait for new ball
  servo.write(STOPPOS);       // close bolt
  delay(272);                 // wait ball to settle
}

void loop() {
  set = !digitalRead(MODE);
  trigger = !digitalRead(TRIGGER);

  if (trigger && !set && !flag) {
    shot();
    flag = 1;
  }

  if (!trigger && !set && flag) {
    flag = 0;
  }

  if (trigger && set && !flag) {
    shot();
  }
  delay(5);
}
