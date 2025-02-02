#define MOTOR1_IN1 6
#define MOTOR1_IN2 7
#define MOTOR2_IN1 9
#define MOTOR2_IN2 8
#define MOTOR1_ENA 5
#define MOTOR2_ENB 10

#define TRIG_PIN 1
#define ECHO_PIN 0

void setup() {
    // Motor Setup
    pinMode(MOTOR1_IN1, OUTPUT);
    pinMode(MOTOR1_IN2, OUTPUT);
    pinMode(MOTOR2_IN1, OUTPUT);
    pinMode(MOTOR2_IN2, OUTPUT);
    pinMode(MOTOR1_ENA, OUTPUT);
    pinMode(MOTOR2_ENB, OUTPUT);
    
    // Ultrasonic Sensor Setup
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    Serial.begin(9600);
}

void loop() {
    int distance = getDistance();
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
    
    if (distance < 15) {
        avoidWall();
    } else {
        moveForward();
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

void avoidWall() {
    // Stop the robot
    stopMotors();
    delay(500);
    
    // Turn right
    turnRight(500); // Adjust the delay to achieve a 90-degree turn
}

void moveForward() {
    digitalWrite(MOTOR1_IN1, HIGH);
    digitalWrite(MOTOR1_IN2, LOW);
    digitalWrite(MOTOR2_IN1, HIGH);
    digitalWrite(MOTOR2_IN2, LOW);
    analogWrite(MOTOR1_ENA, 150); // Adjust speed as needed
    analogWrite(MOTOR2_ENB, 150); // Adjust speed as needed
}

void stopMotors() {
    digitalWrite(MOTOR1_IN1, LOW);
    digitalWrite(MOTOR1_IN2, LOW);
    digitalWrite(MOTOR2_IN1, LOW);
    digitalWrite(MOTOR2_IN2, LOW);
    analogWrite(MOTOR1_ENA, 0);
    analogWrite(MOTOR2_ENB, 0);
}

void turnRight(int duration) {
    digitalWrite(MOTOR1_IN1, HIGH);
    digitalWrite(MOTOR1_IN2, LOW);
    digitalWrite(MOTOR2_IN1, LOW);
    digitalWrite(MOTOR2_IN2, HIGH);
    analogWrite(MOTOR1_ENA, 150); // Adjust speed as needed
    analogWrite(MOTOR2_ENB, 150); // Adjust speed as needed
    delay(duration);
    stopMotors();
}