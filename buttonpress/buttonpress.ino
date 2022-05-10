void setup() {

    pinMode(1, INPUT_PULLUP); //Increase Button
    pinMode(2, INPUT_PULLUP); //Decrease Button
    pinMode(3, INPUT_PULLUP); //Flip Button
    attachInterrupt(digitalPinToInterrupt(2), decreaseB, FALLING);
    attachInterrupt(digitalPinToInterrupt(1), increaseB, FALLING);
    attachInterrupt(digitalPinToInterrupt(3), flipB, FALLING);

    

}

int setTemp = 2;

void loop() {
    delay(100);
}

void increaseB() {
  Serial.print(setTemp);
  if( setTemp > 29 ) {
    setTemp = 30;
    return;
  }
  setTemp++;
  delay(3000);
}

void decreaseB() {
  Serial.print(setTemp);
  if( setTemp < 3 ) {
    setTemp = 2;
    return;
  }
  setTemp--;
  delay(3000);
}

void flipB() {
  Serial.print(setTemp);
  if( setTemp < 16 ) {
    setTemp = 30;
  } else {
    setTemp = 2;
  }
  delay(3000);
}
