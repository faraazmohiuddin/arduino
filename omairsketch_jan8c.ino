#include <Wire.h>

#define MPU 0x68

// -------- MOTOR PINS --------
#define ENA 3
#define IN1 4
#define IN2 5
#define ENB 6
#define IN3 7
#define IN4 8

// -------- PID GAINS (STABLE START) --------
float Kp = 12.0;
float Ki = 0.0;
float Kd = 0.8;

// -------- TUNING CONSTANTS --------
float setpoint = 0.0;     // adjust after observing pitch
float deadband = 1.0;     // degrees

int maxPWM = 80;          // motor speed limit
int minPWM = 45;          // minimum useful motor power

// -------- MPU VARIABLES --------
int16_t ax, ay, az;
int16_t gx, gy, gz;

float pitch = 0.0;
float accelPitch = 0.0;
float gyroRate = 0.0;

// -------- PID VARIABLES --------
float error = 0;
float lastError = 0;
float integral = 0;

unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Wake up MPU6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);      // PWR_MGMT_1
  Wire.write(0x00);
  Wire.endTransmission();

  lastTime = micros();
  Serial.println("✅ Final self-balancing code running");
}

void loop() {
  readMPU();

  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  // -------- ANGLE ESTIMATION --------
  accelPitch = atan2((float)ax,
                     sqrt((float)ay * ay + (float)az * az))
               * 57.2958;   // radians → degrees

  gyroRate = gy / 131.0;    // deg/sec (±250 dps range)

  pitch = 0.98 * (pitch + gyroRate * dt)
        + 0.02 * accelPitch;

  // -------- PID CONTROL --------
  error = (pitch - setpoint) * 0.7;  // soften reaction

  // Deadband (ignore tiny noise)
  if (abs(error) < deadband) {
    driveMotors(0);
    printDebug();
    return;
  }

  integral += error * dt;
  float derivative = (error - lastError) / dt;
  lastError = error;

  float output = Kp * error
               + Ki * integral
               + Kd * derivative;

  // Limit motor power
  output = constrain(output, -maxPWM, maxPWM);

  // Apply minimum PWM so motors actually move
  if (output > 0) output = max(output, minPWM);
  if (output < 0) output = min(output, -minPWM);

  driveMotors(output);
  printDebug();

  delay(5); // ~200 Hz loop
}

// -------- READ MPU6050 --------
void readMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);

  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();

  Wire.read(); Wire.read(); // skip temperature

  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();
}

// -------- MOTOR CONTROL --------
void driveMotors(int speed) {
  if (speed > 0) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  speed = abs(speed);
  analogWrite(ENA, speed);
  analogWrite(ENB, speed);
}

// -------- SERIAL DEBUG --------
void printDebug() {
  Serial.print("Pitch: ");
  Serial.print(pitch);
  Serial.print(" | Motor: ");
  Serial.println(lastError * Kp);
}
