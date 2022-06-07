bool state = LOW;
void setup() {
  // put your setup code here, to run once:
  pinMode(4,OUTPUT);
}

void loop() {
  digitalWrite(4,state);      //Arc Motor: 400Step/sec hence 1step = 0.0025sec
  //delayMicroseconds(1250); 
  delay(200);   
  state = !state;
}
