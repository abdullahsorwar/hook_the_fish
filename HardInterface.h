#ifndef HARDINTERFACE_H
#define HARDINTERFACE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void loadHardFishAssets();
void loadObjectiveAssets(int type, int index);
void spawnHardFish();
void updateHardFishMotion();
void renderHardFishAndRipples();
void handleHardFishClick(int x, int y);
void endGame(int targetScore);

void initHardInterface();
void initHardObjective();
void initgameWin();
void renderHardInterface();
void renderHardObjective();
void rendergameWin();
void handleHardInterfaceEvents(SDL_Event& e, bool& interfaceOpen);
void handleHardInterfaceLogics(SDL_Event& e, bool& interfaceOpen);
void destroyHardInterface();
bool isHardInterfaceOpen();

#endif