#include <SoftwareSerial.h>
#include <SD.h>

// Pin Definitions
#define TRIG_PIN 4
#define ECHO_PIN 5
#define FLOAT_SWITCH_PIN 6
#define FLOW_SENSOR_PIN 7
#define RELAY1_PIN 2
#define RELAY2_PIN 3
#define BATTERY_PIN A0
#define SD_CS_PIN 10

// GSM Module Setup
SoftwareSerial gsmSerial(8, 9); // RX, TX

// Variables
long duration;
int distance;
volatile int flowPulses = 0;
int flowRate;
int waterLevel;
bool tankEmpty = false;
unsigned long lastFlowReadTime = 0;
unsigned long lastWateringTime = 0;
unsigned long lastBatteryCheckTime = 0;
unsigned long manualOverrideStartTime = 0;
bool manualOverrideActive = false;
unsigned long lastReportTime = 0;
unsigned long reportInterval = 14400000; // 4 hours

#define MAX_NUMBERS 5
String registeredNumbers[MAX_NUMBERS];
int numberCount = 0;

// Watering Schedule
#define WATERING_INTERVAL 3600000 // 1 hour

// Battery Threshold
#define BATTERY_LOW_THRESHOLD 3.3

// Data Logging
File logFile;

void setup() {
  // Initialize Serial Communication
  Serial.begin(9600);
  gsmSerial.begin(9600);

  // Initialize Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(FLOAT_SWITCH_PIN, INPUT);
  pinMode(FLOW_SENSOR_PIN, INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(BATTERY_PIN, INPUT);
  pinMode(SD_CS_PIN, OUTPUT);

  // Initialize SD Card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Initialize GSM Module
  initializeGSM();

  // Attach interrupt for flow sensor
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowCounter, RISING);
}

void loop() {
  // Read incoming SMS
  readSMS();

  // Measure Battery Voltage
  float batteryVoltage = measureBatteryVoltage();

  // Measure Water Level
  measureWaterLevel();

  // Check Water Level Sensor
  checkWaterLevel();

  // Measure Water Flow
  measureFlowRate();

  // Control Solenoid Valves
  controlValves();

  // Automatic Water Refill
  automaticWaterRefill();

  // Scheduled Watering
  scheduledWatering();

  // Battery Status Alerts
  batteryStatusAlerts(batteryVoltage);

  // Data Logging
  logData(batteryVoltage);

  // Send Report
  sendReport();

  // Print Data to Serial Monitor
  printData();

  delay(1000);
}

void measureWaterLevel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration * 0.034 / 2; // Calculate distance
}

void checkWaterLevel() {
  waterLevel = digitalRead(FLOAT_SWITCH_PIN);
  if (waterLevel == LOW) {
    tankEmpty = true;
    sendSMStoAll("Tank is empty!");
  } else {
    tankEmpty = false;
  }
}

void measureFlowRate() {
  if (millis() - lastFlowReadTime > 1000) {
    noInterrupts();
    flowRate = flowPulses;
    flowPulses = 0;
    interrupts();
    lastFlowReadTime = millis();
  }
}

void controlValves() {
  if (tankEmpty || manualOverrideActive) {
    digitalWrite(RELAY1_PIN, LOW); // Turn off valve 1
    digitalWrite(RELAY2_PIN, LOW); // Turn off valve 2
  } else {
    digitalWrite(RELAY1_PIN, HIGH); // Turn on valve 1
    digitalWrite(RELAY2_PIN, HIGH); // Turn on valve 2
  }
}

void automaticWaterRefill() {
  if (tankEmpty) {
    // Logic to refill the tank (e.g., turn on a pump)
    // For example, you can add a relay to control a pump
    // digitalWrite(PUMP_PIN, HIGH); // Turn on the pump
    sendSMStoAll("Refilling the tank...");
    delay(10000); // Wait for 10 seconds to simulate refilling
    // digitalWrite(PUMP_PIN, LOW); // Turn off the pump
  }
}

void scheduledWatering() {
  if (millis() - lastWateringTime > WATERING_INTERVAL) {
    lastWateringTime = millis();
    sendSMStoAll("Scheduled watering started.");
    // Logic to start watering (e.g., turn on valves)
    digitalWrite(RELAY1_PIN, HIGH); // Turn on valve 1
    digitalWrite(RELAY2_PIN, HIGH); // Turn on valve 2
    delay(10000); // Water for 10 seconds
    digitalWrite(RELAY1_PIN, LOW); // Turn off valve 1
    digitalWrite(RELAY2_PIN, LOW); // Turn off valve 2
    sendSMStoAll("Scheduled watering completed.");
  }
}

void batteryStatusAlerts(float batteryVoltage) {
  if (millis() - lastBatteryCheckTime > 60000) { // Check every minute
    lastBatteryCheckTime = millis();
    if (batteryVoltage < BATTERY_LOW_THRESHOLD) {
      sendSMStoAll("Battery voltage low: " + String(batteryVoltage) + "V");
    }
  }
}

void logData(float batteryVoltage) {
  logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    logFile.print("Distance: ");
    logFile.print(distance);
    logFile.print(" cm, Flow Rate: ");
    logFile.print(flowRate);
    logFile.print(" pulses/sec, Tank Empty: ");
    logFile.print(tankEmpty ? "Yes" : "No");
    logFile.print(", Battery Voltage: ");
    logFile.print(batteryVoltage);
    logFile.println("V");
    logFile.close();
  } else {
    Serial.println("Error opening log file");
  }
}

