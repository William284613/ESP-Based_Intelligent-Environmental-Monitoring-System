#include <ESP8266WiFi.h>        // Include ESP8266 WiFi library
#include <WiFiClient.h>         // Include WiFi client library for ESP8266
#include <ESP8266WebServer.h>   // Include ESP8266 WebServer library
#include <ESP8266HTTPClient.h>  // Include ESP8266 HTTPClient library
#include <Wire.h>               // Include Wire library for I2C communication
#include <LiquidCrystal_I2C.h>  // Include LiquidCrystal_I2C library for LCD
#include <MQ135.h>              // Include MQ135 library for gas sensor
#include "DHT.h"                // Include DHT library for DHT11 sensor
#include "indexpage.h"          // Include HTML page stored in indexpage.h
#include <ArduinoJson.h>        // Include ArduinoJson library for JSON handling

#define DHTTYPE DHT11  // Define DHT sensor type
#define LEDonBoard 2   // Define onboard LED pin
#define MQ_PIN A0      // Define analog pin for MQ135 sensor
#define co2Zero 70     // Define CO2 sensor zero point calibration value

const char* ssid = "Wiphone";            // WiFi SSID
const char* password = "0174652300abc";  // WiFi Password

bool isSystemActive = false;  // System activation flag
bool isAutoMode = true;       // Automatic mode flag
bool relayStatus = false;     // Relay status flag

ESP8266WebServer server(80);         // Create a web server on port 80
const int relayPin = 13;             // Pin connected to relay
const int DHTPin = 2;                // Pin connected to DHT sensor
DHT dht(DHTPin, DHTTYPE);            // Initialize DHT sensor
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Initialize LCD with I2C address 0x27
MQ135 gasSensor(MQ_PIN);             // Initialize MQ135 gas sensor

const int GOOD_THRESHOLD = 500;      // Threshold for good air quality
const int MODERATE_THRESHOLD = 700;  // Threshold for moderate air quality

float lastTemperature = 0.0;  // Last recorded temperature
float lastHumidity = 0.0;     // Last recorded humidity
int lastCO2 = 0;              // Last recorded CO2 concentration
String lastAirQuality = "";   // Last recorded air quality

// Function to classify CO2 concentration
String classifyCO2Concentration(int concentration) {
  if (concentration <= GOOD_THRESHOLD) {
    return "Good";
  } else if (concentration <= MODERATE_THRESHOLD) {
    return "Moderate";
  } else {
    return "Danger";
  }
}

// Handler for the root URL
void handleRoot() {
  String s = MAIN_page;              // Get HTML content from indexpage.h
  server.send(200, "text/html", s);  // Send HTML content to client
}

// Handler to activate the system
void handleActivateSystem() {
  isSystemActive = true;  // Activate the system
  server.send(200, "text/plain", "System activated");

  // Initialize sensors and LCD
  dht.begin();      // Start DHT sensor
  lcd.init();       // Initialize LCD
  lcd.backlight();  // Turn on LCD backlight
  lcd.clear();      // Clear LCD display
  lcd.print("System Active");
}


// Handler to read all sensors
void handleAllSensors() {
  if (!isSystemActive) {
    server.send(400, "text/plain", "System not active");
    return;
  }

  // Read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  // Read CO2 concentration
  int co2now[10];
  int co2raw = 0;
  int co2ppm = 0;
  int zzz = 0;

  // Take multiple readings from the gas sensor
  for (int x = 0; x < 10; x++) {
    co2now[x] = analogRead(MQ_PIN);
    delay(50);
  }

  // Calculate average CO2 concentration
  for (int x = 0; x < 10; x++) {
    zzz += co2now[x];
  }

  co2raw = zzz / 10;
  co2ppm = co2raw - co2Zero;

  // Classify air quality based on CO2 concentration
  String airQuality = classifyCO2Concentration(co2ppm);

  // Store last sensor readings
  lastTemperature = t;
  lastHumidity = h;
  lastCO2 = co2ppm;
  lastAirQuality = airQuality;

  // Prepare response with sensor data
  String response = String(t) + "," + String(h) + "," + String(co2ppm) + "," + airQuality;
  server.send(200, "text/plain", response);

  // Update LCD display
  lcd.clear();
  lcd.setCursor(0, 0);

  // Display sensor readings on LCD
  if (isnan(t) || isnan(h) || isnan(co2ppm)) {
    lcd.print("<Error:>");
    lcd.setCursor(0, 1);
    lcd.print("nan");
  } else {
    lcd.print(t);
    lcd.print("C ");
    lcd.print(h);
    lcd.print("% ");
    lcd.setCursor(0, 1);
    lcd.print(co2ppm);
    lcd.print(" ");
    lcd.print(airQuality);
  }
}

