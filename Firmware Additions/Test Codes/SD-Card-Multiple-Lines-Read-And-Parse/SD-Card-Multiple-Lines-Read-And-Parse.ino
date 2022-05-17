
#include <SD.h>
#include <SPI.h>
void SD_Card_read(); //function to read the text file from SD-card
void parseData();    //function to parse the data

const int chipSelect = 10;
char value[250],temp[25];    //token values
static int ndx = 0;       //line index

// Variables to store parse data:
static char valstring[12];
static float LatitudeVal, LongitudeVal;

File MyFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);

  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset button on the board and reopen this Serial Monitor after fixing your issue!");
    while (true);
  }

  Serial.println("initialization done.");


  // re-open the file for reading:
  MyFile = SD.open("Hello.txt");
  if (MyFile) {
    Serial.println("Hello.txt is avaiable");
    while (MyFile.available()) {
      Serial.write(MyFile.read());
    }
    // close the file:
    MyFile.close();
  }
  else {
    // if the file didn't open, print an error:
    Serial.println("error opening Hello.txt");
  }
  Serial.println();
  MyFile = SD.open("Hello.txt");
  SD_Card_read();
 // parseData();
}

void loop() {
  // nothing happens after setup

}
void SD_Card_read() {

  static int i = 0;                                                         //buffer(value) index
  static int character_count =0 ,previous_count = 0;
  
  while (MyFile.available()) {
      // Serial.println(MyFile.available());                                // To cross-verify my loop
      
    //if(MyFile.available() != 2){
     if (MyFile.peek() != '\r' || MyFile.peek() == 'V') {  
          value[i] = MyFile.read();
          i++;
          character_count++;
    }
    else {
      MyFile.read();
      ++ndx;
      //Serial.print("********Charater Count: ");
      //Serial.println(character_count);
      //substr(previous_line, character)
      //->parseData
      int k = 0;
      
      for(int j = previous_count; j <= character_count ; j++ ){
          if( value[j] != '\n'){
                temp[k] = value[j];
                k++;
              }
          if(j == character_count){ 
           // temp[j+1] = '\0';
            k=0;
          }
        }
       Serial.print("temp[j] = ");
       Serial.println(temp);
       
      parseData(temp);                                                      //Sending a single line to parse

      previous_count = character_count;

      //Serial.print("MCU reads the Line number#");
      //Serial.print(ndx);
      //Serial.println(" from the SD-card");
    }
   // Serial.println("*********Value array**********");
   // Serial.println(value);
  }
}
void parseData(char* workVar)
{ // split the data into its parts

  char * strtokIndx;                                // this is used by strtok() as an index

  strtokIndx = strtok(workVar, ",");     
  strcpy(valstring, strtokIndx); 

  strtokIndx = strtok(NULL, ","); 
  LatitudeVal = atof(strtokIndx);     

  strtokIndx = strtok(NULL, ",");
  LongitudeVal = atof(strtokIndx);     

  Serial.println("***** Parsed Data ******");
  Serial.println(valstring);
  Serial.print("Latitude: ");
  Serial.println(LatitudeVal);
  Serial.print("Longitude: ");
  Serial.println(LongitudeVal);
}
