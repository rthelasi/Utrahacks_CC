// Motor Pins
#define MOTOR1_A 8
#define MOTOR1_B 9
#define MOTOR2_A 10
#define MOTOR2_B 11

// Color Sensor Pins
#define S0 A1
#define S1 A2
#define S2 A3
#define S3 A4
#define sensorOut A0
#define MAXMIN_VARIANCE 200

// Ultrasonic Sensor
#define TRIG_PIN 7
#define ECHO_PIN 6

// LED
#define LED_PIN A5

// Sequence Tracking
String targetSequence[] = {"Red", "Green", "Blue", "Green", "Blue"};
int sequenceStep = 0;

// Position Tracking
struct Position {
    int x;
    int y;
};
Position robotPos = {0, 0}; // Starting position
Position lastGreenPos = {-1, -1}; // Track last unique green
Position lastBluePos = {-1, -1};  // Track last unique blue

// Color Sensor Variables
int gFreq, rFreq, bFreq, maxFreq;

// Movement history to detect loops
Position movementHistory[100]; 
int movementCount = 0;

// Wall detection tracking
int consecutiveWallHits = 0;

void setup() {
    // Motor Setup
    pinMode(MOTOR1_A, OUTPUT);
    pinMode(MOTOR1_B, OUTPUT);
    pinMode(MOTOR2_A, OUTPUT);
    pinMode(MOTOR2_B, OUTPUT);
    
    // Ultrasonic Sensor
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    // Color Sensor
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(sensorOut, INPUT);

    digitalWrite(S0, HIGH);  // 20% frequency scaling
    digitalWrite(S1, LOW);
    
    // LED
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

        // If stuck in a loop, trigger pathfinding
        if (isLooping()) {
            Serial.println("Loop detected! Switching path.");
            avoidWall();
        }

        String color = detectColor();
        checkSequence(color);
    }
}

//----- Color Detection Functions -----//
String detectColor() {
    // Read Red Frequency
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    rFreq = pulseIn(sensorOut, LOW);
    rFreq = map(rFreq, 500 - MAXMIN_VARIANCE, 950 + MAXMIN_VARIANCE, 255, 0);

    // Read Green Frequency
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    gFreq = pulseIn(sensorOut, LOW);
    gFreq = map(gFreq, 500 - MAXMIN_VARIANCE, 900 + MAXMIN_VARIANCE, 255, 0);

    // Read Blue Frequency
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    bFreq = pulseIn(sensorOut, LOW);
    bFreq = map(bFreq, 600 - MAXMIN_VARIANCE, 930 + MAXMIN_VARIANCE, 255, 0);

    Serial.print("R="); Serial.print(rFreq);
    Serial.print(" G="); Serial.print(gFreq);
    Serial.print(" B="); Serial.println(bFreq);

    maxFreq = max(rFreq, max(bFreq, gFreq));

    if (maxFreq <= 0) {
        Serial.println("(black)");
        return "Unknown";
    } else if (maxFreq == rFreq) {
        Serial.println("(red)");
        return "Red";
    } else if (maxFreq == gFreq) {
        Serial.println("(green)");
        return "Green";
    } else {
        Serial.println("(blue)");
        return "Blue";
    }
}

//----- Motor Control Functions -----//
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

//----- Position Tracking -----//
void updatePosition() {
    robotPos.x += 1; // Move forward in X direction (adjust for real movement)
    
    // Save the new position in movement history
    movementHistory[movementCount++] = robotPos;
}

//----- Pathfinding & Obstacle Handling -----//
void avoidWall() {
    stopMotors();
    delay(500);
    
    consecutiveWallHits++;

    if (consecutiveWallHits > 3) { // If stuck, pick a random turn
        int turnDirection = random(0, 2); // Randomly choose left or right
        if (turnDirection == 0) {
            turnLeft();
        } else {
            turnRight();
        }
    } else {
        turnRight();
    }
    
    consecutiveWallHits = 0; // Reset after a turn
}

//----- Check for Duplicates -----//
bool isDuplicateColor(String color, Position pos) {
    if ((color == "Green" && pos.x == lastGreenPos.x && pos.y == lastGreenPos.y) ||
        (color == "Blue" && pos.x == lastBluePos.x && pos.y == lastBluePos.y)) {
        return true;
    }
    return false;
}

//----- Check for Loops -----//
bool isLooping() {
    int count = 0;
    for (int i = 0; i < movementCount; i++) {
        if (movementHistory[i].x == robotPos.x && movementHistory[i].y == robotPos.y) {
            count++;
            if (count > 3) return true;
        }
    }
    return false;
}

//----- Check Sequence & Validate Unique Colors -----//
void checkSequence(String color) {
    if (color == targetSequence[sequenceStep]) {
        if (isDuplicateColor(color, robotPos)) return;

        blinkLED();
        
        if (color == "Green") lastGreenPos = robotPos;
        if (color == "Blue") lastBluePos = robotPos;

        sequenceStep++;
        
        if (sequenceStep >= 5) {
            stopMotors();
            while(1);
        }
    }
}

//----- Utilities -----//
int getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    return pulseIn(ECHO_PIN, HIGH) * 0.034 / 2;
}

void blinkLED() {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
}

//----- Turning Functions -----//
void turnRight() {
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, HIGH);
    delay(500);
    stopMotors();
}

void turnLeft() {
    digitalWrite(MOTOR1_A, LOW);
    digitalWrite(MOTOR1_B, HIGH);
    digitalWrite(MOTOR2_A, HIGH);
    digitalWrite(MOTOR2_B, LOW);
    delay(500);
    stopMotors();
}
