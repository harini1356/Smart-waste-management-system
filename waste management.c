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
