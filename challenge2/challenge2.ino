#define TRIG_PIN 7
#define ECHO_PIN 6

void setup() {
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    Serial.begin(9600);
}

void loop() {
    int distance = getDistance();

    if (distance < 10) {
        Serial.println("Wall detected, making decision");
        readColor();  // Apply color rule
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
    return duration * 0.034 / 2;  // Convert to cm
}
