
#include <Servo.h>

#define S2 4
#define S3 5
#define sensorOut 6
#define MOTOR1_A 8
#define MOTOR1_B 9
#define MOTOR2_A 10
#define MOTOR2_B 11
#define SERVO_PIN 3

Servo flagServo;
int red, green, blue;

void setup() {
    Serial.begin(9600);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(sensorOut, INPUT);
    pinMode(MOTOR1_A, OUTPUT);
    pinMode(MOTOR1_B, OUTPUT);
    pinMode(MOTOR2_A, OUTPUT);
    pinMode(MOTOR2_B, OUTPUT);
   
    flagServo.attach(SERVO_PIN);
    flagServo.write(0);
}

void loop() {
    readColor();
   
    if (red > green && red > blue) {
        Serial.println("Red detected, turning right");
        moveRight();
    } else if (green > red && green > blue) {
        Serial.println("Green detected, going forward");
        moveForward();
    } else if (blue > red && blue > green) {
        Serial.println("Blue detected, turning left");
        moveLeft();
    }

    if (atCenter()) {
        dropFlag();
    }
}

void readColor() {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    red = pulseIn(sensorOut, LOW);
   
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    green = pulseIn(sensorOut, LOW);

    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    blue = pulseIn(sensorOut, LOW);
}

void moveForward() {
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
}

void moveLeft() {
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, HIGH);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
}

void moveRight() {
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, HIGH);
}

bool atCenter() {
    return (red > 200 && green > 200 && blue > 200);
}

void dropFlag() {
    Serial.println("Dropping flag!");
    flagServo.write(90);
    delay(2000);
    flagServo.write(0);
}

