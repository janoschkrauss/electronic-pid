#define B 4275.0          // Grove-Temperatursensor V1.2

void setup() {
  Serial.begin(9600);
}

void loop() {
  int a = analogRead(A0);                     // 0..1023
  float R = 100000.0 * (1023.0 / a - 1.0);    // Widerstand des NTC
  float T = 1.0 / (log(R / 100000.0) / B + 1.0 / 298.15) - 273.15;
  Serial.println(T);
  delay(1000);
}