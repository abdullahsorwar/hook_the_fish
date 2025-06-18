#ifndef EASYINTERFACE_H
#define EASYINTERFACE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void loadEasyFishAssets();
void loadEasyObjectiveAssets(int type, int index);
void spawnEasyFish();
void updateEasyFishMotion();
void renderEasyFishAndRipples();
void handleEasyFishClick(int x, int y);
void EasyendGame(int targetScore);

void initEasyInterface();
void initEasyObjective();
void EasyinitGameWin();
void renderEasyInterface();
void renderEasyObjective();
void EasyrenderGameWin();
void handleEasyInterfaceEvents(SDL_Event& e, bool& interfaceOpen);
void handleEasyInterfaceLogics();
void destroyEasyInterface();
bool isEasyInterfaceOpen();

#endif

