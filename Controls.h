#ifndef CONTROLS_H
#define CONTROLS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initControls(SDL_Renderer* settingsRenderer);
void handleControlsEvents(SDL_Event& e, bool& ControlOpen);
void renderControls();
void destroyControls();
bool isControlsOpen();
extern bool ControlOpen;

#endif
