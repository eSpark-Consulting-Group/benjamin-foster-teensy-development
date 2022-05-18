#include <SD.h>
#include <SPI.h>
#include <AccelStepper.h>

//Functions:
void ArcMotor_Initialization();     //Motor Initialization
void ArcTicker();                   //Arc Motor Continuous Motion
void SD_Card_Initialization();      //SD Card Initialization
void SD_Card_read();                //function to read the text file from SD-card
void SDCard_LineExtraction();       // Extracting each line read by SD-card
void parseData();                   //function to parse the data

//Motors Pinouts & Configuration:
#define motorInterfaceType          1
#define Dir_ArcRotate               2 //6
#define STEP_ArcRotate              3 //7
#define ArcRotate_ENAPIN            4 //20
#define ArcRotateMaxSpeed           400   
#define ArcRotateAcceleration       300   
#define ArcRotateSpeed              400    
 
AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);
unsigned long previousTimeARC = millis();
long timeIntervalARC = 3;

//SD Card Pinouts & Configuration
const int chipSelect = 10;//BUILTIN_SDCARD            //SD-Card default pin
char value[100]={},temp[15]={};                       //Arrays to hold the data MCU read from the text file


//Variables to hold the parsed data:         
int indexNum[4],LatitudeVal[4], LongitudeVal[4];
File MyFile;

void setup() {
  Serial.begin(9600);
  ArcMotor_Initialization();
  SD_Card_Initialization();
  SD_Card_read();
}

void loop() {
  ArcTicker();
}
void ArcMotor_Initialization(){
  ArcMotor.setEnablePin(ArcRotate_ENAPIN);
  ArcMotor.setPinsInverted(false, false, true);
  ArcMotor.enableOutputs();
  ArcMotor.setMaxSpeed(ArcRotateMaxSpeed);
  ArcMotor.setAcceleration(ArcRotateAcceleration);
  ArcMotor.setSpeed(ArcRotateSpeed);  
}
void ArcTicker(){
  unsigned long currentTime = millis();
  if(currentTime - previousTimeARC > timeIntervalARC) {
    previousTimeARC = currentTime;
     ArcMotor.runSpeed();
  } 
}
void SD_Card_Initialization(){
  while (!Serial);
  pinMode(10/*BUILTIN_SDCARD*/, OUTPUT);
  digitalWrite(10/*BUILTIN_SDCARD*/, HIGH);

  Serial.print("Initializing SD card...");

  if (!SD.begin(10/*BUILTIN_SDCARD*/)) {
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
void SD_Card_read(){
  
  static int i = 0;                                                         //buffer(value) index
  static int character_count =0 ,previous_count = 0;                        //Variables to hold the number of characters read by MCU
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
      SDCard_LineExtraction(&character_count,&previous_count,&k);
      Serial.print("temp[j] = ");                                          //Printing the each line data extracted from the text file
      Serial.println(temp);
       
      parseData(temp);                                                      //Sending a single line to parse
      previous_count = character_count;
    }
  }
}
void SDCard_LineExtraction(int* character_count,int* previous_count, int* k){
      for(int j = *previous_count; j <= *character_count ; j++ ){
          if( value[j] != '\n'){
                temp[*k] = value[j];
                (*k)++;
              }
          if(j == *character_count){ 
            *k=0;
          }
        }
}
void parseData(char* workVar)
{ // split the data into its parts
  static int ndx = 0;                               //indexes of the EncoderIndex,Latitude & Longitude arrays
  char * strtokIndx;                                // this is used by strtok() as an index

  strtokIndx = strtok(workVar, ",");  
  indexNum[ndx] = atoi(strtokIndx);   

  strtokIndx = strtok(NULL, ","); 
  LatitudeVal[ndx] = atoi(strtokIndx);     

  strtokIndx = strtok(NULL, ",");
  LongitudeVal[ndx] = atoi(strtokIndx);     

  Serial.println("***** Parsed Data ******");
  Serial.println(indexNum[ndx]);
  Serial.print("Latitude: ");
  Serial.println(LatitudeVal[ndx]);
  Serial.print("Longitude: ");
  Serial.println(LongitudeVal[ndx]);
}
