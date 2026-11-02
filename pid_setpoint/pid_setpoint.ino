// Schritt 6: PID mit Befehlen im Serial Monitor, ohne neu hochzuladen
// Befehle (oben ins Eingabefeld tippen, Enter):
//   s25.3  -> Sollwert auf 25.3 C
//   p60    -> Kp = 60
//   i1     -> Ki = 1
//   d0     -> Kd = 0
#define B 4275.0
float Tset = 26.3;                  // = Tmax - 3
float Kp = 60, Ki = 1.0, Kd = 0;    // PID-Parameter (Ki > 0 lassen)

float Tf, Tprev, integ = 0;

float readT() {
  int a = analogRead(A0);
  float R = 100000.0 * (1023.0 / a - 1.0);
  return 1.0 / (log(R / 100000.0) / B + 1.0 / 298.15) - 273.15;
}

void befehle() {
  if (!Serial.available()) return;
  char c = Serial.read();
  if (c == 's') Tset = Serial.parseFloat();
  if (c == 'p') Kp = Serial.parseFloat();
  if (c == 'i') { Ki = Serial.parseFloat(); integ = 0; }
  if (c == 'd') Kd = Serial.parseFloat();
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);
  pinMode(11, OUTPUT);
  Tf = Tprev = readT();
}

void loop() {
  befehle();

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
