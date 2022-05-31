#include <AccelStepper.h>
#include <FastLED.h>
#include <SPI.h>
#include <SD.h>

//Functions:
void SD_Card_Initialization();
void geoSeq_Read();                                           //Function to Read Geo Sequence text file
void geoSeq_parseData(char *str_geoSeq);                      //Function to extract array EncoderPosition,Latitude,Longitude 
void Ledseq_Read();
void Ledseq_parseData(char* workVar);
void Led_Initialization();
void LedExecution();
void ArcTimer_setup();                      
void ArcMotor_Initialization();                               //Arc Motor Initialization
void LatMotor_Initialization();                               //Latitude Motor Initialization
void LonMotor_Initialization();                               //Longitude Motor Initialization


//Flags:
bool arcStart = false;
bool geoSeq_readflag = true;
bool Ledseq_readflag = false; //true in M3
bool stopSys = false; 

//Motors Pinouts & Configuration:
#define motorInterfaceType          1
//Motor1 : Arc Motor
/*#define Dir_ArcRotate               2 //6
#define STEP_ArcRotate              3 //7
#define ArcRotate_ENAPIN            4 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400 */

//Motor2 : Latitude
#define Dir_Latitude                2   //8
#define STEP_Latitude               3   //9

//Motor3 : Longitude
#define Dir_Longitude           4
#define STEP_Longitude          5

//SD Card Pinouts & Configuration
const int chipSelect = 10; //BUILTIN_SDCARD;  
char token[30];
String _buffer;
File MyFile;

//GeoSequence Variables ( SD-card ):
int indexNum_geoSeq[50], LatitudeVal[50], LongitudeVal[50];
int LatLon_ndx = 0; // To keep track of lat/lon values index

//LedSequence Values:
int ndx_Ledseq = 0 ;
int indexNum_Ledseq,ledNum,brightness[3];

//LED Variables:
#define NUM_LEDS 50
#define DATA_PIN 3
#define CLOCK_PIN 13
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

//ENCODER:
int32_t enc;

void setup()
{
  Serial.begin(115200);
  SD_Card_Initialization();
  geoSeq_Read();
  ArcTimer_setup();  
  Led_Initialization();
}
void loop()
{
  Ledseq_Read();
}
void SD_Card_Initialization() {
  while (!Serial);
  pinMode(chipSelect, OUTPUT);
  digitalWrite(chipSelect, HIGH);
  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    while (true);
  }
  Serial.println("initialization done.");
}
void geoSeq_Read()
{
  MyFile = SD.open("GeoSeq.txt");
    if (MyFile) 
    {
      Serial.println("GeoSeq.txt is avaiable");
      MyFile.close();
    }
    else 
    {
      Serial.println("error opening GeoSeq.txt");
      while(1); // <- not neccessary but I used this to stop my code here if .txt is not available.
    }

  MyFile = SD.open("GeoSeq.txt");
  while (MyFile.available()) {
     _buffer = MyFile.readStringUntil('\r');
     _buffer.toCharArray(token,50);
     geoSeq_parseData(token);
    }
  MyFile.close();   //closing the GeoSequence.txt file after the Lat/Lon values are stored
  geoSeq_readflag = false;   //indicates Geo Sequence reading,parsing & storing is done.
  Serial.println("GeoSeq.txt is closed!");
  MyFile = SD.open("Image.txt");
  Serial.println("Image.txt is open!");
}
void geoSeq_parseData(char *str_geoSeq)
{
  char *strtokIndx;
 
  strtokIndx = strtok(str_geoSeq, ",");
  indexNum_geoSeq[LatLon_ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LatitudeVal[LatLon_ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LongitudeVal[LatLon_ndx] = atoi(strtokIndx);

  Serial.println("***** Parsed Data ******");
  Serial.println(indexNum_geoSeq[LatLon_ndx]);
  Serial.print("Latitude: ");
  Serial.println(LatitudeVal[LatLon_ndx]);
  Serial.print("Longitude: ");
  Serial.println(LongitudeVal[LatLon_ndx]);
  LatLon_ndx++;     
}
void Ledseq_Read() 
{
  while (MyFile.available() && Ledseq_readflag == true && arcStart == true && geoSeq_readflag == false) {
      _buffer = MyFile.readStringUntil('\r');
      _buffer.toCharArray(token,20);
      Ledseq_parseData(token);
    }
}
void Ledseq_parseData(char* workVar)
{                                      //indexes of the EncoderIndex,Latitude & Longitude arrays
  if( Ledseq_readflag == true){
    char * strtokIndx;                                  // this is used by strtok() as an index
    strtokIndx = strtok(workVar, ",");
    indexNum_Ledseq = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    ledNum = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    brightness[0] = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    brightness[1] = atoi(strtokIndx);

    strtokIndx = strtok(NULL, "\r");
    brightness[2] = atoi(strtokIndx);

    Serial.println("***** Parsed Data ******");
    Serial.println(indexNum_Ledseq);
    Serial.print("LED Number: ");
    Serial.println(ledNum);
    Serial.print("R: ");
    Serial.println(brightness[0]);
    Serial.print("G: ");
    Serial.println(brightness[1]);
    Serial.print("B: ");
    Serial.println(brightness[2]);
    ndx_Ledseq++;
    Ledseq_readflag = false;
    LedExecution();
  }
}
void Led_Initialization()
{
  FastLED.addLeds<SK9822, DATA_PIN, CLOCK_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
}
void LedExecution()
{
   // Turn the LED on, then pause
 if(Ledseq_readflag == false && indexNum_Ledseq == (enc/2) )
 {
  leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
  FastLED.show();
  Ledseq_readflag = true;
 }
 else
 {
  leds[ledNum] = CRGB::Black;
  FastLED.show();
  Ledseq_readflag = false;
  stopSys = true;
  MyFile.close();
 }
}
void ArcTimer_setup()
{
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 30;// = (16*10^6) / (1024*400) - 1 = 38
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}
ISR(TIMER1_COMPA_vect)
{ 
  if( geoSeq_readflag == false && stopSys == false )   //Checking if geoSeq reading,parsing,storing is done
  {
    static bool stepArc = false;  //It will step arc motor after each HIGH/LOW is completed
    stepArc = !stepArc;
    //digitalWrite(STEP_ArcRotate,stepArc);
    enc++; // Working as encoder here : (enc/2 = enc position)
    arcStart = true; // To indicate LED's and Lat/Lon motors to start execution
    Ledseq_readflag = true; //Ledseq_Read will work after this flag.
    Serial.println("ARC MOVING");
  }
}
