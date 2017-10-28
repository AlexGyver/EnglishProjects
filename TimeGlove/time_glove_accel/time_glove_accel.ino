int light = 200;      // LIGHTS ON delay, microseconds
int dark;             // LIGHTS OFF delay
int min_dark = 1;     // min dark delay
int max_dark = 50;    // max dark delay
#define light_pin 2   // MOSFET pin
#define  potent_pin 6 // potentiometer pin
int angle;
boolean flag;
long lastchange;

#include <Wire.h>
#include "Kalman.h"
Kalman kalmanX;
Kalman kalmanZ;
uint8_t IMUAddress = 0x68;
/* IMU Data */
int16_t accX;
int16_t accY;
int16_t accZ;
int16_t tempRaw;
int16_t gyroX;
int16_t gyroY;
int16_t gyroZ;
double accXangle; // Angle calculate using the accelerometer
double accZangle;
double temp;
double gyroXangle = 180; // Angle calculate using the gyro
double gyroZangle = 180;
double compAngleX = 180; // Calculate the angle using a Kalman filter
double compAngleZ = 180;
double kalAngleX; // Calculate the angle using a Kalman filter
double kalAngleZ;
uint32_t timer;

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(light_pin, OUTPUT);
  i2cWrite(0x6B, 0x00); // Disable sleep mode
  kalmanX.setAngle(180); // Set starting angle
  kalmanZ.setAngle(180);
  timer = micros();
}

void loop() {
  measure();                                            // obtain acceleration and angle speeds
  if (accZ > 25000 && (millis() - lastchange > 300)) {  // Z axis shake detection
    flag = !flag;                                       // toggle light
    lastchange = millis();                              // timer
  }

  if (flag == 1) {
    angle = 250 - kalAngleZ;   // calculate angle (with 250 degrees offset)
    
    // dark times calculation
    dark = map(analogRead(potent_pin), 0, 1024, min_dark, max_dark);
    digitalWrite(light_pin, 1);    // lights up
    delayMicroseconds(light);      // wait
    digitalWrite(light_pin, 0);    // lights down
    delay(dark);                   // wait
    delayMicroseconds(2000 + angle * 10); // extra wait
  }
}

// oh my god, it's some tricky sh*t
void measure() {
  uint8_t* data = i2cRead(0x3B, 14);
  accX = ((data[0] << 8) | data[1]);
  accY = ((data[2] << 8) | data[3]);
  accZ = ((data[4] << 8) | data[5]);
  tempRaw = ((data[6] << 8) | data[7]);
  gyroX = ((data[8] << 8) | data[9]);
  gyroY = ((data[10] << 8) | data[11]);
  gyroZ = ((data[12] << 8) | data[13]);
  /* Calculate the angls based on the different sensors and algorithm */
  accZangle = (atan2(accX, accY) + PI) * RAD_TO_DEG;
  accXangle = (atan2(accY, accX) + PI) * RAD_TO_DEG;
  double gyroXrate = (double)gyroX / 131.0;
  double gyroZrate = -((double)gyroZ / 131.0);
  gyroXangle += kalmanX.getRate() * ((double)(micros() - timer) / 1000000); // Calculate gyro angle using the unbiased rate
  gyroZangle += kalmanZ.getRate() * ((double)(micros() - timer) / 1000000);
  kalAngleX = kalmanX.getAngle(accXangle, gyroXrate, (double)(micros() - timer) / 1000000); // Calculate the angle using a Kalman filter
  kalAngleZ = kalmanZ.getAngle(accZangle, gyroZrate, (double)(micros() - timer) / 1000000);
  timer = micros();
}

void i2cWrite(uint8_t registerAddress, uint8_t data) {
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.write(data);
  Wire.endTransmission(); // Send stop
}
uint8_t* i2cRead(uint8_t registerAddress, uint8_t nbytes) {
  uint8_t data[nbytes];
  Wire.beginTransmission(IMUAddress);
  Wire.write(registerAddress);
  Wire.endTransmission(false); // Don't release the bus
  Wire.requestFrom(IMUAddress, nbytes); // Send a repeated start and then release the bus after reading
  for (uint8_t i = 0; i < nbytes; i++)
    data [i] = Wire.read();
  return data;
}
