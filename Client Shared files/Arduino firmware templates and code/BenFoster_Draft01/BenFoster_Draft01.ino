// info on rotary encoder https://au.mouser.com/datasheet/2/678/S22_Kit_Encoder_DS102-1289131.pdf
// info on "WWW" SK9822 LED (actually it's just a RGB chip with white phosphors) https://cdn-shop.adafruit.com/product-files/2351/SK9822_datasheet_SHIJI.pdf 
// info on a 2014 paper 15 seconds in is a similar rig https://www.youtube.com/watch?v=UgPQRAZ5OPI

/*  Please correct as you go my 
 *    - mistakes
 *    - nomenculture 
 *    - formatting
 *    
 *  Please suggest a good way of sharing arduino files whether live through a cloud service or just emailing files back and forth
 *  If you feel we need to migrate to different PLC and software, please inform me asap
 *  
 *  The LED arc will begin to rotate using a stepper motor (something which may be added to this code in the future along with other features).  
 *  - In the first full rotation past the rotatry encoder's "ZERO_Point", all LEDs will be off 
 *  - For every following rotation up up to about 15th rotations, a different sequence of LEDs will light up, triggered (for fixed lengths of time but differeing PWM values) by the rotary encoder counting up from 0-1000
 *  - once all rotations have been completed, the seqence resets
 *  - For the time being, the tables I will provide will only be for 10 LEDs & for the whole 1000 steps per rotation. 
 *    - I have concerns about arduinos filling up their memory with the huge tables
 *    - I have concerns about the clock speed of the arduino and if it is able to execute all these commands to the LEDs
 *    - the final product has:
 *      - about 140 LEDs x 3 WWW PWM values x 1000 steps x 16 tables... with a few more signal commands to make every second!
 *        - The Arc will always rotate at a constant speed
 *        - The Arc will always rotate the same direction (counting UP on the encoder)
 *        - LED PWM values can be triggered by the state change of the encoder (eg, every time it +1) rather than exact encoder values (eg, step 764)... I believe it'll make the code more compact and resilient but you are welcome to tell me otherwise
 *          - It resets back to zero after every rotation anyway
 *      - WWW value is 0-253,0-253,0-253. Therefore I would like to have 0-761 levels of brightness if possible (note, I can supply the tables with 3 or 1 cells. ie. 0-253,0-253,0-253 or 0-761 per LED per rotary encoder increment
 *  Signal voltage 5V (arduino driven)
 *  LEDs & Arduino Power (external 5V power supply)

#define           // what I'm hoping to have as my calibration parameters
ZERO_Point  002   // value 0-999 on encoder where we take the ZERO_Point to be.
NUM_LEDS    10    // number of LEDs in the strip (lets start with just 10. Eventually, 140!)
TRIG_START  004   // When to trigger CAMERA_TRG on rotary encoder to ON 
TRIG_END    997   // When to trigger CAMERA_TRG on rotary encoder to OFF
GAMMA       25    // Values are from 1-32. Refer to SK9822 for notes. I'd like to have this as a global calibration value depending on the lense and F-Stop

pins              //
LED_PIN 11        // Output - Signal to the SK9822 LED strip
LED_CLK 13        // Clock to the LED strip or vice versa. 
CAMERA_TRG 5      // Output - Camera shutter trigger - HIGH or LOW signal
ROT_Ch+A 3        // Rotary Encoder
ROT_Ch-A 4        // Rotary Encoder
ROT_Ch+B 6        // Rotary Encoder
ROT_Ch-B 7        // Rotary Encoder
ROT_Ch+I 9        // Rotary Encoder

int
RotaryCounter = 0;    // counter for the number of rotations
TriggerState = 0;     // current state of the camera trigger
lastTriggerState = 0; // previous state of the trigger

const int
PULSE       450   // Time in microseconds of how long LEDs stay on per rotary encoder increment (there may be issues with PWM, SK9822 capabilities, Arduino limits... but lets try). millis will be too large with 1000 steps and 1000 milliseconds per 1 second rotation

- Excel pages copied in CSV style? please define a good and efficient style.
- Im also not sure how you'll go about it. 

- Below is a guess...
state{
OFF   // 01st rotation LEDs OFF
-1_0  // 02nd rotation 
+0_0  // 03rd rotation 
+1_0  // 04th rotation 
-2_1  // 05th rotation 
-1_1  // 06th rotation 
+0_1  // 07th rotation 
+1_1  // 08th rotation 
+2_1  // 09th rotation 
-3_2  // 10th rotation 
-2_2  // 11th rotation 
-1_2  // 12th rotation 
+0_2  // 13th rotation 
+1_2  // 14th rotation 
+2_2  // 15th rotation 
+3_2  // 16th rotation 
}
*/


