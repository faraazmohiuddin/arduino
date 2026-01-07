void setup() {
  pinMode(13, OUTPUT);   // Built-in LED
}

void loop() {
  digitalWrite(13, HIGH);  // LED ON
  delay(1000);             // 1 second
  digitalWrite(13, LOW);   // LED OFF
  delay(1000);             // 1 second
}
