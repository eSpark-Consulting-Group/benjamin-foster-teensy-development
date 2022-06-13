#include <SD.h>
#include <SPI.h>

int TrigOn,TrigOff;       //Variables to hold Camera Triggering On/Off values.
uint16_t num_LedSeqFiles; //variable to hold the number of LedSeq Text files.
String str;               //String variable to use as an "LedSeqn.txt", where 'n' is the number of LedSeq text file.
bool revFlag = false;

//SD Card Pinouts & Configuration
const int chipSelect = 10; //BUILTIN_SDCARD;  
char token[30];
String _buffer;
File MyFile;

void setup() 
{
  Serial.begin(9600);
  SD_Card_Initialization();
  init_configuration();
}

void loop() 
{
  LedSeqFiles();
  revFlag = !revFlag;
}
void init_configuration()
{
  MyFile = SD.open("Config.txt");
  
  _buffer = MyFile.readStringUntil('\r');
  _buffer.toCharArray(token,20);
  
  char* strtokIndx;                                  
  strtokIndx = strtok(token, ",");
  num_LedSeqFiles = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  TrigOn = atoi(strtokIndx);

  strtokIndx = strtok(NULL, ",");
  TrigOff = atoi(strtokIndx);
  
  MyFile.close();

  Serial.print("No of LedSeq Text Files is ");
  Serial.println(num_LedSeqFiles);
  Serial.print("Camera Trigger On : ");
  Serial.println(TrigOn);
  Serial.print("Camera Trigger Off : ");
  Serial.println(TrigOff);
}
void LedSeqFiles()
{
  static uint16_t n = 1; 
  if( revFlag == true )
  {
    MyFile.close();
    if( n <= num_LedSeqFiles )
    {
      str = "LedSeq" + String(n) + ".txt";
      Serial.println(str);
    
      MyFile = SD.open(str);
      Serial.println(str + " is open!");
    
      if(n == num_LedSeqFiles){ Serial.println("Objective Achieved!"); } //n = 0;
      n++;
    }
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
