// Chip-Tester: Arduino steuert die Eingaenge eines echten Logik-ICs
// und liest den Ausgang. Gibt die Wahrheitstabelle aus und erkennt das Gatter.
//
// Verdrahtung fuer CD4011 (NAND) oder CD4001 (NOR), erstes Gatter:
//   IC Pin 14 (VDD) -> Arduino 5V
//   IC Pin 7  (VSS) -> Arduino GND
//   IC Pin 1  (A)   -> Arduino D8
//   IC Pin 2  (B)   -> Arduino D9
//   IC Pin 3  (Q)   -> Arduino D10   (optional zusaetzlich LED + 1 kOhm nach GND)
//   Unbenutzte Eingaenge (Pins 5,6,8,9,12,13) an GND!

const int PIN_A = 8, PIN_B = 9, PIN_Q = 10;

void setup() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_Q, INPUT);
  Serial.begin(9600);
}

void loop() {
  int ergebnis[4];
  Serial.println();
  Serial.println(" A B | Q");
  Serial.println("-----+--");
  for (int i = 0; i < 4; i++) {
    int A = (i >> 1) & 1;
    int B = i & 1;
    digitalWrite(PIN_A, A);
    digitalWrite(PIN_B, B);
    delay(1000);                  // 1 s pro Zeile, damit man die LED sieht
    int Q = digitalRead(PIN_Q);
    ergebnis[i] = Q;
    Serial.print(" "); Serial.print(A); Serial.print(" "); Serial.print(B);
    Serial.print(" | "); Serial.println(Q);
  }

  // Muster der Q-Spalte (00,01,10,11) mit bekannten Gattern vergleichen
  int muster = ergebnis[0] << 3 | ergebnis[1] << 2 | ergebnis[2] << 1 | ergebnis[3];
  Serial.print("Erkannt: ");
  switch (muster) {
    case 0b0001: Serial.println("AND");  break;
    case 0b0111: Serial.println("OR");   break;
    case 0b1110: Serial.println("NAND"); break;
    case 0b1000: Serial.println("NOR");  break;
    case 0b0110: Serial.println("XOR");  break;
    case 0b1001: Serial.println("XNOR"); break;
    default:     Serial.println("unbekannt - Verdrahtung pruefen"); break;
  }
  delay(2000);
}
