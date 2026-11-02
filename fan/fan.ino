void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
}

void loop() {
  Serial.println("Luefter 100%");
  analogWrite(11, 255);   // 255 = volle Drehzahl
  delay(3000);

  Serial.println("Luefter 30%");
  analogWrite(11, 80);    // Teildrehzahl
  delay(3000);

  Serial.println("Luefter aus");
  analogWrite(11, 0);
  delay(3000);
}