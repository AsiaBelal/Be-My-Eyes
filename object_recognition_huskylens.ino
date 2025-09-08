// Object Recognition with HuskyLens + DFPlayer Mini
// Identifies objects and plays corresponding voice messages
// Audio files stored on SD card: 0006.mp3 to 0015.mp3

#include "HUSKYLENS.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

// Serial connections
SoftwareSerial huskySerial(50, 51);   // RX, TX for HuskyLens
SoftwareSerial dfSerial(10, 11);      // RX, TX for DFPlayer Mini

HUSKYLENS huskylens;
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(9600);
  huskySerial.begin(9600);
  dfSerial.begin(9600);

  // Connect to HuskyLens
  while (!huskylens.begin(huskySerial)) {
    Serial.println(F("HuskyLens not connected!"));
    delay(1000);
  }
  Serial.println(F("HuskyLens connected! Set to Object Recognition mode."));

  // Connect to DFPlayer
  if (!myDFPlayer.begin(dfSerial)) {
    Serial.println(F("DFPlayer Mini not found!"));
    while (true); // Stop here if DFPlayer isn't connected
  }
  myDFPlayer.volume(25);  // Set speaker volume (0-30)
  Serial.println(F("DFPlayer Mini ready! Speaker working."));
}

void loop() {
  if (!huskylens.request()) return;
  if (!huskylens.isLearned()) return;

  if (huskylens.available()) {
    while (huskylens.available()) {
      HUSKYLENSResult result = huskylens.read();

      if (result.command == COMMAND_RETURN_BLOCK) {
        int objectID = result.ID;
        Serial.print(F("Object Detected: ID = "));
        Serial.println(objectID);

        // Map object ID to MP3 file number (start from 0006.mp3)
        int fileNumber = objectID + 5;

        // Object List:
        // ID 1 → Bottle       (0006.mp3)
        // ID 2 → Chair        (0007.mp3)
        // ID 3 → Table        (0008.mp3)
        // ID 4 → Bag          (0009.mp3)
        // ID 5 → Phone        (0010.mp3)
        // ID 6 → Door         (0011.mp3)
        // ID 7 → Cup          (0012.mp3)
        // ID 8 → Book         (0013.mp3)
        // ID 9 → Remote       (0014.mp3)
        // ID 10 → Glasses     (0015.mp3)

        if (fileNumber >= 6 && fileNumber <= 15) {
          myDFPlayer.play(fileNumber);
          Serial.print(F("Playing audio file: 00"));
          Serial.print(fileNumber);
          Serial.println(F(".mp3"));
        } else {
          Serial.println(F("Unrecognized object ID."));
        }

        delay(2000); // Short delay to avoid multiple repeats
      }
    }
  }
}
