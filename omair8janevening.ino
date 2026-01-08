#include <Wire.h>

#define MPU 0x68

// -------- FUNCTION PROTOTYPES --------
void readMPU();
void driveMotors(int speed);
void printDebug();

// -------- MOTOR PINS --------
#define ENA 3
#define IN1 4
#define IN2 5
#define ENB 6
#define IN3 7
#define IN4 8

// -------- PID GAINS --------
float Kp = 26.0;
float Ki = 0.0;
float Kd = 1.8;

// -------- BALANCE & TUNING --------
float setpoint = 0.0;
float deadband = 0.5;

// -------- SPEED CONTROL --------
int maxPWM_far  = 190;
int maxPWM_near = 60;
int minPWM      = 55;

float nearAngle = 3.0;
float fallAngle = 30.0;

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
float lastOutput = 0;

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
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  lastTime = micros();
  Serial.println("✅ Self-balancing robot running (calmed version)");
}

void loop() {
  readMPU();

  unsigned long now = micros();
  float dt = (now - lastTime) / 1000000.0;
  lastTime = now;

  if (dt <= 0 || dt > 0.05) dt = 0.005;

  // -------- ANGLE ESTIMATION --------
  accelPitch = atan2((float)ax,
                     sqrt((float)ay * ay + (float)az * az))
               * 57.2958;

  gyroRate = gy / 131.0;

  pitch = 0.98 * (pitch + gyroRate * dt)
        + 0.02 * accelPitch;

  // -------- SAFETY --------
  if (abs(pitch - setpoint) > fallAngle) {
    integral = 0;
    lastError = 0;
    driveMotors(0);
    lastOutput = 0;
    printDebug();
    return;
  }

  // -------- PID CONTROL --------
  error = (pitch - setpoint);

  if (abs(error) < deadband) {
    driveMotors(0);
    lastOutput = 0;
    printDebug();
    return;
  }

  integral += error * dt;
  float derivative = (error - lastError) / dt;
  lastError = error;

  float output = Kp * error + Ki * integral + Kd * derivative;

  // -------- DYNAMIC SPEED LIMITING --------
  int dynamicMaxPWM =
      (abs(error) < nearAngle) ? maxPWM_near : maxPWM_far;

  output = constrain(output, -dynamicMaxPWM, dynamicMaxPWM);

  if (abs(error) > nearAngle) {
    if (output > 0) output = max(output, (float)minPWM);
    if (output < 0) output = min(output, (float)-minPWM);
  }

  driveMotors((int)output);
  lastOutput = output;

  printDebug();
  delay(3);
}

// -------- READ MPU6050 --------
void readMPU() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 14, true);

  ax = (Wire.read() << 8) | Wire.read();
  ay = (Wire.read() << 8) | Wire.read();
  az = (Wire.read() << 8) | Wire.read();

  Wire.read(); Wire.read();

  gx = (Wire.read() << 8) | Wire.read();
  gy = (Wire.read() << 8) | Wire.read();
  gz = (Wire.read() << 8) | Wire.read();
}

// -------- MOTOR CONTROL --------
void driveMotors(int speed) {
  if (speed == 0) {
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    return;
  }

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
  Serial.println(lastOutput);
}
