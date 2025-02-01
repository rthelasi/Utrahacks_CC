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

// Color Definitions (Calibrate these values based on your environment)
#define RED_THRESHOLD 150
#define GREEN_THRESHOLD 150
#define BLUE_THRESHOLD 150
#define ORANGE_THRESHOLD 150

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
            turnRight();
        } else if (color == "Green") {
            turnLeft();
        } else if (color == "Blue") {
            uTurn();
        } else {
            // Default action if color not recognized
            turnRight();
        }
    } else {
        moveForward();
    }
}

// Ultrasonic Sensor Distance Measurement
int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    int duration = pulseIn(ECHO_PIN, HIGH);
    return duration * 0.034 / 2;
}

// Color Detection Function
String detectColor() {
    // Detect Red
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    int red = pulseIn(OUT_PIN, LOW);
    
    // Detect Green
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    int green = pulseIn(OUT_PIN, LOW);
    
    // Detect Blue
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    int blue = pulseIn(OUT_PIN, LOW);
    
    // Determine dominant color (calibration needed)
    if (red < RED_THRESHOLD && green > GREEN_THRESHOLD && blue > BLUE_THRESHOLD) return "Red";
    if (green < GREEN_THRESHOLD && red > RED_THRESHOLD && blue > BLUE_THRESHOLD) return "Green";
    if (blue < BLUE_THRESHOLD && red > RED_THRESHOLD && green > GREEN_THRESHOLD) return "Blue";
    if (red < ORANGE_THRESHOLD && green < ORANGE_THRESHOLD) return "Orange"; // Example for orange
    return "Unknown";
}

// Motor Control Functions
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
    delay(400);
    stopMotors();
}

void uTurn() {
    // Perform a 180-degree turn
    turnLeft();
    delay(400); // Additional delay for full U-turn
    turnLeft();
}

void stopMotors() {
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, LOW);
}