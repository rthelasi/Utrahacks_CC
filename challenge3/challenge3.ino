// Motor Pins
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

// Ultrasonic Sensor
#define TRIG_PIN 7
#define ECHO_PIN 6

// LED
#define LED_PIN A1

// Sequence Tracking
String targetSequence[] = {"Red", "Green", "Blue", "Green", "Blue"};
int sequenceStep = 0;
String lastGreenID = "";
String lastBlueID = "";

// Color Detection Thresholds (CALIBRATE THESE!)
int redMin = 25, redMax = 85;
int greenMin = 30, greenMax = 90;
int blueMin = 20, blueMax = 70;

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
        String color = detectColor();
        checkSequence(color);
        moveForward();
    }
}

//----- Color Detection Functions -----//
String detectColor() {
    int red = readColor('R');
    int green = readColor('G');
    int blue = readColor('B');

    Serial.print("R:"); Serial.print(red);
    Serial.print(" G:"); Serial.print(green);
    Serial.print(" B:"); Serial.println(blue);

    if (red > green && red > blue && red >= redMin && red <= redMax) return "Red";
    if (green > red && green > blue && green >= greenMin && green <= greenMax) return "Green";
    if (blue > red && blue > green && blue >= blueMin && blue <= blueMax) return "Blue";
    return "Unknown";
}

int readColor(char color) {
    switch(color) {
        case 'R':
            digitalWrite(S2, LOW); digitalWrite(S3, LOW);
            break;
        case 'G':
            digitalWrite(S2, HIGH); digitalWrite(S3, HIGH);
            break;
        case 'B':
            digitalWrite(S2, LOW); digitalWrite(S3, HIGH);
            break;
    }
    return pulseIn(OUT_PIN, LOW);
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

//----- Core Logic -----//
void avoidWall() {
    stopMotors();
    delay(500);
    // Right turn
    digitalWrite(MOTOR1_A, HIGH);
    digitalWrite(MOTOR1_B, LOW);
    digitalWrite(MOTOR2_A, LOW);
    digitalWrite(MOTOR2_B, HIGH);
    delay(400);  // Adjust for 90° turn
    stopMotors();
}

void checkSequence(String color) {
    if (color == targetSequence[sequenceStep]) {
        if ((sequenceStep == 3 || sequenceStep == 4) && !validateUnique(color)) return;
        
        blinkLED();
        sequenceStep++;
        if (sequenceStep >= 5) {
            stopMotors();
            while(1); // Mission complete
        }
    }
}

bool validateUnique(String color) {
    String currentID = String(millis()); // Unique timestamp ID
    
    if (sequenceStep == 3 && color == "Green") {
        if (currentID == lastGreenID) return false;
        lastGreenID = currentID;
    }
    
    if (sequenceStep == 4 && color == "Blue") {
        if (currentID == lastBlueID) return false;
        lastBlueID = currentID;
    }
    return true;
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