// Handler to active system
void handleSendData() {
  if (!isSystemActive) {
    server.send(400, "text/plain", "System not active");
    return;
  }
  sendDataToServer();  // Call function to start request
  server.send(200, "text/plain", "Data sent to server");
}

// Function to send sensor data to server
void sendDataToServer() {
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClient client;
    HTTPClient http;
    String serverPath = "http://172.20.10.5/ESP-based-Monitoring-System/submit_data.php";
    Serial.print("Sending request to: ");
    Serial.println(serverPath);

    // Construct the POST request body
    String requestBody = "temperature=" + String(lastTemperature) + "&humidity=" + String(lastHumidity) + "&co2=" + String(lastCO2) + "&air_quality=" + lastAirQuality;

    http.begin(client, serverPath);  // Specify the URL and endpoint of your server
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Send the POST request
    int httpResponseCode = http.POST(requestBody);

    // Check for response
    if (httpResponseCode > 0) {
      String payload = http.getString();
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      Serial.println("Received response:");
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.print("Error message: ");
      Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    // End HTTP connection
    http.end();

    // Call the PHP script to calculate and store averages
    String avgServerPath = "http://172.20.10.5/ESP-based-Monitoring-System/calculate_averages.php";
    http.begin(client, avgServerPath);
    int avgHttpResponseCode = http.GET();

    if (avgHttpResponseCode > 0) {
      String avgPayload = http.getString();
      Serial.print("Average HTTP Response code: ");
      Serial.println(avgHttpResponseCode);
      Serial.println("Received average response:");
      Serial.println(avgPayload);
    } else {
      Serial.print("Average Error code: ");
      Serial.println(avgHttpResponseCode);
      Serial.print("Average Error message: ");
      Serial.println(http.errorToString(avgHttpResponseCode).c_str());
    }

    http.end();
  } else {
    Serial.println("WiFi not connected.");
  }
}

// Handler to fetch last readings from server
void handleFetchLastReadings() {
  if (!isSystemActive) {
    server.send(400, "text/plain", "System not active");
    return;
  }
  WiFiClient client;
  HTTPClient http;
  String serverPath = "http://172.20.10.5/ESP-based-Monitoring-System/fetch_last_readings.php";
  Serial.print("Fetching data from: ");
  Serial.println(serverPath);

  http.begin(client, serverPath);  // Start connection to server
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
    server.send(200, "application/json", payload);  // Send JSON response to the client
  } else {
    Serial.print("Error fetching data. Error code: ");
    Serial.println(httpResponseCode);
    server.send(500, "text/plain", "Error fetching data");
  }
  http.end();  // Close connection
}

void handleFetchAverageReadings() {
  if (!isSystemActive) {
    server.send(400, "text/plain", "System not active");
    return;
  }
  WiFiClient client;
  HTTPClient http;
  String serverPath = "http://172.20.10.5/ESP-based-Monitoring-System/fetch_average_readings.php";
  Serial.print("Fetching average data from: ");
  Serial.println(serverPath);

  http.begin(client, serverPath);  // Start connection to server
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();
    Serial.println(payload);

    // Parse the existing JSON array
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      server.send(500, "text/plain", "Error parsing JSON data");
      return;
    }

    // Create a new JSON object for mode and relay status
    JsonObject statusObj = doc.createNestedObject();
    statusObj["mode"] = isAutoMode ? "AUTO" : "MANUAL";
    statusObj["relay"] = relayStatus ? "ON" : "OFF";

    // Serialize the modified JSON back to a string
    String modifiedPayload;
    serializeJson(doc, modifiedPayload);

    server.send(200, "application/json", modifiedPayload);
  } else {
    server.send(500, "text/plain", "Error fetching average data");
  }
  http.end();
}

void fetchAverageAndControlRelay() {
  if (!isSystemActive || !isAutoMode) return;

  WiFiClient client;
  HTTPClient http;
  String serverPath = "http://172.20.10.5/ESP-based-Monitoring-System/fetch_average_readings.php";

  http.begin(client, serverPath);
  int httpResponseCode = http.GET();
  if (httpResponseCode > 0) {
    String payload = http.getString();

    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    JsonObject data = doc[0];

    float averageTemperature = data["average_temperature"];
    float averageCO2 = data["average_co2"];

    relayStatus = (averageTemperature >= 24 || averageCO2 >= 500);
    digitalWrite(relayPin, relayStatus ? HIGH : LOW);
  }
  http.end();
}

