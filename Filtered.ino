#include "HX711.h"
#include <WiFi.h>

const char* ssid = "Galaxy A314EB7"; //WiFi SSID
const char* password = "MaishaMagavha"; //WiFi password
#define DT_PIN 5 // GPIO pin connected to HX711's DT pin
#define SCK_PIN 18 // GPIO pin connected to HX711's SCK pin

HX711 scale;
float calibration_factor = 1032295; // calibration factor, adjust as needed
const int num_measurements = 10; // Number of measurements to average
float measurements[num_measurements]; // Array to store measurements
int measurement_index = 0;

void setup() {
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);
  scale.set_scale(calibration_factor); // Set the calibration factor
  scale.tare(); // Reset the scale to 0

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  float raw_value = scale.get_units(1); // Assuming single reading

  // Check if the raw value is within a reasonable range
  if (raw_value > -1000 && raw_value < 1000) {
    measurements[measurement_index] = raw_value;
    measurement_index = (measurement_index + 1) % num_measurements;
  }

  // Check if all measurements have been collected
  if (measurement_index == 0) {
    // Compute filtered value using Kalman Filter
    float filtered_value = kalman_filter(measurements, num_measurements) - 0.0034;

    // Print the filtered value
    Serial.print("Filtered Weight: ");
    Serial.print(filtered_value, 4); // Print with 4 decimal places
    Serial.println(" KG");
  }

  delay(1500); // Delay between readings
}

float kalman_filter(float* measurements, int num_measurements) {
  // Kalman Filter parameters
  float Q = 0.001; // Process noise covariance (adjust as needed)
  float R = 0.01; // Measurement noise covariance (adjust as needed)
  float x = measurements[0]; // Initial estimate

  // Kalman Filter variables
  float P = 1.0;
  float K;

  for (int i = 0; i < num_measurements; i++) {
    // Prediction update
    P += Q;

    // Measurement update
    K = P / (P + R);
    x += K * (measurements[i] - x);
    P *= (1 - K);
  }

  return x;
}