#include <AccelStepper.h>
// Create an IntervalTimer object 
IntervalTimer ArcMotor_timer;
void Arc_Motor_Operation();

#define Dir_ArcRotate               6
#define STEP_ArcRotate              7
#define ArcRotate_ENAPIN            20
#define ArcRotateMaxSpeed           400   
#define ArcRotateAcceleration       300   
#define ArcRotateSpeed              400     

AccelStepper ArcMotor(AccelStepper::DRIVER, STEP_ArcRotate, Dir_ArcRotate);

void setup() {
    ArcMotor.setSpeed(ArcRotateSpeed);
    ArcMotor.setAcceleration(ArcRotateAcceleration);
    ArcMotor.setEnablePin(ArcRotate_ENAPIN);
    Serial.begin(9600);
    ArcMotor_timer.begin(Arc_Motor_Operation, 15000);  // blinkLED to run every 0.15 seconds
}

volatile unsigned long steps = 0; // use volatile for shared variables


void loop() {
  unsigned long stepsCopy;  // holds a copy of the blinkCount

  // to read a variable which the interrupt code writes, we
  // must temporarily disable interrupts, to be sure it will
  // not change while we are reading.  To minimize the time
  // with interrupts off, just quickly make a copy, and then
  // use the copy while allowing the interrupt to keep working.
  noInterrupts();
  stepsCopy = steps;
  interrupts();

  Serial.println("To verify that the Arc Motor is running:");
  Serial.println("Steps = ");
  Serial.println(stepsCopy);
  delay(100);
}
void Arc_Motor_Operation(){
      ArcMotor.runSpeed();
  }
