// Treiber Instalieren (Port + Bord Treiber)
// Richtiges Bord und Port auswelen
// Einfügen der Serbiblotoeck für den ESP32 (Servo.h ist nicht kompatibel mit dem ESP32)
#include <Arduino.h>
#include <ESP32Servo.h>

// Erstellt Objekt Servomotoren zum Controlieren im programm
Servo servoHorizontal;
Servo servoVertical;

// Pinzuweisung der Servomotoren
const int servoHorizontalPin = 18;
const int servoVerticalPin = 19;

// Zuweisung der LDR Sensoren zu einem Pin
// Const ersetzt #define (Const erstellt genau wie defin eine Konstante nur das const auch bei Arrays funktioniert und defin nicht.)
const int lightDependentResistorLeft = A0;
const int lightDependentResistorRight = A5;
const int lightDependentResistorUpper = A6;
const int lightDependentResistorLower = A7;

// Legt die Startposition (0-180) der servomotoren fest.
int positionHorizontal = 90;
int positionVertical = 90;

void setup()
{
  // Pinzuweisung der Servomotoren
  servoHorizontal.attach(servoHorizontalPin);
  servoVertical.attach(servoVerticalPin);
  //
  servoHorizontal.write(positionHorizontal);
  servoVertical.write(positionVertical);

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
    int leftLDRValue = analogRead(lightDependentResistorLeft);
    int rightLDRValue = analogRead(lightDependentResistorRight);
    int upperLDRValue = analogRead(lightDependentResistorUpper);
    int lowerLDRValue = analogRead(lightDependentResistorLower);

    // Horizontale Anpassung
    if (leftLDRValue > rightLDRValue + 10)
    {
      positionHorizontal = constrain(positionHorizontal - 1, 0, 180);
    }
    else if (rightLDRValue > leftLDRValue + 10)
    {
      positionHorizontal = constrain(positionHorizontal + 1, 0, 180);
    }

    // Vertikale Anpassung
    if (upperLDRValue > lowerLDRValue + 10)
    {
      positionVertical = constrain(positionVertical + 1, 0, 180);
    }
    else if (lowerLDRValue > upperLDRValue + 10)
    {
      positionVertical = constrain(positionVertical - 1, 0, 180);
    }

    // Servos bewegen
    servoHorizontal.write(positionHorizontal);
    servoVertical.write(positionVertical);

    // Debugging
    Serial.print("Horizont: ");
    Serial.print(positionHorizontal);
    Serial.print(" Vertikal: ");
    Serial.println(positionVertical);
  }
}
