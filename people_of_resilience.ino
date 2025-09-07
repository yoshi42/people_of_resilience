#include <FastLED.h>

#define LED_PIN     4
#define BUTTON_PIN  3
#define NUM_LEDS    144
#define BRIGHTNESS  25
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
bool ledStates[NUM_LEDS] = {false};

enum Mode {
  READ,
  FILLING,
  HOLD,
  FADEOUT,
  WAIT,
};

Mode currentMode = READ;
unsigned long modeStartTime = 0;

// -------------------------
// 🔧 НАЛАШТУВАННЯ ЕФЕКТУ
// -------------------------

// ⌛ Тривалість фаз (мс)
const unsigned long DURATION_FILL  = 20000;  // фаза засвічування
const unsigned long DURATION_HOLD  = 10000;  // утримання світла
const unsigned long DURATION_WAIT  = 3000;   // гасіння перед новим циклом
const unsigned long DURATION_FADE  = 500;   // тривалість fadeout
//private/var/folders/hm/k_sprrnd565bfqlhlqgckdqc0000gn/T/.arduinoIDE-unsaved202587-50396-gj91hj.bv7vo/Debounce/Debounce.ino
// ⏱️ Затримка між оновленнями (мс)
const unsigned long UPDATE_DELAY_MS = 80;

// 🎛️ Ймовірність вмикання та вимкнення (від 0 до 1)
const float LIGHT_ON_PROB_START  = 0.10;  // на початку
const float LIGHT_ON_PROB_END    = 0.15;  // в кінці

const float LIGHT_OFF_PROB_START = 0.05;  // на початку
const float LIGHT_OFF_PROB_END   = 0.03;  // в кінці

const int UPDATES_PER_CYCLE = 2;  // скільки світлодіодів обробляємо за один loop

// 🎨 Кольори (змінюй як хочеш)
CRGB COLOR_1 = CRGB(255, 160, 25);
CRGB COLOR_2 = CRGB(255, 160, 25);

// -------------------------

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
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
  case READ: {
      if (digitalRead(BUTTON_PIN) == LOW) {
        delay(50);
        currentMode = FILLING;
        modeStartTime = millis();
      }
    break;
  } 

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
        
        for (int i = 0; i < NUM_LEDS; i++) {
        int j = random(NUM_LEDS);
        leds[j] = CRGB(255, 160, 25);
        FastLED.show();
        }
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
        leds[i].fadeToBlackBy(255);
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
        currentMode = READ;
        modeStartTime = now;
      }
      break;
  }
}