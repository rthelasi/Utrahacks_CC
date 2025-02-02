#define TRIG_PIN 0
#define ECHO_PIN 1

void setup() {
    // Ultrasonic Sensor Setup
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    
    Serial.begin(9600);
}

void loop() {
    int distance = getDistance();
    unsigned long currentTime = millis();
    Serial.print("Time: ");
    Serial.print(currentTime);
    Serial.print(" ms | Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
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