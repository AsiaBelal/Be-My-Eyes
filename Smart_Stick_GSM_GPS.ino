// Smart Stick - GSM (SIM900) Emergency Location Sender
// Arduino Mega + SIM900 GSM Module + NEO-6M GPS Module

#include <SoftwareSerial.h>

// SIM900 GSM module
SoftwareSerial gsmSerial(7, 8);   // RX, TX for GSM
// GPS module
SoftwareSerial gpsSerial(4, 5);   // RX, TX for GPS

#define btnSOS 26   // SOS push button pin

String phoneNumber = "01018344387"; // Replace with your number
String smsMessage = "";

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);  // SIM900 default baud rate
  gpsSerial.begin(9600);  // NEO-6M GPS default baud rate

  pinMode(btnSOS, INPUT_PULLUP);

  delay(1000);
  Serial.println("Initializing SIM900...");

  sendCommand("AT");
  sendCommand("AT+CMGF=1");           // SMS text mode
  sendCommand("AT+CNMI=1,2,0,0,0");   // Forward incoming SMS directly

  // Send test SMS on startup
  sendSMS(phoneNumber, "Hello! This is a test SMS from Arduino.");
  Serial.println("System Ready.");
}

// ---------------- Loop ----------------
void loop() {
  // Check SOS button
  if (digitalRead(btnSOS) == LOW) {
    Serial.println("SOS Button Pressed!");
    String location = getGPSLocation();
    sendSMS(phoneNumber, "Emergency! My Location: " + location);
    delay(5000); // debounce delay
  }

  // Check if SIM900 received SMS
  if (gsmSerial.available()) {
    smsMessage = gsmSerial.readString();
    Serial.println("Received SMS: " + smsMessage);

    smsMessage.toUpperCase();
    if (smsMessage.indexOf("FIND") != -1) {
      String location = getGPSLocation();
      sendSMS(phoneNumber, "My Location: " + location);
    }
  }
}

// ---------------- Functions ----------------
void sendCommand(String cmd) {
  gsmSerial.println(cmd);
  delay(500);
  while (gsmSerial.available()) {
    Serial.write(gsmSerial.read());
  }
}

void sendSMS(String number, String message) {
  sendCommand("AT+CMGF=1"); // SMS text mode
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(number);
  gsmSerial.println("\"");
  delay(500);
  gsmSerial.print(message);
  delay(500);
  gsmSerial.write(26); // CTRL+Z to send SMS
  delay(2000);
  Serial.println("SMS Sent: " + message);
}

// Function to read GPS coordinates
String getGPSLocation() {
  String latitude = "0.0000";
  String longitude = "0.0000";

  long start = millis();
  while (millis() - start < 5000) { // wait max 5 seconds
    while (gpsSerial.available()) {
      String data = gpsSerial.readStringUntil('\n');
      if (data.startsWith("$GPGGA")) { // GPS data
        int comma2 = data.indexOf(',', data.indexOf(',') + 1);
        int comma3 = data.indexOf(',', comma2 + 1);
        int comma4 = data.indexOf(',', comma3 + 1);

        latitude = data.substring(comma2 + 1, comma3);
        longitude = data.substring(comma4 + 1, data.indexOf(',', comma4 + 1));

        return latitude + "," + longitude;
      }
    }
  }
  return "GPS not fixed yet.";
}
