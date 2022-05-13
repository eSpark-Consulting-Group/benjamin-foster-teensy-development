/*
Wiring:
VCC 9-42 – VDC
GND - Power supply ground
ENA- - Teensy GND
ENA+ - Teensy Pin 7
DIR- - Teensy GND
DIR+ - Teensy Pin 8
PUL- - Teensy GND
PUL+ - Teensy Pin 9
A-, A+ - Coil 1 stepper motor
B-, B+ - Coil 2 stepper motor

The pinouts provided: 

#define Dir_ArcRotate           6
#define STEP_ArcRotate          7
#define Dir_Latitude            8
#define STEP_Latitude           9
#define Dir_Longitude           10
#define STEP_Longitude          11
#define Dir_AspectRatio         12
#define STEP_AspectRatio        28
#define Dir_Altitude            24          
#define STEP_Altitude           25
#define ArcRotate_ENAPIN        20             // LED Arc Motor Lock
#define Longitude_ENAPIN        21             // Longitude Motor Lock
#define Latitude_ENAPIN         22             // Latitude Motor Lock
#define AspectRatio_ENAPIN      23             // AspectRatio Motor Lock
#define Altitude_ENAPIN         29             // Altitude Motor Lock
*/

#include<mwc_stepper.h>

//Motor1 : ARC
#define Dir_ArcRotate           6
#define STEP_ArcRotate          7

//Motor2 : Latitude
#define Dir_Latitude            8
#define STEP_Latitude           9

//Motor3 : Longitude
#define Dir_Longitude           10
#define STEP_Longitude          11

//Motor4 : AspectRatio (Camera Rotation)
#define Dir_AspectRatio         12
#define STEP_AspectRatio        28

//Motor5 : Altitude (Camera Forward/Backwar)
#define Dir_Altitude            24          
#define STEP_Altitude           25

//Motor Enable Pins:
#define ArcRotate_ENAPIN        20             // LED Arc Motor Lock
#define Longitude_ENAPIN        21             // Longitude Motor Lock
#define Latitude_ENAPIN         22             // Latitude Motor Lock
#define AspectRatio_ENAPIN      23             // AspectRatio Motor Lock
#define Altitude_ENAPIN         29             // Altitude Motor Lock

// Limiting Switches:
#define Latitude_South          14             // Latitude Min Stop
#define Latitude_North          15             // Latitude Max Stop
#define AspectRatio_Landscape   16             // AspectRatio Landscape stop
#define AspectRatio_Portrait    17             // AspectRatio Portrait stop
#define Altitude_Min            18             // MIN distance to object stop 0mm
#define Altitude_Max            19             // MAX distance to object stop 200mm

#define RPM 50
#define RPM1 50

#define PULSE 200

#define ClOCKWISE 1
#define OTHERWISE 0


//3-Stepper Motors Configuration:
MWCSTEPPER nema23_1(ArcRotate_ENAPIN, Dir_ArcRotate, STEP_ArcRotate);         //Arc Motor
MWCSTEPPER nema23_2(Latitude_ENAPIN, Dir_Latitude, STEP_Latitude );           //Latitude Motor
MWCSTEPPER nema23_3(Longitude_ENAPIN,Dir_Longitude,STEP_Longitude);           //Longitude Motor
MWCSTEPPER nema23_4(AspectRatio_ENAPIN, Dir_AspectRatio, STEP_AspectRatio);   //AspectRatio Motor
MWCSTEPPER nema23_5(Altitude_ENAPIN,Dir_Altitude,STEP_Altitude);              //Altitude Motor

// Functions:
void disabling_Motors();
void Arc_Motor();
void Latitude_Motor();
void Longitude_Motor();
void AspectRatio_Motor();
void Altitude_Motor();

void setup() {

  nema23_1.init();
  nema23_2.init();
  nema23_3.init();
  nema23_4.init();
  nema23_5.init();
  pinMode(Latitude_South,INPUT_PULLUP);
  pinMode(Latitude_North,INPUT_PULLUP);
  pinMode(AspectRatio_Landscape,INPUT_PULLUP);
  pinMode(AspectRatio_Portrait,INPUT_PULLUP);
  pinMode(Altitude_Min,INPUT_PULLUP);
  pinMode(Altitude_Max,INPUT_PULLUP);

  disabling_Motors(); 
}

void loop() { 
    Arc_Motor();
    Latitude_Motor();
    Longitude_Motor();
    AspectRatio_Motor();
    Altitude_Motor();
    disabling_Motors(); 
}
void disabling_Motors(){
    nema23_1.active(true);
    nema23_2.active(true);
    nema23_3.active(true);
    nema23_4.active(true);
  }
void Arc_Motor(){
  nema23_1.set(ClOCKWISE, RPM, PULSE);
  nema23_1.active(false);
  for (size_t i = 0; i < 200; i++)  //*********** changes *********** 
  {
      nema23_1.run();
  }
}
void Latitude_Motor(){
  nema23_2.set(ClOCKWISE, RPM, PULSE);
  nema23_2.active(false);
  while( (Latitude_South == 1) && (Latitude_North == 1) ){ nema23_2.run(); }
  nema23_2.active(true);
}
void Longitude_Motor(){
  nema23_3.set(ClOCKWISE, RPM, PULSE);
  nema23_3.active(false);
  for (size_t i = 0; i < 200; i++)  //*********** changes *********** 
  {
      nema23_3.run();
  }
}
void AspectRatio_Motor(){
  nema23_4.set(ClOCKWISE, RPM, PULSE);
  nema23_4.active(false);
  while( (AspectRatio_Landscape == 1) && (AspectRatio_Portrait == 1) ){ nema23_4.run(); }
  nema23_4.active(true);
}
void Altitude_Motor(){
  nema23_5.set(ClOCKWISE, RPM, PULSE);
  nema23_5.active(false);
  while( (Altitude_Min == 1) && (Altitude_Max == 1) ){ nema23_5.run(); }
  nema23_5.active(true);
}
