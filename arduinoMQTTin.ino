#include <WiFi.h> // Use WiFi.h for ESP32
#include <PubSubClient.h>
#include "secrets.h" // Include your secrets header for WiFi credentials

// WiFi settings
char ssid[] = SECRET_SSID;  // Network SSID
char pass[] = SECRET_PASS;  // WPA key

// MQTT Broker settings
const char *mqtt_broker = "broker.emqx.io"; // EMQX broker endpoint
const char *mqtt_topic = "YourTopicHere"; // MQTT topic
const char *mqtt_username = "emqx"; // MQTT username for authentication
const char *mqtt_password = "public"; // MQTT password for authentication
const int mqtt_port = 1883; // MQTT port (TCP)

WiFiClient wifiClient;
PubSubClient mqtt_client(wifiClient);

void connectToWiFi();
void connectToMQTTBroker();
void mqttCallback(char *topic, byte *payload, unsigned int length);

void setup() {
    Serial.begin(115200);
    connectToWiFi();
    mqtt_client.setServer(mqtt_broker, mqtt_port);
    mqtt_client.setCallback(mqttCallback);
    connectToMQTTBroker();
}

void connectToWiFi() {
    Serial.println("Connecting to WiFi");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to the WiFi network");
}

void connectToMQTTBroker() {
    while (!mqtt_client.connected()) {
        String client_id = "esp32-client-" + String(WiFi.macAddress()); // Change to esp32-client for clarity
        Serial.print("Connecting to MQTT Broker as ");
        Serial.println(client_id); // Correct usage of Serial.println
        if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Connected to MQTT broker");
            mqtt_client.subscribe(mqtt_topic);
            // Publish message upon successful connection
            mqtt_client.publish(mqtt_topic, "succesful connection");
        } else {
            Serial.print("Failed to connect to MQTT broker, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void mqttCallback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message received on topic: ");
    Serial.println(topic);
    Serial.print("Message: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void loop() {
    if (!mqtt_client.connected()) {
        connectToMQTTBroker();
    }
    mqtt_client.loop();
}
