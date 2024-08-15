#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

const char* ssid = "Dans iPhone";     // WiFi SSID
const char* password = "_danromm";     // WiFi password
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec);

WebServer server(80);

void setup() {
    Serial.begin(115200);

    // Connect to WiFi as a client (host)
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connecting to WiFi");
    Serial.println("WiFi connected");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Set up NTP client
    timeClient.begin();

    // Set up routes
    server.on("/", HTTP_GET, handleRoot);

    // Start server
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    timeClient.update();
    server.handleClient();
}

void handleRoot() {
    Serial.println("Request received"); // Log when request is received

    // Get current time from NTP client
    unsigned long currentTime = timeClient.getEpochTime();

    // Create JSON object
    StaticJsonDocument<70000> doc;
    //doc["sensor"] = "Weight";
    doc["Weight"] = 25.5;
    doc["timestamp"] = currentTime; // Add current time to JSON object
    doc["Image"] = "iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAAOUlEQVR42mP8//8/AyMjIwAiRHl5gAAAABJRU5ErkJggg==";
    //    doc["Image"] = image;
    // Serialize JSON object to string
    String jsonString;
    serializeJson(doc, jsonString);

    // Print JSON data
    Serial.println("JSON Data:");
    Serial.println(jsonString);

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");

    // Send JSON response
    //server.send(200, "application/json", jsonString);
    server.sendHeader("Content-Type", "application/json");
    server.send(200, "application/json", jsonString);

    Serial.println("Data sent"); // Log when data is sent
}
