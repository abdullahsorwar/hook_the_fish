#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initHighscores(SDL_Renderer* mainRenderer);
void renderHighscores();
void handleHighscoreEvents(SDL_Event& e, bool& highscoreOpen);
void destroyHighscores();
bool isHighscoreOpen();

#endif
