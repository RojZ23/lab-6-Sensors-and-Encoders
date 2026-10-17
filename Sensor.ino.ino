/*
  Ultrasonic Distance Module Exercises

  EXERCISE 1:
    Print distance to Serial Monitor.
    Switch open   -> centimeters
    Switch closed -> inches
    LEDs remain off.

  EXERCISE 2:
    Five-LED bar graph:
      0 cm  -> all off
      20 cm or more -> all on

  EXERCISE 3:
    Five-LED bar graph:
      10 cm or less -> all off
      100 cm or more -> all on

  Arduino Uno wiring:
    HC-SR04 VCC  -> 5V
    HC-SR04 TRIG -> D2
    HC-SR04 ECHO -> D3
    HC-SR04 GND  -> GND

    Unit switch between D4 and GND

    LEDs left to right:
      D5, D6, D7, D8, D9

    Each LED needs its own series resistor.
*/

// ---------- Settings ----------

const byte EXERCISE = 3;  // Choose 1, 2, or 3 to test all three

const byte trigPin = 2;
const byte echoPin = 3;
const byte unitSwitchPin = 4;

const byte ledPins[5] = {5, 6, 7, 8, 9};
const byte numberOfLEDs = 5;

const unsigned long echoTimeoutUs = 30000UL;
const unsigned long readingIntervalMs = 100UL;

// ---------- Switch debounce ----------

bool lastSwitchReading = HIGH;
bool stableSwitchState = HIGH;

unsigned long switchChangedAt = 0;
const unsigned long debounceMs = 30;

// ---------- Measurement timing ----------

unsigned long lastReadingMs = 0;

// ---------- Setup ----------

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);
  pinMode(unitSwitchPin, INPUT_PULLUP);

  for (byte i = 0; i < numberOfLEDs; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  lastSwitchReading = digitalRead(unitSwitchPin);
  stableSwitchState = lastSwitchReading;

  Serial.print("Ultrasonic exercise ");
  Serial.println(EXERCISE);
}

// ---------- Main loop ----------

void loop() {
  updateUnitSwitch();

  unsigned long nowMs = millis();

  if (nowMs - lastReadingMs < readingIntervalMs) {
    return;
  }

  lastReadingMs = nowMs;

  float distanceCm = readDistanceCm();

  if (distanceCm < 0.0f) {
    showBar(0);
    Serial.println("No valid echo");
    return;
  }

  // Print distance in the selected units.
  printDistance(distanceCm);

  if (EXERCISE == 1) {
    showBar(0);
  }
  else if (EXERCISE == 2) {
    int ledsOn = calculateBar(distanceCm, 0.0f, 20.0f);
    showBar(ledsOn);
  }
  else if (EXERCISE == 3) {
    int ledsOn = calculateBar(distanceCm, 10.0f, 100.0f);
    showBar(ledsOn);
  }
}

// ---------- Distance measurement ----------

// Returns centimeters, or -1 if no echo is received.
float readDistanceCm() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long durationUs = pulseIn(
    echoPin,
    HIGH,
    echoTimeoutUs
  );

  if (durationUs == 0) {
    return -1.0f;
  }

  return durationUs / 58.0f;
}

// ---------- Unit-selection switch ----------

void updateUnitSwitch() {
  bool reading = digitalRead(unitSwitchPin);
  unsigned long nowMs = millis();

  if (reading != lastSwitchReading) {
    lastSwitchReading = reading;
    switchChangedAt = nowMs;
  }

  if (nowMs - switchChangedAt >= debounceMs) {
    stableSwitchState = reading;
  }
}

// ---------- Serial output ----------

void printDistance(float distanceCm) {
  Serial.print("Distance: ");

  if (stableSwitchState == LOW) {
    float distanceInches = distanceCm / 2.54f;

    Serial.print(distanceInches, 2);
    Serial.println(" in");
  }
  else {
    Serial.print(distanceCm, 1);
    Serial.println(" cm");
  }
}

// ---------- Bar-graph calculation ----------

// Divides the selected distance span into five equal steps.
int calculateBar(float distanceCm, float minCm, float maxCm) {
  if (distanceCm <= minCm) {
    return 0;
  }

  if (distanceCm >= maxCm) {
    return numberOfLEDs;
  }

  float fraction = (distanceCm - minCm) / (maxCm - minCm);
  int ledsOn = (int)(fraction * numberOfLEDs);

  return constrain(ledsOn, 0, numberOfLEDs);
}

// ---------- LED output ----------

void showBar(int ledsOn) {
  for (byte i = 0; i < numberOfLEDs; i++) {
    digitalWrite(
      ledPins[i],
      i < ledsOn ? HIGH : LOW
    );
  }
}