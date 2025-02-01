#define LED_PIN 13

const char* sequence[] = {"Red", "Green", "Blue", "Green", "Blue"};
int step = 0;
int lastGreen = -1;
int lastBlue = -1;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    Serial.begin(9600);
}

void loop() {
    readColor();

    if (colorMatches(sequence[step])) {
        Serial.print(sequence[step]);
        Serial.println(" detected, moving to next step");
       
        if (step == 1 || step == 3) {
            if (lastGreen == getTileID()) return;  // Ensure different tile
            lastGreen = getTileID();
        }
       
        if (step == 2 || step == 4) {
            if (lastBlue == getTileID()) return;  // Ensure different tile
            lastBlue = getTileID();
        }
       
        blinkLED();
        step++;
    }

    moveForward();
}

bool colorMatches(const char* color) {
    if (strcmp(color, "Red") == 0 && red > green && red > blue) return true;
    if (strcmp(color, "Green") == 0 && green > red && green > blue) return true;
    if (strcmp(color, "Blue") == 0 && blue > red && blue > green) return true;
    return false;
}

void blinkLED() {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
}

int getTileID() {
    return random(1, 10);  // Simulate tile ID
}