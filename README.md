# Arduino Lab: Rotary Encoder, Ultrasonic Distance Sensor, and NeoPixel Light Show

## Overview

This README documents three Arduino lab activities:

1. Rotary encoder: LED position, sequencer acceleration, and LED brightness.
2. Ultrasonic sensor: distance reporting, unit selection, and two LED bar-graph ranges.
3. NeoPixels: an animated RGB LED light show.

The examples assume an Arduino Uno, a five-pin KY-040 encoder module, a four-pin HC-SR04 sensor, and a compatible 5V RGB NeoPixel strip.

> These are separate circuit configurations. Disconnect power before changing wiring. Pins are reused between activities; do not connect every peripheral to the same pins simultaneously.

## Project status

The programs were developed during the lab discussion. This README describes their intended behavior, not a verified hardware test report. Record actual results in the testing checklist before submitting the project.

## Contents

- [Safety](#safety)
- [Parts and software](#parts-and-software)
- [Suggested sketch organization](#suggested-sketch-organization)
- [Rotary encoder activities](#rotary-encoder-activities)
- [Ultrasonic distance activities](#ultrasonic-distance-activities)
- [NeoPixel light show](#neopixel-light-show)
- [Uploading and operation](#uploading-and-operation)
- [Troubleshooting](#troubleshooting)
- [Testing checklist](#testing-checklist)
- [Known limitations](#known-limitations)

## Safety

- Disconnect USB and external power before wiring or moving components.
- If the Arduino, sensor, strip, or wiring becomes unusually hot, disconnect all power immediately. Do not repeatedly reconnect a board that overheats.
- Inspect for reversed power connections, shorts between power and ground, damaged components, and metal touching the underside of the board.
- Use one series current-limiting resistor per discrete LED.
- A pull-up resistor is not the same as an LED current-limiting resistor.
- Verify device labels and supply voltage. Do not identify connections solely by wire color or physical pin order.
- The HC-SR04 wiring described here is for a 5V Uno. Do not apply its 5V echo output directly to a 3.3V-only input.
- Size NeoPixel power separately. Do not assume the Uno can power an entire strip.
- Ask the lab instructor to inspect any circuit with an unresolved overheating problem.

## Parts and software

### Shared equipment

- Arduino Uno and USB cable
- Arduino IDE
- Breadboard and jumper wires
- Five discrete LEDs
- Five 220 ohm or 330 ohm LED resistors

### Rotary encoder

- KY-040 module with CLK, DT, SW, +/VCC, and GND

### Ultrasonic sensor

- HC-SR04 module with VCC, TRIG, ECHO, and GND
- Maintained switch or momentary pushbutton for unit selection

### NeoPixels

- 5V RGB NeoPixel-compatible strip
- One 330–470 ohm data-line resistor
- Regulated 5V supply with sufficient current capacity
- Recommended: 500–1000 microfarad electrolytic capacitor, rated at least 6.3V

### Libraries

The rotary encoder and ultrasonic examples use standard Arduino functions and need no additional library.

Install the NeoPixel library using:

```text
Arduino IDE → Sketch → Include Library → Manage Libraries
Search: Adafruit NeoPixel
Install: Adafruit NeoPixel by Adafruit
```

## Suggested sketch organization

These filenames are suggestions, not a statement that the files are included with this README.

```text
Rotary_All_Exercises/Rotary_All_Exercises.ino
Ultrasonic_All_Exercises/Ultrasonic_All_Exercises.ino
NeoPixel_Light_Show/NeoPixel_Light_Show.ino
```

Alternatively, save each rotary exercise as its own sketch using the separate versions from the lab discussion.

Each sketch must contain only one `setup()` and one `loop()`. Do not concatenate several complete sketches into a single file.

## Rotary encoder activities

### Wiring

| Component | Uno connection |
|---|---|
| Encoder CLK / A | D2 |
| Encoder DT / B | D3 |
| Encoder SW | D4 |
| Encoder + / VCC | 5V |
| Encoder GND | GND |
| LED 1, leftmost | D5 through its resistor |
| LED 2 | D6 through its resistor |
| LED 3, middle | D7 through its resistor |
| LED 4 | D8 through its resistor |
| LED 5, rightmost | D9 through its resistor |

Each discrete LED branch is:

```text
Arduino output → resistor → LED anode (long leg)
                            LED cathode (short leg) → GND
```

Arrange the five LEDs physically from left to right in the order D5, D6, D7, D8, D9.

The program uses `INPUT_PULLUP` for CLK, DT, and SW. A typical KY-040 module already has rotation-signal pull-ups; no separate LED-style series resistor is needed for the encoder inputs. The button reads LOW when pressed.

### Selecting an exercise

In the combined rotary sketch, edit and upload:

```cpp
const byte EXERCISE = 1;  // 1, 2, or 3
```

This is a compile-time selection. Pressing the knob does not switch exercises.

### Exercise 1: Single-LED position

Requirements:

- Exactly one LED is on.
- Turning left moves the light left.
- Turning right moves it right.
- At either endpoint, the light holds position rather than wrapping.

The final version also implements the requested button reset: pressing the knob lights the middle LED on D7.

The position is bounded from 0 to 4:

```cpp
ledPosition = constrain(ledPosition, 0, 4);
```

Clamping the displayed position after each logical movement prevents hidden overshoot from requiring many reverse turns before the LED moves again.

### Exercise 2: Sequencer acceleration

The program tracks:

| Variable | Meaning |
|---|---|
| `knobOffset` | Signed displacement from the software center |
| `acceleration` | Signed acceleration derived from the offset |
| `velocity` | Signed sequence speed in LED positions per second |
| `sequencePosition` | Continuous position of the moving light |

The update is:

```cpp
acceleration = knobOffset * accelerationScale;
velocity += acceleration * dt;
sequencePosition += velocity * dt;
```

Expected behavior:

- Greater offset produces greater acceleration, up to the configured offset limit.
- An opposite-signed offset applies opposite acceleration.
- Opposite acceleration first slows existing motion, then reverses it if maintained.
- Returning to zero offset removes acceleration; it does not remove existing velocity.
- Pressing the knob resets offset and velocity to zero and returns the light to D7.

The supplied implementation wraps the sequence from the last LED to the first, and vice versa. Confirm that this matches the pattern in the earlier switch-based sequencing lab. The earlier lab code was not supplied for comparison.

Default tuning:

```cpp
const int maxKnobOffset = 10;
const float accelerationScale = 1.0f;
const float maxSpeed = 15.0f;
```

These limits prevent acceleration offset and speed from growing indefinitely.

### Exercise 3: Dimmer

Only the LED on D9 is used. Other LEDs remain off.

```cpp
brightness += step * brightnessIncrement;
brightness = constrain(brightness, 0, 255);
analogWrite(dimmerPin, brightness);
```

- Starts at brightness 0: off.
- Rotation increases or decreases brightness.
- Brightness stops at 0 and 255 rather than wrapping.
- The supplied dimmer mode does not use the button.

D9 is a PWM-capable Uno pin. PWM varies the fraction of time the LED output is on; this is not a true continuously variable DC voltage output.

### Decoder settings

```cpp
const int directionSign = 1;
const int countsPerStep = 4;
```

- Change `directionSign` to `-1` if physical rotation moves the output in the wrong direction.
- `countsPerStep` groups valid A/B transitions into one logical movement. Try 2 or 1 if 4 requires too much rotation.
- One logical movement is not guaranteed to equal one physical detent for every encoder.
- The transition-table decoder rejects sampled changes in which both A and B change simultaneously and resynchronizes to the sampled state.
- Encoder reading is polling-based. Keep the main loop responsive; fast turning, contact behavior, or blocking work can cause missed transitions.

The final combined rotary sketch does not print Serial Monitor messages. A blank Serial Monitor is therefore expected.

## Ultrasonic distance activities

### Wiring

Remove the rotary encoder connections before using these pins for the sensor.

| Component | Uno connection |
|---|---|
| HC-SR04 VCC | 5V |
| HC-SR04 TRIG | D2 |
| HC-SR04 ECHO | D3 |
| HC-SR04 GND | GND |
| Unit switch | Between D4 and GND |
| Five LEDs, left to right | D5, D6, D7, D8, D9 through individual resistors |

Keep every LED cathode connected to GND.

### Unit switch

```text
D4 → switch → GND
```

The sketch uses:

```cpp
pinMode(unitSwitchPin, INPUT_PULLUP);
```

| Switch state | Display units |
|---|---|
| Open / released | Centimeters |
| Closed / held | Inches |

No external pull-up resistor or 5V switch wire is required for this arrangement.

A momentary pushbutton selects inches only while held. A maintained switch keeps the selected state. The supplied code does not toggle units on successive presses.

For a four-leg tactile button, identify the two internally connected pairs and use one contact from each pair. Do not connect D4 and GND to contacts that are already permanently joined. Mount the button so the breadboard does not short its separate contacts together.

### Selecting an exercise

```cpp
const byte EXERCISE = 1;  // 1, 2, or 3
```

Upload again after changing the selection. Set the Serial Monitor to 115200 baud.

### Measurement method

The sketch:

1. Sends a HIGH trigger pulse for 10 microseconds.
2. Measures the echo HIGH duration with `pulseIn()`.
3. Converts duration to centimeters.
4. Prints the reading in the selected units.
5. Updates the LEDs for the selected bar-graph mode.

```cpp
float distanceCm = durationUs / 58.0f;
float distanceInches = distanceCm / 2.54f;
```

The supplied sketch uses a 30,000-microsecond echo timeout and starts readings at approximately 100-millisecond intervals. A timeout produces `No valid echo`; it is not treated as a measured distance of zero.

### Exercise 1: Serial distance and units

Select mode 1. LEDs remain off.

Example output:

```text
Distance: 20.0 cm
```

Closing the switch changes the display to approximately:

```text
Distance: 7.87 in
```

This is the only ultrasonic exercise that requires the unit switch. In the combined sketch the switch also changes the optional Serial output in modes 2 and 3, but not their LED thresholds.

### Exercise 2: Bar graph from 0 to 20 cm

Select mode 2. The code divides the distance span into five equal 4-cm increments.

| Valid distance | LEDs on |
|---|---:|
| Less than 4 cm | 0 |
| 4 to less than 8 cm | 1 |
| 8 to less than 12 cm | 2 |
| 12 to less than 16 cm | 3 |
| 16 to less than 20 cm | 4 |
| 20 cm or greater | 5 |

The HC-SR04's specified minimum range is approximately 2 cm. The software defines zero as all off, but an object at 0 cm cannot be measured reliably with this sensor.

### Exercise 3: Bar graph from 10 to 100 cm

Select mode 3. The 90-cm span is divided into five equal 18-cm increments.

```cpp
int ledsOn = calculateBar(distanceCm, 10.0f, 100.0f);
showBar(ledsOn);
```

| Valid distance | LEDs on |
|---|---:|
| Less than 28 cm, including 10 cm and below | 0 |
| 28 to less than 46 cm | 1 |
| 46 to less than 64 cm | 2 |
| 64 to less than 82 cm | 3 |
| 82 to less than 100 cm | 4 |
| 100 cm or greater | 5 |

This is a lower-bound, completed-step mapping: the first LED turns on one full increment above the minimum. If the instructor expects the first LED immediately above the minimum, change the quantization convention and document the new thresholds.

Changing the bar-graph range does not change the sensor's physical measuring range.

### Testing the sensor

Use a broad, flat target facing the sensor, such as a book or cardboard panel. Move it to several measured distances and compare with the Serial output.

If no echo is received, the supplied program turns all LEDs off and prints an error. This indicates an invalid measurement, not that the object is necessarily close. If a valid echo is returned beyond the display maximum, all five LEDs turn on.

## NeoPixel light show

### Lab objective

Create a light show using an addressable RGB strip. The provided slide uses D5 as the data output and requires the Adafruit NeoPixel library.

The supplied light-show example repeats:

1. Red, green, and blue color wipes.
2. A rainbow animation.
3. A moving comet.
4. A theater-chase pattern.
5. Colored sparkles.

No button, encoder, ultrasonic sensor, or Serial Monitor is needed by this sketch.

### Wiring with an external 5V supply

Disconnect the earlier circuit from D5 before attaching the strip.

| Connection | Destination |
|---|---|
| Uno D5 | 330–470 ohm resistor, then strip DIN |
| Uno GND | Strip GND / supply negative |
| Regulated 5V supply positive | Strip +5V |
| Supply negative | Strip GND |
| Uno USB | Computer |

```text
Uno D5 ── 330–470 ohm ───────── DIN
Uno GND ────────────┬───────── GND
Supply negative ───┘
Supply +5V ─────────────────── +5V
```

Leave the Uno's 5V pin unconnected in this external-strip-power arrangement. Do not join external +5V to the Uno's 5V pin while USB also powers the Uno in this setup.

- Use the input end labeled DIN or DI, not DOUT.
- Follow the strip's data-direction arrows.
- Put the resistor near the first pixel's DIN.
- Establish a common ground between Uno and strip supply.
- Recommended: place a 500–1000 microfarad capacitor across the strip power input, rated at least 6.3V, with correct polarity.
- A traditional RGB NeoPixel strip does not need a separate external LED resistor for every pixel; its data resistor is a different component serving a different purpose.

### Power sizing

For traditional RGB NeoPixels, up to 60 mA per pixel at full-brightness white is a conservative planning estimate. Actual current depends on the pixel model and pattern.

Example: 30 pixels can approach 1.8 A under that estimate. A regulated 5V, 2A supply can support that nominal pixel load; consider wiring losses, margin, and any additional loads when choosing the actual supply.

Do not use a 9V or 12V source directly on a 5V strip. Low software brightness is useful for initial testing, but does not replace a properly sized and protected power arrangement.

### Sketch configuration

```cpp
#include <Adafruit_NeoPixel.h>

const byte DATA_PIN = 5;
const uint16_t NUM_PIXELS = 8;  // Replace with actual count
const byte BRIGHTNESS = 25;

Adafruit_NeoPixel strip(
  NUM_PIXELS,
  DATA_PIN,
  NEO_GRB + NEO_KHZ800
);
```

The example assumes common GRB-order RGB pixels with an 800 kHz protocol. Verify the actual strip specification. RGBW strips or strips with different color ordering require a different pixel-type setting.

The startup sequence is:

```cpp
strip.begin();
strip.setBrightness(BRIGHTNESS);
strip.clear();
strip.show();
```

Calls to `setPixelColor()` update the library's pixel buffer. Call `show()` to send that buffer to the strip.

The supplied animations use delays intentionally. They are standalone demonstrations, not responsive combined sensor programs.

## Uploading and operation

1. Disconnect power and wire only the activity being tested.
2. Check power polarity, ground continuity, resistor placement, and pin labels.
3. Open the appropriate sketch.
4. Install the NeoPixel library if needed.
5. Select the correct board and port in the Arduino IDE.
6. Set `EXERCISE` for a combined encoder or ultrasonic sketch.
7. Set `NUM_PIXELS` for the NeoPixel sketch.
8. Upload and confirm that uploading succeeded.
9. For ultrasonic output, open Serial Monitor at 115200 baud.
10. Run the relevant checklist below.

The board runs only the most recently uploaded sketch. Uploading a new activity replaces the previously running program.

## Troubleshooting

### Arduino or wiring heats up

Disconnect all power. Do not add a random resistor to a supply lead as a fix. Inspect for shorts, reversed polarity, excessive loads, missing LED resistors, or damaged hardware. Have the lab instructor inspect the setup before further testing if the cause is unclear.

### Serial Monitor shows question marks or garbage

Match the monitor baud setting to the sketch:

```cpp
Serial.begin(115200);
```

If you change the program to 9600 and upload it, change the monitor to 9600 too. The final rotary and NeoPixel sketches do not send Serial output.

### No discrete LEDs light

- Confirm that uploading succeeded.
- Check the ground rail's connection to Uno GND.
- Check LED polarity and individual resistors.
- Verify that LED legs occupy electrically separate breadboard rows.
- Check for a break in the breadboard power rails.
- Test one LED at a time before debugging the sensor logic.

### Encoder does not respond

- Check CLK to D2 and DT to D3.
- Confirm module power and common ground.
- Verify that the correct sketch is running.
- Test A and B states with the earlier raw-input diagnostic sketch.
- Try `countsPerStep = 2` or `1` only after checking wiring.

### Encoder movement is reversed

Set:

```cpp
const int directionSign = -1;
```

### Encoder button does not reset

- Check SW to D4.
- Confirm `INPUT_PULLUP` is enabled.
- Use rotary exercise 1 or 2; the dimmer does not implement reset.
- Confirm the fixed exercise 1 call passes `buttonPressed` to its function.

### Ultrasonic reading is invalid or unstable

- Check VCC, GND, TRIG, and ECHO labels.
- Use a broad, flat target facing the sensor.
- Avoid trying to measure an object touching the sensor.
- Check the Serial output before debugging the LED scale.
- Distinguish `No valid echo` from a valid distance beyond the selected display range.
- Readings near an LED threshold can cause flicker; the supplied program has no filtering or hysteresis.

### Unit switch always shows the same units

- Check the D4-to-GND switch wiring.
- For four-leg buttons, verify that the chosen contacts are separate when released.
- A momentary button changes units only while held.
- Do not expect the switch to change the LED bar-graph scale.

### NeoPixels remain dark

- Verify strip voltage and supply polarity.
- Check the external supply and common ground.
- Confirm D5 connects through the resistor to DIN, not DOUT.
- Set the actual pixel count.
- Install the Adafruit NeoPixel library.
- Verify the strip's protocol and RGB/RGBW type.

### NeoPixel colors are wrong

Check the constructor's color-order setting against the strip specification. A strip expecting a different order may display red, green, or blue on the wrong channel.

## Testing checklist

Mark a box only after observing the result on the actual hardware.

### Rotary encoder

- [ ] Circuit remains cool and wiring has been checked.
- [ ] Exercise 1 starts with the D7 middle LED on.
- [ ] Right rotation moves right; left rotation moves left.
- [ ] Exactly one LED is on in exercise 1.
- [ ] Both endpoints hold without wrapping.
- [ ] Pressing resets exercise 1 to D7.
- [ ] Exercise 2 accelerates according to signed encoder offset.
- [ ] Opposite acceleration slows and can reverse the sequence.
- [ ] Pressing stops exercise 2 and returns the light to D7.
- [ ] Exercise 2's sequence pattern matches the earlier lab requirement.
- [ ] Exercise 3 changes only the D9 LED brightness.
- [ ] Brightness stops at fully off and fully on.

### Ultrasonic sensor

- [ ] Serial text is readable at the selected baud rate.
- [ ] Distance changes when the target moves.
- [ ] The switch selects centimeters and inches correctly.
- [ ] Exercise 2 follows the documented 0–20 cm LED thresholds.
- [ ] All five LEDs turn on at 20 cm or beyond with a valid reading.
- [ ] Exercise 3 follows the documented 10–100 cm LED thresholds.
- [ ] All five LEDs turn on at 100 cm or beyond with a valid reading.
- [ ] Invalid readings print an error rather than a false zero distance.

### NeoPixels

- [ ] Pixel count and strip type are configured correctly.
- [ ] Supply voltage, current capacity, common ground, and polarity are checked.
- [ ] Data enters DIN through the series resistor.
- [ ] Color wipes run.
- [ ] Rainbow runs.
- [ ] Comet runs.
- [ ] Theater chase runs.
- [ ] Sparkles run.
- [ ] The light show repeats without abnormal heating.

## Known limitations

- No physical test results are implied by this documentation.
- Combined sketches run one selected exercise at a time; they do not include runtime mode selection.
- The encoder uses polling and transition accumulation, not hardware interrupts or a dedicated encoder library.
- Encoder detent behavior varies; `countsPerStep` may need adjustment.
- The acceleration offset and velocity are limited by software settings.
- The previous switch-based sequencer was not supplied, so its exact movement pattern has not been verified against the wrapping implementation.
- The ultrasonic sensor cannot reliably measure 0 cm; its nominal minimum is approximately 2 cm.
- The bar graphs use discrete completed-step thresholds rather than a continuous display.
- Ultrasonic readings are not averaged and have no LED-threshold hysteresis.
- The unit switch selects a state; a momentary button is not a press-to-toggle control in the supplied code.
- NeoPixel power requirements and constructor settings depend on the actual strip.
- This README documents setup and operation. Keep the complete `.ino` programs alongside it as separate files.
