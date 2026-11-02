// Ein Durchlauf fuer Task 20, 22/23 und 24
// Phasen: 1 Drehzahl-Scan, 2 kalt machen, 3 Zweipunkt (von unten),
//         4 heiss machen, 5 Zweipunkt (von oben), 6 Gleichgewicht
#define TACHO  A2          // Luefter Pin 3 (gruen), interner Pull-up
#define T_HOCH 25.8        // Zweipunkt: darueber Luefter 100 %
#define T_TIEF 25.4        // Zweipunkt: darunter Luefter 0 %
#define T_HEISS 26.3       // Phase 4: so lange heizen, bis diese Temperatur erreicht ist

const int stufen[] = {0, 15, 30, 45, 60, 80, 100};   // Phase 6: Duty in %

float temperatur() {
  float R = 100000.0 * (1023.0 / analogRead(A0) - 1.0);
  return 1.0 / (log(R / 100000.0) / 4275.0 + 1.0 / 298.15) - 273.15;
}

long zaehlePulse(unsigned long ms) {   // fallende Flanken am Tacho zaehlen
  long n = 0;
  int alt = digitalRead(TACHO);
  unsigned long t0 = millis();
  while (millis() - t0 < ms) {
    int neu = digitalRead(TACHO);
    if (alt == HIGH && neu == LOW) n++;
    alt = neu;
  }
  return n;
}

// Luefter setzen, 1 s Pulse zaehlen, Temperatur lesen, alles senden
float messe(int phase, int duty, float thoch) {
  analogWrite(11, duty * 255 / 100);
  long n = zaehlePulse(1000);
  float T = temperatur();
  Serial.print("P:");      Serial.print(phase);
  Serial.print(",duty:");  Serial.print(duty);
  Serial.print(",rpm:");   Serial.print(n * 30);       // 2 Pulse pro Umdrehung, 1 s
  Serial.print(",T:");     Serial.print(T);
  Serial.print(",Thoch:"); Serial.println(thoch);
  return T;
}

void zweipunkt(int phase, int sekunden) {
  int duty = 0;
  float T = temperatur();
  for (int s = 0; s < sekunden; s++) {
    if (T > T_HOCH) duty = 100;    // zu warm -> kuehlen
    if (T < T_TIEF) duty = 0;      // kalt genug -> aus
    T = messe(phase, duty, T_HOCH);
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  pinMode(TACHO, INPUT_PULLUP);
  delay(2000);
}

void loop() {
  for (int p = 0; p <= 100; p += 5)            // 1) Drehzahl-Scan, 4 s pro Stufe
    for (int k = 0; k < 4; k++) messe(1, p, 0);

  for (int s = 0; s < 90; s++) messe(2, 100, 0);          // 2) kalt machen
  zweipunkt(3, 240);                                       // 3) regeln (von unten)
  for (int s = 0; s < 300; s++)                            // 4) heiss machen
    if (messe(4, 0, 0) >= T_HEISS) break;
  zweipunkt(5, 240);                                       // 5) regeln (von oben)

  for (int i = 0; i < 7; i++) {                            // 6) Gleichgewicht
    int dauer = (i == 0) ? 600 : 360;
    for (int s = 0; s < dauer; s++) messe(6, stufen[i], 0);
  }

  analogWrite(11, 0);
  Serial.println("ENDE");
  while (true) {}
}
