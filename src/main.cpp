// Einfügen der Biblothek für den ESP32
#include <Arduino.h> //Standardbibliothek des ESP32
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN 170
#define SERVOMAX 650
// Zuweisung der LDR Sensoren zu einem Pin
// Const ersetzt #define (Const erstellt genau wie defin eine Konstante nur das const auch bei Arrays funktioniert und defin nicht.)
const int lightDependentResistorLeft = A4;
const int lightDependentResistorRight = A5;
const int lightDependentResistorUpper = A6;
const int lightDependentResistorLower = A7;

// Legt die Startposition (0-180) der servomotoren fest.
int positionHorizontal = 90;
int positionVertical = 90;

//Die setup() Funktion wird nur einmal am Anfang des Programms aufgerufen.
void setup()
{
  // Initialisiert die Servomotoren
  servoHorizontal.attach(servoHorizontalPin);
  servoVertical.attach(servoVerticalPin);
  
  // Setzt die Servomotoren auf die Startpositionen
  servoHorizontal.write(positionHorizontal);
  servoVertical.write(positionVertical);

  Serial.begin(115200);
}
/**
 * Nicht-blockierender Code (mit millis()):
 * Die Aufgaben werden alle 50 Millisekunden ausgeführt, ohne den Programmfluss zu unterbrechen.
 * Das System erledigt gleichzeitig andere Aufgaben wie Sensorabfragen oder Benutzerinteraktionen – ideal für komplexe oder multitasking-fähige Anwendungen.
 * 
 * Blockierender Code (mit delay()):
 * Die Programmausführung wird während der Wartezeit vollständig pausiert (z. B. 50 ms), was Multitasking verhindert.
 * Diese Funktion ist nur für sehr einfache Programme geeignet.
 * 
 * loop()-Funktion:
 * In der loop()-Funktion werden regelmäßig (alle 50 ms) lichtabhängige Widerstände (LDRs) ausgelesen und deren Werte verglichen.
 * Je nach Lichtverhältnis wird die Position von Servomotoren angepasst und ausgegeben. Dabei wird nicht-blockierend gearbeitet, um andere Prozesse nicht zu stören.
 */
u_int64_t lastExecute = 0;
void loop()
{
  // Wenn 50 Millisekunden vergangen sind, wird der Code innerhalb des if-Statements ausgeführt
  if (millis() - lastExecute > 50)
  {
    // Aktuelle Zeit speichern
    lastExecute = millis();
    // Werte der Lichtabh ngigen Widerst nde auslesen
    int leftLDRValue = analogRead(lightDependentResistorLeft);
    int rightLDRValue = analogRead(lightDependentResistorRight);
    int upperLDRValue = analogRead(lightDependentResistorUpper);
    int lowerLDRValue = analogRead(lightDependentResistorLower);

    // Horizontale Anpassung
    if (leftLDRValue > rightLDRValue + 10)
    {
      // Wenn der linke LDR-Wert höher ist als der rechte LDR-Wert, 
      //wird die Position des horizontalen Servomotors nach links angepasst
      positionHorizontal = constrain(positionHorizontal - 1, 0, 180);
    }
    else if (rightLDRValue > leftLDRValue + 10)
    {
      //Wenn der rechte LDR-Wert höher ist als der linke LDR-Wert, 
      //wird die Position des horizontalen Servomotors nach rechts angepasst
      positionHorizontal = constrain(positionHorizontal + 1, 0, 180);
    }

    // Vertikale Anpassung
    if (upperLDRValue > lowerLDRValue + 10)
    {
      // Wenn der obere LDR-Wert höher ist als der untere LDR-Wert, 
      //wird die Position des vertikalen Servomotors nach oben angepasst
      positionVertical = constrain(positionVertical + 1, 0, 180);
    }
    else if (lowerLDRValue > upperLDRValue + 10)
    {
      // Wenn der untere LDR-Wert höher ist als der obere LDR-Wert, 
      //wird die Position des vertikalen Servomotors nach unten angepasst
      positionVertical = constrain(positionVertical - 1, 0, 180);
    }

    // Servos bewegen
    servoHorizontal.write(positionHorizontal);
    servoVertical.write(positionVertical);

    // Debugging
    Serial.print("Horizont: ");
    Serial.println(positionHorizontal);
    Serial.print("Vertikal: ");
    Serial.println(positionVertical);
    Serial.print("leftLDRValue: ");
    Serial.println(leftLDRValue);
    Serial.print("rightLDRValue: ");
    Serial.println(rightLDRValue);
    Serial.print("lowerLDRValue: ");
    Serial.println(lowerLDRValue);
    Serial.print("upperLDRValue: ");
    Serial.println(upperLDRValue);
  }
}
