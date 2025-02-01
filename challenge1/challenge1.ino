#include <Servo.h>

// Motor Control Pins
#define MOTOR1_A 8
#define MOTOR1_B 9
#define MOTOR2_A 10
#define MOTOR2_B 11

// TCS230 Color Sensor Pins
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define OUT_PIN A0

// Flag Mechanism
#define SERVO_PIN 12
Servo flagServo;

// Navigation State
int ringCount = 0;
bool flagDropped = false;

void setup() {
    // Initialize Motors
    pinMode(MOTOR1_A, OUTPUT);
    pinMode(MOTOR1_B, OUTPUT);
    pinMode(MOTOR2_A, OUTPUT);
    pinMode(MOTOR2_B, OUTPUT);

    // Configure Color Sensor
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    digitalWrite(S0, HIGH);  // 20% frequency scaling
    digitalWrite(S1, LOW);

    // Attach Flag Servo
    flagServo.attach(SERVO_PIN);
    flagServo.write(0);  // Initial position: flag held

    // Initial U-Turn (robot starts facing away from center)
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, HIGH);
    delay(850);  // Calibrate for 180° turn
    stopMotors();

    Serial.begin(9600);
}

void loop() {
    if(flagDropped) return;  // Stop after success

    String color = detectColor();
    Serial.print("Ring: "); Serial.print(ringCount);
    Serial.print(" | Color: "); Serial.println(color);

    // Center detection logic
    if(ringCount >= 6 && color == "Blue") {
        stopMotors();
        flagServo.write(90);  // Drop flag
        flagDropped = true;
        while(1);  // Permanent stop
    }

    // New ring detected
    if(color != "Unknown" && color != "White") {
        ringCount++;
        adjustPath(color);
        delay(300);  // Stabilization pause
    }
    
    moveForward();
}

//--------------------------------------------------
// Color Detection (Threshold-Free)
//--------------------------------------------------
String detectColor() {
    // Read raw RGB values
    float red = pulseInWithFilter(LOW, LOW);    // Red filter
    float green = pulseInWithFilter(HIGH, HIGH);// Green filter
    float blue = pulseInWithFilter(LOW, HIGH);  // Blue filter

    // Normalize values
    float total = red + green + blue;
    float r = red/total;
    float g = green/total;
    float b = blue/total;

    // Dynamic ratio-based detection
    if(r > 0.4 && r > g && r > b) return "Red";
    if(g > 0.4 && g > r && g > b) return "Green";
    if(b > 0.4 && b > r && b > g) return "Blue";
    return "Unknown";
}

float pulseInWithFilter(int s2, int s3) {
    digitalWrite(S2, s2);
    digitalWrite(S3, s3);
    return pulseIn(OUT_PIN, LOW);  // Lower value = more light
}

//--------------------------------------------------
// Movement Functions
//--------------------------------------------------
void adjustPath(String color) {
    if(color == "Red") {
        // Left turn to spiral inward
        digitalWrite(MOTOR1_A, LOW);
        digitalWrite(MOTOR1_B, HIGH);
        digitalWrite(MOTOR2_A, HIGH);
        digitalWrite(MOTOR2_B, LOW);
        delay(280);  // Calibrate for ~30° turn
    }
    else if(color == "Green") {
        // Right turn to spiral inward
        digitalWrite(MOTOR1_A, HIGH);
        digitalWrite(MOTOR1_B, LOW);
        digitalWrite(MOTOR2_A, LOW);
        digitalWrite(MOTOR2_B, HIGH);
        delay(280);
    }
    stopMotors();
}

void moveForward() {
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
}

void stopMotors() {
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, LOW);
}