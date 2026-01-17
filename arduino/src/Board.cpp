#include "Board.h"

void Board::Initialize() {
  Serial.begin(115200);
  led_.Begin();
  btn_.Begin();
}

void Board::Poll() {
}

void Board::SetLed(bool on) {
  led_.Set(on);
}

