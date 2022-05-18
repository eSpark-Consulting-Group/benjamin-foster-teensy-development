
#include <SD.h>
#include <SPI.h>
void SD_Card_read();                //function to read the text file from SD-card
void parseData();                   //function to parse the data

const int chipSelect = BUILTIN_SDCARD;          //SD-Card default pin

char value[25000]={},temp[25]={};           //Arrays to hold the data MCU read from the text file
static int ndx = 0;                        //To verify the line number MCU is reading


//Variables to hold the parsed data:         
int indexNum;             
float LatitudeVal, LongitudeVal;

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


  // Reading and Printing the "Hello.txt" file on the Serial Monitor to verify its content: 
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

  // Opening the "Hello.txt" and then by calling SD_Card_read() function, we're performing the reading & parsing of our data
  MyFile = SD.open("Hello.txt");
  SD_Card_read();
}

void loop() {

}
void SD_Card_read() {

  static int i = 0;                                                         //buffer(value) index
  static int character_count =0 ,previous_count = 0;                        //Variables to hold the number of characters read by MCU
  int k;
  
  while (MyFile.available()) {
      
     if (MyFile.peek() != '\r' /*|| MyFile.peek() == 'V'*/) {  
          value[i] = MyFile.read();
          i++;
          character_count++;
    }
    else {
      MyFile.read();
      ++ndx;
      k = 0;
      
      for(int j = previous_count; j <= character_count ; j++ ){
          if( value[j] != '\n'){
                temp[k] = value[j];
                k++;
              }
          if(j == character_count){ 
            k=0;
          }
        }
        Serial.print("temp[j] = ");                                          //Printing the each line data extracted from the text file
        Serial.println(temp);

        Serial.print("MCU reads the Line number#");
        Serial.print(ndx);
        Serial.println(" from the SD-card");
       
       parseData(temp);                                                      //Sending a single line to parse

      previous_count = character_count;
    }
  }
}
void parseData(char* workVar)
{ // split the data into its parts

  char * strtokIndx;                                // this is used by strtok() as an index

  strtokIndx = strtok(workVar, ",");  
  indexNum = atoi(strtokIndx);   

  strtokIndx = strtok(NULL, ","); 
  LatitudeVal = atof(strtokIndx);     

  strtokIndx = strtok(NULL, ",");
  LongitudeVal = atof(strtokIndx);     

  Serial.println("***** Parsed Data ******");
  Serial.println(indexNum);
  Serial.print("Latitude: ");
  Serial.println(LatitudeVal);
  Serial.print("Longitude: ");
  Serial.println(LongitudeVal);
}
