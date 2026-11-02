#define B 4275.0
float Tset = 26.3;                  // = Tmax - 3  <- anpassen!
float Kp = 60, Ki = 1.0, Kd = 0;    // PID-Parameter (Ki > 0 lassen)

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