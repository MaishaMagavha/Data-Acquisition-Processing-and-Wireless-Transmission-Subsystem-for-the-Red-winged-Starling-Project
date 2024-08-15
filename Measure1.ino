#include "HX711.h"
#include <WiFi.h>

// WiFi credentials
const char* ssid = "Galaxy A314EB7";
const char* password = "MaishaMagavha";

// HX711 pin configuration
#define DT_PIN 5
#define SCK_PIN 18

// Deep sleep configuration
#define WAKEUP_INTERVAL_SECONDS 0.5 // 5 minutes
#define WAKEUP_PIN 33

// Load cell calibration factor
float calibration_factor = 1032295;

// Weight threshold (in grams or kilograms)
float weight_threshold = 10.0; // Adjust this value as needed

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare(); // Reset the scale to 0

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Configure WAKEUP_PIN as input with pull-down resistor
  pinMode(WAKEUP_PIN, INPUT_PULLDOWN);
}

void loop() {
  // Get the calibrated weight from the load cell
  float weight = scale.get_units(10)*10;

  Serial.print("Weight: ");
  Serial.print(weight, 2); // Print the weight with 2 decimal places
  Serial.println(" g");

  // Check if the weight threshold is reached
  if (weight >= weight_threshold) {
    Serial.println("Weight threshold reached");
    // Perform any necessary actions here
  } else {
    Serial.println("Entering sleep mode...");
    // Enter deep sleep mode for a specified interval
    esp_sleep_enable_timer_wakeup(WAKEUP_INTERVAL_SECONDS * 1000000);
    esp_deep_sleep_start();
  }
}
