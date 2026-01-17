#include "Application.h"

Application app;

void setup() {
  app.Initialize();
}

void loop() {
  app.Tick();
}