//rotary variables:
#define DEBUG_ENCODER 0
#include <Encoder.h>
int32_t absolute_position=0;
int32_t last_position=0; //the last position where we applied led values:
int32_t relative_position=0;
int32_t first_positions=0;
uint8_t RotaryCounter = 0;    // counter for the number of rotations
uint8_t rotaryIndexFound =0;
int32_t rotaryStart =0;
int8_t currentRotation=0;

#define rotaryResolution 20 //1000 //2000 pulses is one rotation
#define ZERO_Point  002   // value 0-999 on encoder where we take the ZERO_Point to be.

#define rotaryA 1 //pin up
#define rotaryB 2 //pin down
#define rotaryIndex 3 //not part of the rotary lbibrary --> just do normal readout, we only need to check this once in the start, so no interrupt
Encoder MainEncoder(rotaryA, rotaryB);

//camera variables

uint8_t TriggerState = 0;     // current state of the camera trigger
uint8_t lastTriggerState = 0; // previous state of the trigger

#define TRIG_START  004   // When to trigger CAMERA_TRG on rotary encoder to ON 
//MAKE SURE TRIG_END >TRIG_START +100 (this is hysteresis we implemented
#define TRIG_END    997   // When to trigger CAMERA_TRG on rotary encoder to OFF
#define TRIG_PIN  5

//led_variables
#define DEBUG_LEDS 0
#include <FastLED.h>
#define GAMMA       25    // Values are from 1-32. Refer to SK9822 for notes. I'd like 
#define PULSE       450   // Time in microseconds of how long LEDs stay on per rotary encoder increment (there may be issues with PWM, SK9822 capabilities, Arduino limits... but lets try). millis will be too large with 1000 steps and 1000 milliseconds per 1 second rotation
#define NUM_LEDS  137
#define LED_TYPE SK9822
#define COLOR_ORDER BGR
#define LED_DATA_PIN 26
#define LED_CLK_PIN 27
#define NUM_ROTATIONS 16
CRGB leds[NUM_LEDS];

//the PROGMEM makes it store in FLASH --> we have 8MB so should be sufficient...
//uint8_t ledValuesFromSD[NUM_ROTATIONS][rotaryResolution][NUM_LEDS][3];  //this should fit in FLASH: search the word to store in FLash

//sd variables
#define DEBUG_SD 1
#include <SD.h>
#define chipSelect BUILTIN_SDCARD
File  myFile;
elapsedMicros tic;

//internal FLASH
//#include <LittleFS.h>

