#include <avr/io.h>
#include <avr/interrupt.h>
#include <AccelStepper.h>
#include <FastLED.h>
#include <SPI.h>
#include <SD.h>

//Flags:
bool ledExec_Flag = true;
bool Ledseq_readflag = true;
int arcstep = 1;
bool arcFlag = false;
//bool arcledflag

//SD Card Pinouts & Configuration
const int chipSelect = 10; //BUILTIN_SDCARD;  
char token[30];
String _buffer;
File MyFile;

//GeoSequence Variables ( SD-card ):
int indexNum_geoSeq[100], LatitudeVal[100], LongitudeVal[100];
int LatLon_ndx = 0; // To keep track of lat/lon values index

//LedSequence Values:
int ndx_Ledseq = 0 ;
int indexNum_Ledseq,ledNum,brightness[3];

//LED Variables:
#define NUM_LEDS 50
#define DATA_PIN 3
//#define CLOCK_PIN 13
#define FRAMES_PER_SECOND  120
CRGB leds[NUM_LEDS];

//Motors Pinouts and Configurations:
#define motorInterfaceType          1
#define Dir_ArcRotate               4 //6
#define STEP_ArcRotate              5 //7
#define ArcRotate_ENAPIN            6 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400

AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  cli();
  SD_Card_Initialization();
  MyFile = SD.open("LedSeq.txt");
  Led_Initialization();
  ArcMotor_Initialization();
  ArcTimer_setup();
  //EncoderInit();
  sei();
}

void loop() {
    Ledseq_Read();
}
/*void EncoderInit()
{
  pinMode(2,INPUT);
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE);
}
void updateEncoder()
{
  
}*/
void Led_Initialization()
{
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS); 
  FastLED.setMaxRefreshRate(400,true);
  Serial.println("RGB Led Initialized");
}
void LedExecution()
{
  leds[ledNum] = CRGB(brightness[0], brightness[1], brightness[2]);
  if(ledNum == 50 && ledExec_Flag == true &&  arcFlag == false ) { 
    FastLED.show(); 
    Serial.println("EXECUTED");
    arcFlag = true;
    }
}
void SD_Card_Initialization() 
{
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
void Ledseq_Read() 
{
  Serial.println(arcstep);
  Serial.println(indexNum_Ledseq);
  if(arcstep >= 400){
    MyFile.close();
    MyFile = SD.open("LedSeq.txt");
    arcstep = 0;
  }
  while ( ( MyFile.available()) && (Ledseq_readflag == true) && (arcstep<=400)  ) 
  {
      //Serial.println("LedSeq Reading 50 Lines.");
      for( int lines = 1; lines <= 50; lines++)
      {
        _buffer = MyFile.readStringUntil('\r');
        _buffer.toCharArray(token,20);
        /*if( lines == 50 )
        {
          Ledseq_readflag = false;
          ledExec_Flag = true;
        }*/
        Ledseq_parseData(token);
      }
      
      //Ledseq_readflag = false; //140 leds are actuated, wait for encoder interrupt
  }
}
void Ledseq_parseData(char* workVar)
{                                      //indexes of the EncoderIndex,Latitude & Longitude arrays
  //if( Ledseq_readflag == true){
    //Ledseq_readflag = false;
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

    /*Serial.println("***** Parsed Data ******");
    Serial.println(indexNum_Ledseq);
    Serial.print("LED Number: ");
    Serial.println(ledNum);
    Serial.print("R: ");
    Serial.print(brightness[0]);
    Serial.print(",  G: ");
    Serial.print(brightness[1]);
    Serial.print(",  B: ");
    Serial.println(brightness[2]);*/
    ndx_Ledseq++;
    //Ledseq_readflag = false;
    LedExecution();
  //}
}
void ArcTimer_setup()
{
  //cli();
  TCCR4A = 0;// set entire TCCR1A register to 0
  TCCR4B = 0;// same for TCCR1B
  TCNT4  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR4A = 449; //457(working);// = (16*10^6) / (1024*1) -1 = 19.5
  // turn on CTC mode
  TCCR4B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR4B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK4 |= (1 << OCIE4A);
  //sei();
}
ISR(TIMER4_COMPA_vect)
{ 
  if(arcstep == 400){
    arcstep++;
  Serial.print("One Rev Complete : arcstep = ");
  Serial.println(arcstep);
  }
  else if ( arcstep < 400 ){
   if(arcFlag == true) {
    arcstep++;
    static bool flag = true;
    digitalWrite(STEP_ArcRotate,flag);
    flag = !flag;
    digitalWrite(STEP_ArcRotate,flag);
    flag = !flag;
    arcFlag = false;
   }
  }
  else{ 
   // arcstep = 0;
    }

}
void ArcMotor_Initialization() {
  pinMode(ArcRotate_ENAPIN,OUTPUT);
  pinMode(STEP_ArcRotate,OUTPUT);
  ArcMotor.setEnablePin(ArcRotate_ENAPIN);
  ArcMotor.setPinsInverted(false, false, true);
  ArcMotor.enableOutputs();
  ArcMotor.setMaxSpeed(ArcRotateMaxSpeed);
  ArcMotor.setAcceleration(ArcRotateAcceleration);
  ArcMotor.setSpeed(ArcRotateSpeed);
}
