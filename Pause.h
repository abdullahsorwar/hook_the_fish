#ifndef PAUSE_H
#define PAUSE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

extern bool isPaused;
extern Uint32 pauseStartTime;
extern Uint32 totalPaused;
void loadEasyPauseObjectiveAssets(int type, int index);
void loadHardPauseObjectiveAssets(int type, int index);

void initPauseMenu();
void renderPauseMenu();
void handlePauseMenuEvents(SDL_Event& e, bool& isPaused);
void destroyPauseMenu();
bool isPauseOpen();

void initPauseExit();
void renderPauseExit();
void handlePauseExitEvents(SDL_Event& e, bool& isExit);
void destroyPauseExit();
bool ispauseExitOpen(); 

void initPauseObjective();
void renderPauseObjective();
void handlePauseObjectiveEvents(SDL_Event& e, bool& objectiveOpen);

#endif