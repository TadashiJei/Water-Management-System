# Water Management System

This project is a water management system that monitors water flow, measures pressure, checks if the tank is empty, and sends SMS updates using a GSM module. The system is battery-powered and includes solar charging capabilities. It also logs data to an SD card and sends summary reports via SMS.

## Table of Contents

- [Project Overview](#project-overview)
- [Hardware Components](#hardware-components)
- [Wiring Configuration](#wiring-configuration)
- [Functionalities](#functionalities)
- [Usage](#usage)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## Project Overview

The project is designed to automate and monitor water management tasks, providing real-time data and notifications via SMS. It includes features such as water level monitoring, flow rate measurement, automatic valve control, and battery status monitoring. The system is powered by an Arduino Uno and uses various sensors and modules to achieve its functionality.

## Hardware Components

1. **Arduino Uno R3**
   - **Purpose**: Microcontroller to control the system.
   - **Quantity**: 1

2. **SIM800L V2 GSM Module**
   - **Purpose**: For sending SMS notifications.
   - **Quantity**: 1

3. **1 Channel 5V 12V 10A Relay Module**
   - **Purpose**: To control the solenoid valves.
   - **Quantity**: 2

4. **Ultrasonic Sensor (HC-SR04)**
   - **Purpose**: To measure water level.
   - **Quantity**: 1

5. **Water Level Sensor Float Switch**
   - **Purpose**: Backup for water level detection.
   - **Quantity**: 1

6. **YF-G1 Water Flow Sensor**
   - **Purpose**: To measure water flow rate.
   - **Quantity**: 1

7. **Plastic Water Solenoid Valve (12V, 1/2" NPS)**
   - **Purpose**: To control water flow.
   - **Quantity**: 2

8. **Type C Micro USB 5V 1A 18650 TP4056 Lithium Battery Charger Module Charging Board With Protection**
   - **Purpose**: To charge the LiPo battery.
   - **Quantity**: 1

9. **XL4016 CC-CV DC-DC Buck/Step-Down Converter**
   - **Purpose**: To step down the battery voltage to 5V and 12V.
   - **Quantity**: 2

10. **SD Card Module**
    - **Purpose**: To log data.
    - **Quantity**: 1

## Wiring Configuration

1. **Battery to XL4016 Converters**:
   - **Battery Positive** -> **XL4016 Input Positive** (for both converters)
   - **Battery Negative** -> **XL4016 Input Negative** (for both converters)

2. **XL4016 for 5V**:
   - **XL4016 Output Positive** -> **Arduino 5V**
   - **XL4016 Output Negative** -> **Arduino GND**
   - **XL4016 Output Positive** -> **Sensors VCC**
   - **XL4016 Output Negative** -> **Sensors GND**

3. **XL4016 for 12V**:
   - **XL4016 Output Positive** -> **Solenoid Valves Positive**
   - **XL4016 Output Negative** -> **Solenoid Valves Negative**

4. **Arduino to SIM800L**
   - **Arduino TX** -> **SIM800L RX**
   - **Arduino RX** -> **SIM800L TX**
   - **Arduino GND** -> **SIM800L GND**
   - **Arduino 5V** -> **SIM800L VCC**

5. **Arduino to Relay Module**
   - **Arduino Digital Pin 2** -> **Relay Module IN1**
   - **Arduino Digital Pin 3** -> **Relay Module IN2**
   - **Arduino GND** -> **Relay Module GND**
   - **Arduino 5V** -> **Relay Module VCC**

6. **Arduino to Ultrasonic Sensor (HC-SR04)**
   - **Arduino Digital Pin 4** -> **HC-SR04 Trig**
   - **Arduino Digital Pin 5** -> **HC-SR04 Echo**
   - **Arduino GND** -> **HC-SR04 GND**
   - **Arduino 5V** -> **HC-SR04 VCC**

7. **Arduino to Water Level Sensor Float Switch**
   - **Arduino Digital Pin 6** -> **Water Level Sensor**
   - **Arduino GND** -> **Water Level Sensor GND**
   - **Arduino 5V** -> **Water Level Sensor VCC**

8. **Arduino to Water Flow Sensor (YF-G1)**
   - **Arduino Digital Pin 7** -> **Water Flow Sensor Signal**
   - **Arduino GND** -> **Water Flow Sensor GND**
   - **Arduino 5V** -> **Water Flow Sensor VCC**

9. **Relay Module to Solenoid Valves**
   - **Relay Module COM1** -> **Solenoid Valve 1**
   - **Relay Module COM2** -> **Solenoid Valve 2**
   - **12V Power Supply** -> **Solenoid Valves**
   - **12V Power Supply GND** -> **Solenoid Valves GND**

10. **Arduino to SD Card Module**
    - **Arduino Digital Pin 10** -> **SD Card Module CS**
    - **Arduino Digital Pin 11** -> **SD Card Module MOSI**
    - **Arduino Digital Pin 12** -> **SD Card Module MISO**
    - **Arduino Digital Pin 13** -> **SD Card Module SCK**
    - **Arduino 5V** -> **SD Card Module VCC**
    - **Arduino GND** -> **SD Card Module GND**

## Functionalities

1. **Water Level Monitoring**:
   - Measures water level using an ultrasonic sensor (HC-SR04).
   - Sends an SMS notification when the tank is empty.

2. **Water Flow Monitoring**:
   - Measures water flow rate using a YF-G1 water flow sensor.
   - Logs flow rate data to an SD card.

3. **Valve Control**:
   - Controls two solenoid valves using relay modules.
   - Automatically refills the tank when it is empty.
   - Scheduled watering based on a predefined interval.

4. **SMS Notifications**:
   - Sends SMS notifications for tank status, scheduled watering, and battery status.
   - Allows remote monitoring and control via SMS commands.

5. **Data Logging**:
   - Logs water level, flow rate, and battery voltage to an SD card.
   - Sends a summary report via SMS every 3-4 hours (customizable via SMS command).

6. **Battery Status Monitoring**:
   - Monitors battery voltage and sends alerts when the battery is low.
   - Logs battery status to an SD card.

7. **Manual Override**:
   - Allows manual control of valves via SMS commands.
   - Supports setting a custom duration for manual override.

## Usage

1. **Add Number**: Send an SMS with the text `add number +1234567890`.
2. **Remove Number**: Send an SMS with the text `remove number +1234567890`.
3. **List Numbers**: Send an SMS with the text `list numbers`.
4. **Send SMS**: Send an SMS with the text `send sms Hello, this is a test message!`.
5. **Manual Override**:
   - Turn on valves: Send an SMS with the text `manual on`.
   - Turn off valves: Send an SMS with the text `manual off`.
6. **Set Report Interval**: Send an SMS with the text `set interval 120` to set the report interval to 120 minutes.

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For more information, visit my website: [tadashijei.com](https://tadashijei.com)

Follow me on GitHub: [@TadashiJei](https://github.com/TadashiJei)

---
