#include "HX711.h"

// GPIO pins for DT and SCK
const int DT_PIN = 5;
const int SCK_PIN = 18;

HX711 scale;

void setup() {
  Serial.begin(115200);
  scale.begin(DT_PIN, SCK_PIN);
  Serial.println("Remove all weights from the scale");
  scale.tare();
  Serial.println("Tare operation complete");
}

void loop() {
  bool shouldExit = false;

  while (!shouldExit) {
    Serial.println("Starting calibration process...");
    Serial.println("Please place a known weight on the scale and press 'r' to get the raw data value.");

    while (!Serial.available());
    char input = Serial.read();

    if (input == 'r') {
      unsigned long rawDataWithWeight = scale.read();
      Serial.print("Raw data with weight: ");
      Serial.println(rawDataWithWeight);
      Serial.println("Remove the weight from the scale and press 'r' again.");

      while (!Serial.available());
      input = Serial.read();

      if (input == 'r') {
        unsigned long rawDataWithoutWeight = scale.read();
        Serial.print("Raw data without weight: ");
        Serial.println(rawDataWithoutWeight);

        Serial.println("Enter the known weight value (in grams):");
        float knownWeight = readFloat();

        float calibrationFactor = (rawDataWithWeight - rawDataWithoutWeight) / knownWeight;
        Serial.print("Calculated calibration factor: ");
        Serial.println(calibrationFactor);
        Serial.println("Update the calibrationFactor in your code with this value.");
      }
    }

    Serial.println("Press 'r' to repeat the calibration process or any other key to exit.");
    while (!Serial.available());
    input = Serial.read();

    if (input != 'r') {
      shouldExit = true; // Set the flag to exit the loop
    }
  }
}

float readFloat() {
  while (!Serial.available());
  String input = Serial.readString();
  return input.toFloat();
}