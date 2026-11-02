#define R0 10000.0   // Widerstand im Spannungsteiler (10 kOhm)

float tempFromR(float R, float R25, float B) {
  return 1.0 / (log(R / R25) / B + 1.0 / 298.15) - 273.15;
}

void setup() { Serial.begin(9600); pinMode(11, OUTPUT); analogWrite(11, 0); }

void loop() {
  float Rg = 100000.0 * (1023.0 / analogRead(A0) - 1.0);  // Grove (Referenz, 100k)
  float Rn = R0 * (1023.0 / analogRead(A1) - 1.0);        // eigener NTC (10k)
  Serial.print("Tref:");    Serial.print(tempFromR(Rg, 100000.0, 4275.0));
  Serial.print(",Tntc:");   Serial.print(tempFromR(Rn, 10000.0, 3950.0));
  Serial.print(",R_kOhm:"); Serial.println(Rn / 1000.0);
  delay(1000);
}