// Handler to update mode (AUTO or MANUAL) and relay status
void handleUpdateMode() {
  if (!isSystemActive) {
    server.send(400, "text/plain", "System not active");
    return;
  }
  isAutoMode = (server.arg("auto") == "true");  // Update mode based on request parameter
  if (!isAutoMode) {
    relayStatus = (server.arg("relay") == "ON");  // Update relay status in MANUAL mode
    digitalWrite(relayPin, relayStatus ? HIGH : LOW);
  }
  String response = "{\"mode\":\"" + String(isAutoMode ? "AUTO" : "MANUAL") + "\",\"relay\":\"" + String(relayStatus ? "ON" : "OFF") + "\"}";
  server.send(200, "application/json", response);  // Send updated mode and relay status as JSON response
}

void setup() {
  lcd.init();            // Initialize LCD
  lcd.backlight();       // Turn on LCD backlight
  lcd.clear();           // Clear LCD display
  Serial.begin(115200);  // Start serial communication at 115200 baud
  delay(500);
  dht.begin();  // Initialize DHT sensor
  delay(500);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);     // Initially turn off the relay
  pinMode(LEDonBoard, OUTPUT);     // Set LED pin as output
  digitalWrite(LEDonBoard, HIGH);  // Turn off LED
  WiFi.begin(ssid, password);      // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  lcd.print("Connecting...");
  int attempt = 0;
  while (WiFi.status() != WL_CONNECTED && attempt < 20) {  // Try up to 20 times to connect to WiFi
    Serial.print("WiFi connection attempt ");
    Serial.println(attempt + 1);
    digitalWrite(LEDonBoard, LOW);   // Blink LED
    delay(1000);                     // Increase delay to 1 second
    digitalWrite(LEDonBoard, HIGH);  // Blink LED
    delay(1000);                     // Increase delay to 1 second
    attempt++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("Successfully connected to WiFi!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    lcd.clear();              // Clear LCD display
    lcd.print("Connected!");  // Print connection message
    lcd.setCursor(0, 1);      // Set cursor to second row
    lcd.print("IP: ");
    lcd.print(WiFi.localIP());

    server.on("/", handleRoot);  // Define handler for root URL
    server.on("/activateSystem", handleActivateSystem);
    server.on("/readAllSensors", handleAllSensors);
    server.on("/fetchLastReadings", handleFetchLastReadings);        // Define handler to fetch last readings
    server.on("/fetchAverageReadings", handleFetchAverageReadings);  // Define handler to fetch average readings
    server.on("/sendData", handleSendData);                          // Define handler to fetch average readings
    server.on("/updateMode", handleUpdateMode);

    server.begin();  // Start the server
    Serial.println("HTTP server started");
    delay(5000);
    lcd.init();       // Reinitialize LCD
    lcd.backlight();  // Turn on LCD backlight
    lcd.clear();      // Clear LCD display


  } else {
    Serial.println("");
    Serial.println("Connection Failed! Please Try Again!");
    lcd.clear();              // Clear LCD display
    lcd.print("Connection");  // Print connection failed message
    lcd.setCursor(0, 1);      // Set cursor to second row
    lcd.print("Failed!");
  }
}

// Variables and constants for timing intervals
unsigned long previousMillis = 0;
const long readInterval = 3000;  // Read sensors every 3 seconds
const long sendInterval = 8000;  // Send data every 8 seconds
unsigned long lastSendTime = 0;

// Loop function, runs continuously after setup
void loop() {
  server.handleClient();  // Handle incoming client requests

  if (isSystemActive) {
    unsigned long currentMillis = millis();

    // Read sensors at regular intervals
    if (currentMillis - previousMillis >= readInterval) {
      previousMillis = currentMillis;
      handleAllSensors();             // Call function to read all sensors
      fetchAverageAndControlRelay();  // Call function to fetch average readings and control relay
    }

    // Send sensor data to server at regular intervals
    if (currentMillis - lastSendTime >= sendInterval) {
      lastSendTime = currentMillis;
      if (!isnan(lastTemperature) && !isnan(lastHumidity) && lastCO2 != 0) {
        sendDataToServer();  // Call function to send sensor data to server
      } else {
        Serial.println("Invalid sensor readings, not sending data.");
      }
    }
  }
}
