#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

void initGameOver();
void renderGameOver();
void handleGameOverEvents(SDL_Event& e, bool& gameoverOpen);
void destroyGameOver();
bool isGameOverOpen();

#endif