
## Smart waste management and system
The Smart Waste Management System is an IoT-based embedded project that automates waste bin monitoring using ESP32 and Arduino UNO, with dry/wet waste segregation, real-time level detection, and web-based dashboard monitoring. This helps improve municipal waste collection efficiency, hygiene, and eco-friendly waste handling.


## Table of Contents
Introduction

Project Overview

Key Components

How It Work

Code overview

 Output

Future Enhancements
## Project Overview
Dry and Wet Waste Segregation: Sensors detect and separate dry and wet waste into different bins.

Bin Level Monitoring: Ultrasonic or IR sensors measure the fill levels of bins.

Web Dashboard: ESP32 uploads sensor data to a local web server or cloud dashboard.

Alert System: Notifications when bins are full or misused.

Optional Version: Arduino UNO version with serial monitoring instead of web upload.





## Key Components
This project uses an ESP32 Dev Board as the main microcontroller, chosen for its built-in Wi-Fi capabilities. This allows the system to send real-time data from the smart waste bin to a web dashboard. An Arduino UNO is also optionally supported for offline versions that do not require internet connectivity, suitable for basic testing through serial output.

To monitor how full the bin is, one or two Ultrasonic Sensors (HC-SR04) are used. These sensors calculate the distance from the top of the bin to the waste level, helping estimate the bin’s fill percentage.

For dry and wet waste identification, an Infrared (IR) Sensor is used. The IR sensor detects the surface properties of the waste (like color or reflectivity) to distinguish between dry and wet types. While not as precise as a moisture sensor, it works effectively for basic segregation in controlled environments.

A Servo Motor is connected to the controller to rotate and direct the waste into the appropriate bin (dry or wet) based on the IR sensor’s output.

An OLED Display (SSD1306) is optionally included in the design to display information such as bin level, system status, or waste type on a small screen.

The Wi-Fi module built into the ESP32 allows the device to connect to a local network and transmit data to a web interface or IoT platform. Jumper wires and a breadboard are used for easy wiring and circuit testing during prototyping.










## How It Works
Waste is inserted into the intake section of the smart bin system.

An IR Sensor detects the type of waste based on surface reflection and color characteristics. This helps in distinguishing between dry and wet waste.

Based on the sensor’s detection, the Servo Motor rotates the lid or chute to direct the waste into the appropriate bin — either for dry or wet waste.

An Ultrasonic Sensor continuously monitors the fill level of each bin by measuring the distance from the sensor to the waste surface.

The ESP32 microcontroller collects the data from the IR and ultrasonic sensors and uploads the bin status to a web dashboard via its built-in Wi-Fi module.

Optionally, an OLED Display is used to show real-time information locally, such as bin level percentage and waste type detection status.




## Code Overview

Automatically monitor garbage levels and detect waste entry using sensors, then display data via a web server.

## Output
![1](https://github.com/user-attachments/assets/857381f6-0337-48ef-81c2-137fb5b912da)

![2](https://github.com/user-attachments/assets/e1e8eaf3-d3d9-477b-b6c8-a62b9350a634)

Accurate classification of waste into dry/wet bins

Real-time monitoring of bin status

Live display on OLED or serial monitor

Dashboard data accessible via IP (ESP32 Webserver)


