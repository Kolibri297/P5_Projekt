// Einfügen der Biblothek für den ESP32
#include <Arduino.h> //Standardbibliothek des ESP32
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN 150
#define SERVOMAX 600

// Initialisiert der dafruit_PWMServoDriver klasse
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Number of servos you have
const int servoCount = 2;
// Define the PCA9685 pins for your servos
const int servoPins[servoCount] = {0, 3}; // Todo: Anstatt 0,1 die Richtigen Pins.
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
int positionHorizontal = 90;
int positionVertical = 90;

//! Function declarations (prototypes)
void moveServoSlow(int servoIndex, int targetAngle); // Funktion zum langsamen Bewegen des Servos
void setServoAngle(int servoIndex, int angle); // Funktion zum direkten Setzen der Servoposition
int angleToPulse(int angle);

// Die setup() Funktion wird nur einmal am Anfang des Programms aufgerufen.
void setup()
{
  Serial.begin(115200);

  // Initialisiert die Servomotoren
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // Set to 27MHz for better accuracy
  pwm.setPWMFreq(50);  // Set PWM frequency to 50Hz

  // Setzt die Servomotoren auf die Startpositionen
  for (int i = 0; i < servoCount; i++)
  {
    servoAngles[i] = defaultPositions[i];
    setServoAngle(i, servoAngles[i]);
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
    // Werte der Lichtabhängigen Widerstände auslesen
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

    // Servos langsam bewegen (nur wenn sich Position geändert hat)
    if (servoAngles[0] != positionHorizontal) {
      moveServoSlow(0, positionHorizontal);
    }
    if (servoAngles[1] != positionVertical) {
      moveServoSlow(1, positionVertical);
    }

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
 * @brief Set servo angle directly (0-180 degrees)
 * @param servoIndex The index of the servo to move (0 or 1)
 * @param angle The angle to set the servo to
 */
void setServoAngle(int servoIndex, int angle) {
  // Constrain angle to valid range
  angle = constrain(angle, 0, 180);
  
  // Map angle to pulse width
  int pulseWidth = angleToPulse(angle);
  
  // Set PWM
  pwm.setPWM(servoPins[servoIndex], 0, pulseWidth);
}

/**
 * @brief Smoothly move the servo to the target angle with slow movement
 * @param servoIndex The index of the servo to move (0 or 1)
 * @param targetAngle The target angle to move the servo to
 */
void moveServoSlow(int servoIndex, int targetAngle)
{
  int currentAngle = servoAngles[servoIndex];
  
  // Move servo step by step with 50ms delay for slower movement
  if (currentAngle < targetAngle) {
    for (int angle = currentAngle; angle <= targetAngle; angle++) {
      setServoAngle(servoIndex, angle);
      servoAngles[servoIndex] = angle;
      delay(50);  // 50ms delay for slower movement (like in reference code)
    }
  } else if (currentAngle > targetAngle) {
    for (int angle = currentAngle; angle >= targetAngle; angle--) {
      setServoAngle(servoIndex, angle);
      servoAngles[servoIndex] = angle;
      delay(50);  // 50ms delay for slower movement (like in reference code)
    }
  }
  
  // Update final position
  servoAngles[servoIndex] = targetAngle;
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