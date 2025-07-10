#ifndef HARDINTERFACE_H
#define HARDINTERFACE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void loadHardFishAssets();
void loadObjectiveAssets(int type, int index);
void spawnHardFish();
void updateHardFishMotion();
void renderHardFishAndRipples();
void renderFadedText(int type, Uint32 init_time, int obj_type, int obj_count);
void handleHardFishClick(int x, int y);

void initHardInterface();
void initHardObjective();
void renderHardInterface();
void renderHardObjective();
void handleHardInterfaceEvents(SDL_Event& e, bool& interfaceOpen);
void handleHardInterfaceLogics();
void destroyHardInterface();
bool isHardInterfaceOpen();

#endif