//store led values as const in FLash
void setup() {
  // set up as necessary
  absolute_position=0;  //contains rotation * rotartyResolution + relative position in the rotary...
  rotaryIndexFound =0;
  RotaryCounter =0;
  //power_LEDS(0, 0);//LEDS OFF
  digitalWriteFast(TRIG_PIN,0); //camera trigger OFF

  //array (rotations, position within a rotation, LED, BGR VALUE
  //uint8_t led_array[15][rotaryResolution][NUM_LEDS][3] ;
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect.
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    return;
  }

  //read back the data from the SD_CARD and store these in FLash so we can quickly read them when program executes

    char* fileName = "rot0.txt";
    Serial.println(fileName);

    myFile = SD.open(fileName, FILE_READ);
    Serial.println("OK");
    if (myFile) {
      //ledValuesFromSD[NUM_ROTATIONS][rotaryResolution][NUM_LEDS][3];
      // read from the file until there's nothing else in it:
      if(DEBUG_SD){
        Serial.write(myFile.read());
      }
   }
   else {
      // if the file didn't open, print an error:
      Serial.println("error opening ");
      Serial.println(fileName);
   }

  Serial.println("FILEREAD_DONE");
  FastLED.addLeds<LED_TYPE, LED_DATA_PIN, LED_CLK_PIN, COLOR_ORDER, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
  
  FastLED.setBrightness(GAMMA);
  for (int i=0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }

  if(DEBUG_LEDS){
    //let's try a walking "BLUE"
    FastLED.show(); //all leds black
     for(int dot = 0; dot < NUM_LEDS; dot++) { 
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

  if(DEBUG_ENCODER)  Serial.println("encoder_searching_index");
  //#################first loop until rotary encoder index toggles --> this is an absolute position: rotaryStart.##########
  while(rotaryIndexFound ==0){
    if(DEBUG_ENCODER) Serial.println(MainEncoder.read());
    if(digitalReadFast(rotaryIndex) ==1){
     rotaryIndexFound =1; //breaks the while loop
     rotaryStart = MainEncoder.read();
     if(DEBUG_ENCODER)  Serial.println("ROTARY_START");
     if(DEBUG_ENCODER)  Serial.println(rotaryStart);
    }
  }
  if(DEBUG_ENCODER)  Serial.println("############################");
  if(DEBUG_ENCODER)  Serial.println("encoder_searching_index_DONE");
  currentRotation =0;
  last_position = 0; //just initialize so we know later when we are in first run
  
  while(currentRotation < NUM_ROTATIONS){
    absolute_position = MainEncoder.read(); //counts rotary steps since start
    currentRotation=(absolute_position -  ZERO_Point - rotaryStart)/rotaryResolution;
    relative_position =  (absolute_position -ZERO_Point - rotaryStart)-currentRotation  * rotaryResolution;
    if(DEBUG_ENCODER){
    Serial.println("");
    Serial.print(absolute_position);Serial.print("    ");
    Serial.print(currentRotation);Serial.print("    ");
    Serial.print(relative_position);Serial.print("    ");
    }
    if(currentRotation >=1){ //power up
                             //the sdcard acts as a fifo buffer just need to keep track if we need to skip led_reads or not (e,g, when rotation is faster than read speed)
        if(relative_position >0 and last_position ==0){//FIRST TIME IN THIS ROTATION --> START POWERING THE LEDS, this happens only once
          last_position=absolute_position;
        }
        while(absolute_position>= last_position){ //the last position is increased in the while loop
          //read the flash content: to apply to led but also to update sd card buffer when rotary is faster  than the led
          for(int dot = 0; dot < NUM_LEDS; dot++) { 
              uint8_t r=myFile.read();
              uint8_t g=myFile.read();
              uint8_t b=myFile.read();
              if(last_position ==absolute_position)
                leds[dot] = CRGB(r, g,b);
          }
          if(last_position ==absolute_position){//apply leds when this is not the case --> absolute position is updating faster than last_position --> we are rotating in the wrong direction
            FastLED.show();
            tic =0;
            //next step: all leds BLACK
            for(int dot = 0; dot < NUM_LEDS; dot++) { 
                leds[dot] = CRGB::Black;
            }
            while(tic<PULSE){
              //keep waiting
            }
            FastLED.show(); //LED'S BLACK
          }
          last_position++;//we have readback new position from the sd-file
      } 
    }
    if(relative_position >= TRIG_START && relative_position < TRIG_END-100){//the -100 is some hysteresis to avoid retriggering when we turn back a bit
      //drive camera trigger high
      digitalWriteFast(TRIG_PIN,1); //camera trigger ON
    }
    if(relative_position >= TRIG_END){
      digitalWriteFast(TRIG_PIN,0); //camera trigger OFF
      //drive camera trigger low
    }
  }
}
