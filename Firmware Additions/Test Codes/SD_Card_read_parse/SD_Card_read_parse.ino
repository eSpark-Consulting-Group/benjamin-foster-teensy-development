
#include <SD.h>
#include <SPI.h>
void SD_Card_read(); //function to read the text file from SD-card 
void parseData();    //function to parse the data 
 
const int chipSelect = 10;
char* value;    //token values
String bufferVal[100];
static int ndx = 0;       //line index

// Variables to store parse data:
static char valstring[12];
static float LatitudeVal,LongitudeVal;

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

  MyFile = SD.open("Hello.txt");
  SD_Card_read();
  parseData();
}

void loop() {
  // nothing happens after setup
  
}
void SD_Card_read(){
  
   static int numString = 0, index = 0;                //buffer(value) number
   
    
    while(MyFile.available()){
    
      if(MyFile.peek() != '\0'){
            bufferVal[numString] = MyFile.readStringUntil('\n');
            index++;
      }
      else{
            strcpy(value,bufferVal[numString]);
            ++numString; 
            Serial.print("MCU reads the Line number#");
            Serial.print(ndx);
            Serial.print(" from the SD-card");
        }
    }
}   
void parseData(void)
 {      // split the data into its parts
  
  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(value, ",");     // get the first part - the string
  strcpy(valstring, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  LatitudeVal = atof(strtokIndx);     // convert this part to an integer

  strtokIndx = strtok(NULL, ",");
  LongitudeVal = atof(strtokIndx);     // convert this part to a float

  Serial.print(valstring);
  Serial.print(" Latitude: ");
  Serial.print(LatitudeVal);
  Serial.print(" Longitude: ");
  Serial.println(LongitudeVal);
}
