#include "Kaleidoscope-LEDEffect-FunctionalColor.h"
#include "assert.h"

namespace kaleidoscope {

constexpr cRGB LEDFunctionalColor::colorMap::defaultColor;
constexpr cRGB LEDFunctionalColor::colorMap::shift;
constexpr cRGB LEDFunctionalColor::colorMap::control;
  //Command keys (macOS) or Windows Logo keys
constexpr cRGB LEDFunctionalColor::colorMap::gui;//dim(pink, 190);
constexpr cRGB LEDFunctionalColor::colorMap::alt;
  // Includes all the above modifiers
constexpr cRGB LEDFunctionalColor::colorMap::modifier;

constexpr cRGB LEDFunctionalColor::colorMap::alpha;
constexpr cRGB LEDFunctionalColor::colorMap::number;
constexpr cRGB LEDFunctionalColor::colorMap::punctuation;
constexpr cRGB LEDFunctionalColor::colorMap::function;
constexpr cRGB LEDFunctionalColor::colorMap::navigation;
constexpr cRGB LEDFunctionalColor::colorMap::system;
constexpr cRGB LEDFunctionalColor::colorMap::arrow;
constexpr cRGB LEDFunctionalColor::colorMap::keypad;
constexpr cRGB LEDFunctionalColor::colorMap::media;

constexpr cRGB LEDFunctionalColor::colorMap::mouseWheel;
constexpr cRGB LEDFunctionalColor::colorMap::mouseButton;
constexpr cRGB LEDFunctionalColor::colorMap::mouseWarp;
constexpr cRGB LEDFunctionalColor::colorMap::mouseMove;
  // Includes all above mouse functions
constexpr cRGB LEDFunctionalColor::colorMap::mouse;

  // Important single keys
constexpr cRGB LEDFunctionalColor::colorMap::space;
constexpr cRGB LEDFunctionalColor::colorMap::tab;
constexpr cRGB LEDFunctionalColor::colorMap::enter;
constexpr cRGB LEDFunctionalColor::colorMap::backspace;
constexpr cRGB LEDFunctionalColor::colorMap::escape;
constexpr cRGB LEDFunctionalColor::colorMap::del;
constexpr cRGB LEDFunctionalColor::colorMap::fn;
  //Numlock or any use of lockLayer
constexpr cRGB LEDFunctionalColor::colorMap::lock;
constexpr cRGB LEDFunctionalColor::colorMap::LEDEffectNext;
  // Where's the any key?

constexpr cRGB LEDFunctionalColor::colorMapDefault::shift;
constexpr cRGB LEDFunctionalColor::colorMapDefault::control;
constexpr cRGB LEDFunctionalColor::colorMapDefault::gui;
constexpr cRGB LEDFunctionalColor::colorMapDefault::alt;
constexpr cRGB LEDFunctionalColor::colorMapDefault::alpha;
constexpr cRGB LEDFunctionalColor::colorMapDefault::number;
constexpr cRGB LEDFunctionalColor::colorMapDefault::punctuation;
constexpr cRGB LEDFunctionalColor::colorMapDefault::function;
constexpr cRGB LEDFunctionalColor::colorMapDefault::navigation;
constexpr cRGB LEDFunctionalColor::colorMapDefault::system;
constexpr cRGB LEDFunctionalColor::colorMapDefault::arrow;
constexpr cRGB LEDFunctionalColor::colorMapDefault::keypad;
constexpr cRGB LEDFunctionalColor::colorMapDefault::mouseWheel;
constexpr cRGB LEDFunctionalColor::colorMapDefault::mouseButton;
constexpr cRGB LEDFunctionalColor::colorMapDefault::mouseWarp;
constexpr cRGB LEDFunctionalColor::colorMapDefault::mouseMove;
constexpr cRGB LEDFunctionalColor::colorMapDefault::media;
constexpr cRGB LEDFunctionalColor::colorMapDefault::space;
constexpr cRGB LEDFunctionalColor::colorMapDefault::tab;
constexpr cRGB LEDFunctionalColor::colorMapDefault::enter;
constexpr cRGB LEDFunctionalColor::colorMapDefault::backspace;
constexpr cRGB LEDFunctionalColor::colorMapDefault::escape;
constexpr cRGB LEDFunctionalColor::colorMapDefault::del;
constexpr cRGB LEDFunctionalColor::colorMapDefault::fn;
constexpr cRGB LEDFunctionalColor::colorMapDefault::lock;
constexpr cRGB LEDFunctionalColor::colorMapDefault::LEDEffectNext;

// themeDefault just sets the default theme.
void LEDFunctionalColor::FCPlugin::themeDefault() {
  this->setColorLookup(&LEDFunctionalColor::groupColorLookup<LEDFunctionalColor::colorMapDefault>);
}

void LEDFunctionalColor::FCPlugin::brightness(byte b) {
  brightnessSetting = b;
}

byte LEDFunctionalColor::FCPlugin::brightness() {
  return brightnessSetting;
}

//These are special macros that allow the brightness of an FC instance to be adjusted
void LEDFunctionalColor::FCPlugin::thisBrightnessUp(uint8_t keyState) {
  if (keyToggledOn(keyState)) {
    brightnessSetting += 16;
    this->refresh();
  }
}

void LEDFunctionalColor::FCPlugin::thisBrightnessDown(uint8_t keyState) {
  if (keyToggledOn(keyState)) {
    brightnessSetting -= 16;
    this->refresh();
  }
}

void LEDFunctionalColor::FCPlugin::brightnessUp(uint8_t keyState) {
  if(!LEDFunctionalColor::FCPlugin::lastFC) return;
  LEDFunctionalColor::FCPlugin::lastFC->thisBrightnessUp(keyState);
}

void LEDFunctionalColor::FCPlugin::brightnessDown(uint8_t keyState) {
  if(!LEDFunctionalColor::FCPlugin::lastFC) return;
  LEDFunctionalColor::FCPlugin::lastFC->thisBrightnessDown(keyState);
}

//setKeyLed accepts a Key position and sets it to the appropriate color from a lookup function
void LEDFunctionalColor::FCPlugin::setKeyLed(uint8_t r, uint8_t c) {
  Key k = Layer.lookupOnActiveLayer(r, c);
  bool skip = false, none = false;
  auto color = exceptionsLookup(k, skip, none);
  // if there was no match, we continue to mainColorLookup
  if (none) {
    if(mainColorLookup) color = mainColorLookup(k, skip);
    // Otherwise we return the defaultColor from exceptions (nocolor if no custom function is defined)  
  }
  if (skip) return;
  ::LEDControl.setCrgbAt(r, c, dim(color, brightnessSetting));
}// end setKeyLed()

LEDFunctionalColor::FCPlugin* LEDFunctionalColor::FCPlugin::lastFC;


//Forces an update of the LEDs
void LEDFunctionalColor::FCPlugin::refresh(void) {
  // Loop through every row and column and set each LED based on its key's function
  for (uint8_t r = 0; r < ROWS; r++) {
    for (uint8_t c = 0; c < COLS; c++) {
      Key k = Layer.lookupOnActiveLayer(r, c);
      setKeyLed(r, c);
    }
  }
  lastFC = this;
}


void LEDFunctionalColor::FCPlugin::onActivate(void) {
  this->refresh();
  lastFC = this;
}


//When the active layer is changed, update the colors.
void LEDFunctionalColor::FCPlugin::update(void) {
  uint8_t current_layerState = Layer_::getLayerState();

  // Only set the colors again if the active layer changed
  if (current_layerState != last_layerState) { this->refresh(); }

  last_layerState = current_layerState;

}// end update()



}//namespace
