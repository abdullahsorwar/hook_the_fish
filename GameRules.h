#ifndef GAMERULES_H
#define GAMERULES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initGameRules(SDL_Renderer* settingsRenderer);
void handleGameRulesEvents(SDL_Event& e, bool& GameRulesOpen);
void renderGameRules();
void destroyGameRules();
bool isGameRulesOpen();

#endif
