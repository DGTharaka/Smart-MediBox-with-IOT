# Smart MediBox with IOT

## Introduction
The MediBox project is designed to create an intelligent pharmaceutical storage system that efficiently manages medications. This document describes the design, implementation, and capabilities of the embedded software responsible for controlling the MediBox's hardware components.

## System Overview
The MediBox system integrates several hardware elements including an OLED display, a buzzer, push buttons, Light Dependent Resistors (LDRs), and a servo motor. The software stack includes embedded code for hardware management, an NTP client for accurate timekeeping, an alarm system, environmental monitoring for temperature and humidity, light intensity measurement, and a user interface for system setup and interaction.

![Overall View](Images/Diagram.png)

## Key Features

### Essential Features
- **Time Synchronization**: Configure the time zone and synchronize with an NTP server.
- **Alarm Management**: Set, modify, and deactivate alarms.
- **Time Display**: Present the current time on the OLED screen.
- **Alarm Alerts**: Activate visual and audible alerts for alarms; use push buttons to turn off alarms.
- **Environmental Monitoring**: Track temperature and humidity, with alerts for unsafe levels.
- **Light Intensity Measurement**: Use LDRs to monitor ambient light, and display real-time and historical data on a Node-RED dashboard.
- **Servo Motor Adjustment**: Control the position of the shaded sliding window based on light levels and user-defined settings for various medicines.

### Advanced Features
- **Persistent Data Storage**: Retain alarm settings and user preferences even after power cycles.
- **Intuitive User Interface**: Navigate a menu-driven interface on the OLED screen for settings and interaction.
- **Efficient Monitoring**: Employ on-change detection to reduce power consumption.
- **Ongoing Observation**: Continuously monitor temperature, humidity, and light levels.

## Software Design

### Hardware Abstraction Layer
- Provides an interface to manage hardware components such as the OLED display, buzzer, push buttons, LDR sensors, and servo motor.
- Encapsulates hardware-specific details, making it easier to develop and adapt software.

### Sensor Management
- Manages data collection from the DHT sensor (for temperature and humidity) and LDRs (for light intensity).
- Processes and stores sensor data, which is then visualized on the Node-RED dashboard.

### Alarm Management
- Facilitates the setup, disabling, and management of medication alarms.
- Activates the buzzer and shows notifications on the OLED display when alarms are triggered.
- Keeps alarm settings in non-volatile memory for consistency across power cycles.

### Time Management
- Synchronizes the system clock with an NTP server to ensure accurate timekeeping.
- Allows customization of the time zone offset from UTC.
- Stores the time zone offset in non-volatile memory for consistent operation.

### User Interface
- Offers a menu-driven OLED display interface for configuring and interacting with the system via push buttons.
- Allows users to set alarms, adjust time zones, and check system status.
- Shows warning messages when temperature or humidity levels exceed safe thresholds.

### Communication Management
- Connects securely to an MQTT broker for remote data handling and control.
- Publishes sensor data (temperature, humidity, light intensity) to designated MQTT topics for visualization and analysis on the Node-RED dashboard.
- Subscribes to MQTT topics for receiving control commands, allowing remote adjustments to the shaded sliding window.

This modular approach ensures a well-organized system, simplifies maintenance, and allows for future upgrades and integration of additional features.

![Dashboard](Images/Dashboard.png)

## Hardware Requirements
To build the MediBox system, you will need the following components:

- **ESP32 Development Board**: The main controller for processing and connectivity.
- **OLED Display**: Provides visual feedback for time, alarms, and system status.
- **Buzzer**: Emits sound for alarm notifications.
- **Push Buttons**: Used for interacting with the menu and controlling alarms.
- **Light Dependent Resistors (LDRs)**: Measures ambient light to control the shaded sliding window.
- **Servo Motor**: Adjusts the position of the sliding window based on light intensity and user settings.

> **Note**: Changing hardware components will require adjusting configuration settings.

## Requirements

### Hardware Components
- ESP32 Development Board
- OLED Display
- Buzzer
- Push Buttons
- Light Dependent Resistors (LDRs)
- Servo Motor

### Software Components
- Wokwi
- Node-RED
- MQTT Broker
- VSCode

## Installation

### Hardware Setup
- Connect the components as described in the Port Map.

### Software Setup
1. Clone the repository:
    ```sh
    git clone https://github.com/DGTharaka/Smart-Medibox.git
    ```
2. Assemble the hardware as detailed [here](Hardware).
3. Open the Arduino IDE and upload the [sketch](Software/sketch02.ino).
4. Follow the [official guide](https://nodered.org/docs/getting-started/) to install Node-RED.
5. Import the Node-RED flow provided in the repository.
6. Run the Project:
    - Upload the firmware to the ESP32 development board.
    - Start the Node-RED server with the command `node-red` in the terminal.
    - Access the Node-RED dashboard in your browser at `http://localhost:1880/ui`.

## Simulation
To simulate the project, set up the Wokwi simulator according to the guide and simulation details found [here](Wokwi%20Simulation).

---

## Contact

For further assistance or questions, please reach out to me at tharakadidd456@gmail.com

---
