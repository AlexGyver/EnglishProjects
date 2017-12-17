/*
     SUPER-DUPER COOL ARDUINO BASED MULTICOLOR SOUND PLAYING LIGHTSABER!
   HARDWARE:
     Addressable LED strip (WS2811) to get any blade color and smooth turn on effect
     MicroSD card module to play some sounds
     IMU MPU6050 (accel + gyro) to generate hum. Frequency depends on angle velocity of blade
     OR measure angle speed and play some hum sounds from SD
   CAPABILITIES:
     Smooth turning on/off with lightsaber-like sound effect
     Randomly pulsing color (you can turn it off)
     Sounds:
       MODE 1: generated hum. Frequency depends on angle velocity of blade
       MODE 2: hum sound from SD card
         Slow swing - long hum sound (randomly from 4 sounds)
         Fast swing - short hum sound (randomly from 5 sounds)
     Bright white flash when hitting
     Play one of 16 hit sounds, when hit
       Weak hit - short sound
       Hard hit - long "bzzzghghhdh" sound
     After power on blade shows current battery level from 0 to 100 percent
     Battery safe mode:
       Battery is drain BEFORE TURNING ON: GyverSaber will not turn on, button LED will PULSE a couple of times
       Battery is drain AFTER TURNING ON: GyverSaber will be turned off automatically
   CONTROL BUTTON:
     HOLD - turn on / turn off GyverSaber
     TRIPLE CLICK - change color (red - green - blue - yellow - pink - ice blue)
     QUINARY CLICK - change sound mode (hum generation - hum playing)
     Selected color and sound mode stored in EEPROM (non-volatile memory)
     
   Project GitHub repository: https://github.com/AlexGyver/EnglishProjects/tree/master/GyverSaber
   YouTube channel: https://www.youtube.com/channel/UCNEOyqhGzutj-YS-d5ckYdg?sub_confirmation=1
   Author: MadGyver
*/

// ---------------------------- SETTINGS -------------------------------
#define NUM_LEDS 30         // number of microcircuits WS2811 on LED strip (note: one WS2811 controls 3 LEDs!)
#define BTN_TIMEOUT 800     // button hold delay, ms
#define BRIGHTNESS 255      // max LED brightness (0 - 255)

#define SWING_TIMEOUT 500   // timeout between swings
#define SWING_L_THR 150     // swing angle speed threshold
#define SWING_THR 300       // fast swing angle speed threshold
#define STRIKE_THR 150      // hit acceleration threshold
#define STRIKE_S_THR 320    // hard hit acceleration threshold
#define FLASH_DELAY 80      // flash time while hit

#define PULSE_ALLOW 1       // blade pulsation (1 - allow, 0 - disallow)
#define PULSE_AMPL 20       // pulse amplitude
#define PULSE_DELAY 30      // delay between pulses

#define R1 100000           // voltage divider real resistance
#define R2 51000            // voltage divider real resistance
#define BATTERY_SAFE 1      // battery monitoring (1 - allow, 0 - disallow)

#define DEBUG 0             // debug information in Serial (1 - allow, 0 - disallow)
// ---------------------------- SETTINGS -------------------------------

#define LED_PIN 6
#define BTN 3
#define IMU_GND A1
#define SD_GND A0
#define VOLT_PIN A6
#define BTN_LED 4

// -------------------------- LIBS ---------------------------
#include <avr/pgmspace.h>   // PROGMEM library
#include <SD.h>
#include <TMRpcm.h>         // audio from SD library
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <toneAC.h>         // hum generation library
#include "FastLED.h"        // addressable LED library
#include <EEPROM.h>

CRGB leds[NUM_LEDS];
#define SD_ChipSelectPin 10
TMRpcm tmrpcm;
MPU6050 accelgyro;
// -------------------------- LIBS ---------------------------


