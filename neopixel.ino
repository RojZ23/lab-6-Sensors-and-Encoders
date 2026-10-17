#include <Adafruit_NeoPixel.h>

// ---------- Settings ----------

const byte DATA_PIN = 5;

// IMPORTANT: Change this to your actual number of pixels.
const uint16_t NUM_PIXELS = 10;

// Start with low brightness.
// Brightness setting does not replace proper power sizing.
const byte BRIGHTNESS = 25;

// For a common RGB WS2812/NeoPixel-compatible strip.
// RGBW strips require a different pixel-type setting.
Adafruit_NeoPixel strip(
  NUM_PIXELS,
  DATA_PIN,
  NEO_GRB + NEO_KHZ800
);

// ---------- Setup ----------

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);

  strip.clear();
  strip.show();
}

// ---------- Main light show ----------

void loop() {
  // Pattern 1: Color wipes
  colorWipe(strip.Color(255, 0, 0), 60);
  colorWipe(strip.Color(0, 255, 0), 60);
  colorWipe(strip.Color(0, 0, 255), 60);

  lightsOff(250);

  // Pattern 2: Rainbow
  rainbowShow();

  lightsOff(250);

  // Pattern 3: Moving comet
  cometShow();

  lightsOff(250);

  // Pattern 4: Theater chase
  theaterChase(strip.Color(255, 0, 100), 80);

  lightsOff(250);

  // Pattern 5: Sparkles
  sparkleShow();

  lightsOff(500);
}

// ---------- Pattern 1: Color wipe ----------

void colorWipe(uint32_t color, unsigned int waitMs) {
  strip.clear();

  for (uint16_t i = 0; i < NUM_PIXELS; i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(waitMs);
  }

  delay(250);
}

// ---------- Pattern 2: Rainbow ----------

void rainbowShow() {
  for (uint16_t frame = 0; frame < 256; frame++) {
    for (uint16_t i = 0; i < NUM_PIXELS; i++) {
      uint16_t hue = (uint16_t)(
        ((uint32_t)i * 65536UL / NUM_PIXELS)
        + ((uint32_t)frame * 256UL)
      );

      uint32_t color = strip.gamma32(
        strip.ColorHSV(hue)
      );

      strip.setPixelColor(i, color);
    }

    strip.show();
    delay(20);
  }
}

// ---------- Pattern 3: Moving comet ----------

void cometShow() {
  for (byte lap = 0; lap < 3; lap++) {
    for (uint16_t head = 0; head < NUM_PIXELS; head++) {
      strip.clear();

      // Bright head and progressively dimmer tail.
      for (byte tail = 0; tail < 4; tail++) {
        int position = (int)head - tail;

        if (position >= 0) {
          byte intensity = 255 >> tail;

          strip.setPixelColor(
            position,
            strip.Color(0, intensity, intensity)
          );
        }
      }

      strip.show();
      delay(70);
    }
  }
}

// ---------- Pattern 4: Theater chase ----------

void theaterChase(uint32_t color, unsigned int waitMs) {
  for (byte cycle = 0; cycle < 12; cycle++) {
    for (byte offset = 0; offset < 3; offset++) {
      strip.clear();

      for (
        uint16_t i = offset;
        i < NUM_PIXELS;
        i += 3
      ) {
        strip.setPixelColor(i, color);
      }

      strip.show();
      delay(waitMs);
    }
  }
}

// ---------- Pattern 5: Sparkles ----------

void sparkleShow() {
  for (byte frame = 0; frame < 60; frame++) {
    strip.clear();

    uint16_t pixel = (uint16_t)random(NUM_PIXELS);

    strip.setPixelColor(
      pixel,
      strip.Color(
        (byte)random(256),
        (byte)random(256),
        (byte)random(256)
      )
    );

    strip.show();
    delay(80);
  }
}

// ---------- Turn everything off ----------

void lightsOff(unsigned int waitMs) {
  strip.clear();
  strip.show();
  delay(waitMs);
}