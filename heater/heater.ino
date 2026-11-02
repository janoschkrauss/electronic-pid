#define B 4275.0

void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  analogWrite(11, 0);     // Luefter aus -> aufheizen
}

void loop() {
  int a = analogRead(A0);
  float R = 100000.0 * (1023.0 / a - 1.0);
  float T = 1.0 / (log(R / 100000.0) / B + 1.0 / 298.15) - 273.15;
  Serial.println(T);
  delay(1000);
}