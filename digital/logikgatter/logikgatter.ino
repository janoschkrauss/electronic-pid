// Logikgatter-Spielplatz
// Taster A an Pin 2, Taster B an Pin 3, Taster "Gatter wechseln" an Pin 4
// (jeweils andere Seite an GND, interne Pull-ups -> kein Widerstand noetig)
// LED (mit 220 Ohm) an Pin 13, oder einfach die eingebaute LED nutzen.

const int PIN_A = 2, PIN_B = 3, PIN_MODE = 4, PIN_LED = 13;
const char* namen[] = {"AND", "OR", "NAND", "NOR", "XOR"};
int gatter = 0;
int altA = -1, altB = -1, altQ = -1;
bool modeVorher = false;

void setup() {
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);
  Serial.begin(9600);
  Serial.println("Gatter: AND");
}

void loop() {
  // gedrueckt = LOW wegen Pull-up, darum invertieren: gedrueckt -> 1
  int A = !digitalRead(PIN_A);
  int B = !digitalRead(PIN_B);

  bool modeJetzt = !digitalRead(PIN_MODE);
  if (modeJetzt && !modeVorher) {          // nur bei steigender Flanke
    gatter = (gatter + 1) % 5;
    Serial.print("Gatter: "); Serial.println(namen[gatter]);
    altQ = -1;                              // Ausgabe neu erzwingen
  }
  modeVorher = modeJetzt;

  int Q;
  switch (gatter) {
    case 0: Q = A && B;    break;  // AND
    case 1: Q = A || B;    break;  // OR
    case 2: Q = !(A && B); break;  // NAND
    case 3: Q = !(A || B); break;  // NOR
    default: Q = A ^ B;    break;  // XOR
  }
  digitalWrite(PIN_LED, Q);

  if (A != altA || B != altB || Q != altQ) {  // nur bei Aenderung ausgeben
    Serial.print(namen[gatter]); Serial.print("  A=");
    Serial.print(A); Serial.print(" B="); Serial.print(B);
    Serial.print("  ->  Q="); Serial.println(Q);
    altA = A; altB = B; altQ = Q;
  }
  delay(20);  // einfaches Entprellen
}
