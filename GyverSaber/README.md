[![MadGyver YouTube](http://alexgyver.ru/git_madgyver.jpg)](https://www.youtube.com/channel/UCNEOyqhGzutj-YS-d5ckYdg?sub_confirmation=1)
# GyverSaber - Arduino based multicolor sound playing lightsaber
* [Project description](#chapter-0)
* [Folders](#chapter-1)
* [Schemes](#chapter-2)
* [Components](#chapter-3)
* [Assemply and set up](#chapter-7)

<a id="chapter-0"></a>
## Project description
SUPER-DUPER COOL ARDUINO BASED MULTICOLOR SOUND PLAYING LIGHTSABER!
https://youtu.be/__gp2RfTMnk


## Download last release
[**Click me**](https://github.com/AlexGyver/EnglishProjects/releases/download/GyverSaber/GyverSaber.rar)

#### CAPABILITIES:
* Smooth turning on/off with lightsaber-like sound effect
* Randomly pulsing color (you can turn it off)
* Sounds:
  + MODE 1: generated hum. Frequency depends on angle velocity of blade
  + MODE 2: hum sound from SD card
    - Slow swing - long hum sound (randomly from 4 sounds)
    - Fast swing - short hum sound (randomly from 5 sounds)
* Bright white flash when hitting
* Play one of 16 hit sounds, when hit
  + Weak hit - short sound
  + Hard hit - long "bzzzghghhdh" sound
* After power on blade shows current battery level from 0 to 100 percent
* Battery safe mode:
  + Battery is drain BEFORE TURNING ON: GyverSaber will not turn on, button LED will PULSE a couple of times
  + Battery is drain AFTER TURNING ON: GyverSaber will be turned off automatically
#### CONTROL BUTTON:
* HOLD - turn on / turn off GyverSaber
* TRIPLE CLICK - change color (red - green - blue - yellow - pink - ice blue)
* QUINARY CLICK - change sound mode (hum generation - hum playing)
* Selected color and sound mode stored in EEPROM (non-volatile memory)

<a id="chapter-1"></a>
## Folders
- **libraries** - libraries
- **GyverSaber** - arduino sketch
- **SDsounds** - GyverSaber sound pack

<a id="chapter-2"></a>
## Schemes
![SCHEME](https://github.com/AlexGyver/EnglishProjects/blob/master/GyverSaber/schemes/draw.jpg)
![SCHEME](https://github.com/AlexGyver/EnglishProjects/blob/master/GyverSaber/schemes/scheme1.jpg)
![SCHEME](https://github.com/AlexGyver/EnglishProjects/blob/master/GyverSaber/schemes/scheme2.jpg)

<a id="chapter-3"></a>
## Components
* Arduino NANO https://ali.ski/jmFH9_ https://ali.ski/5OBVTQ
* Addressable LED strip. WS2811, 12V. Take **white PCB, IP30, 60 LEDs per meter**
https://ali.ski/NMjk8 https://ali.ski/zW95D
* Button with LED. **Take 5V version** https://ali.ski/Mwcxt
* MPU6050 https://ali.ski/v5KZLL https://ali.ski/uXu5vi
* Cheap MicroSD https://ali.ski/GxgI4D
* MicroSD module mini https://ali.ski/JHIePz https://ali.ski/KsPKO
* Or this https://ali.ski/WKZdDk
* Batteries 18650 with protection https://ali.ski/XBqThJ https://ali.ski/lx5do https://ali.ski/3DZON9
* DCDC Step Down https://ali.ski/Gu17D https://ali.ski/LGc7S https://ali.ski/AZ7FEd
* Amplifier https://ali.ski/w3a6s https://ali.ski/UJQHAm https://ali.ski/9chyq
* Speaker https://ali.ski/fkf3b5 https://ali.ski/f3bznw https://ali.ski/NGb4L
* Resistors KIT https://ali.ski/DsRXU https://ali.ski/0QeVi
* Power button https://ali.ski/ADn0zC
* Charging port https://ali.ski/-02oW https://ali.ski/-gRAOg
* CC CV charger for 3 cells https://ali.ski/uPeDvv https://ali.ski/QY79Lj https://ali.ski/xxHSRf
* Prototype board https://ali.ski/XTO9n

<a id="chapter-7"></a>
## Assembly and set up
* [Quick start with Arduino](https://learn.sparkfun.com/tutorials/installing-arduino-ide)
* Open GyverSaber.ino and tune:
  - Number of microcircuits WS2811 on LED strip (note: one WS2811 controls 3 LEDs!)
  - Turn on or turn off blade pulsation
  - Hardly recommend measure real resistance of voltage divider resistors
    + System can work without battery monitoring, just deactivate BATTERY_SAFE. **BUT IT IS NOT RECOMMENDED**
* Flash arduino
* MicroSD info:
  - Size < 4G
  - Format to FAT
  - Copy audiofiles **in the root**
  - If you want add your own sounds, convert them to .WAV:
    + 8 bit
	+ 16-32 kHz
	+ Mono
	+ Use online converters or Total Audio Converter
* Assemble schmeme
  - **WARNING!** If you are flashing **assembled scheme**, you must power up it! Arduino will not work correct with connected DCDC converter!
* Enjoy!

## GyverSaber settings
    NUM_LEDS 30         // number of microcircuits WS2811 on LED strip (note: one WS2811 controls 3 LEDs!)
    BTN_TIMEOUT 800     // button hold delay, ms
    BRIGHTNESS 255      // max LED brightness (0 - 255)

    SWING_TIMEOUT 500   // timeout between swings
    SWING_L_THR 150     // swing angle speed threshold
    SWING_THR 300       // fast swing angle speed threshold
    STRIKE_THR 150      // hit acceleration threshold
    STRIKE_S_THR 320    // hard hit acceleration threshold
    FLASH_DELAY 80      // flash time while hit

    PULSE_ALLOW 1       // blade pulsation (1 - allow, 0 - disallow)
    PULSE_AMPL 20       // pulse amplitude
    PULSE_DELAY 30      // delay between pulses

    R1 100000           // voltage divider real resistance
    R2 51000            // voltage divider real resistance
    BATTERY_SAFE 1      // battery monitoring (1 - allow, 0 - disallow)

    DEBUG 0             // debug information in Serial (1 - allow, 0 - disallow)