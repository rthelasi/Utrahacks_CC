//#include <Servo.h>
#include <stdlib.h>

#define TRIG_PIN 0
#define ECHO_PIN 1
#define MOTOR1_A 6
#define MOTOR1_B 7
#define MOTOR2_A 9
#define MOTOR2_B 8
#define MOTOR1_EN 5
#define MOTOR2_EN 10

// Color Sensor Pins
#define S0 A1
#define S1 A2
#define S2 A3
#define S3 A4
#define OUT_PIN A0
#define MAXMIN_VARIANCE 200

int rFreq, gFreq, bFreq, maxFreq;

void setup() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(MOTOR1_EN, OUTPUT);
    pinMode(MOTOR2_EN, OUTPUT);
    pinMode(MOTOR1_A, OUTPUT);
    pinMode(MOTOR1_B, OUTPUT);
    pinMode(MOTOR2_A, OUTPUT);
    pinMode(MOTOR2_B, OUTPUT);
    
    // Color Sensor Setup
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(OUT_PIN, INPUT);
    // Set frequency scaling to 20%
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    
    Serial.begin(9600);
}

void loop() {
    int distance = getDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    if (distance < 30) { // Wall detected
        Serial.println("Wall/Boundary detected");
        stopMotors();
        delay(500);
        
        // Read color and perform action
        String color = detectColor();
        Serial.print("Detected Color: ");
        Serial.println(color);
        
        if (color == "Red") {
            turnRight();
            turnRight();
            Serial.print("u-turn\n");
        } else if (color == "Green") {
            turnRight();
            Serial.print("right turn\n");
        } else if (color == "Blue") {
            turnLeft();
            Serial.print("left turn\n");
        }
    } else {
        moveForward();
        Serial.print("move forward1\n");
        delay(500);
    }
}

String detectColor() {
    // Setting red filtered photodiodes to be read
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    rFreq = pulseIn(OUT_PIN, LOW);
    rFreq = map(rFreq, 500 - MAXMIN_VARIANCE, 950 + MAXMIN_VARIANCE, 255, 0);

    // Setting green filtered photodiodes to be read
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    gFreq = pulseIn(OUT_PIN, LOW);
    gFreq = map(gFreq, 500 - MAXMIN_VARIANCE, 900 + MAXMIN_VARIANCE, 255, 0);

    // Setting blue filtered photodiodes to be read
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    bFreq = pulseIn(OUT_PIN, LOW);
    bFreq = map(bFreq, 600 - MAXMIN_VARIANCE, 930 + MAXMIN_VARIANCE, 255, 0);

    maxFreq = max(rFreq, max(bFreq, gFreq));
    if (maxFreq <= 0) {
        return "Black";
    } else if (maxFreq == rFreq) {
        return "Red";
    } else if (maxFreq == gFreq) {
        return "Green";
    } else {
        return "Blue";
    }
}

int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    int duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2;
}

void stopMotors() {
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, LOW);
    analogWrite(MOTOR1_EN, 0);
    analogWrite(MOTOR2_EN, 0);
}

void moveForward() {
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
    analogWrite(MOTOR1_EN, 150);
    analogWrite(MOTOR2_EN, 150);
}

void turnRight() {
    // Turn 90 degrees (calibrate delay as needed)
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, HIGH);
    analogWrite(MOTOR1_EN, 150);
    analogWrite(MOTOR2_EN, 150);
    delay(900); // Adjust based on motor speed
    stopMotors();
}

void turnLeft() {
    // Turn 90 degrees
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, HIGH);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
    analogWrite(MOTOR1_EN, 150);
    analogWrite(MOTOR2_EN, 150);
    delay(900); // Adjust based on motor speed
    stopMotors();
}
