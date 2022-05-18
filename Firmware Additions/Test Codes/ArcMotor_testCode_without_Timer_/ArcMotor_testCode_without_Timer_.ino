// Include the AccelStepper library:
#include <AccelStepper.h>

#define dirPin 6
#define stepPin 7
#define ENPin 20
#define motorInterfaceType 1

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

void setup() {

  stepper.setEnablePin(ENPin);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
  stepper.setMaxSpeed(1000);
  stepper.setSpeed(200);
}

void loop() {
  // Step the motor with a constant speed as set by setSpeed():
  stepper.runSpeed();
}
