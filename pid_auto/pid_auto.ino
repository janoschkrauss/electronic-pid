#define B 4275.0
float Tset = 24.3;                  // Sollwert
float Tstart = 27.0;                // Regelung startet, sobald T so warm ist
float Kp = 60, Ki = 10.0, Kd = 0;   // "schlecht": Ki = 10 (gut war Ki = 1)
#define REGEL_S 300                 // Sekunden Regelung, danach ENDE

float Tf, Tprev, integ = 0, Tcheck;
bool heizen = true;                 // Phase 1: aufheizen, Phase 2: PID
unsigned long tCheck, tRegel;

float readT() {
  int a = analogRead(A0);
  float R = 100000.0 * (1023.0 / a - 1.0);
  return 1.0 / (log(R / 100000.0) / B + 1.0 / 298.15) - 273.15;
}

void setup() {
  Serial.begin(9600);
  pinMode(11, OUTPUT);
  Tf = Tprev = Tcheck = readT();
  tCheck = millis();
}

void loop() {
  float dt = 0.5;
  Tf = 0.3 * readT() + 0.7 * Tf;                    // Tiefpass
  float dTdt = (Tf - Tprev) / dt;
  Tprev = Tf;
  int duty = 0;

  if (heizen) {                                     // Phase 1: Luefter aus
    if (Tf >= Tstart) heizen = false;               // warm genug
    if (millis() - tCheck >= 60000) {               // oder: steigt kaum noch
      if (Tf - Tcheck < 0.05) heizen = false;
      Tcheck = Tf;
      tCheck = millis();
    }
    if (!heizen) tRegel = millis();
  } else {                                          // Phase 2: PID
    float e = Tf - Tset;
    integ = constrain(integ + e * dt, 0, 255 / Ki);
    duty = constrain(Kp * e + Ki * integ + Kd * dTdt, 0, 255);
  }

  if (!heizen && millis() - tRegel > REGEL_S * 1000UL) {
    analogWrite(11, 0);
    Serial.println("ENDE");
    delay(500);
    return;
  }
  analogWrite(11, duty);

  Serial.print("T:");        Serial.print(Tf);
  Serial.print(",Tset:");    Serial.print(heizen ? 0 : Tset);
  Serial.print(",Luefter/10:"); Serial.println(duty / 10.0);
  delay(500);
}
