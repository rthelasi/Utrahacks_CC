#include <Servo.h>

// Pin Definitions
#define TRIG_PIN 0
#define ECHO_PIN 1
#define MOTOR1_A 6
#define MOTOR1_B 7
#define MOTOR2_A 9
#define MOTOR2_B 8
#define MOTOR1_EN 5
#define MOTOR2_EN 10
#define LED_PIN A5  // LED for blinking

// Color Sensor (TCS3200)
#define S0 A1
#define S1 A2
#define S2 A3
#define S3 A4
#define OUT_PIN A0
#define MAXMIN_VARIANCE 200

// Global Variables
int rFreq, gFreq, bFreq, maxFreq;
String detectedColor;
String targetSequence[] = {"Red", "Green", "Blue", "Green", "Blue"};
int sequenceStep = 0;
bool secondGreenFound = false, secondBlueFound = false;
struct Position { int x, y; };
Position robotPos = {0, 0};
Position movementHistory[100];
int movementCount = 0;

// Setup Function
void setup() {
    Serial.begin(9600);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(MOTOR1_EN, OUTPUT);
    pinMode(MOTOR2_EN, OUTPUT);
    pinMode(MOTOR1_A, OUTPUT);
    pinMode(MOTOR1_B, OUTPUT);
    pinMode(MOTOR2_A, OUTPUT);
    pinMode(MOTOR2_B, OUTPUT);
    pinMode(LED_PIN, OUTPUT);  // LED setup

    // Color Sensor Setup
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(OUT_PIN, INPUT);
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);
    
    Serial.println("--- Color Pattern Detection System ---");
}

// Main Loop
void loop() {
    if (detectWall()) {
        avoidWall();
    } else {
        moveForward();
        detectColor();
        checkColorSequence();
        detectLoop();
    }
    delay(500);
}

// Function to Detect Color
void detectColor() {
    digitalWrite(S2, LOW); digitalWrite(S3, LOW);
    rFreq = pulseIn(OUT_PIN, LOW);
    rFreq = map(rFreq, 500 - MAXMIN_VARIANCE, 950 + MAXMIN_VARIANCE, 255, 0);
    
    digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
    gFreq = pulseIn(OUT_PIN, LOW);
    gFreq = map(gFreq, 500 - MAXMIN_VARIANCE, 900 + MAXMIN_VARIANCE, 255, 0);
    
    digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
    bFreq = pulseIn(OUT_PIN, LOW);
    bFreq = map(bFreq, 600 - MAXMIN_VARIANCE, 930 + MAXMIN_VARIANCE, 255, 0);
    
    maxFreq = max(rFreq, max(bFreq, gFreq));
    if (maxFreq <= 0) {
        detectedColor = "Black";
    } else if (maxFreq == rFreq) {
        detectedColor = "Red";
    } else if (maxFreq == gFreq) {
        detectedColor = "Green";
    } else {
        detectedColor = "Blue";
    }
    
    Serial.print("Detected Color: "); Serial.println(detectedColor);
}

// Function to Check Color Sequence & Blink LED
void checkColorSequence() {
    if (detectedColor == targetSequence[sequenceStep]) {
        Serial.println("-- Correct color step!");
        
        // Ensure second green & blue are different tiles
        if (sequenceStep == 1) secondGreenFound = true;
        if (sequenceStep == 3 && secondGreenFound) return;
        if (sequenceStep == 4 && secondBlueFound) return;
        
        blinkLED();  // Blink LED when correct color detected
        
        sequenceStep++;
        if (sequenceStep >= 5) {
            Serial.println(">> SEQUENCE COMPLETE <<");
            sequenceStep = 0;
        }
    }
}

// Function to Blink LED
void blinkLED() {
    digitalWrite(LED_PIN, HIGH);
    delay(300);
    digitalWrite(LED_PIN, LOW);
}

// Function to Detect Walls
bool detectWall() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    long duration = pulseIn(ECHO_PIN, HIGH);
    int distance = duration * 0.034 / 2; // Convert to cm
    Serial.print("Wall Distance: "); Serial.println(distance);
    
    return (distance < 10);
}

// Function to Avoid Walls
void avoidWall() {
    Serial.println("Wall detected! Avoiding...");
    turnRight();
}

// Function to Move Forward
void moveForward() {
    Serial.println("Moving forward...");
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
    delay(500);
    updatePosition();
}

// Function to Detect Loop
void detectLoop() {
    Serial.print("Position: ("); Serial.print(robotPos.x); Serial.print(","); Serial.print(robotPos.y); Serial.println(")");
    
    for (int i = 0; i < movementCount - 1; i++) {
        if (movementHistory[i].x == robotPos.x && movementHistory[i].y == robotPos.y) {
            Serial.println("Loop detected! Changing path...");
            turnLeft();
            return;
        }
    }
}

// Function to Update Robot Position
void updatePosition() {
    robotPos.x++;
    movementHistory[movementCount] = robotPos;
    movementCount = (movementCount + 1) % 100;
}

// Function to Turn Right
void turnRight() {
    Serial.println("Turning right...");
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, HIGH);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
    delay(300);
}

// Function to Turn Left
void turnLeft() {
    Serial.println("Turning left...");
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, HIGH);
    delay(300);
}
