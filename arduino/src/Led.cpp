#include "Led.h"
#include "BoardConfig.h"
#include <Adafruit_NeoPixel.h>

Led::Led(uint8_t pin) : pin_(pin) {
  pixel_ = new Adafruit_NeoPixel(1, pin, NEO_GRB + NEO_KHZ800);
}

void Led::Begin() {
  if (pixel_) {
    pixel_->begin();
    pixel_->setBrightness(5);
    pixel_->clear();
    pixel_->show();
  }
}

void Led::Set(bool on) {
  if (pixel_) {
    if (on) {
      // Green for active/connected
      pixel_->setPixelColor(0, pixel_->Color(0, 255, 0));
    } else {
      pixel_->clear();
    }
    pixel_->show();
  }
}
