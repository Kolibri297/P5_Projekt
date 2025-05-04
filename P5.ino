//Treiber Instalieren (Port + Bord Treiber)
//Richtiges Bord und Port auswelen
//Einfügen der Serbiblotoeck für den ESP32 (Servo.h ist nicht kompatibel mit dem ESP32)
#include <ESP32Servo.h>

// Erstellt Objekt Servomotoren zum Controlieren im programm
Servo servoHorizont;
Servo servoVertikal;

//Zuweisung der LDR Sensoren zu einem Pin
//Const ersetzt #define (Const erstellt genau wie defin eine Konstante nur das const auch bei Arrays funktioniert und defin nicht.)
const int LDR_Links   = 32;    
const int LDR_Rechts  = 33;   
const int LDR_Oben    = 34;     
const int LDR_Unten   = 35;

// Legt die Startposition (0-180) der servomotoren fest.
int posHorizontal = 90;      
int posVertical = 90;

void setup() {
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

void loop(){
 int lichtLinks = analogRead(LDR_Links);
    int lichtRechts = analogRead(LDR_Rechts);
    int lichtOben = analogRead(LDR_Oben);
    int lichtUnten = analogRead(LDR_Unten);

    // Horizontale Anpassung
    if (lichtLinks > lichtRechts + 10) {
        posHorizontal = constrain(posHorizontal - 1, 0, 180);
    } else if (lichtRechts > lichtLinks + 10) {
        posHorizontal = constrain(posHorizontal + 1, 0, 180);
    }

    // Vertikale Anpassung
    if (lichtOben > lichtUnten + 10) {
        posVertical = constrain(posVertical + 1, 0, 180);
    } else if (lichtUnten > lichtOben + 10) {
        posVertical = constrain(posVertical - 1, 0, 180);
    }

    // Servos bewegen
    servoHorizont.write(posHorizontal);
    servoVertikal.write(posVertical);

    // Debugging
    Serial.print("Horizont: "); Serial.print(posHorizontal);
    Serial.print(" Vertikal: "); Serial.println(posVertical);

    delay(50);
}

