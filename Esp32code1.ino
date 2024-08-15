/********************************************
Rumbidzai Mashumba
Load cell filter
Created On: 05/05/2024

********************************************/
#include <SD.h>  // Include the SD library
#include <HX711.h> // Include the HX711 library
#include <esp_sleep.h> // Include the ESP32 sleep library



#define LOADCELL_DOUT_PIN 16//choose the right pins
#define LOADCELL_SCK_PIN 4

#define Threshold 1   // if loadcell measures anything more wake up mcu

#define WAKEUP_PIN 33 // GPIO pin to trigger wakeup from deep sleep


HX711 scale; // HX711 load cell instance


//calibration factor
float calibrationFactor = -15.2267; //calculate calibration factor


// SD card pins
const int SD_CS_PIN = 21;   //check the SD pin


// Declare the file to store weight data
File weightDataFile;
File dataFile;

// Variable to hold the current weight
float currentWeight = 0.0;
float prevWeight[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
float currentFilter = 0.0;
bool isRecording = false;
float a[] = {0.391791462, 0.575709608, 0.376036868, 0.092118815};
float b[] = {5.84323939e-08, 2.33729573e-07, 3.50594369e-07, 2.33729570e-07, 5.84323957e-08};



void initialisation() {
  scale.tare();
  Serial.println("Scale Zeroed");
  delay(1000);
  scale.set_scale(calibrationFactor);
  Serial.println("Scale Calibration Factor set to:  ");
  Serial.println(calibrationFactor);
}

void writeWeightToFile() {
  Serial.println("isRecording: " + String(isRecording));
  // Check if recording is in progress
  if (isRecording) {
    // Open the file on the SD card
    File dataFile = SD.open("/data.txt", FILE_APPEND);
    // Check if the file was opened successfully
    if (dataFile) {
      Serial.println("Recording...");
      // Write the current weight to the file
      dataFile.println(String(currentWeight));
      // Close the file
     // dataFile.close();
    }
  }
}



float filterIIR(float current_weight) {
  int len_b = sizeof(b) / sizeof(b[0]);
  int len_a = sizeof(a) / sizeof(a[0]);
  int len_prevWeight = sizeof(prevWeight) / sizeof(prevWeight[0]);

  for (int i = len_prevWeight - 1; i > 0; i--) {
    prevWeight[i] = prevWeight[i - 1];
  }

  

  // Apply the filter
  float filtered_weight = 0.0;
  prevWeight[0] = current_weight;
  for (int t = 0; t < len_b; t++) {
    filtered_weight+= b[t] * prevWeight[t-2];
    delay(50);
    //Serial.println("value is " + String(t) + " filtered_weight is " + String(filtered_weight) + " for: " + String(b[t]));

  }
  
  for (int t = 1; t < len_a; t++) {
    filtered_weight -= a[t] * prevWeight[t-1];
    delay(50);
    //Serial.println("value is " + String(t) + " filtered_weight is " + String(filtered_weight) + " for: " + String(a[t]));

  }
  
  return filtered_weight;
}


void printPrevWeight() {
  int len_prevWeight = sizeof(prevWeight) / sizeof(prevWeight[0]);
  
  Serial.println("Previous Weights:");
  for (int i = 0; i < len_prevWeight; i++) {
    Serial.print("prevWeight[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(prevWeight[i]);
  }
}

void setup() {

  //pinMode(WAKEUP_PIN, INPUT_PULLDOWN); // Configure the wakeup pin as an input with pull-up resistor
  Serial.println("Load Cell initializing...");
  Serial.begin(115200);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_gain(128);
  delay(1000);

  initialisation();


//SD connection check
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("Failed to initialize SD card!");
  } else {
    Serial.println("SD card initialized.");
  }


  Serial.println("HX711 initialized. Place an object on the load cell.");
   

  //attachInterrupt(digitalPinToInterrupt(WAKEUP_PIN), wakeupInterrupt, RAISING);


}

void loop() {
  if (scale.is_ready()) {
    Serial.println("Put weight");
    delay(500);
    //isRecording = True;
    currentFilter = scale.get_units(10);
    currentWeight = filterIIR(currentFilter);

    Serial.print("Unfiltered weight is: ");
    Serial.println(currentFilter);
    Serial.print("Filtered weight is: ");
    Serial.println(currentWeight);
    delay(1000);
    printPrevWeight();

  }



}




