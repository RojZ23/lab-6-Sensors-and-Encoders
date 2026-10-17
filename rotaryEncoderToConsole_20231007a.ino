/*
  Rotary Encoder: All Three Quadrature Exercises

  Choose EXERCISE:
    1 = Move one lit LED, stopping at either end.
        Press the knob to reset to the middle LED.

    2 = Encoder-controlled acceleration of an LED sequence.
        Press the knob to reset acceleration and speed,
        and return to the middle LED.

    3 = Dim the LED connected to pin 9.

  Wiring:
    Encoder CLK -> D2
    Encoder DT  -> D3
    Encoder SW  -> D4
    Encoder +   -> 5V (Arduino Uno)
    Encoder GND -> GND

    LEDs left to right:
      LED 1 -> D5
      LED 2 -> D6
      LED 3 -> D7 (middle)
      LED 4 -> D8
      LED 5 -> D9

    Each LED must have its own series resistor.
*/

// ---------- Settings ----------

const byte EXERCISE = 3;  // Change to 1, 2, or 3 to test all three

const byte pinA = 2;
const byte pinB = 3;
const byte buttonPin = 4;

const byte ledPins[5] = {5, 6, 7, 8, 9};
const byte dimmerPin = 9;

const int numberOfLEDs = 5;
const int middlePosition = numberOfLEDs / 2;

// Change to -1 if clockwise moves the light left.
const int directionSign = 1;

// Four valid A/B transitions produce one logical step.
// Try 2 or 1 if the response requires too much rotation.
const int countsPerStep = 4;

// Exercise 2 tuning:
const int maxKnobOffset = 10;
const float accelerationScale = 1.0f;
const float maxSpeed = 15.0f;

// Exercise 3 tuning:
const int brightnessIncrement = 5;

// ---------- Encoder state ----------

byte previousAB = 0;
int pendingCounts = 0;

// Index = (previousAB << 2) | currentAB.
// Direction matches the supplied starter code.
const int8_t transitionTable[16] = {
   0,  1, -1,  0,
  -1,  0,  0,  1,
   1,  0,  0, -1,
   0, -1,  1,  0
};

// ---------- Exercise state ----------

int ledPosition = middlePosition;

// Exercise 2:
int knobOffset = 0;
float velocity = 0.0f;
float sequencePosition = middlePosition;
unsigned long previousUpdateUs = 0;

// Exercise 3:
int brightness = 0;

// ---------- Button debounce state ----------

bool lastButtonReading = HIGH;
bool stableButtonState = HIGH;

unsigned long buttonChangedAt = 0;
const unsigned long debounceMs = 30;

// ---------- Setup ----------

void setup() {
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);

  for (byte i = 0; i < numberOfLEDs; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  previousAB = readAB();

  lastButtonReading = digitalRead(buttonPin);
  stableButtonState = lastButtonReading;

  previousUpdateUs = micros();

  if (EXERCISE == 3) {
    analogWrite(dimmerPin, brightness);
  } else {
    showOneLED(middlePosition);
  }
}

// ---------- Main loop ----------

void loop() {
  int step = readEncoderStep();
  bool buttonPressed = readButtonPress();

  unsigned long nowUs = micros();
  float dt = (nowUs - previousUpdateUs) / 1000000.0f;
  previousUpdateUs = nowUs;

  // Avoid a large motion jump after a long pause.
  if (dt > 0.05f) {
    dt = 0.05f;
  }

  switch (EXERCISE) {
    case 1:
      runExercise1(step, buttonPressed);
      break;

    case 2:
      runExercise2(step, buttonPressed, dt);
      break;

    case 3:
      runExercise3(step);
      break;
  }
}

// ---------- Encoder decoding ----------

byte readAB() {
  return (digitalRead(pinA) << 1) | digitalRead(pinB);
}

void resetEncoderTracking() {
  pendingCounts = 0;
  previousAB = readAB();
}

int readEncoderStep() {
  byte currentAB = readAB();

  if (currentAB == previousAB) {
    return 0;
  }

  // Both bits changing between samples is invalid.
  if ((currentAB ^ previousAB) == 0b11) {
    previousAB = currentAB;
    pendingCounts = 0;
    return 0;
  }

  byte tableIndex = (previousAB << 2) | currentAB;

  int movement = transitionTable[tableIndex];
  previousAB = currentAB;

  pendingCounts += movement * directionSign;

  if (pendingCounts >= countsPerStep) {
    pendingCounts -= countsPerStep;
    return 1;
  }

  if (pendingCounts <= -countsPerStep) {
    pendingCounts += countsPerStep;
    return -1;
  }

  return 0;
}

// ---------- Button handling ----------

// Returns true once for each debounced press.
bool readButtonPress() {
  bool reading = digitalRead(buttonPin);
  unsigned long nowMs = millis();

  if (reading != lastButtonReading) {
    buttonChangedAt = nowMs;
    lastButtonReading = reading;
  }

  if (nowMs - buttonChangedAt >= debounceMs) {
    if (reading != stableButtonState) {
      stableButtonState = reading;

      if (stableButtonState == LOW) {
        return true;
      }
    }
  }

  return false;
}

// ---------- Exercise 1 ----------

void runExercise1(int step, bool buttonPressed) {
  // Button press takes priority over encoder movement.
  if (buttonPressed) {
    ledPosition = middlePosition;
    resetEncoderTracking();
    showOneLED(ledPosition);
    return;
  }

  if (step == 0) {
    return;
  }

  ledPosition += step;

  // Stop at either end. Do not wrap.
  ledPosition = constrain(
    ledPosition,
    0,
    numberOfLEDs - 1
  );

  showOneLED(ledPosition);
}

// ---------- Exercise 2 ----------

void runExercise2(int step, bool buttonPressed, float dt) {
  if (buttonPressed) {
    // Treat the current knob location as the new center.
    knobOffset = 0;
    velocity = 0.0f;

    ledPosition = middlePosition;
    sequencePosition = middlePosition;

    resetEncoderTracking();

    showOneLED(middlePosition);
    return;
  }

  knobOffset += step;

  knobOffset = constrain(
    knobOffset,
    -maxKnobOffset,
    maxKnobOffset
  );

  float acceleration = knobOffset * accelerationScale;

  // Acceleration changes velocity over time.
  velocity += acceleration * dt;

  if (velocity > maxSpeed) {
    velocity = maxSpeed;
  } else if (velocity < -maxSpeed) {
    velocity = -maxSpeed;
  }

  // Velocity changes position over time.
  sequencePosition += velocity * dt;

  // Continuous circular sequence:
  // right end wraps to left, left end wraps to right.
  while (sequencePosition >= numberOfLEDs) {
    sequencePosition -= numberOfLEDs;
  }

  while (sequencePosition < 0.0f) {
    sequencePosition += numberOfLEDs;
  }

  // Select the nearest LED position.
  int displayPosition = (int)(sequencePosition + 0.5f);
  displayPosition %= numberOfLEDs;

  showOneLED(displayPosition);
}

// ---------- Exercise 3 ----------

void runExercise3(int step) {
  if (step == 0) {
    return;
  }

  brightness += step * brightnessIncrement;
  brightness = constrain(brightness, 0, 255);

  analogWrite(dimmerPin, brightness);
}

// ---------- LED output ----------

void showOneLED(int position) {
  for (byte i = 0; i < numberOfLEDs; i++) {
    digitalWrite(
      ledPins[i],
      i == position ? HIGH : LOW
    );
  }
}