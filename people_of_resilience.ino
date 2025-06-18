#include <FastLED.h>

#define LED_PIN     4       // Пін керування світлодіодами
#define NUM_LEDS    144     // Кількість діодів у стрічці
#define BRIGHTNESS  128     // Максимальна яскравість
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

void setup() {
  delay(100); // Трохи часу на старт ESP
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
}

void loop() {
  rainbowCycle(4);  // Візуальний ефект (демо)
}

void rainbowCycle(uint8_t wait) {
  static uint16_t startIndex = 0;
  startIndex++;

  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV((i * 256 / NUM_LEDS + startIndex) % 255, 255, 255);
  }
  FastLED.show();
  delay(wait);
}