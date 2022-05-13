/*

The pinouts provided: 

Motors:
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

Limiting Switches:
#define Latitude_South          14             // Latitude Min Stop
#define Latitude_North          15             // Latitude Max Stop
#define AspectRatio_Landscape   16             // AspectRatio Landscape stop
#define AspectRatio_Portrait    17             // AspectRatio Portrait stop
#define Altitude_Min            18             // MIN distance to object stop 0mm
#define Altitude_Max            19             // MAX distance to object stop 200mm

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
MWCSTEPPER Arc_Motor(ArcRotate_ENAPIN, Dir_ArcRotate, STEP_ArcRotate);                  //Arc Motor
MWCSTEPPER Latitude_Motor(Latitude_ENAPIN, Dir_Latitude, STEP_Latitude );               //Latitude Motor
MWCSTEPPER Longitude_Motor(Longitude_ENAPIN,Dir_Longitude,STEP_Longitude);              //Longitude Motor
MWCSTEPPER AspectRatio_Motor(AspectRatio_ENAPIN, Dir_AspectRatio, STEP_AspectRatio);    //AspectRatio Motor
MWCSTEPPER Altitude_Motor(Altitude_ENAPIN,Dir_Altitude,STEP_Altitude);                  //Altitude Motor

// Functions:
void disablingMotors();
void arcMovement();
void latitudeMovement();
void objectRotation();
void cameraAspectRatio();
void altitudeMovement();

void setup() {

  Arc_Motor.init();
  Latitude_Motor.init();
  Longitude_Motor.init();
  AspectRatio_Motor.init();
  Altitude_Motor.init();
  pinMode(Latitude_South,INPUT_PULLDOWN);
  pinMode(Latitude_North,INPUT_PULLDOWN);
  pinMode(AspectRatio_Landscape,INPUT_PULLDOWN);
  pinMode(AspectRatio_Portrait,INPUT_PULLDOWN);
  pinMode(Altitude_Min,INPUT_PULLDOWN);
  pinMode(Altitude_Max,INPUT_PULLDOWN);

  disablingMotors(); 
}

void loop() { 
    arcMovement();
    latitudeMovement();
    objectRotation();
    cameraAspectRatio();
    altitudeMovement();
    disablingMotors(); 
}
void disablingMotors(){
    Arc_Motor.active(true);
    Latitude_Motor.active(true);
    Longitude_Motor.active(true);
    AspectRatio_Motor.active(true);
  }
void arcMovement(){
  Arc_Motor.set(ClOCKWISE, RPM, PULSE);
  Arc_Motor.active(false);
  for (size_t i = 0; i < 200; i++)  //*********** changes *********** 
  {
      Arc_Motor.run();
  }
}
void latitudeMovement(){
  Latitude_Motor.set(ClOCKWISE, RPM, PULSE);
  Latitude_Motor.active(false);
  while( (digitalRead(Latitude_South == 0)) && (digitalRead(Latitude_North == 0)) ){ Latitude_Motor.run(); }
  Latitude_Motor.active(true);
}
void objectRotation(){
  Longitude_Motor.set(ClOCKWISE, RPM, PULSE);
  Longitude_Motor.active(false);
  for (size_t i = 0; i < 200; i++)  //*********** changes *********** 
  {
      Longitude_Motor.run();
  }
}
void cameraAspectRatio(){
  AspectRatio_Motor.set(ClOCKWISE, RPM, PULSE);
  AspectRatio_Motor.active(false);
  while( (digitalRead(AspectRatio_Landscape == 0)) && (digitalRead(AspectRatio_Portrait == 0)) ){ AspectRatio_Motor.run(); }
  AspectRatio_Motor.active(true);
}
void altitudeMovement(){
  Altitude_Motor.set(ClOCKWISE, RPM, PULSE);
  Altitude_Motor.active(false);
  while( (digitalRead(Altitude_Min == 0)) && (digitalRead(Altitude_Max == 0)) ){ Altitude_Motor.run(); }
  Altitude_Motor.active(true);
}
