/*
  zyn: 16 x 8 SK6812 mini-e matrix on an ESP32-C3 (Arduino IDE, Espressif ESP32 core)

  written with help from AI. the pins were checked against the netlist and the
  ESP32-C3-WROOM-02 pin layout, but this exact file has not been run on the board yet.

  needs the FastLED library (Library Manager, "FastLED").

  buttons
  - BTN_A (IO9, the boot button): cycle effects
  - BTN_B (RXD / GPIO20): cycle brightness

  the mic (ICS-43434) is not used in this version.
*/

#include <FastLED.h>

const uint8_t LED_DATA_PIN = 10;   // IO10 -> LED_DATA -> level shifter -> first pixel DIN
const uint8_t LED_EN_PIN   = 4;    // IO4 -> LED_EN, HIGH turns VLED on for the matrix
const uint8_t BTN_A_PIN    = 9;    // IO9 (boot), active low
const uint8_t BTN_B_PIN    = 20;   // RXD / GPIO20, BTN_B, active low
const uint8_t STATUS_PIN   = 8;    // IO8 -> blue status LED (D3)

const uint8_t COLS = 16;
const uint8_t ROWS = 8;
const uint16_t NUM_LEDS = COLS * ROWS;  // 128

// the power rail for the matrix is VSYS, so on battery the pixels see 3.0 to 4.2 V.
// keep the brightness low until you have checked the pixels at that voltage.
const uint8_t BRIGHT[] = {10, 25, 50};

CRGB leds[NUM_LEDS];
uint8_t mode = 0;          // 0 rainbow, 1 scan line, 2 sparkle, 3 breathe
uint8_t brightLevel = 0;

// the pixels are chained DIN to DOUT in a snake: row 0 runs left to right,
// row 1 runs right to left, and so on. this gives the index in the chain for (x, y).
uint16_t xyToIndex(uint8_t x, uint8_t y) {
  uint8_t col = (y % 2 == 0) ? x : (COLS - 1 - x);
  return y * COLS + col;
}

void effectRainbow(uint32_t t) {
  for (uint8_t y = 0; y < ROWS; y++)
    for (uint8_t x = 0; x < COLS; x++)
      leds[xyToIndex(x, y)] = CHSV((x * 8 + y * 16 + t / 20) & 0xFF, 255, 255);
}

void effectScan(uint32_t t) {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  uint8_t col = (t / 120) % COLS;
  for (uint8_t y = 0; y < ROWS; y++)
    leds[xyToIndex(col, y)] = CRGB::Cyan;
}

void effectSparkle() {
  fadeToBlackBy(leds, NUM_LEDS, 40);
  if (random8() < 90) leds[random16(NUM_LEDS)] = CHSV(random8(), 255, 255);
}

void effectBreathe(uint32_t t) {
  (void)t;
  uint8_t v = beatsin8(20, 0, 255);
  fill_solid(leds, NUM_LEDS, CHSV(160, 255, v));
}

// simple edge detect, polled every 20 ms
bool lastA = HIGH, lastB = HIGH;
void readButtons() {
  bool a = digitalRead(BTN_A_PIN);
  bool b = digitalRead(BTN_B_PIN);
  if (lastA == HIGH && a == LOW) mode = (mode + 1) % 4;
  if (lastB == HIGH && b == LOW) brightLevel = (brightLevel + 1) % 3;
  lastA = a;
  lastB = b;
}

void setup() {
  pinMode(LED_EN_PIN, OUTPUT);
  digitalWrite(LED_EN_PIN, HIGH);
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(STATUS_PIN, OUTPUT);

  FastLED.addLeds<SK6812, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps(3.7, 400);
  FastLED.setBrightness(BRIGHT[brightLevel]);
}

void loop() {
  static uint32_t lastPoll = 0, lastFrame = 0;
  uint32_t t = millis();

  if (t - lastPoll >= 20) {
    lastPoll = t;
    readButtons();
  }

  if (t - lastFrame >= 33) {
    lastFrame = t;
    switch (mode) {
      case 0: effectRainbow(t); break;
      case 1: effectScan(t); break;
      case 2: effectSparkle(); break;
      case 3: effectBreathe(t); break;
    }
    FastLED.setBrightness(BRIGHT[brightLevel]);
    FastLED.show();
    digitalWrite(STATUS_PIN, (t / 500) & 1);  // heartbeat blink
  }
}
