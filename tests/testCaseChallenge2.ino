#define TRIG_PIN 7
#define ECHO_PIN 6
#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define OUT_PIN A0

void setup() {
    // Ultrasonic Sensor Setup
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
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
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    if (distance <= 20) {
        String color = detectColor();
        Serial.print("Detected Color: ");
        Serial.println(color);
    }
    delay(1000);  // Delay for readability
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

String detectColor() {
    int red = readColor('R');
    int green = readColor('G');
    int blue = readColor('B');

    Serial.print("R:"); Serial.print(red);
    Serial.print(" G:"); Serial.print(green);
    Serial.print(" B:"); Serial.println(blue);

    if (red > green && red > blue) return "Red";
    if (green > red && green > blue) return "Green";
    if (blue > red && blue > green) return "Blue";
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