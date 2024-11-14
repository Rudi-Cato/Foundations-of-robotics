#include <ArduinoJson.h> // Include the ArduinoJson library for parsing JSON

const int ledPin = 13; // Pin number where the LED is connected

// Define the maximum size for the JSON document
#define JSON_DOC_SIZE 200

void setup() {
  Serial.begin(9600);  // Start the serial communication at 9600 baud
  pinMode(ledPin, OUTPUT); // Set the LED pin as an output
  digitalWrite(ledPin, LOW); // Initially turn off the LED
  Serial.println("Enter the JSON message for Twist with angular velocities:");
}

void loop() {
  if (Serial.available() > 0) {
    // Create a StaticJsonDocument to store the parsed JSON data
    StaticJsonDocument<JSON_DOC_SIZE> doc;

    // Read the incoming JSON string from the Serial Monitor
    String json = Serial.readStringUntil('\n');  // Read until newline

    // Deserialize the JSON data into the document
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
      // If deserialization failed, print the error
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.f_str());
      return;
    }

    // Extract the angular velocity (z) value from the JSON document
    float angular_z = doc["angular"]["z"];

    // Print the angular velocity to the Serial Monitor
    Serial.print("Received angular velocity (z): ");
    Serial.println(angular_z);

    // Check if angular velocity is 0.5
    if (angular_z == 0.5) {
      digitalWrite(ledPin, HIGH); // Turn on the LED
      Serial.println("Angular velocity is 0.5, LED ON.");
    } else {
      digitalWrite(ledPin, LOW); // Turn off the LED
      Serial.println("Angular velocity is not 0.5, LED OFF.");
    }
  }
}
