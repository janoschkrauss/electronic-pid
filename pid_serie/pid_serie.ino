#define B 4275.0
float Tset = 24.3;                  // Sollwert
float Tstart = 27.0;                // Regelung startet, sobald T so warm ist
float KiListe[] = {1.0, 10.0};      // ein Lauf pro Ki-Wert
float Kp = 60, Kd = 0;
#define REGEL_S 300                 // Sekunden Regelung pro Lauf
const int N = sizeof(KiListe) / sizeof(KiListe[0]);

float Tf, Tprev, integ = 0, Tcheck;
bool heizen = true;
int lauf = 0;
unsigned long tCheck, tRegel;

float readT() {
  int a = analogRead(A0);
  float R = 100000.0 * (1023.0 / a - 1.0);
  return 1.0 / (log(R / 100000.0) / B + 1.0 / 298.15) - 273.15;
}

void startLauf() {                  // Luefter aus, neu aufheizen
  heizen = true;
  integ = 0;
  Tcheck = Tf;
  tCheck = millis();
  Serial.print("LAUF ");
  Serial.print(lauf + 1);
  Serial.print(" Ki=");
  Serial.println(KiListe[lauf]);
}

void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  Tf = Tprev = readT();
  startLauf();
}

void loop() {
  if (lauf >= N) {                  // alle Laeufe fertig
    analogWrite(11, 0);
    Serial.println("ENDE");
    delay(1000);
    return;
  }

  float dt = 0.5;
  Tf = 0.3 * readT() + 0.7 * Tf;    // Tiefpass
  float dTdt = (Tf - Tprev) / dt;
  Tprev = Tf;
  int duty = 0;

  if (heizen) {                     // Phase 1: Luefter aus
    if (Tf >= Tstart) heizen = false;
    if (millis() - tCheck >= 60000) {
      if (Tf - Tcheck < 0.05) heizen = false;   // steigt kaum noch
      Tcheck = Tf;
      tCheck = millis();
    }
    if (!heizen) tRegel = millis();
  } else {                          // Phase 2: PID
    float Ki = KiListe[lauf];
    float e = Tf - Tset;
    integ = constrain(integ + e * dt, 0, 255 / Ki);
    duty = constrain(Kp * e + Ki * integ + Kd * dTdt, 0, 255);
    if (millis() - tRegel > REGEL_S * 1000UL) {  // Lauf fertig
      lauf++;
      analogWrite(11, 0);
      if (lauf < N) startLauf();
      return;
    }
  }
  analogWrite(11, duty);

  Serial.print("T:");        Serial.print(Tf);
  Serial.print(",Tset:");    Serial.print(heizen ? 0 : Tset);
  Serial.print(",Luefter/10:"); Serial.println(duty / 10.0);
  delay(500);
}
