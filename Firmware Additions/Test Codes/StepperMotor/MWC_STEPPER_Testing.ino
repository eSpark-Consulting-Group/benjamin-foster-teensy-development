/*
Wiring:
VCC 9-42 – VDC
GND - Power supply ground
ENA- - Teensy GND
ENA+ - Teensy Pin 7
DIR- - Teensy GND
DIR+ - Teensy Pin 8
PUL- - Teensy GND
PUL+ - Teensy Pin 9
A-, A+ - Coil 1 stepper motor
B-, B+ - Coil 2 stepper motor

*/
#include<mwc_stepper.h>

#define EN_PIN 7
#define DIR_PIN 8
#define STEP_PIN 9

#define RPM 50
#define RPM1 50

#define PULSE 200

#define ClOCKWISE 1
#define OTHERWISE 0

MWCSTEPPER nema23(EN_PIN, DIR_PIN, STEP_PIN);

void setup() {

  nema23.init();
  
}

void loop() {
  nema23.set(ClOCKWISE, RPM, PULSE);

  for (size_t i = 0; i < 200; i++)
  {
    nema23.run();
  }

  delay(1000);

  nema23.set(OTHERWISE, RPM1, PULSE);

  for (size_t i = 0; i < 200; i++)
  {
    nema23.run();
  }
}