// ------------------------------ VARIABLES ---------------------------------
int16_t ax, ay, az;
int16_t gx, gy, gz;
unsigned long ACC, GYR, COMPL;
int gyroX, gyroY, gyroZ, accelX, accelY, accelZ, freq, freq_f = 20;
float k = 0.2;
unsigned long humTimer = -9000, mpuTimer, nowTimer;
int stopTimer;
boolean bzzz_flag, ls_chg_state, ls_state;
boolean btnState, btn_flag, hold_flag;
byte btn_counter;
unsigned long btn_timer, PULSE_timer, swing_timer, swing_timeout, battery_timer, bzzTimer;
byte nowNumber;
byte LEDcolor;  // 0 - red, 1 - green, 2 - blue, 3 - pink, 4 - yellow, 5 - ice blue
byte nowColor, red, green, blue, redOffset, greenOffset, blueOffset;
boolean eeprom_flag, swing_flag, swing_allow, strike_flag, HUMmode;
float voltage;
int PULSEOffset;
// ------------------------------ VARIABLES ---------------------------------

// --------------------------------- SOUNDS ----------------------------------
const char strike1[] PROGMEM = "SK1.wav";
const char strike2[] PROGMEM = "SK2.wav";
const char strike3[] PROGMEM = "SK3.wav";
const char strike4[] PROGMEM = "SK4.wav";
const char strike5[] PROGMEM = "SK5.wav";
const char strike6[] PROGMEM = "SK6.wav";
const char strike7[] PROGMEM = "SK7.wav";
const char strike8[] PROGMEM = "SK8.wav";

const char* const strikes[] PROGMEM  = {
  strike1, strike2, strike3, strike4, strike5, strike6, strike7, strike8
};

int strike_time[8] = {779, 563, 687, 702, 673, 661, 666, 635};

const char strike_s1[] PROGMEM = "SKS1.wav";
const char strike_s2[] PROGMEM = "SKS2.wav";
const char strike_s3[] PROGMEM = "SKS3.wav";
const char strike_s4[] PROGMEM = "SKS4.wav";
const char strike_s5[] PROGMEM = "SKS5.wav";
const char strike_s6[] PROGMEM = "SKS6.wav";
const char strike_s7[] PROGMEM = "SKS7.wav";
const char strike_s8[] PROGMEM = "SKS8.wav";

const char* const strikes_short[] PROGMEM = {
  strike_s1, strike_s2, strike_s3, strike_s4,
  strike_s5, strike_s6, strike_s7, strike_s8
};
int strike_s_time[8] = {270, 167, 186, 250, 252, 255, 250, 238};

const char swing1[] PROGMEM = "SWS1.wav";
const char swing2[] PROGMEM = "SWS2.wav";
const char swing3[] PROGMEM = "SWS3.wav";
const char swing4[] PROGMEM = "SWS4.wav";
const char swing5[] PROGMEM = "SWS5.wav";

const char* const swings[] PROGMEM  = {
  swing1, swing2, swing3, swing4, swing5
};
int swing_time[8] = {389, 372, 360, 366, 337};

const char swingL1[] PROGMEM = "SWL1.wav";
const char swingL2[] PROGMEM = "SWL2.wav";
const char swingL3[] PROGMEM = "SWL3.wav";
const char swingL4[] PROGMEM = "SWL4.wav";

const char* const swings_L[] PROGMEM  = {
  swingL1, swingL2, swingL3, swingL4
};
int swing_time_L[8] = {636, 441, 772, 702};

char BUFFER[10];
// --------------------------------- SOUNDS ---------------------------------

