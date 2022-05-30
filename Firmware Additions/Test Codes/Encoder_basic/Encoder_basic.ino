#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <FastLED.h>
#include <SD.h>
#include <SPI.h>

//functions:
void LedExecution();
void SD_Card_Initialization();                                //SD Card Initialization
void SD_Card_read();                                          //function to read the text file from SD-card
void parseData(char* workVar);                                //function to parse the data
void updateEncoder();
void EncoderInit();

//SD-card Variables:
char token[20];
String _buffer;
int ndx = 0 ;
int indexNum,ledNum,brightness[3];
bool flag = true;
File MyFile;

//LED Variables:
#define NUM_LEDS 50
#define DATA_PIN 3
#define CLOCK_PIN 13
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

//Encoder Definitions:
#define ZERO_Point  2               // value 0-999 on encoder where we take the ZERO_Point to be.
#define DEBUG_ENCODER 0
#define rotaryA 1                   //pin up
#define rotaryB 2                   //pin down
//#define rotaryIndex 3              //not part of the rotary lbibrary --> just do normal readout, we only need to check this once in the start, so no interrupt
Encoder MainEncoder(rotaryA, rotaryB);

void setup() 
{
  
  Serial.begin(115200);
  SD_Card_Initialization();
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  EncoderInit();
}

void loop() 
{
  SD_Card_read();
}
void updateEncoder()
{
  if(MainEncoder.read() == indexNum)
  {
    LedExecution();
  }
}
void EncoderInit()
{
  attachInterrupt(1, updateEncoder, CHANGE);
  attachInterrupt(2, updateEncoder, CHANGE);
}
void LedExecution()
{
   // Turn the LED on, then pause
 if(flag == false)
 {
  leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
  FastLED.show();
  flag = true;
 }
}
void SD_Card_Initialization() {
  while (!Serial);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  Serial.print("Initializing SD card...");

  if (!SD.begin(8)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }
  Serial.println("initialization done.");
  MyFile = SD.open("Hello.txt");
  if (MyFile) {
    Serial.println("Hello.txt is avaiable");
    MyFile.close();
  }
  else {
    Serial.println("error opening Hello.txt");
  }
  MyFile = SD.open("Hello.txt");
}
void SD_Card_read() {

  while (MyFile.available() && flag == true ) 
  {
      _buffer = MyFile.readStringUntil('\r');
      _buffer.toCharArray(token,20);
      parseData(token);
  }
}
void parseData(char* workVar)
{                                      //indexes of the EncoderIndex,Latitude & Longitude arrays
  if( flag == true){
  char * strtokIndx;                                  // this is used by strtok() as an index
  strtokIndx = strtok(workVar, ",");
  indexNum = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  ledNum = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  //strcpy(brightness,strtokIndx);
  brightness[0] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  //strcpy(brightness,strtokIndx);
  brightness[1] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, "\r");
  //strcpy(brightness,strtokIndx);
  brightness[2] = atoi(strtokIndx);

  /*Serial.println("***** Parsed Data ******");
  Serial.println(indexNum);
  Serial.print("LED Number: ");
  Serial.println(ledNum);
  Serial.print("R: ");
  Serial.println(brightness[0]);
  Serial.print("G: ");
  Serial.println(brightness[1]);
  Serial.print("B: ");
  Serial.println(brightness[2]);*/
  ndx++;
  flag = false;
  }
}
