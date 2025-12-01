#ifndef _LED
#define _LED

#include <Arduino.h>
#include <FastLED.h>
#include "types.h"
#include "patterns.h"

// Forward declaration - defined in mqttmanager
void controlChamberLight(bool on);

// Maximum supported LEDs
constexpr uint16_t MAX_LEDS = 300;

// LED array
extern CRGB leds[MAX_LEDS];

// Current color and pattern state
extern CRGB currentColor;
extern uint8_t currentPattern;
extern CRGB currentBgColor;

// Timing
extern unsigned long lastUpdatems;
extern unsigned long oldms;

// Timing constants
constexpr unsigned long MQTT_OFFLINE_TIMEOUT_MS = 30000;
constexpr unsigned long DOOR_DEBOUNCE_MS = 1000;
constexpr unsigned long DOOR_DOUBLE_TAP_MS = 2000;
constexpr unsigned long DOOR_INTERACTION_TIMEOUT_MS = 6000;
constexpr unsigned long LOG_DEBOUNCE_MS = 3000;

// Relay state tracking
extern bool relayCurrentState;

// Relay functions
void setupRelay();
void setRelayState(bool on);

// Color conversion
COLOR hex2rgb(String hex);

// Helper to convert WhitePlacement enum to FastLED EOrderW
inline EOrderW getEOrderW(uint8_t wPlacement) {
    switch (wPlacement) {
        case W_PLACEMENT_0: return W0;
        case W_PLACEMENT_1: return W1;
        case W_PLACEMENT_2: return W2;
        case W_PLACEMENT_3: return W3;
        default: return W3;
    }
}

// LED setup functions
template<uint8_t DATA_PIN>
void addLedsForChipType(uint8_t chipType, uint16_t count, uint8_t colorOrder, uint8_t wPlacement);

template<uint8_t DATA_PIN, uint8_t CLOCK_PIN>
void addLedsAPA102(uint16_t count, uint8_t colorOrder);

void setupLeds();

// LED state functions
void setLedState(CRGB color, uint8_t pattern, CRGB bgColor = CRGB::Black);
void setLedState(const COLOR& color, uint8_t pattern, CRGB bgColor = CRGB::Black);
void setLedColor(CRGB color);
void setLedColor(const COLOR& color);
void setLedsOff();
bool areLedsOff();

// Logging functions
void printLogs(String Desc, COLOR thisColor);
void printLogs(String Desc, uint8_t r, uint8_t g, uint8_t b);

// LED State Handlers
bool handleMaintenanceMode();
bool handleWifiDebugMode();
bool handleTestColorMode();
bool handleDiscoMode();
bool handleInitialBoot();
bool handleDoorDoubleTap();
bool handleErrorStates();
bool handlePauseStates();
bool handleOffStates();
bool handleStageColors();
bool handleIdleTimeout(bool inFinishWindow);
bool handleRunningStates(bool inFinishWindow);
bool handleFinishIndication();
bool handleLedReplicationOn(bool inFinishWindow);

// Main LED update dispatcher
void updateleds();

// Main LED loop
void ledsloop();

// Helper macro to add RGBW LEDs with specific color order and white placement
#define ADD_LEDS_RGBW(CHIP, PIN, ORDER, WPLACE) \
    FastLED.addLeds<CHIP, PIN, ORDER>(leds, count).setRgbw(Rgbw(kRGBWDefaultColorTemp, kRGBWExactColors, WPLACE))

// Template implementations must remain in header
// Note: Color order selection is only fully supported for RGBW chips.
// For non-RGBW chips, most use GRB (the default for WS2812B/SK6812).
template<uint8_t DATA_PIN>
void addLedsForChipType(uint8_t chipType, uint16_t count, uint8_t colorOrder, uint8_t wPlacement) {
    EOrderW wOrder = getEOrderW(wPlacement);

    // Handle RGBW chip types with full color order and W placement support
    if (chipType == CHIP_SK6812_RGBW) {
        switch (colorOrder) {
            case ORDER_RGB: ADD_LEDS_RGBW(SK6812, DATA_PIN, RGB, wOrder); break;
            case ORDER_RBG: ADD_LEDS_RGBW(SK6812, DATA_PIN, RBG, wOrder); break;
            case ORDER_GBR: ADD_LEDS_RGBW(SK6812, DATA_PIN, GBR, wOrder); break;
            case ORDER_BRG: ADD_LEDS_RGBW(SK6812, DATA_PIN, BRG, wOrder); break;
            case ORDER_BGR: ADD_LEDS_RGBW(SK6812, DATA_PIN, BGR, wOrder); break;
            default: ADD_LEDS_RGBW(SK6812, DATA_PIN, GRB, wOrder); break;
        }
        return;
    }

    if (chipType == CHIP_WS2814_RGBW) {
        switch (colorOrder) {
            case ORDER_GRB: ADD_LEDS_RGBW(WS2811, DATA_PIN, GRB, wOrder); break;
            case ORDER_RBG: ADD_LEDS_RGBW(WS2811, DATA_PIN, RBG, wOrder); break;
            case ORDER_GBR: ADD_LEDS_RGBW(WS2811, DATA_PIN, GBR, wOrder); break;
            case ORDER_BRG: ADD_LEDS_RGBW(WS2811, DATA_PIN, BRG, wOrder); break;
            case ORDER_BGR: ADD_LEDS_RGBW(WS2811, DATA_PIN, BGR, wOrder); break;
            default: ADD_LEDS_RGBW(WS2811, DATA_PIN, RGB, wOrder); break;
        }
        return;
    }

    // Handle non-RGBW chip types (GRB is most common default)
    switch (chipType) {
        case CHIP_WS2812B:
            FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, count);
            break;
        case CHIP_SK6812:
            FastLED.addLeds<SK6812, DATA_PIN, GRB>(leds, count);
            break;
        case CHIP_WS2811:
            FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, count);
            break;
        case CHIP_NEOPIXEL:
            FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, count);
            break;
        default:
            FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, count);
            break;
    }
}

template<uint8_t DATA_PIN, uint8_t CLOCK_PIN>
void addLedsAPA102(uint16_t count, uint8_t colorOrder) {
    FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR>(leds, count);
}

#endif
