// Schritt 7: automatischer Sollwertsprung fuer die Messung mit log.py
// 0-3 min: Tset = T_HOCH (einschwingen), danach Tset = T_TIEF (Sprung)
#define B 4275.0
#define T_HOCH 27.3
#define T_TIEF 26.3
#define SPRUNG_S 180                // Sprung nach 3 Minuten
float Kp = 60, Ki = 1.0, Kd = 0;    // fuer Vergleiche hier aendern

float Tset = T_HOCH;
float Tf, Tprev, integ = 0;

float readT() {
  int a = analogRead(A0);
  float R = 100000.0 * (1023.0 / a - 1.0);
  return 1.0 / (log(R / 100000.0) / B + 1.0 / 298.15) - 273.15;
}

void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  Tf = Tprev = readT();
}

void loop() {
  if (millis() / 1000 >= SPRUNG_S) Tset = T_TIEF;  // der Sprung

  float dt = 0.5;                                   // Sekunden pro Schritt
  Tf = 0.3 * readT() + 0.7 * Tf;                    // Tiefpass gegen Rauschen
  float e = Tf - Tset;                              // > 0: zu warm -> Luefter
  integ = constrain(integ + e * dt, 0, 255 / Ki);   // I-Anteil, begrenzt
  float dTdt = (Tf - Tprev) / dt;                   // D-Anteil
  Tprev = Tf;

  int duty = constrain(Kp * e + Ki * integ + Kd * dTdt, 0, 255);
  analogWrite(11, duty);

  Serial.print("T:");        Serial.print(Tf);
  Serial.print(",Tset:");    Serial.print(Tset);
  Serial.print(",Luefter/10:"); Serial.println(duty / 10.0);
  delay(500);
}
