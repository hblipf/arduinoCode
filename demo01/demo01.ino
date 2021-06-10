#define PIN 15
void setup() {
  pinMode(PIN, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(2, OUTPUT);
}
void loop() {
  digitalWrite(4, HIGH);
  digitalWrite(2, LOW);
  for(int i=0;i<=9;i++){
    digitalWrite(PIN, HIGH);
    delay(500);
    digitalWrite(PIN, LOW);
    delay(500);
  }
  digitalWrite(PIN, HIGH);
  delay(1000);  
  digitalWrite(PIN, LOW);
  digitalWrite(4, LOW);
  digitalWrite(2, HIGH);
  delay(1000);
}
