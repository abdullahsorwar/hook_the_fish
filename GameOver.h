#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <string>
#include <vector>

void initGameOver();
void renderGameOver();
void renderHighScoreTrue();
void renderLifeOver();
void renderCrocodileOver();
void renderTimeOver();
void renderHighScoreFalse();
void handleTimeOverEvents(SDL_Event& e, bool& timeoverOpen);
void handleCrocodileOverEvents(SDL_Event& e, bool& crocodileoverOpen);
void handleHighScoreFalse(SDL_Event& e, bool& gameoverOpen);
void handleLifeOverEvents(SDL_Event& e, bool& lifeoverOpen);
void handleGameOverEvents(SDL_Event& e, bool& gameoverOpen);
void handleHighScoreTrue(SDL_Event& e, bool& gameoverOpen);
void destroyGameOver();
bool isGameOverOpen();


#endif