#ifndef EXIT_H
#define EXIT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initExit(SDL_Renderer* mainRenderer);
void renderExit();
void handleExitEvents(SDL_Event& e, bool& exitOpen);
void destroyExit();
bool isExitOpen();

#endif
