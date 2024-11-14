#include <ArduinoMqttClient.h>
#include <Ethernet.h>
#include <ArduinoJson.h>


byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {192, 168, 55, 177};         // IP address
byte dns[] = {192, 168, 55, 254};          // DNS server
byte gateway[] = {192, 168, 55, 254};      // Default gateway
byte subnet[] = {255, 255, 255, 0};     // Subnet mask

EthernetClient ethernetClient;
MqttClient mqttClient(ethernetClient);

const char broker[] = "broker.emqx.io";
int        port     = 1883;
const char topic[]  = "robot/velocity";


void setup() {

  Serial.begin(9600);
  delay(500);

  // disable SD card if one in the slot
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  pinMode(8,OUTPUT);
  digitalWrite(8,LOW);
  delay(200);
  digitalWrite(8,HIGH);
  delay(200);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Initialize Ethernet with a static IP, DNS, gateway, and subnet
  Serial.println("Starting Ethernet with a static IP...");
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  delay(200);

  Serial.print("Assigned IP: ");
  Serial.println(Ethernet.localIP());

  // Connect to the MQTT broker
  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);
  delay(2000);
  
  if (!mqttClient.connect(broker, port))   
  {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());
    while (1);
  }

  Serial.println("Connected to the MQTT broker!");

  mqttClient.subscribe(topic);


  Serial.println("Subscribed to topics:");
  Serial.println(topic);

}

void loop() {
delay(1000);
 int messageSize = mqttClient.parseMessage();
  if (messageSize) {
    // we received a message, print out the topic and contents

    Serial.print("Received a message with topic '");
    Serial.print(mqttClient.messageTopic());
    Serial.print("', length ");
    Serial.print(messageSize);
    Serial.println(" bytes:");


   // Read the message content
    String message = "";
    while (mqttClient.available()) {
      message += (char)mqttClient.read();
    }

    // Print out the entire message
    Serial.println(message);

    // Now, we parse the JSON message
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, message);

    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.f_str());
      return;
    }

    // Extract the angular_velocity value
    float angularZ = doc["angular"]["z"];

    // Debug: Print angular_velocity
    Serial.print("Angular Z Velocity: ");
    Serial.println(angularZ);

    // Switch the LED on if angular_velocity is 0.5
    if (angularZ == 0.5) {
      digitalWrite(7, HIGH);  // Turn LED on
      Serial.println("LED ON");
      delay(1000);
    } else {
      digitalWrite(7, LOW);   // Turn LED off
      Serial.println("LED OFF");
    }
  }
}
