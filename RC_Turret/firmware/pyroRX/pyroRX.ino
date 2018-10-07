/*   Данный скетч делает следующее: передатчик (TX) отправляет массив
     данных, который генерируется согласно показаниям с кнопки и с
     двух потенциомтеров. Приёмник (RX) получает массив, и записывает
     данные на реле, сервомашинку и генерирует ШИМ сигнал на транзистор.
    by AlexGyver 2016
*/

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <Servo.h>

Servo servoX, servoY;

byte MOSFET[12] = {4, 18, 5, 17, 7, 15, 3, 19, 6, 16, 8, 14}; //массив пинов, к которым подключены мосфеты
boolean FLAGS[12]; // массив, хранящий время для таймера каждого мосфета? по умолчанию {0,0,0,0,0,0,0,0}
unsigned long TIMES[12]; // массив, хранящий состояния мосфетов

int fuse_time = 1800;  // время в миллисекундах, которое ток будет подаваться  на спираль
byte fuse_number;
boolean launch, launch_flag;
int in_data[3] = {0, 512, 512};
byte pipeNo;

RF24 radio(9, 10); // "создать" модуль на пинах 9 и 10 Для Уно

byte address[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node", "6Node"}; //возможные номера труб

int posX0 = 1470;      // начальное положение серво по оси Х (серво постоянного вращения, покоится на месте)
int posY = 1500;      // начальное положение серво по оси Х (серво постоянного вращения, покоится на месте)
int posX;
int errorX, errorY;    //  переменные для расчёта ошибки
unsigned long last;

void setup() {
  //Serial.begin(9600); //открываем порт для связи с ПК

  servoX.attach(1);
  servoY.attach(0);

  // настраиваем пины мосфетов как выходы по массиву
  for (int i = 0; i <= 11; i++) {
    pinMode(MOSFET[i], OUTPUT);
  }

  radio.begin(); //активировать модуль
  radio.setAutoAck(1);         //режим подтверждения приёма, 1 вкл 0 выкл
  radio.setRetries(0, 15);    //(время между попыткой достучаться, число попыток)
  radio.enableAckPayload();    //разрешить отсылку данных в ответ на входящий сигнал
  radio.setPayloadSize(6);     //размер пакета, в байтах

  radio.openReadingPipe(1, address[0]);     //хотим слушать трубу 0
  radio.setChannel(0x60);  //выбираем канал (в котором нет шумов!)

  radio.setPALevel (RF24_PA_MAX); //уровень мощности передатчика. На выбор RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  radio.setDataRate (RF24_1MBPS); //скорость обмена. На выбор RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  //должна быть одинакова на приёмнике и передатчике!
  //при самой низкой скорости имеем самую высокую чувствительность и дальность!!

  radio.powerUp(); //начать работу
  radio.startListening();  //начинаем слушать эфир, мы приёмный модуль
}

void loop() {

  if (millis() - last > 3) {
    posX = posX0 - errorX / 3; // управляем скоростью вращения сервы по Х
    posY = posY - errorY / 300; // управляем скоростью вращения сервы по Y

    posX = constrain(posX, 900, 2000);         // ограничиваем текущее положение серв, чтобы не выходило за пределы вращения
    posY = constrain(posY, 900, 2000);

    last = millis();
  }

  while ( radio.available(&pipeNo)) {             // слушаем эфир со всех труб
    radio.read( &in_data, sizeof(in_data) );      // читаем входящий сигнал
    
    errorX = in_data[0];         // расчёт ошибки, как начальное положение ручки - текущее
    errorY = in_data[1];         // расчёт ошибки, как начальное положение ручки - текущее
    
    servoX.writeMicroseconds(posX);
    servoY.writeMicroseconds(posY);
    
    launch = in_data[2];

    if (launch == 1 && launch_flag == 0) {
      launch_flag = 1;

      if (FLAGS[fuse_number] == 0) {
        FLAGS[fuse_number] = 1;                           // поднять флаг для мосфета, по входящему сигналу
        TIMES[fuse_number] = millis();                    // запомнить время прихода сигнала
        digitalWrite(MOSFET[fuse_number], HIGH);          // подать питание на мосфет (на запал)
        //Serial.print("Fuse #"); Serial.print(fuse_number); Serial.println(" ON");
      }
      fuse_number++;
      if (fuse_number > 11) fuse_number = 0;
    }
    if (launch == 0 && launch_flag == 1) {
      launch_flag = 0;
    }

  }

  for (int i = 0; i <= 11; i++) {                                   // пройтись по всем 10ти мосфетам
    if (millis() - TIMES[i] > fuse_time && FLAGS[i] == 1) {         // если время с момента открытия мосфета > заданного
      digitalWrite(MOSFET[i], LOW);                                 // закрыть мосфет, погасить запал
      FLAGS[i] = 0;
      //Serial.print("Fuse #"); Serial.print(i); Serial.println(" OFF");
    }
  }
}
