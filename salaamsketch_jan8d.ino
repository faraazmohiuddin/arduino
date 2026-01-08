#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 33
#define IN3 14
#define IN4 12

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // New ESP32 PWM API
  ledcAttach(ENA, 1000, 8);
  ledcAttach(ENB, 1000, 8);

  Serial.println("Motor test starting...");
}

void loop() {
  // ---- FORWARD ----
  Serial.println("Forward slow");

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  ledcWrite(ENA, 80);   // slow speed
  ledcWrite(ENB, 80);

  delay(3000);

  // ---- STOP ----
  Serial.println("Stop");
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
  delay(2000);

  // ---- REVERSE ----
  Serial.println("Reverse slow");

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);

  ledcWrite(ENA, 80);
  ledcWrite(ENB, 80);

  delay(3000);

  // ---- STOP ----
  Serial.println("Stop");
  ledcWrite(ENA, 0);
  ledcWrite(ENB, 0);
  delay(4000);
}