void setup() {
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(100);  // ~40% of LED strip brightness
  setAll(0, 0, 0);             // and turn it off

  Wire.begin();
  Serial.begin(9600);

  // ---- НАСТРОЙКА ПИНОВ ----
  pinMode(BTN, INPUT_PULLUP);
  pinMode(IMU_GND, OUTPUT);
  pinMode(SD_GND, OUTPUT);
  pinMode(BTN_LED, OUTPUT);
  digitalWrite(IMU_GND, 0);
  digitalWrite(SD_GND, 0);
  digitalWrite(BTN_LED, 1);
  // ---- НАСТРОЙКА ПИНОВ ----

  randomSeed(analogRead(2));    // starting point for random generator

  // IMU initialization
  accelgyro.initialize();
  accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_250);
  if (DEBUG) {
    if (accelgyro.testConnection()) Serial.println(F("MPU6050 OK"));
    else Serial.println(F("MPU6050 fail"));
  }

  // SD initialization
  tmrpcm.speakerPin = 9;
  tmrpcm.setVolume(5);
  tmrpcm.quality(1);
  if (DEBUG) {
    if (SD.begin(8)) Serial.println(F("SD OK"));
    else Serial.println(F("SD fail"));
  } else {
    SD.begin(8);
  }

  if ((EEPROM.read(0) >= 0) && (EEPROM.read(0) <= 5)) {  // check first start
    nowColor = EEPROM.read(0);   // remember color
    HUMmode = EEPROM.read(1);    // remember mode
  } else {                       // first start
    EEPROM.write(0, 0);          // set default
    EEPROM.write(1, 0);          // set default
    nowColor = 0;                // set default
  }

  setColor(nowColor);
  byte capacity = voltage_measure();       // get battery level
  capacity = map(capacity, 100, 0, (NUM_LEDS / 2 - 1), 1);  // convert into blade lenght
  if (DEBUG) {
    Serial.print(F("Battery: "));
    Serial.println(capacity);
  }

  for (char i = 0; i <= capacity; i++) {   // show battery level
    setPixel(i, red, green, blue);
    setPixel((NUM_LEDS - 1 - i), red, green, blue);
    FastLED.show();
    delay(25);
  }
  delay(1000);                         // 1 second to show battery level
  setAll(0, 0, 0);
  FastLED.setBrightness(BRIGHTNESS);   // set bright
}

// --- MAIN LOOP---
void loop() {
  randomPULSE();
  getFreq();
  on_off_sound();
  btnTick();
  strikeTick();
  swingTick();
  batteryTick();
}
// --- MAIN LOOP---

void btnTick() {
  btnState = !digitalRead(BTN);
  if (btnState && !btn_flag) {
    if (DEBUG) Serial.println(F("BTN PRESS"));
    btn_flag = 1;
    btn_counter++;
    btn_timer = millis();
  }
  if (!btnState && btn_flag) {
    btn_flag = 0;
    hold_flag = 0;
  }
  // если кнопка удерживается
  if (btn_flag && btnState && (millis() - btn_timer > BTN_TIMEOUT) && !hold_flag) {
    ls_chg_state = 1;                     // flag to change saber state (on/off)
    hold_flag = 1;
    btn_counter = 0;
  }
  // если кнопка была нажата несколько раз до таймаута
  if ((millis() - btn_timer > BTN_TIMEOUT) && (btn_counter != 0)) {
    if (ls_state) {
      if (btn_counter == 3) {               // 3 press count
        nowColor++;                         // change color
        if (nowColor >= 6) nowColor = 0;
        setColor(nowColor);
        setAll(red, green, blue);
        eeprom_flag = 1;
      }
      if (btn_counter == 5) {               // 5 press count
        HUMmode = !HUMmode;
        if (HUMmode) {
          noToneAC();
          tmrpcm.play("HUM.wav");
        } else {
          tmrpcm.disable();
          toneAC(freq_f);
        }
        eeprom_flag = 1;
      }
    }
    btn_counter = 0;
  }
}

