#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <AccelStepper.h>

//Motors Pinouts & Configuration:
IntervalTimer ArcTimer;
#define motorInterfaceType          1
#define Dir_ArcRotate               6 //6
#define STEP_ArcRotate              7 //7
#define ArcRotate_ENAPIN            20 //20
#define ArcRotateMaxSpeed           400
#define ArcRotateAcceleration       300
#define ArcRotateSpeed              400
AccelStepper ArcMotor = AccelStepper(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);

//Encoder Definitions:
#define DEBUG_ENCODER 3             //Homing Pin
#define rotaryA 1                   //pin up
#define rotaryB 2                   //pin down
Encoder MainEncoder(rotaryA, rotaryB);

int stepCount;
uint8_t nRev = 0;   //To execute 2 blank Rotations

void setup() 
{
  Serial.begin(115200);
  ArcMotor_Initialization();
  ArcTimer.begin(ArcStep, 5000);  // Run ArcMotor every 0.00125 seconds ( 2500 / 2  = 1250 ) , 2500 - > for 1 complete step including HIGH & LOW at step-pin
  EncoderInit();
  delay(1000);
}

void loop() 
{ 
  
}
void EncoderInit()
{
  pinMode(DEBUG_ENCODER, INPUT_PULLUP);
  //pinMode(1,INPUT_PULLUP);
  //pinMode(2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(1), updateEncoder, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), zeroPos_isr, FALLING);
}

void ArcMotor_Initialization()
{
  pinMode(ArcRotate_ENAPIN,OUTPUT);
  digitalWrite(ArcRotate_ENAPIN,HIGH);
  digitalWrite(Dir_ArcRotate,HIGH);
  ArcMotor.setEnablePin(ArcRotate_ENAPIN);
  ArcMotor.setPinsInverted(false, false, true);
  ArcMotor.enableOutputs();
  ArcMotor.setMaxSpeed(ArcRotateMaxSpeed);
  ArcMotor.setAcceleration(ArcRotateAcceleration);
  ArcMotor.setSpeed(ArcRotateSpeed);
}
void ArcStep()
{
  static bool flag = false;
  flag = !flag;
  digitalWrite(STEP_ArcRotate, flag);
}
void updateEncoder()
{
  
  if(nRev >= 2)
  {
  
  //Serial.print("stepCOunt : ");
  //Serial.println(stepCount);
  //Serial.print("MainEncoder.read() : ");
  //Serial.println( (MainEncoder.read() / 4) );
  //stepCount++;
  
  }
}
void zeroPos_isr()
{
  stepCount = 0;
  MainEncoder.write(0);
  if(nRev < 2)
  {
    Serial.println("1");
    nRev++;  
  }
  else
  {
  Serial.print(" Rev Completed! ");
  }
}
