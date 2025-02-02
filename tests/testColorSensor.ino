#include <stdlib.h>
#define S0 A1
#define S1 A2
#define S2 A3
#define S3 A4
#define sensorOut A0
#define MAXMIN_VARIANCE 200

int gFreq;
int rFreq;
int bFreq;
int maxFreq;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  
  // Setting frequency-scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  
  Serial.begin(9600);
}

void loop() {
  // Setting red filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Reading the output frequency
  rFreq = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  rFreq = map(rFreq, 500 - MAXMIN_VARIANCE,950 + MAXMIN_VARIANCE,255,0);
  // Printing the value on the serial monitor
  Serial.print("R= ");//printing name
  Serial.print(rFreq);//printing RED color frequency
  Serial.print("  ");
  delay(100);

  // Setting Green filtered photodiodes to be read
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  gFreq = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  gFreq = map(gFreq, 500 - MAXMIN_VARIANCE,900 + MAXMIN_VARIANCE,255,0);
  // Printing the value on the serial monitor
  Serial.print("G= ");//printing name
  Serial.print(gFreq);//printing RED color frequency
  Serial.print("  ");
  delay(100);

  // Setting Blue filtered photodiodes to be read
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Reading the output frequency
  bFreq = pulseIn(sensorOut, LOW);
  //Remaping the value of the frequency to the RGB Model of 0 to 255
  bFreq = map(bFreq, 600 - MAXMIN_VARIANCE,930 + MAXMIN_VARIANCE,255,0);
  // Printing the value on the serial monitor
  Serial.print("B= ");//printing name
  Serial.print(bFreq);//printing RED color frequency
  Serial.print("  ");

  maxFreq = max(rFreq, max(bFreq, gFreq));
  if (maxFreq <= 0){
    Serial.print("(black)\n");
  }
  else if (maxFreq == rFreq) {
    Serial.print("(red)\n");
  }
  else if (maxFreq == gFreq) {
    Serial.print("(green)\n");
  }
  else {
    Serial.print("(blue)\n");
  }

  

  delay(1000);
}
