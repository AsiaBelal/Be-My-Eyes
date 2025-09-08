// Face Recognition with HuskyLens + DFPlayer Mini
// Adjusted so detection sounds use 1-4, faces start from 5+

#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial huskySerial(50, 51);   // RX, TX for HuskyLens
SoftwareSerial dfSerial(10, 11);      // RX, TX for DFPlayer Mini

HUSKYLENS huskylens;
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(9600);
  huskySerial.begin(9600);
  dfSerial.begin(9600);

  while (!huskylens.begin(huskySerial)) {
    Serial.println(F("HuskyLens not connected!"));
    delay(1000);
  }
  Serial.println(F("HuskyLens connected! Set to Face Recognition mode."));

  if (!myDFPlayer.begin(dfSerial)) {
    Serial.println(F("DFPlayer Mini not found!"));
    while (true);
  }
  myDFPlayer.volume(25);
  Serial.println(F("DFPlayer Mini ready!"));
}

void loop() {
  if (!huskylens.request()) return;
  if (!huskylens.isLearned()) return;

  if (huskylens.available()) {
    while (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();

      if (result.command == COMMAND_RETURN_BLOCK) {
        int faceID = result.ID;
        Serial.print(F("Face Detected: ID = "));
        Serial.println(faceID);

        // Offset by 4 so that face sounds start at 0005.mp3
        int fileNumber = faceID + 4;
        myDFPlayer.play(fileNumber);

        delay(2000);
      }
    }
  }
}
