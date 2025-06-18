#include <FastLED.h>

#define LED_PIN     4
#define NUM_LEDS    144
#define BRIGHTNESS  32
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
bool ledStates[NUM_LEDS] = {false};

enum Mode {
  FILLING,
  HOLD,
  FADEOUT,
  WAIT,
};

Mode currentMode = FILLING;
unsigned long modeStartTime = 0;

// -------------------------
// 🔧 НАЛАШТУВАННЯ ЕФЕКТУ
// -------------------------

// ⌛ Тривалість фаз (мс)
const unsigned long DURATION_FILL  = 60000;  // фаза засвічування
const unsigned long DURATION_HOLD  = 10000;  // утримання світла
const unsigned long DURATION_WAIT  = 5000;   // гасіння перед новим циклом
const unsigned long DURATION_FADE  = 1500;   // тривалість fadeout

// ⏱️ Затримка між оновленнями (мс)
const unsigned long UPDATE_DELAY_MS = 80;

// 🎛️ Ймовірність вмикання та вимкнення (від 0 до 1)
const float LIGHT_ON_PROB_START  = 0.18;  // на початку
const float LIGHT_ON_PROB_END    = 0.08;  // в кінці

const float LIGHT_OFF_PROB_START = 0.03;  // на початку
const float LIGHT_OFF_PROB_END   = 0.12;  // в кінці

const int UPDATES_PER_CYCLE = 3;  // скільки світлодіодів обробляємо за один loop

// 🎨 Кольори (змінюй як хочеш)
CRGB COLOR_1 = CRGB::Blue;
CRGB COLOR_2 = CRGB::Yellow;

// -------------------------

void setup() {
  delay(100);
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
         .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(true);
  modeStartTime = millis();
}

void loop() {
  unsigned long now = millis();
  unsigned long elapsed = now - modeStartTime;

  switch (currentMode) {
    case FILLING: {
      float progress = (float)elapsed / DURATION_FILL;
      progress = constrain(progress, 0.0, 1.0);

      float lightOnProb = LIGHT_ON_PROB_END +
                          (LIGHT_ON_PROB_START - LIGHT_ON_PROB_END) * (1.0 - progress);
      float lightOffProb = LIGHT_OFF_PROB_START +
                           (LIGHT_OFF_PROB_END - LIGHT_OFF_PROB_START) * progress;

      for (int i = 0; i < UPDATES_PER_CYCLE; i++) {
        int index = random(NUM_LEDS);

        if (!ledStates[index] && random(1000) < lightOnProb * 1000) {
          ledStates[index] = true;
          leds[index] = random(2) ? COLOR_1 : COLOR_2;
        }
        else if (ledStates[index] && random(1000) < lightOffProb * 1000) {
          ledStates[index] = false;
          leds[index] = CRGB::Black;
        }
      }

      FastLED.show();
      delay(UPDATE_DELAY_MS);

      if (elapsed >= DURATION_FILL) {
        currentMode = HOLD;
        modeStartTime = now;
      }
      break;
    }

    case HOLD:
      if (elapsed >= DURATION_HOLD) {
        currentMode = FADEOUT;
        modeStartTime = now;
      }
      break;

    case FADEOUT:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].fadeToBlackBy(40);
        ledStates[i] = false;
      }
      FastLED.show();
      delay(UPDATE_DELAY_MS);

      if (elapsed >= DURATION_FADE) {
        FastLED.clear(true);
        currentMode = WAIT;
        modeStartTime = now;
      }
      break;

    case WAIT:
      if (elapsed >= DURATION_WAIT) {
        currentMode = FILLING;
        modeStartTime = now;
      }
      break;
  }
}