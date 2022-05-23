#include <avr/io.h>
#include <avr/interrupt.h>
#include <AccelStepper.h>
#include <SD.h>
#include <SPI.h>

//Functions:
void ArcMotor_Initialization();                               //Arc Motor Initialization
void ArcTimer_setup();                                        // Arc Motor Timer
void LatMotor_Initialization();                               //Latitude Motor Initialization
void SD_Card_Initialization();                                //SD Card Initialization
void SD_Card_read();                                          //function to read the text file from SD-card
void SDCard_LineExtraction();                                 // Extracting each line read by SD-card
void parseData(char* workVar, int* ndx);                      //function to parse the data


//Motors Pinouts and Configurations:
#define motorInterfaceType          1
#define Dir_ArcRotate               5 //6
#define STEP_ArcRotate              6 //7
#define ArcRotate_ENAPIN            7 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400
#define Dir_Latitude                2
#define STEP_Latitude               3
#define Latitude_ENAPIN             4

AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);
AccelStepper LatMotor = AccelStepper(motorInterfaceType, STEP_Latitude, Dir_Latitude);

unsigned long previousTimeLatLon = millis();
long timeIntervalLatLon = 3;

//SD Card Pinouts & Configuration
//const int chipSelect = BUILTIN_SDCARD;                //SD-Card default pin
char value[120] = {}, temp[14] = {};                  //Arrays to hold the data MCU read from the text file


int indexNum[10], LatitudeVal[10], LongitudeVal[10];     //Variables to hold the parsed data:
int ndx = 0;                                          //indexes of the EncoderIndex,Latitude & Longitude arrays
File MyFile;

void setup()
{
  cli();//stop interrupts
  Serial.begin(9600);
  SD_Card_Initialization();
  SD_Card_read();
  ArcMotor_Initialization();
  LatMotor_Initialization();
  ArcTimer_setup();
  sei(); //allow interrupts
}
void loop()
 {  
    LatLonTicker(&ndx);
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
void LatMotor_Initialization() {
  LatMotor.setEnablePin(Latitude_ENAPIN);
  LatMotor.setPinsInverted(false, false, true);
  LatMotor.enableOutputs();
  LatMotor.setMaxSpeed(1000);
  LatMotor.setAcceleration(50);
  LatMotor.setSpeed(200);
}
void LatLonTicker(int* ndx) 
{
  static int action_index = 0;
  unsigned long currentTimeLatLon = millis();

  // checking the ndx of the txt file
  if (action_index < *ndx) {
    if (currentTimeLatLon - previousTimeLatLon > timeIntervalLatLon)
    {
      currentTimeLatLon = previousTimeLatLon;
      //Serial.println(LatitudeVal[action_index]);
      LatMotor.moveTo(LatitudeVal[action_index]);
      LatMotor.setSpeed(200);
      if (LatMotor.distanceToGo() != 0)
      {
        Serial.println("working");
        LatMotor.runSpeed();
      }
      else
      {
        LatMotor.setCurrentPosition(0);
        Serial.println("****Latitude and Longitude Motors Moves****");
        Serial.print("action index : ");
        Serial.print(action_index);
        Serial.print(" , Latitude Moves: ");
        Serial.println(LatitudeVal[action_index]);
        action_index++;
      }
    }
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
  static bool flag = false;
  flag = !flag;
  digitalWrite(STEP_ArcRotate,flag);
  //ArcMotor.runSpeed();
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
}
void SD_Card_read() {

  static int i = 0;                                                         //buffer(value) index
  static int character_count = 0 , previous_count = 0;                      //Variables to hold the number of characters read by MCU
  static int k;
  MyFile = SD.open("Hello.txt");
  while (MyFile.available()) {
    if (MyFile.peek() != '\r' /*|| MyFile.peek() == 'V'*/) {
      value[i] = MyFile.read();
      i++;
      character_count++;
    }
    else {
      MyFile.read();
      k = 0;
      SDCard_LineExtraction(&character_count, &previous_count, &k);
      Serial.print("temp[j] = ");                                          //Printing the each line data extracted from the text file
      Serial.println(temp);

      parseData(temp, &ndx);                                                     //Sending a single line to parse
      previous_count = character_count;
    }
  }
}
void SDCard_LineExtraction(int* character_count, int* previous_count, int* k) {
  for (int j = *previous_count; j <= *character_count ; j++ ) {
    if ( value[j] != '\n') {
      temp[*k] = value[j];
      (*k)++;
    }
    if (j == *character_count) {
      *k = 0;
    }
  }
}
void parseData(char* workVar, int* ndx)
{ // split the data into its parts
  //int ndx = 0;                                       //indexes of the EncoderIndex,Latitude & Longitude arrays
  char * strtokIndx;                                  // this is used by strtok() as an index

  strtokIndx = strtok(workVar, ",");
  indexNum[*ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LatitudeVal[*ndx] = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  LongitudeVal[*ndx] = atoi(strtokIndx);

  Serial.println("***** Parsed Data ******");
  Serial.println(indexNum[*ndx]);
  Serial.print("Latitude: ");
  Serial.println(LatitudeVal[*ndx]);
  Serial.print("Longitude: ");
  Serial.println(LongitudeVal[*ndx]);
  (*ndx)++;
}