void on_off_sound() {
  if (ls_chg_state) {                // if change flag
    if (!ls_state) {                 // if GyverSaber is turned off
      if (voltage_measure() > 10 || !BATTERY_SAFE) {
        if (DEBUG) Serial.println(F("SABER ON"));
        tmrpcm.play("ON.wav");
        delay(200);
        light_up();
        delay(200);
        bzzz_flag = 1;
        ls_state = true;               // remember that turned on
        if (HUMmode) {
          noToneAC();
          tmrpcm.play("HUM.wav");
        } else {
          tmrpcm.disable();
          toneAC(freq_f);
        }
      } else {
        if (DEBUG) Serial.println(F("LOW VOLTAGE!"));
        for (int i = 0; i < 5; i++) {
          digitalWrite(BTN_LED, 0);
          delay(400);
          digitalWrite(BTN_LED, 1);
          delay(400);
        }
      }
    } else {                         // if GyverSaber is turned on
      noToneAC();
      bzzz_flag = 0;
      tmrpcm.play("OFF.wav");
      delay(300);
      light_down();
      delay(300);
      tmrpcm.disable();
      if (DEBUG) Serial.println(F("SABER OFF"));
      ls_state = false;
      if (eeprom_flag) {
        eeprom_flag = 0;
        EEPROM.write(0, nowColor);   // write color in EEPROM
        EEPROM.write(1, HUMmode);    // write mode in EEPROM
      }
    }
    ls_chg_state = 0;
  }

  if (((millis() - humTimer) > 9000) && bzzz_flag && HUMmode) {
    tmrpcm.play("HUM.wav");
    humTimer = millis();
    swing_flag = 1;
    strike_flag = 0;
  }
  long delta = millis() - bzzTimer;
  if ((delta > 3) && bzzz_flag && !HUMmode) {
    if (strike_flag) {
      tmrpcm.disable();
      strike_flag = 0;
    }
    toneAC(freq_f);
    bzzTimer = millis();
  }
}

void randomPULSE() {
  if (PULSE_ALLOW && ls_state && (millis() - PULSE_timer > PULSE_DELAY)) {
    PULSE_timer = millis();
    PULSEOffset = PULSEOffset * k + random(-PULSE_AMPL, PULSE_AMPL) * (1 - k);
    if (nowColor == 0) PULSEOffset = constrain(PULSEOffset, -15, 5);
    redOffset = constrain(red + PULSEOffset, 0, 255);
    greenOffset = constrain(green + PULSEOffset, 0, 255);
    blueOffset = constrain(blue + PULSEOffset, 0, 255);
    setAll(redOffset, greenOffset, blueOffset);
  }
}

void strikeTick() {
  if ((ACC > STRIKE_THR) && (ACC < STRIKE_S_THR)) {
    if (!HUMmode) noToneAC();
    nowNumber = random(8);
    // читаем название трека из PROGMEM
    strcpy_P(BUFFER, (char*)pgm_read_word(&(strikes_short[nowNumber])));
    tmrpcm.play(BUFFER);
    hit_flash();
    if (!HUMmode)
      bzzTimer = millis() + strike_s_time[nowNumber] - FLASH_DELAY;
    else
      humTimer = millis() - 9000 + strike_s_time[nowNumber] - FLASH_DELAY;
    strike_flag = 1;
  }
  if (ACC >= STRIKE_S_THR) {
    if (!HUMmode) noToneAC();
    nowNumber = random(8);
    // читаем название трека из PROGMEM
    strcpy_P(BUFFER, (char*)pgm_read_word(&(strikes[nowNumber])));
    tmrpcm.play(BUFFER);
    hit_flash();
    if (!HUMmode)
      bzzTimer = millis() + strike_time[nowNumber] - FLASH_DELAY;
    else
      humTimer = millis() - 9000 + strike_time[nowNumber] - FLASH_DELAY;
    strike_flag = 1;
  }
}

void swingTick() {
  if (GYR > 80 && (millis() - swing_timeout > 100) && HUMmode) {
    swing_timeout = millis();
    if (((millis() - swing_timer) > SWING_TIMEOUT) && swing_flag && !strike_flag) {
      if (GYR >= SWING_THR) {      
        nowNumber = random(5);          
        // читаем название трека из PROGMEM
        strcpy_P(BUFFER, (char*)pgm_read_word(&(swings[nowNumber])));
        tmrpcm.play(BUFFER);               
        humTimer = millis() - 9000 + swing_time[nowNumber];
        swing_flag = 0;
        swing_timer = millis();
        swing_allow = 0;
      }
      if ((GYR > SWING_L_THR) && (GYR < SWING_THR)) {
        nowNumber = random(5);            
        // читаем название трека из PROGMEM
        strcpy_P(BUFFER, (char*)pgm_read_word(&(swings_L[nowNumber])));
        tmrpcm.play(BUFFER);              
        humTimer = millis() - 9000 + swing_time_L[nowNumber];
        swing_flag = 0;
        swing_timer = millis();
        swing_allow = 0;
      }
    }
  }
}

