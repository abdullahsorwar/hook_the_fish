#ifndef SETTINGS_H
#define SETTINGS_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initSettings(SDL_Renderer* mainRenderer);
void handleSettingsEvents(SDL_Event& e, bool& settingsOpen);
void renderSettings();
void destroySettings();
bool isSettingsOpen();

#endif
