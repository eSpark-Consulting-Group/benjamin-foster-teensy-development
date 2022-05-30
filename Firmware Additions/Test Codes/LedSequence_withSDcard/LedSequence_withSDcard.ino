#include <FastLED.h>
#include <SD.h>
#include <SPI.h>

//functions:
void LedExecution();
void SD_Card_Initialization();                                //SD Card Initialization
void SD_Card_read();                                          //function to read the text file from SD-card
void parseData(char* workVar);                                //function to parse the data

//SD-card Variables:
#define chipSelect BUILTIN_SDCARD
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

//FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); 

void setup() 
{ 
  Serial.begin(115200);
  SD_Card_Initialization();
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
}

void loop() {
  SD_Card_read(); 
  LedExecution();
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
  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, HIGH);

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }
  Serial.println("initialization done.");
  MyFile = SD.open("Image.txt");
  if (MyFile) {
    Serial.println("Image.txt is avaiable");
    MyFile.close();
  }
  else {
    Serial.println("error opening Image.txt");
  }
  MyFile = SD.open("Image.txt");
}
void SD_Card_read() {

  while (MyFile.available() && flag == true ) {
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
