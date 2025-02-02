#include <stdlib.h>

// Motor Pins
#define MOTOR1_A 6
#define MOTOR1_B 7
#define MOTOR2_A 9
#define MOTOR2_B 8
#define MOTOR1_EN 5
#define MOTOR2_EN 10

// Ultrasonic Sensor
#define TRIG_PIN 0
#define ECHO_PIN 1

// Color Sensor
#define S0 A1
#define S1 A2
#define S2 A3
#define S3 A4
#define sensorOut A0
#define MAXMIN_VARIANCE 200

// LED
#define LED_PIN A5

// Sequence & Position Tracking
String targetSequence[] = {"Red", "Green", "Blue", "Green", "Blue"};
int sequenceStep = 0;

struct Position { int x, y; };
Position robotPos = {0, 0};
Position lastGreenPos = {-1, -1}, lastBluePos = {-1, -1};
Position movementHistory[100];
int movementCount = 0, consecutiveWallHits = 0;

// Color Frequency Variables
int rFreq, gFreq, bFreq, maxFreq;

void setup() {
    pinMode(MOTOR1_A, OUTPUT); pinMode(MOTOR1_B, OUTPUT);
    pinMode(MOTOR2_A, OUTPUT); pinMode(MOTOR2_B, OUTPUT);
    pinMode(MOTOR1_EN, OUTPUT); pinMode(MOTOR2_EN, OUTPUT);
    
    pinMode(TRIG_PIN, OUTPUT); pinMode(ECHO_PIN, INPUT);
    
    pinMode(S0, OUTPUT); pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT); pinMode(S3, OUTPUT);
    pinMode(sensorOut, INPUT);
    digitalWrite(S0, HIGH); digitalWrite(S1, LOW); // 20% scaling
    
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    int distance = getDistance();
    
    if (distance < 10) {
        avoidWall();
    } else {
        moveForward();
        updatePosition();
        
        if (isLooping()) {
            Serial.println("Loop detected! Adjusting path.");
            avoidWall();
        }

        String color = detectColor();
        checkSequence(color);
    }
}

//----- Motor Control -----//
void moveForward() {
    digitalWrite(MOTOR1_A, HIGH); digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, HIGH); digitalWrite(MOTOR2_B, LOW);
    analogWrite(MOTOR1_EN, 150); analogWrite(MOTOR2_EN, 150);
}

void stopMotors() {
    digitalWrite(MOTOR1_A, LOW); digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW); digitalWrite(MOTOR2_B, LOW);
    analogWrite(MOTOR1_EN, 0); analogWrite(MOTOR2_EN, 0);
}

void turnRight() {
    digitalWrite(MOTOR1_A, HIGH); digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW); digitalWrite(MOTOR2_B, HIGH);
    analogWrite(MOTOR1_EN, 150); analogWrite(MOTOR2_EN, 150);
    delay(900);
    stopMotors();
}

void turnLeft() {
    digitalWrite(MOTOR1_A, LOW); digitalWrite(MOTOR1_B, HIGH);
    digitalWrite(MOTOR2_A, HIGH); digitalWrite(MOTOR2_B, LOW);
    analogWrite(MOTOR1_EN, 150); analogWrite(MOTOR2_EN, 150);
    delay(900);
    stopMotors();
}

//----- Smart Wall Avoidance (Turns Toward Open Space) -----//
void avoidWall() {
    stopMotors();
    delay(500);
    
    int leftDistance = checkLeftDistance();
    int rightDistance = checkRightDistance();

    if (leftDistance > rightDistance) {
        Serial.println("Turning Left (More Space)");
        turnLeft();
    } else {
        Serial.println("Turning Right (More Space)");
        turnRight();
    }
    
    consecutiveWallHits = 0;
}

//----- Check Distance in Left & Right Directions -----//
int checkLeftDistance() {
    turnLeft();
    delay(500);
    int distance = getDistance();
    turnRight();
    delay(500);
    return distance;
}

int checkRightDistance() {
    turnRight();
    delay(500);
    int distance = getDistance();
    turnLeft();
    delay(500);
    return distance;
}

//----- Color Detection with Maximum Variance Mapping -----//
String detectColor() {
    digitalWrite(S2, LOW); digitalWrite(S3, LOW);
    rFreq = pulseIn(sensorOut, LOW);
    rFreq = map(rFreq, 500 - MAXMIN_VARIANCE, 950 + MAXMIN_VARIANCE, 255, 0);
    
    digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
    gFreq = pulseIn(sensorOut, LOW);
    gFreq = map(gFreq, 500 - MAXMIN_VARIANCE, 900 + MAXMIN_VARIANCE, 255, 0);
    
    digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
    bFreq = pulseIn(sensorOut, LOW);
    bFreq = map(bFreq, 600 - MAXMIN_VARIANCE, 930 + MAXMIN_VARIANCE, 255, 0);
    
    maxFreq = max(rFreq, max(gFreq, bFreq));
    if (maxFreq <= 50) return "Unknown";
    return (maxFreq == rFreq) ? "Red" : (maxFreq == gFreq) ? "Green" : "Blue";
}

//----- Tracking & Sequence -----//
void updatePosition() {
    robotPos.x++;
    movementHistory[movementCount++] = robotPos;
    if (movementCount >= 100) movementCount = 0;
}

bool isLooping() {
    for (int i = 0; i < movementCount - 1; i++) {
        if (movementHistory[i].x == robotPos.x && movementHistory[i].y == robotPos.y)
            return true;
    }
    return false;
}

void checkSequence(String color) {
    if (color == targetSequence[sequenceStep] && !isDuplicate(color)) {
        if (color == "Green") lastGreenPos = robotPos;
        else if (color == "Blue") lastBluePos = robotPos;
        blinkLED();
        sequenceStep = (sequenceStep >= 4) ? 0 : sequenceStep + 1;
    }
}

bool isDuplicate(String color) {
    return (color == "Green" && robotPos.x == lastGreenPos.x) || 
           (color == "Blue" && robotPos.x == lastBluePos.x);
}

void blinkLED() {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
}

//----- Ultrasonic Sensor -----//
int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
}
