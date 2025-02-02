#include <Servo.h>

#define TRIG_PIN 7
#define ECHO_PIN 6
#define MOTOR1_A 8
#define MOTOR1_B 9
#define MOTOR2_A 10
#define MOTOR2_B 11

// Color Sensor Pins
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define OUT_PIN A0
#define MAXMIN_VARIANCE 200

int rFreq, gFreq, bFreq, maxFreq;

void setup() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
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
    
    if (distance < 10) { // Wall detected
        Serial.println("Wall/Boundary detected");
        stopMotors();
        delay(500);
        
        // Read color and perform action
        String color = detectColor();
        Serial.print("Detected Color: ");
        Serial.println(color);
        
        if (color == "Red") {
            uTurn();
        } else if (color == "Green") {
            turnLeft();
        } else if (color == "Blue") {
            turnRight();
        }
    } else {
        moveForward();
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
}

void moveForward() {
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
}

void turnRight() {
    // Turn 90 degrees (calibrate delay as needed)
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, HIGH);
    delay(400); // Adjust based on motor speed
    stopMotors();
}

void turnLeft() {
    // Turn 90 degrees
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, HIGH);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
    delay(400); // Adjust based on motor speed
    stopMotors();
}

void uTurn() {
    // Perform a 180-degree turn (calibrate delay as needed)
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, HIGH);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
    delay(800); // Adjust based on motor speed for 180-degree turn
    stopMotors();
}