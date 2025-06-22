int buzzer = 2;
void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  tone(buzzer, 3000); // A4
  delay(200);

  noTone(buzzer); // B4
  delay(200);
}

