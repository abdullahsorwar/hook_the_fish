#ifndef HARDINTERFACE_H
#define HARDINTERFACE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

//extern SDL_Renderer* hardInterfaceRenderer;

void loadHardFishAssets();
void loadObjectiveAssets(int type, int index);
void spawnHardFish();
void updateHardFishMotion();
void renderHardFishAndRipples();
void renderFadedText(int type, Uint32 init_time, int obj_type, int obj_count);
void handleHardFishClick(int x, int y);
void gameCondition();

void initHardInterface();
void initHardObjective();
void initgameWin();
void renderHardInterface();
void renderHardObjective();
void rendergameWin();
void handleHardInterfaceEvents(SDL_Event& e, bool& interfaceOpen);
void handleHardInterfaceLogics();
void destroyHardInterface();
bool isHardInterfaceOpen();

#endif