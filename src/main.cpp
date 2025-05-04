// Treiber Instalieren (Port + Bord Treiber)
// Richtiges Bord und Port auswelen
// Einfügen der Serbiblotoeck für den ESP32 (Servo.h ist nicht kompatibel mit dem ESP32)
#include <Arduino.h>
#include <ESP32Servo.h>

// Erstellt Objekt Servomotoren zum Controlieren im programm
Servo servoHorizont;
Servo servoVertikal;

// Zuweisung der LDR Sensoren zu einem Pin
// Const ersetzt #define (Const erstellt genau wie defin eine Konstante nur das const auch bei Arrays funktioniert und defin nicht.)
const int LDR_Links = A0;
const int LDR_Rechts = A5;
const int LDR_Oben = A6;
const int LDR_Unten = A7;

// Legt die Startposition (0-180) der servomotoren fest.
int posHorizontal = 90;
int posVertical = 90;

void setup()
{
  // Standard-Servo: 50 Hz
  servoHorizont.setPeriodHertz(50);
  servoVertikal.setPeriodHertz(50);
  // Pinzuweisung der Servomotoren
  servoHorizont.attach(18);
  servoVertikal.attach(19);
  //
  servoHorizont.write(posHorizontal);
  servoVertikal.write(posVertical);

  Serial.begin(115200);
}
/**
 *  ✅ Non-blocking code (millis-based)
 *  The millis() approach doesn't block execution. The loop() function keeps running, and you only do your task every 50 milliseconds.
 *  This allows other code to run in the meantime — like checking sensors, handling communication (like serial or network), or responding to user inputs.
 *  It's ideal for multitasking, especially in microcontroller environments like Arduino.
 *  ❌ Blocking code (delay-based)
 *  The delay(50) line blocks the CPU for 50 milliseconds, doing absolutely nothing.
 *  During the delay, the program can't respond to inputs, run other logic, or check sensors.
 *  This is fine for very simple programs, but scales poorly as your project becomes more complex.
 */
u_int64_t lastExecute = 0;
void loop()
{
  if (millis() - lastExecute > 50)
  {
    lastExecute = millis();
    int lichtLinks = analogRead(LDR_Links);
    int lichtRechts = analogRead(LDR_Rechts);
    int lichtOben = analogRead(LDR_Oben);
    int lichtUnten = analogRead(LDR_Unten);

    // Horizontale Anpassung
    if (lichtLinks > lichtRechts + 10)
    {
      posHorizontal = constrain(posHorizontal - 1, 0, 180);
    }
    else if (lichtRechts > lichtLinks + 10)
    {
      posHorizontal = constrain(posHorizontal + 1, 0, 180);
    }

    // Vertikale Anpassung
    if (lichtOben > lichtUnten + 10)
    {
      posVertical = constrain(posVertical + 1, 0, 180);
    }
    else if (lichtUnten > lichtOben + 10)
    {
      posVertical = constrain(posVertical - 1, 0, 180);
    }

    // Servos bewegen
    servoHorizont.write(posHorizontal);
    servoVertikal.write(posVertical);

    // Debugging
    Serial.print("Horizont: ");
    Serial.print(posHorizontal);
    Serial.print(" Vertikal: ");
    Serial.println(posVertical);
  }
}
