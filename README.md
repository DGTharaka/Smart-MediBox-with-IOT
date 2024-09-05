# Smart Medibox Project

## Overview

This project involves developing a Smart Medibox using an ESP32 microcontroller, various sensors, and Node-RED. The system adjusts the position of a shaded sliding window based on temperature and light intensity, and provides a user interface for controlling the system's parameters via a Node-RED dashboard.

## Problem Statement

Different medicines may have different requirements for the minimum angle and the controlling factor used to adjust the position of the shaded sliding window. The following functionalities are implemented:

- **Adjustable Parameters:** Users can adjust the minimum angle and controlling factor using sliders in the Node-RED dashboard.
- **Dropdown Menu:** A dropdown menu allows users to select between predefined medicines or a custom option. Selecting a specific medicine automatically applies predefined values for the minimum angle and controlling factor.
  
## Node-RED Dashboard

The Node-RED dashboard includes:
- Two sliders: 
  - One for adjusting the minimum angle (range: 0 to 120).
  - One for adjusting the controlling factor (range: 0 to 1).
- A dropdown menu with options for common medicines (Tablet A, B, C) and a custom option.

![NodeRed Dashboard](NodeRed_Dashboard.png)

## Node-RED Flow Diagram

The Node-RED flow diagram illustrates the connections and logic used to control the Smart Medibox system.

![NodeRed Flow Diagram](NodeRed_Flow_Diagram.png)

## Wokwi Circuit

The Wokwi circuit diagram shows the hardware setup for the Smart Medibox, including the ESP32, sensors, and actuators.

![Wokwi Circuit](Wokwi_Circuit.png)

## Code

The code for the project is implemented on the ESP32 microcontroller. Below is a summary of the code functionalities:

1. **Libraries and Pin Definitions:**
   - Libraries: WiFi, DHTesp, PubSubClient, NTPClient, ESP32Servo.
   - Pins: Defined for DHT sensor, buzzer, servo motor, and LDRs.

2. **Initialization:**
   - Sets up WiFi, MQTT, and sensors.
   - Initializes servo motor with specified pulse width range.

3. **Main Loop:**
   - Reads temperature and light intensity.
   - Publishes data to MQTT topics.
   - Adjusts the servo motor based on light intensity and selected medicine parameters.

4. **MQTT Handling:**
   - Connects to the MQTT broker.
   - Subscribes to topics and processes incoming messages to adjust system parameters.

5. **Servo Motor Adjustment:**
   - Calculates and sets the servo motor angle based on the selected medicine and light intensity.

```cpp
// Import libraries
#include <WiFi.h>
#include "DHTesp.h"
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <ESP32Servo.h>

// Define pins
#define DHT_PIN 15
#define BUZZER 12
#define SERVO 18
#define LEFT_LDR 34
#define RIGHT_LDR 35

// Initialization
WiFiClient espClient;
PubSubClient mqttClient(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
DHTesp dhtSensor;
Servo servoMotor;

// For servo motor angle
float minAngle = 30;
float controlFactor = 0.75;
float minAngleA = 40;
float controlFactorA = 0.6;
float minAngleB = 50;
float controlFactorB = 0.5;
float minAngleC = 60;
float controlFactorC = 0.3;

// Arrays to store inputs as a string
char tempAr[6];
char lightAr[6];

bool isScheduledON = false;
unsigned long scheduledOnTime;
float servoAngle;
double D;
float tablet; // Variable to assign value comes from ENTC-MEDIBOX-DROPDOWN

void setup() {
  Serial.begin(115200);
  setupWifi();
  setupMqtt();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  timeClient.begin();
  timeClient.setTimeOffset(5.5*3600); // Setting time offset
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW); // At the beginning buzzer is silent
  pinMode(LEFT_LDR, INPUT);
  pinMode(RIGHT_LDR, INPUT);
  servoMotor.attach(SERVO, 500, 2400); // Attach servo motor
}

void loop() {
  if (!mqttClient.connected()) {
    connectToBroker();
  }
  mqttClient.loop();
  updateTemperature(); // Read temperature
  Serial.print("Temperature: ");
  Serial.println(tempAr);
  mqttClient.publish("ENTC-DASHBOARD-TEMP", tempAr); // Publish temperature
  checkSchedule();
  updateLightIntensity(); // Read light intensity
  mqttClient.publish("ENTC-DASHBOARD-LIGHT", lightAr); // Publish light intensity
  delay(1000);
}

// Function implementations...

