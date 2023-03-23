#include <FastLED.h>

#define LED_PIN     2
#define NUM_LEDS    150

CRGB leds[NUM_LEDS];

void setup() {

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, 150);
  
}

void loop() {
  for (int i = 0; i < 150; i++) {
    leds[i] = CRGB ( 0, 0, 255);  //corresponds to color 
    FastLED.show(); // actually turns it on
    delay(40);
  }
  for (int i = 149; i >= 0; i--) {
    leds[i] = CRGB ( 255, 0, 0);
    FastLED.show();
    delay(40);
  }
}