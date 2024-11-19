
#include <ArduinoJson.h> // For parsing JSON input

// Motor control variables
#define NMOTORS 4
float targetSpeed[NMOTORS] = {0, 0, 0, 0};
float currentSpeed[NMOTORS] = {0, 0, 0, 0};
float previousSpeed[NMOTORS] = {0, 0, 0, 0};

// Robot-specific parameters
float linearMax = 1.0; // Maximum linear velocity (m/s)
float angularMax = 1.0; // Maximum angular velocity (rad/s)
float wheelDistance = 0.5; // Distance between wheels (m)
float wheelRadius = 0.1; // Radius of the wheels (m)

// Motor configuration arrays (pins, etc.)
int PWM[NMOTORS] = {5, 6, 9, 10};     // PWM pins for motors
int INP_1[NMOTORS] = {2, 3, 4, 7};    // Direction pins 1
int INP_2[NMOTORS] = {8, 12, 11, 13}; // Direction pins 2

// Control-related variables
float throttleValue = 0.0; // Linear velocity (m/s)
float steeringValue = 0.0; // Angular velocity (rad/s)

// Timing variables
volatile long count = 0;
volatile long count_prev = 0;
long prevT = 0;

// Motor control object array
class Motor {
  public:
    void init() {
      // Initialization code for the motor
    }

    void evalu(float current, float target, float deltaT, int &power, int &direction) {
      // Example PID control logic
      float error = target - current;
      float kP = 10.0; // Proportional gain
      power = (int)(kP * error);

      direction = (power > 0) ? 1 : (power < 0) ? -1 : 0;
      power = abs(power);
    }
};

Motor motor[NMOTORS];

// Set motor speed and direction
void setMotor(int direction, int power, int pwmPin, int inp1, int inp2) {
  power = constrain(power, 0, 255); // Ensure PWM value is valid
  if (direction == 1) {
    digitalWrite(inp1, HIGH);
    digitalWrite(inp2, LOW);
  } else if (direction == -1) {
    digitalWrite(inp1, LOW);
    digitalWrite(inp2, HIGH);
  } else {
    digitalWrite(inp1, LOW);
    digitalWrite(inp2, LOW);
  }
  analogWrite(pwmPin, power);
}

// Process incoming twist message from serial
void processTwistMessage() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, input);

    if (!error) {
      throttleValue = doc["linear"];
      steeringValue = doc["angular"];
    } else {
      Serial.println("Error parsing JSON");
    }
  }
}

// Map twist message to motor speeds
void computeMotorSpeeds(float throttle, float steering) {
  float leftSpeed = throttle - steering * (wheelDistance / 2.0);
  float rightSpeed = throttle + steering * (wheelDistance / 2.0);

  // Map speeds to individual motors (adjust for your robot configuration)
  targetSpeed[0] = rightSpeed; // Motor 1
  targetSpeed[1] = leftSpeed;  // Motor 2
  targetSpeed[2] = rightSpeed; // Motor 3
  targetSpeed[3] = leftSpeed;  // Motor 4
}

void setup() {
  // Start serial communication for receiving twist messages
  Serial.begin(9600);
  while (!Serial) {
    ; // Wait for serial port to connect (for boards like Leonardo)
  }
  Serial.println("Robot Initialized. Waiting for commands...");

  // Set motor control pins as outputs
  for (int i = 0; i < NMOTORS; i++) {
    pinMode(PWM[i], OUTPUT);
    pinMode(INP_1[i], OUTPUT);
    pinMode(INP_2[i], OUTPUT);
  }

  // Initialize motor objects (if you have a Motor class or library)
  for (int i = 0; i < NMOTORS; i++) {
    motor[i].init(); // Replace with actual motor initialization code
  }

  // Timer initialization (if required for precise timing)
  prevT = micros();
}

void loop() {
  // Process serial input for twist messages
  processTwistMessage();

  // Map twist to motor speeds
  computeMotorSpeeds(throttleValue, steeringValue);

  // Update motor speeds
  long currT = micros();
  float deltaT = (currT - prevT) / 1e6; // Convert to seconds
  prevT = currT;

  for (int i = 0; i < NMOTORS; i++) {
    int power = 0;
    int direction = 0;
    motor[i].evalu(currentSpeed[i], targetSpeed[i], deltaT, power, direction);
    setMotor(direction, power, PWM[i], INP_1[i], INP_2[i]);
    currentSpeed[i] = targetSpeed[i]; // Update current speed (simulate)
  }
}