void sendReport() {
  if (millis() - lastReportTime > reportInterval) {
    lastReportTime = millis();
    String report = "System Report:\n";
    report += "Distance: " + String(distance) + " cm\n";
    report += "Flow Rate: " + String(flowRate) + " pulses/sec\n";
    report += "Tank Empty: " + String(tankEmpty ? "Yes" : "No") + "\n";
    report += "Battery Voltage: " + String(measureBatteryVoltage()) + "V\n";
    sendSMStoAll(report);
  }
}

void printData() {
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Flow Rate: ");
  Serial.print(flowRate);
  Serial.print(" pulses/sec, Tank Empty: ");
  Serial.print(tankEmpty ? "Yes" : "No");
  Serial.print(", Battery Voltage: ");
  Serial.print(measureBatteryVoltage());
  Serial.println("V");
}

void initializeGSM() {
  gsmSerial.println("AT"); // Check if GSM module is responding
  delay(1000);
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
}

void handleSMSCommand(String message) {
  if (message.startsWith("add number")) {
    String number = message.substring(11);
    addNumber(number);
    sendSMS(number, "Number added: " + number);
  } else if (message.startsWith("remove number")) {
    String number = message.substring(14);
    removeNumber(number);
    sendSMS(number, "Number removed: " + number);
  } else if (message.startsWith("list numbers")) {
    String numbersList = "Registered Numbers:\n";
    for (int i = 0; i < numberCount; i++) {
      numbersList += registeredNumbers[i] + "\n";
    }
    sendSMS(message.substring(0, 13), numbersList);
  } else if (message.startsWith("send sms")) {
    String smsMessage = message.substring(9);
    sendSMStoAll(smsMessage);
  } else if (message.startsWith("manual on")) {
    manualOverrideActive = true;
    manualOverrideStartTime = millis();
    digitalWrite(RELAY1_PIN, HIGH); // Turn on valve 1
    digitalWrite(RELAY2_PIN, HIGH); // Turn on valve 2
    sendSMS(message.substring(0, 13), "Manual override: Valves turned on.");
  } else if (message.startsWith("manual off")) {
    manualOverrideActive = false;
    digitalWrite(RELAY1_PIN, LOW); // Turn off valve 1
    digitalWrite(RELAY2_PIN, LOW); // Turn off valve 2
    sendSMS(message.substring(0, 13), "Manual override: Valves turned off.");
  } else if (message.startsWith("set interval")) {
    String intervalStr = message.substring(13);
    reportInterval = intervalStr.toInt() * 60000; // Convert minutes to milliseconds
    sendSMS(message.substring(0, 13), "Report interval set to " + intervalStr + " minutes.");
  } else {
    sendSMS(message.substring(0, 13), "Unknown command");
  }
}

void readSMS() {
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  gsmSerial.println("AT+CMGL=\"REC UNREAD\""); // Read unread SMS
  delay(1000);

  while (gsmSerial.available()) {
    String response = gsmSerial.readString();
    if (response.indexOf("+CMGL:") >= 0) {
      int index = response.indexOf("\"");
      int endIndex = response.indexOf("\"", index + 1);
      String number = response.substring(index + 1, endIndex);
      index = response.indexOf("\n", endIndex);
      String message = response.substring(index + 1);
      message.trim();
      handleSMSCommand(message);
      deleteSMS(number);
    }
  }
}

void deleteSMS(String number) {
  gsmSerial.println("AT+CMGD=1,4"); // Delete all SMS
  delay(1000);
}

void addNumber(String number) {
  if (numberCount < MAX_NUMBERS) {
    registeredNumbers[numberCount] = number;
    numberCount++;
    Serial.println("Number added: " + number);
  } else {
    Serial.println("Maximum number limit reached");
  }
}

void removeNumber(String number) {
  for (int i = 0; i < numberCount; i++) {
    if (registeredNumbers[i] == number) {
      for (int j = i; j < numberCount - 1; j++) {
        registeredNumbers[j] = registeredNumbers[j + 1];
      }
      numberCount--;
      Serial.println("Number removed: " + number);
      return;
    }
  }
  Serial.println("Number not found: " + number);
}

void sendSMStoAll(String message) {
  for (int i = 0; i < numberCount; i++) {
    sendSMS(registeredNumbers[i], message);
  }
}

void sendSMS(String number, String message) {
  gsmSerial.println("AT+CMGF=1"); // Set SMS mode to text
  delay(1000);
  gsmSerial.print("AT+CMGS=\"");
  gsmSerial.print(number);
  gsmSerial.println("\"");
  delay(1000);
  gsmSerial.print(message);
  delay(1000);
  gsmSerial.write(26); // ASCII code for Ctrl+Z
  delay(1000);
  Serial.println("SMS sent to " + number);
}

void flowCounter() {
  flowPulses++;
}

float measureBatteryVoltage() {
  int sensorValue = analogRead(BATTERY_PIN);
  float voltage = sensorValue * (5.0 / 1023.0) * 2; // Multiply by 2 if using a voltage divider
  return voltage;
}
