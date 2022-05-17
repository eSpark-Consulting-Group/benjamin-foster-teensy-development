#include <AccelStepper.h>
// Create an IntervalTimer object 
IntervalTimer ArcMotor_timer;
void Arc_Motor_Operation();   
#define motorInterfaceType          1
#define Dir_ArcRotate               6
#define STEP_ArcRotate              7
#define ArcRotate_ENAPIN            20
#define ArcRotateMaxSpeed           400   
#define ArcRotateAcceleration       300   
#define ArcRotateSpeed              400     

AccelStepper ArcMotor(motorInterfaceType, STEP_ArcRotate, Dir_ArcRotate);

void setup() {
    ArcMotor.setSpeed(ArcRotateSpeed);
    ArcMotor.setAcceleration(ArcRotateAcceleration);
    ArcMotor.setEnablePin(ArcRotate_ENAPIN);
    Serial.begin(9600);
    ArcMotor_timer.begin(Arc_Motor_Operation, 2500);  // Arc_Motor_Operation to run every 0.0025 seconds
}

volatile unsigned long steps = 0; // use volatile for shared variables


void loop() {
  unsigned long stepsCopy;  

  noInterrupts();
  stepsCopy = steps;
  interrupts();
  if(steps == 400 ){
  Serial.print("Two Revolutions are completed Steps = 400");
  Serial.println(stepsCopy);
  delay(100);
  }
}
void Arc_Motor_Operation(){
      ArcMotor.run();
      ++steps;
  }
