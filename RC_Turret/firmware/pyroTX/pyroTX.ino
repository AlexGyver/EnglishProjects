/*
  Created 2017
  by AlexGyver
  AlexGyver Home Labs Inc.
*/
#include "Keypad.h"       //библиотека клавиатуры
#include <SPI.h>          // библиотека для работы с шиной SPI
#include "nRF24L01.h"     // библиотека радиомодуля
#include "RF24.h"         // ещё библиотека радиомодуля

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно
byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

byte redLED = 2;
byte greenLED = 3;
byte ready_toggle = 4;
byte launch_butt = 8;

byte stickX = 0;
byte stickY = 1;
int stickX0, stickY0;
int errorX, errorY;    //  переменные для расчёта ошибки

int check = 111;

boolean check_answer;
boolean flag, flag_fuse, launch_flag, toggle_flag;

int out_data[3]; // [номер запала, состояние, позиция Х сервы, позиция У сервы]

unsigned long last;

void setup() {
  Serial.begin(9600);

  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, HIGH);    // зажечь зелёный светодиод

  pinMode(launch_butt, INPUT_PULLUP);
  pinMode(ready_toggle, INPUT_PULLUP);

  delay(50);
  stickX0 = analogRead(stickX);
  delay(50);
  stickY0 = analogRead(stickY);

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(6);     //размер пакета, в байтах

  radio.openWritingPipe(address[0]);   //мы - труба 0, открываем канал для передачи данных
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.stopListening();  //не слушаем радиоэфир, мы передатчик
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

