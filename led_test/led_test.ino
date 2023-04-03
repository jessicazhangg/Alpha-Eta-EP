#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    150

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
}

void loop() {
  for (int x = 0; x < 8; x ++ ) {
    for (int y = 0; y < 8; y ++ ) {
      int coord = led_coord(x,y);
      leds[coord] = CRGB::Red;
      leds[coord + 1] = CRGB::Red;
      Serial.println(coord);
      FastLED.show();
      leds[coord] = CRGB::Black;
      leds[coord + 1] = CRGB::Black;
      delay(500);
    }
  }
}

int led_coord(int x, int y) {
  if (x == 0) {
    return 133 + y * 2;
  }
  if (x == 1) {
    return 128 - y * 2;
  }
  if (x == 2) {
    return 95 + y * 2;
  }
  if (x == 3) {
    return 90 - y * 2;
  }
  if (x == 4) {
    return 57 + y * 2;
  }
  if (x == 5) {
    return 52 - y * 2;
  }
  if (x == 6) {
    return 19 + y * 2;
  }
  if (x == 7) {
    return 14 - y * 2;
  }
}


