#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void loadMediumFishAssets();
void loadMediumObjectiveAssets(int type, int index);
void spawnMediumFish();
void updateMediumFishMotion();
void renderMediumFishAndRipples();
void handleMediumFishClick(int x, int y);

void initMediumInterface();
void initMediumObjective();
void renderMediumInterface();
void renderMediumObjective();
void handleMediumInterfaceEvents(SDL_Event& e, bool& interfaceOpen);
void handleMediumInterfaceLogics(SDL_Event& e, bool& interfaceOpen);
void destroyMediumInterface();
bool isMediumInterfaceOpen();


#endif
