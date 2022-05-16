//rotary variables:

#define DEBUG_ENCODER 1
#include <Encoder.h>
int32_t absolute_position = 0;
int32_t relative_position = 0;
int32_t first_positions = 0;
uint8_t RotaryCounter = 0;    // counter for the number of rotations
uint8_t RotaryEncoder_IndexFound = 0;
int32_t rotaryStart = 0;
int8_t currentRotation = 0;
#define rotaryResolution 2000 //2000 pulses is one rotation
#define ZERO_Point  001               // value 0-999 on encoder where we take the ZERO_Point to be.
#define RotaryEncoder_A 1                     // rotary pin up
#define RotaryEncoder_B 2                     // rotary pin down
#define RotaryEncoder_Index 3                 // we only need to check this once in the start, so no interrupt
Encoder MainEncoder(RotaryEncoder_A, RotaryEncoder_B);

//camera variables:

uint8_t TriggerState = 0;             // current state of the camera trigger
uint8_t lastTriggerState = 0;         // previous state of the trigger
#define TRIG_START 004                // when to trigger CAMERA_TRG on rotary encoder to ON   //MAKE SURE TRIG_END >TRIG_START +100 (this is hysteresis we implemented)?
#define TRIG_END 997                  // when to trigger CAMERA_TRG on rotary encoder to OFF
#define CameraTrigger  5 // PIN

//led_variables:

#define DEBUG_LEDS 1
#include <FastLED.h>
#define GAMMA 25                      // Values are from 1-32. Refer to SK9822 for info
#define PULSE 450                     // Time in microseconds. Will it work?
#define NUM_LEDS  137
#define LED_TYPE SK9822
#define COLOR_ORDER BGR
#define LED_DATA_PIN 26               // PIN
#define LED_CLK_PIN 27                // PIN
CRGB leds[NUM_LEDS];

//sd variables
#include <SD.h>
#define chipSelect BUILTIN_SDCARD
elapsedMillis tic;

void setup() {
  absolute_position = 0;              //contains rotation * rotartyResolution + relative position in the rotary...
  RotaryEncoder_IndexFound = 0;
  RotaryCounter = 0;
  power_LEDS(0, 0);                   //LEDS OFF
  digitalWriteFast(CameraTrigger, 0);      //camera trigger OFF

  //array (rotations, position within a rotation, LED, BGR VALUE
  uint8_t led_array[15][rotaryResolution][NUM_LEDS][3] ;
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  Serial.print("Initialising SD card...");
  delay(500);
  if (!SD.begin(chipSelect)) {
    Serial.println("initialisation failed!");
    return;
  delay(500);
  }

  //read back the data from the SD_CARD:
  char fileName [] = "test.txt";
  File  myFile;
  myFile = SD.open(fileName, FILE_READ);    // read from the file until there's nothing else in it:
  if (myFile) {
    Serial.println(fileName);
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();                         // close the file:
  } else {                                  // if the file didn't open, print an error:
    Serial.println("error opening ");
    Serial.println(fileName);
  }

  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLK_PIN, COLOR_ORDER, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);

  FastLED.setBrightness(GAMMA);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }

  //here we copy the codes from sd card:
  led_array[0][0][0][0] = 240;
  //...
  //...
  // ...3

  if (DEBUG_LEDS) {
    //let's try a walking "BLUE"
    FastLED.show(); //all leds black
    for (int dot = 0; dot < NUM_LEDS; dot++) {
      leds[dot] = CRGB::Blue;
      FastLED.show();
      // clear this led for the next time around the loop this is only updated after the next FASTLED.show (so in the next loop)
      leds[dot] = CRGB::Black;
      delay(30);
    }
  }
}

void loop() {
  //  Set up as necessary and cycle through the 16 rotations
  //first look for the absolute index of the rotary (the index signal

  if (DEBUG_ENCODER)  Serial.println("encoder_searching_index");
  while (RotaryEncoder_IndexFound == 0) {
    if (DEBUG_ENCODER) Serial.println(MainEncoder.read());
    if (digitalReadFast(RotaryEncoder_Index) == 1) {
      RotaryEncoder_IndexFound = 1; //breaks the while loop
      rotaryStart = MainEncoder.read();
      if (DEBUG_ENCODER)  Serial.println("ROTARY_START");
      if (DEBUG_ENCODER)  Serial.println(rotaryStart);
    }
  }
  if (DEBUG_ENCODER)  Serial.println("############################");
  if (DEBUG_ENCODER)  Serial.println("encoder_searching_index_DONE");
  currentRotation = 0;

  while (currentRotation < 16) {
    absolute_position = MainEncoder.read(); //counts rotary steps since start
    currentRotation = (absolute_position -  ZERO_Point - rotaryStart) / rotaryResolution;
    relative_position =  (absolute_position - ZERO_Point - rotaryStart) - currentRotation  * rotaryResolution;
    if (DEBUG_ENCODER) {
      Serial.println("");
      Serial.print(absolute_position); Serial.print("    ");
      Serial.print(currentRotation); Serial.print("    ");
      Serial.print(relative_position); Serial.print("    ");
    }
    if (currentRotation >= 1) { //power up
      //power_LEDS(currentRotation, relative_position) //gives spi commands to power the leds for a time PULSE
    }
    if (relative_position >= TRIG_START && relative_position < TRIG_END - 100) { //the -100 is some hysteresis to avoid retriggering when we turn back a bit  //drive camera trigger high
      digitalWriteFast(CameraTrigger, 1); //camera trigger ON
    }
    if (relative_position >= TRIG_END) {
      digitalWriteFast(CameraTrigger, 0); //camera trigger OFF
      //drive camera trigger low
    }
  }
}
