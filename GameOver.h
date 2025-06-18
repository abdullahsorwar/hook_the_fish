#ifndef GAMEOVER_H
#define GAMEOVER

#include <SDL2/SDL.h>
#include <stdbool.h>

void initGameOver();
void handleGameOverEvents(SDL_Event& e, bool& GameOverOpen);
void renderGameOver();
void destroyGameOver();
bool isGameOverOpen();
extern bool GameOverOpen;
extern SDL_Renderer* GameOverRenderer;

#endif
