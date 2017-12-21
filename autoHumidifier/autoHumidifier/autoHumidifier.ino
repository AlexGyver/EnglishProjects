#define DHTTYPE DHT11  // used sensor, DHT11 or DHT22

#define potPIN A1
#define mosPIN 2
#define sensVCC 3
#define sensDATA 4
#define sensGND 5
#define blueLED 11
#define redLED 12

#include "DHT.h"
DHT dht(sensDATA, DHTTYPE);

unsigned long readTimer;
float hum;
int pot;

void setup() {
  // configure pins
  pinMode(mosPIN, OUTPUT);
  pinMode(sensVCC, OUTPUT);
  pinMode(sensGND, OUTPUT);
  pinMode(blueLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(sensDATA, INPUT_PULLUP);
  digitalWrite(mosPIN, 0);
  digitalWrite(sensVCC, 1);
  digitalWrite(sensGND, 0);
  digitalWrite(blueLED, 0);
  digitalWrite(redLED, 0);

  Serial.begin(9600);
  dht.begin();
  delay(1000);
  hum = dht.readHumidity();
}

void loop() {
  pot = analogRead(potPIN) / 10;   // convert pot value to ~0-100 range
  if (pot > hum) {                 // if set value more than air humidity
    digitalWrite(mosPIN, 1);       // power on
    digitalWrite(blueLED, 1);
    digitalWrite(redLED, 1);
  } else {                         // if not
    digitalWrite(mosPIN, 0);       // power off
    digitalWrite(blueLED, 0);
    digitalWrite(redLED, 0);
  }

  if (millis() - readTimer > 1000) {  // 1 second timeout
    hum = dht.readHumidity();         // get humidity
    readTimer = millis();
  }
  delay(5);                         // some delay for system stability
}
