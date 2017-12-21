#define DHTTYPE DHT11  // используемый датчик, DHT11 или DHT22

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
  pot = analogRead(potPIN) / 10;   // перевод в диапазон примерно от 0 до 100
  if (pot > hum) {                 // если установка больше текущей влажности
    digitalWrite(mosPIN, 1);       // врубить всё
    digitalWrite(blueLED, 1);
    digitalWrite(redLED, 1);
  } else {                         // если нет
    digitalWrite(mosPIN, 0);       // вырубить всё
    digitalWrite(blueLED, 0);
    digitalWrite(redLED, 0);
  }

  if (millis() - readTimer > 1000) {  // секундный таймер (для стабильности измерений)
    hum = dht.readHumidity();         // получить значение с датчика
    readTimer = millis();             // обнулить таймер
  }
  delay(5);                         // задержка для стабильности
}
