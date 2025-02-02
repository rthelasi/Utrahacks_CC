
#include <stdlib.h>
#include <Wire.h>
#include <Servo.h>

// Color Sensor Pins
#define S0 A1
#define S1 A2
#define S2 A3
#define S3 A4
#define sensorOut A0
#define MAXMIN_VARIANCE 200

// Motor Pins
#define LEFT_MOTOR_FORWARD 5
#define LEFT_MOTOR_BACKWARD 6
#define RIGHT_MOTOR_FORWARD 9
#define RIGHT_MOTOR_BACKWARD 10

// Servo for Flag Drop
Servo flagServo;
#define SERVO_PIN 3

int rFreq, gFreq, bFreq, maxFreq;
String lastColor = "";
String seenColors[6];  // Store detected colors
int seenIndex = 0;

// Function to read color
String detectColor() {
  // Read Red
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  rFreq = pulseIn(sensorOut, LOW);
  rFreq = map(rFreq, 500 - MAXMIN_VARIANCE, 950 + MAXMIN_VARIANCE, 255, 0);

  // Read Green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  gFreq = pulseIn(sensorOut, LOW);
  gFreq = map(gFreq, 500 - MAXMIN_VARIANCE, 900 + MAXMIN_VARIANCE, 255, 0);

  // Read Blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  bFreq = pulseIn(sensorOut, LOW);
  bFreq = map(bFreq, 600 - MAXMIN_VARIANCE, 930 + MAXMIN_VARIANCE, 255, 0);

  // Determine Dominant Color
  maxFreq = max(rFreq, max(bFreq, gFreq));

  if (maxFreq <= 0) return "black";
  else if (maxFreq == rFreq) return "red";
  else if (maxFreq == gFreq) return "green";
  else return "blue";
}

// Motor Movement Functions
void moveForward() {
  analogWrite(LEFT_MOTOR_FORWARD, 150);
  analogWrite(RIGHT_MOTOR_FORWARD, 150);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
}

void turnLeft(int duration) {
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 150);
  analogWrite(LEFT_MOTOR_BACKWARD, 150);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  delay(duration);
}

void turnRight(int duration) {
  analogWrite(LEFT_MOTOR_FORWARD, 150);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, 150);
  delay(duration);
}

void stopRobot() {
  analogWrite(LEFT_MOTOR_FORWARD, 0);
  analogWrite(RIGHT_MOTOR_FORWARD, 0);
  analogWrite(LEFT_MOTOR_BACKWARD, 0);
  analogWrite(RIGHT_MOTOR_BACKWARD, 0);
}

void dropFlag() {
  flagServo.write(90);
  delay(1000);
  flagServo.write(0);
}

// **🔄 Black Detection and Color Searching Algorithm**
String findPath() {
  Serial.println("Scanning for Color...");
  
  int scanAngles[] = {-90, -45, 0, 45, 90};  // Angles to scan
  String detectedColors[5];  // Stores detected colors at different angles
  
  for (int i = 0; i < 5; i++) {
    stopRobot();
    if (scanAngles[i] < 0) {
      turnLeft(abs(scanAngles[i]) * 5);  // Rotate left
    } else if (scanAngles[i] > 0) {
      turnRight(scanAngles[i] * 5);  // Rotate right
    }
    
    delay(500);
    detectedColors[i] = detectColor();  // Read color
    Serial.print("Angle ");
    Serial.print(scanAngles[i]);
    Serial.print(": ");
    Serial.println(detectedColors[i]);
    
    delay(500);
  }
  
  // **Find the first non-black color**
  for (int i = 0; i < 5; i++) {
    if (detectedColors[i] != "black") {
      Serial.print("Best Direction Found: ");
      Serial.println(scanAngles[i]);
      
      if (scanAngles[i] < 0) {
        turnLeft(abs(scanAngles[i]) * 5);
      } else if (scanAngles[i] > 0) {
        turnRight(scanAngles[i] * 5);
      }
      return detectedColors[i];
    }
  }
  
  return "black";  // If all directions are black, stay put
}

void setup() {
  pinMode(LEFT_MOTOR_FORWARD, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD, OUTPUT);
  pinMode(RIGHT_MOTOR_FORWARD, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD, OUTPUT);
  
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  flagServo.attach(SERVO_PIN);
  flagServo.write(0);

  Serial.begin(9600);
}

void loop() {
  String currentColor = detectColor();
  Serial.print("Detected: ");
  Serial.println(currentColor);

  if (currentColor == "black") {
    Serial.println("Black Detected - Searching for Path...");
    currentColor = findPath();  // Find a new path if black
  }
  
  if (currentColor != lastColor) {
    Serial.println("New Color Detected - Moving Forward...");
    moveForward();
    lastColor = currentColor;
    
    seenColors[seenIndex] = currentColor;
    seenIndex++;
    delay(1000);
  } 
  else {
    Serial.println("Reached Center - Dropping Flag...");
    stopRobot();
    dropFlag();
    while (true);
  }

  delay(500);
}




