
## Smart waste management and system
The Smart Waste Management System is an IoT-based embedded project that automates waste bin monitoring using ESP32 and Arduino UNO, with dry/wet waste segregation, real-time level detection, and web-based dashboard monitoring. This helps improve municipal waste collection efficiency, hygiene, and eco-friendly waste handling.


## Table of Contents
Introduction

Project Overview

Key Components

System Architecture & Setup

Wiring and Circuit Diagram

How It Works

Web Monitoring Dashboard

Code and Embedded C Details

Results and Output

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

#include <Servo.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin Definitions
#define IR_SENSOR_PIN        D2
#define SERVO_PIN            D5
#define ULTRASONIC_WET_TRIG  D3
#define ULTRASONIC_WET_ECHO  D4
#define ULTRASONIC_DRY_TRIG  D6
#define ULTRASONIC_DRY_ECHO  D7
#define LED_PIN              D8

// Wi-Fi credentials
const char* ssid = "october";
const char* password = "october12";

ESP8266WebServer server(80);

Servo sorterServo;

// Global state
float wetDistance = 0;
float dryDistance = 0;
String servoStatus = "Idle";

float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms
  return duration * 0.034 / 2;
}

void setup() {
  Serial.begin(115200);
  
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(ULTRASONIC_WET_TRIG, OUTPUT);
  pinMode(ULTRASONIC_WET_ECHO, INPUT);
  pinMode(ULTRASONIC_DRY_TRIG, OUTPUT);
  pinMode(ULTRASONIC_DRY_ECHO, INPUT);
  pinMode(LED_PIN, OUTPUT);

  sorterServo.attach(SERVO_PIN);
  sorterServo.write(45);

  // OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  // Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected");
  display.println(WiFi.localIP());
  display.display();
  delay(2000);

  // Web server handler
  server.on("/", []() {
    String html = "<html><head><title>Waste Sorter</title></head><body>";
    html += "<h1>Smart Waste Sorter</h1>";
    html += "<p><strong>IR Sensor:</strong> " + String(digitalRead(IR_SENSOR_PIN) == LOW ? "Object Detected" : "None") + "</p>";
    html += "<p><strong>Wet Distance:</strong> " + String(wetDistance) + " cm</p>";
    html += "<p><strong>Dry Distance:</strong> " + String(dryDistance) + " cm</p>";
    html += "<p><strong>Servo Status:</strong> " + servoStatus + "</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  int irDetected = digitalRead(IR_SENSOR_PIN);
  wetDistance = getDistance(ULTRASONIC_WET_TRIG, ULTRASONIC_WET_ECHO);
  dryDistance = getDistance(ULTRASONIC_DRY_TRIG, ULTRASONIC_DRY_ECHO);

  bool wetDetected = wetDistance > 2 && wetDistance < 15;
  bool dryDetected = dryDistance > 2 && dryDistance < 15;

  if (irDetected == LOW) {
    digitalWrite(LED_PIN, HIGH);

    if (wetDetected) {
      sorterServo.write(0);
      servoStatus = "Wet Bin";
    } else if (dryDetected) {
      sorterServo.write(90);
      servoStatus = "Dry Bin";
    } else {
      sorterServo.write(45);
      servoStatus = "Waiting";
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    sorterServo.write(45);
    servoStatus = "Idle";
  }

  // OLED Display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("IR: ");
  display.println(irDetected == LOW ? "Object" : "None");
  display.print("Wet Dist: ");
  display.print(wetDistance);
  display.println(" cm");
  display.print("Dry Dist: ");
  display.print(dryDistance);
  display.println(" cm");
  display.print("Servo: ");
  display.println(servoStatus);
  display.display();

  delay(1000);
}

## Output
![1](https://github.com/user-attachments/assets/857381f6-0337-48ef-81c2-137fb5b912da)

![2](https://github.com/user-attachments/assets/e1e8eaf3-d3d9-477b-b6c8-a62b9350a634)

Accurate classification of waste into dry/wet bins

Real-time monitoring of bin status

Live display on OLED or serial monitor

Dashboard data accessible via IP (ESP32 Webserver)


