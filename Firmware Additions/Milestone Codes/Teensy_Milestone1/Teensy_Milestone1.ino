#include <AccelStepper.h>
#include <SD.h>
#include <SPI.h>

//Functions:
void ArcMotor_Initialization();                               //Arc Motor Initialization
void ArcStep();                                               // Arc Motor Timer  - Continuous Spining function
void LatMotor_Initialization();                               //Latitude Motor Initialization
void LonMotor_Initialization();                               //Longitude Motor Initialization
void SD_Card_Initialization();                                //SD Card Initialization
void SD_Card_read();                                          //function to read the text file from SD-card
void SDCard_LineExtraction();                                 // Extracting each line read by SD-card
void parseData(char* workVar, int* ndx);                      //function to parse the data

// Create an IntervalTimer object 
IntervalTimer ArcTimer;

//Motors Pinouts & Configuration:
#define motorInterfaceType          1
//Motor1 : Arc Motor
#define Dir_ArcRotate               6 //6
#define STEP_ArcRotate              7 //7
#define ArcRotate_ENAPIN            20 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400

//Motor2 : Latitude
#define Dir_Latitude            8
#define STEP_Latitude           9
#define Latitude_ENAPIN         22
             
//Motor3 : Longitude
#define Dir_Longitude           10
#define STEP_Longitude          11
#define Longitude_ENAPIN        21

#define RPM 50
#define PULSE 200
#define ClOCKWISE 1
#define OTHERWISE 0

// Creating Motors objects with defined configuration:
AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);
AccelStepper LatMotor = AccelStepper(motorInterfaceType, STEP_Latitude, Dir_Latitude);
AccelStepper LonMotor = AccelStepper(motorInterfaceType, STEP_Longitude, Dir_Longitude);

//Ticker to service Latitude/Longitude Motors:
unsigned long previousTimeLatLon; 
unsigned long timeIntervalLatLon = 3;

//SD Card Pinouts & Configuration
const int chipSelect = BUILTIN_SDCARD;                      //SD-Card default pin
char value[3500] = {}, temp[100] = {};                       //Arrays to hold the data MCU read from the text file

int indexNum[300], LatitudeVal[300], LongitudeVal[300];     //Variables to hold the parsed data:
int ndx = 0;                                                //indexes of the EncoderIndex,Latitude & Longitude arrays
File MyFile;

void setup() {
  Serial.begin(9600);
  SD_Card_Initialization();
  SD_Card_read();
  ArcMotor_Initialization();
  ArcTimer.begin(ArcStep, 1250);  // Run ArcMotor every 0.00125 seconds ( 2500 / 2  = 1250 ) , 2500 - > for 1 complete step including HIGH & LOW at step-pin
  LatMotor_Initialization();
  LonMotor_Initialization();
}

void loop() 
{  
  LatLonTicker(&ndx);
}
void ArcMotor_Initialization() {
  pinMode(ArcRotate_ENAPIN,OUTPUT);
  ArcMotor.setEnablePin(ArcRotate_ENAPIN);
  ArcMotor.setPinsInverted(false, false, true);
  ArcMotor.enableOutputs();
  ArcMotor.setMaxSpeed(ArcRotateMaxSpeed);
  ArcMotor.setAcceleration(ArcRotateAcceleration);
  ArcMotor.setSpeed(ArcRotateSpeed);
}

void ArcStep(){
  static bool flag = false;
  flag = !flag;
  digitalWrite(STEP_ArcRotate, flag);
}
void LatMotor_Initialization() {
  LatMotor.setEnablePin(Latitude_ENAPIN);
  LatMotor.setPinsInverted(false, false, true);
  LatMotor.enableOutputs();
  LatMotor.setMaxSpeed(1000);
  LatMotor.setAcceleration(50);
  LatMotor.setSpeed(200);
}
void LonMotor_Initialization() {
  LonMotor.setEnablePin(Longitude_ENAPIN);
  LonMotor.setPinsInverted(false, false, true);
  LonMotor.enableOutputs();
  LonMotor.setMaxSpeed(1000);
  LonMotor.setAcceleration(50);
  LonMotor.setSpeed(200);
  previousTimeLatLon = millis();
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

      LatMotor.moveTo(LatitudeVal[action_index]);
      LonMotor.moveTo(LongitudeVal[action_index]);
      LatMotor.setSpeed(200);
      LonMotor.setSpeed(200);
      if (LonMotor.distanceToGo() != 0 && action_index != *ndx)
      {
          LonMotor.runSpeed();
        if(LatMotor.distanceToGo() != 0)
        {
          //Serial.println("working");
          LatMotor.runSpeed();
        }
      }
      else
      {
        LonMotor.setCurrentPosition(0);
        Serial.println("****Latitude and Longitude Motors Moves****");
        Serial.print("action index : ");
        Serial.print(action_index);
        Serial.print(" , Latitude Moves: ");
        Serial.println(LatitudeVal[action_index]);
        Serial.print(" , Longitude Moves: ");
        Serial.println(LongitudeVal[action_index]);
        action_index++;
        if(action_index >= *ndx){
          LatMotor.disableOutputs();
          LonMotor.disableOutputs();
          }
      }
    }
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
