#include <AccelStepper.h>
// Create an IntervalTimer object 
IntervalTimer ArcTimer;

//Motors Pinouts & Configuration:
#define motorInterfaceType          1
#define Dir_ArcRotate               6 //6
#define STEP_ArcRotate              7 //7
#define ArcRotate_ENAPIN            20 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400

AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);

void setup() {
  Serial.begin(9600);
  ArcMotor_Initialization();
  ArcTimer.begin(ArcStep, 1250);  // Run ArcMotor every 0.00125 seconds ( 2500 / 2  = 1250 ) , 2500 - > for 1 complete step including HIGH & LOW at step-pin
}

void loop() 
{  
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
