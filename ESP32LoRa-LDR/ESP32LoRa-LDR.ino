const int RELAY = 22;
const int LDR = 21;
void setup() {

  Serial.begin(115200);

  pinMode(RELAY, OUTPUT);

  pinMode(LDR, INPUT);
}

void loop() {
  int LDRStatus = analogRead(LDR);
  if (LDRStatus < 1600) {
    digitalWrite(RELAY, HIGH);
  } else {
    digitalWrite(RELAY, LOW);
  }
  //checking value
  Serial.println(LDRStatus);
  delay(500);
}