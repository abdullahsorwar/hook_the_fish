#ifndef NEWGAME_H
#define NEWGAME_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initNewGame(SDL_Renderer* mainRenderer);
void handleNewGameEvents(SDL_Event& e, bool& newgameOpen);
void renderNewGame();
void destroyNewGame();
bool isNewGameOpen();

#endif 
