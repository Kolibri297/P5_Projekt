// Einfügen der Biblothek für den ESP32
#include <Arduino.h> //Standardbibliothek des ESP32
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN 170
#define SERVOMAX 650

// Initialisiert der dafruit_PWMServoDriver klasse
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Number of servos you have
const int servoCount = 2;
// Define the PCA9685 pins for your servos
const int servoPins[servoCount] = {0, 1}; // Todo: Anstatt 0,1 die Richtigen Pins.
// Array to store default positions for each servo
int defaultPositions[servoCount] = {90, 90};
// Array to store current servo angles
int servoAngles[servoCount];

// Zuweisung der LDR Sensoren zu einem Pin
// Const ersetzt #define (Const erstellt genau wie defin eine Konstante nur das const auch bei Arrays funktioniert und defin nicht.)
const int lightDependentResistorLeft = A4;
const int lightDependentResistorRight = A5;
const int lightDependentResistorUpper = A6;
const int lightDependentResistorLower = A7;

// Legt die Startposition (0-180) der servomotoren fest.
int positionHorizontal = 0;
int positionVertical = 0;

// Die setup() Funktion wird nur einmal am Anfang des Programms aufgerufen.
void setup()
{
  Serial.begin(115200);

  // Initialisiert die Servomotoren
  pwm.begin();
  pwm.setPWMFreq(60);

  // Setzt die Servomotoren auf die Startpositionen
  for (int i = 0; i < servoCount; i++)
  {
    servoAngles[i] = defaultPositions[i];
    pwm.setPWM(servoPins[i], 0, angleToPulse(servoAngles[i]));
  }
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
      // wird die Position des horizontalen Servomotors nach links angepasst
      positionHorizontal = constrain(positionHorizontal - 1, 0, 180);
    }
    else if (rightLDRValue > leftLDRValue + 10)
    {
      // Wenn der rechte LDR-Wert höher ist als der linke LDR-Wert,
      // wird die Position des horizontalen Servomotors nach rechts angepasst
      positionHorizontal = constrain(positionHorizontal + 1, 0, 180);
    }

    // Vertikale Anpassung
    if (upperLDRValue > lowerLDRValue + 10)
    {
      // Wenn der obere LDR-Wert höher ist als der untere LDR-Wert,
      // wird die Position des vertikalen Servomotors nach oben angepasst
      positionVertical = constrain(positionVertical + 1, 0, 180);
    }
    else if (lowerLDRValue > upperLDRValue + 10)
    {
      // Wenn der untere LDR-Wert höher ist als der obere LDR-Wert,
      // wird die Position des vertikalen Servomotors nach unten angepasst
      positionVertical = constrain(positionVertical - 1, 0, 180);
    }

    // Servos bewegen
    moveServo(servoPins[0], positionHorizontal);
    moveServo(servoPins[1], positionVertical);

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



/**
 * @brief Smoothly move the servo to the target angle
 *
 * @param servoIndex The index of the servo to move
 * @param targetAngle The target angle to move the servo to
 *
 * This function moves the servo to the target angle over a period of time, by
 * smoothly increasing or decreasing the angle in increments of 1 degree. The
 * delay between each increment can be adjusted by changing the value of the
 * `delay` parameter.
 */
void moveServo(int servoIndex, int targetAngle)
{
  // Smoothly move the servo to the target angle
  for (int i = servoAngles[servoIndex]; i != targetAngle; i += (targetAngle > i) ? 1 : -1)
  {
    servoAngles[servoIndex] = i;
    pwm.setPWM(servoPins[servoIndex], 0, angleToPulse(i));
    delay(10); // Adjust the delay for smoother motion
  }
}

/**
 * @brief Converts an angle to a servo pulse length
 * @param angle The angle to convert. Must be between 0 and 180.
 * @return The pulse length corresponding to the given angle.
 */
int angleToPulse(int angle)
{
  // Convert angle to servo pulse length
  return map(angle, 0, 180, SERVOMIN, SERVOMAX);
}