// Komplette Messung in einem Durchlauf (Heizwiderstand die ganze Zeit an):
// Phase 1: Luefter aus, aufheizen bis T stabil ist (max. 5 min) -> T_max
// Phase 2: PID auf Tset = T_max - 2 (2,5 min einschwingen)
// Phase 3: Sollwertsprung auf Tset = T_max - 3 (2,5 min)
// Danach Luefter aus und "ENDE" (beendet messung.py). Ausgabe im selben Format wie bisher (log.py / plot.py)
#define B 4275.0
#define PHASE_S 150                 // Dauer Phase 2 und 3 (2,5 min)
#define HEIZ_MAX_S 300              // Aufheizen hoechstens 5 min
float Kp = 60, Ki = 1.0, Kd = 0;    // fuer Vergleiche hier aendern

int phase = 1;
float Tf, Tprev, integ = 0, Tmax, Tset = NAN;
float Tvor = -100;                  // T vor 30 s (Stabil-Erkennung)
unsigned long tPhase = 0, tCheck = 0;

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
  unsigned long t = millis() / 1000;
  float dt = 0.5;                                   // Sekunden pro Schritt
  Tf = 0.3 * readT() + 0.7 * Tf;                    // Tiefpass gegen Rauschen
  int duty = 0;

  if (phase == 1) {                                 // aufheizen
    if (t - tCheck >= 30) {                         // alle 30 s pruefen
      if ((t >= 90 && Tf - Tvor < 0.05) || t >= HEIZ_MAX_S) {  // stabil oder Zeit um
        Tmax = Tf;
        Tset = Tmax - 2;
        phase = 2;
        tPhase = t;
      }
      Tvor = Tf;
      tCheck = t;
    }
  } else if (phase == 2 || phase == 3) {            // PID-Regelung
    if (phase == 2 && t - tPhase >= PHASE_S) { phase = 3; Tset = Tmax - 3; tPhase = t; }
    if (phase == 3 && t - tPhase >= PHASE_S) {     // fertig
      analogWrite(11, 0);
      Serial.println("ENDE");
      while (true);                                 // anhalten
    }

    float e = Tf - Tset;                            // > 0: zu warm -> Luefter
    integ = constrain(integ + e * dt, 0, 255 / Ki); // I-Anteil, begrenzt
    float dTdt = (Tf - Tprev) / dt;                 // D-Anteil
    duty = constrain(Kp * e + Ki * integ + Kd * dTdt, 0, 255);
  }
  Tprev = Tf;
  analogWrite(11, duty);

  Serial.print("T:");        Serial.print(Tf);
  Serial.print(",Tset:");    Serial.print(Tset);
  Serial.print(",Luefter/10:"); Serial.println(duty / 10.0);
  delay(500);
}
