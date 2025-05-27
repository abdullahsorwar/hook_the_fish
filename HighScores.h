#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void loadScoresFromFile(const std::string& filename);
void checkAndAddHighScore(const std::string& filename, const std::string& playerName, int newScore);
void initHighscores(SDL_Renderer* mainRenderer);
void openSubWindow(const std::string& title, const std::string& filename) ;
void renderHighscores();
void handleHighscoreEvents(SDL_Event& e, bool& highscoreOpen);
void destroyHighscores();
bool isHighscoreOpen();

#endif