void getFreq() {
  if (ls_state) {                                               // if GyverSaber is on
    if (millis() - mpuTimer > 500) {                            
      accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);       

      // find absolute and divide on 100
      gyroX = abs(gx / 100);
      gyroY = abs(gy / 100);
      gyroZ = abs(gz / 100);
      accelX = abs(ax / 100);
      accelY = abs(ay / 100);
      accelZ = abs(az / 100);

      // vector sum
      ACC = sq((long)accelX) + sq((long)accelY) + sq((long)accelZ);
      ACC = sqrt(ACC);
      GYR = sq((long)gyroX) + sq((long)gyroY) + sq((long)gyroZ);
      GYR = sqrt((long)GYR);
      COMPL = ACC + GYR;
      /*
         // отладка работы IMU
         Serial.print("$");
         Serial.print(gyroX);
         Serial.print(" ");
         Serial.print(gyroY);
         Serial.print(" ");
         Serial.print(gyroZ);
         Serial.println(";");
      */
      freq = (long)COMPL * COMPL / 1500;                        // parabolic tone change
      freq = constrain(freq, 18, 300);                          
      freq_f = freq * k + freq_f * (1 - k);                     // smooth filter
      mpuTimer = micros();                                     
    }
  }
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  FastLED.show();
}

void light_up() {
  for (char i = 0; i <= (NUM_LEDS / 2 - 1); i++) {        
    setPixel(i, red, green, blue);
    setPixel((NUM_LEDS - 1 - i), red, green, blue);
    FastLED.show();
    delay(25);
  }
}

void light_down() {
  for (char i = (NUM_LEDS / 2 - 1); i >= 0; i--) {      
    setPixel(i, 0, 0, 0);
    setPixel((NUM_LEDS - 1 - i), 0, 0, 0);
    FastLED.show();
    delay(25);
  }
}

void hit_flash() {
  setAll(255, 255, 255);            
  delay(FLASH_DELAY);                
  setAll(red, blue, green);        
}

void setColor(byte color) {
  switch (color) {
    // 0 - red, 1 - green, 2 - blue, 3 - pink, 4 - yellow, 5 - ice blue
    case 0:
      red = 255;
      green = 0;
      blue = 0;
      break;
    case 1:
      red = 0;
      green = 0;
      blue = 255;
      break;
    case 2:
      red = 0;
      green = 255;
      blue = 0;
      break;
    case 3:
      red = 255;
      green = 0;
      blue = 255;
      break;
    case 4:
      red = 255;
      green = 255;
      blue = 0;
      break;
    case 5:
      red = 0;
      green = 255;
      blue = 255;
      break;
  }
}

void batteryTick() {
  if (millis() - battery_timer > 30000 && ls_state && BATTERY_SAFE) {
    if (voltage_measure() < 15) {
      ls_chg_state = 1;
    }
    battery_timer = millis();
  }
}

byte voltage_measure() {
  voltage = 0;
  for (int i = 0; i < 10; i++) {    
    voltage += (float)analogRead(VOLT_PIN) * 5 / 1023 * (R1 + R2) / R2;
  }
  voltage = voltage / 10;           
  int volts = voltage / 3 * 100;    // 3 cells!!!
  if (volts > 387)
    return map(volts, 420, 387, 100, 77);
  else if ((volts <= 387) && (volts > 375) )
    return map(volts, 387, 375, 77, 54);
  else if ((volts <= 375) && (volts > 368) )
    return map(volts, 375, 368, 54, 31);
  else if ((volts <= 368) && (volts > 340) )
    return map(volts, 368, 340, 31, 8);
  else if (volts <= 340)
    return map(volts, 340, 260, 8, 0);
}
