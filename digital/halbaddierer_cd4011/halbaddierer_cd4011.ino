// Halbaddierer mit CD4011 (+ optional CD4081)
// Arduino gibt A und B vor, der Chip rechnet, der Arduino liest ab
// und erklaert im Serial Monitor, welche LED leuchten sollte und warum.
const bool MIT_4081 = true;    // Schritt 2: CD4081 ist angeschlossen

const int PIN_A = 8, PIN_B = 9, PIN_S = 10, PIN_N1 = 11;

void setup() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_S, INPUT);
  pinMode(PIN_N1, INPUT);
  Serial.begin(9600);
  Serial.println(MIT_4081 ? "Halbaddierer CD4011 + CD4081" : "Halbaddierer mit CD4011");
}

void erklaere(int A, int B) {
  Serial.println();

  // ---- Summe (S) ----
  if (A != B) {
    Serial.println("   S-LED: AN");
    Serial.println("     Warum? 0+1 oder 1+0 ergibt 1. Die hintere Stelle ist also 1.");
    Serial.println("     Der Chip gibt an Pin 11 Strom raus, die LED leuchtet.");
  } else if (A == 0) {
    Serial.println("   S-LED: AUS");
    Serial.println("     Warum? 0+0 ergibt 0. Die hintere Stelle ist 0.");
  } else {
    Serial.println("   S-LED: AUS");
    Serial.println("     Warum? 1+1 ergibt 2, binaer '10'. Die hintere Stelle ist 0.");
  }

  // ---- Uebertrag (C) ----
  if (A && B) {
    Serial.println("   C-LED: AN");
    Serial.println("     Warum? 1+1 passt nicht in eine Stelle, es gibt einen Uebertrag");
    Serial.println("     auf die vordere Stelle (wie bei 5+5=10 im Dezimalsystem).");
  } else {
    Serial.println("   C-LED: AUS");
    Serial.println("     Warum? Das Ergebnis ist hoechstens 1, es braucht keine vordere Stelle.");
  }
}

void loop() {
  Serial.println();
  Serial.println("==========================================");
  for (int i = 0; i < 4; i++) {
    int A = (i >> 1) & 1;          // zaehlt 00, 01, 10, 11 durch
    int B = i & 1;
    digitalWrite(PIN_A, A);
    digitalWrite(PIN_B, B);
    delay(50);                     // kurz warten, bis alles stabil ist

    int S = digitalRead(PIN_S);
    int C = MIT_4081 ? digitalRead(PIN_N1)    // AND liefert C direkt
                     : !digitalRead(PIN_N1);  // N1 ist NAND -> invertieren

    Serial.println();
    Serial.print(A); Serial.print(" + "); Serial.print(B);
    Serial.print("  =  "); Serial.print(C); Serial.print(S);
    Serial.print(" (binaer)  =  "); Serial.print(2 * C + S); Serial.print(" (dezimal)");
    if (2 * C + S != A + B) Serial.print("   <-- FEHLER, Verdrahtung pruefen");
    Serial.println();
    erklaere(A, B);

    delay(3000);                   // 3 s Zeit, die LEDs anzuschauen
  }
}
