// Smart Stick for the Visually Impaired
// Arduino Mega + DFPlayer Mini + Sensors
// Author: Asia Belal's Project

#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// ----------- Pin Definitions -------------
#define trigPin 7       // Ultrasonic trigger
#define echoPin 6       // Ultrasonic echo
#define irLeft A0       // Left IR sensor
#define irRight A1      // Right IR sensor
#define waterPin A2     // Water detection sensor

#define vibrationPin 8  // Vibration motor

// Push buttons
#define btnFront 22     // Button for ultrasonic (front obstacle)
#define btnLeft 23      // Button for left obstacle
#define btnRight 24     // Button for right obstacle
#define btnWater 25     // Button for water detection

// DFPlayer Serial
SoftwareSerial mySerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// ----------- Variables -------------
long duration;
int distance;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  // Ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Sensors
  pinMode(irLeft, INPUT);
  pinMode(irRight, INPUT);
  pinMode(waterPin, INPUT);

  // Outputs
  pinMode(vibrationPin, OUTPUT);

  // Buttons
  pinMode(btnFront, INPUT_PULLUP);
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  pinMode(btnWater, INPUT_PULLUP);

  // Initialize DFPlayer
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini not detected!");
    while (true);
  }
  myDFPlayer.volume(25);  // Volume (0–30)
  Serial.println("Smart Stick Initialized!");
}

void loop() {
  // ----- Ultrasonic Distance -----
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // convert to cm

  // ----- Sensor States -----
  bool frontObstacle = (distance > 0 && distance < 60); // front obstacle if < 60cm
  bool leftObstacle  = (analogRead(irLeft) < 500);      // IR detects object
  bool rightObstacle = (analogRead(irRight) < 500);
  bool waterDetected = (analogRead(waterPin) > 300);    // adjust threshold

  // ----- Vibration Motor (automatic) -----
  if (frontObstacle || leftObstacle || rightObstacle || waterDetected) {
    digitalWrite(vibrationPin, HIGH); // Vibrate
  } else {
    digitalWrite(vibrationPin, LOW);
  }

  // ----- Speaker Alerts (manual button press + obstacle condition) -----
  if (digitalRead(btnFront) == LOW && frontObstacle) {
    playMessage(1); // 0001.mp3
  }
  if (digitalRead(btnLeft) == LOW && leftObstacle) {
    playMessage(2); // 0002.mp3
  }
  if (digitalRead(btnRight) == LOW && rightObstacle) {
    playMessage(3); // 0003.mp3
  }
  if (digitalRead(btnWater) == LOW && waterDetected) {
    playMessage(4); // 0004.mp3
  }
}

// ----------- Function to Play Pre-recorded Voice -------------
void playMessage(int trackNumber) {
  myDFPlayer.play(trackNumber);
  Serial.print("Playing track: ");
  Serial.println(trackNumber);
  delay(1500); // Prevents overlap of messages
}
