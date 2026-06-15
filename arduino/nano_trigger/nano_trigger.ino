const byte TRIGGER_PIN = 2;
const unsigned long DEBOUNCE_MS = 50;

bool lastStableState = HIGH;
bool lastReading = HIGH;
unsigned long lastChangeMs = 0;

String inputLine;

void setup() {
  pinMode(TRIGGER_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  delay(500);
  Serial.println("ack");
}

void loop() {
  handleSerialInput();
  handleTriggerInput();
}

void handleTriggerInput() {
  bool reading = digitalRead(TRIGGER_PIN);

  if (reading != lastReading) {
    lastReading = reading;
    lastChangeMs = millis();
  }

  if (millis() - lastChangeMs < DEBOUNCE_MS) {
    return;
  }

  if (reading != lastStableState) {
    lastStableState = reading;

    if (lastStableState == LOW) {
      Serial.println("tick");
    }
  }
}

void handleSerialInput() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      inputLine.trim();

      if (inputLine == "heartbeat") {
        Serial.println("ack");
      }

      inputLine = "";
    } else {
      inputLine += c;
    }
  }
}
