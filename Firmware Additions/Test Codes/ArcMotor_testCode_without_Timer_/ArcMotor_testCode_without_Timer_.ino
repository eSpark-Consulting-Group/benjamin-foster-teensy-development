// Include the AccelStepper library:
#include <AccelStepper.h>

#define dirPin 6
#define stepPin 7
#define ENPin 20
#define motorInterfaceType 1

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
unsigned long previousTimeARC = millis();
long timeIntervalARC = 0.0025;
bool Arcflag;

void setup() {

  stepper.setEnablePin(ENPin);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(200);
}

void loop() {
  ArcTicker();
}
void ArcTicker(){
  unsigned long currentTime = millis();
  if(currentTime - previousTimeARC > timeIntervalARC) {
    previousTimeARC = currentTime;
     stepper.runSpeed();
  } 
}
   
