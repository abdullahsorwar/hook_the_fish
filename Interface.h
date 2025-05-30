#ifndef INTERFACE_H
#define INTERFACE_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initInterface(SDL_Renderer* newgameRenderer);
void renderInterface();
void handleInterfaceEvents(SDL_Event& e, bool& interfaceOpen);
void destroyInterface();
bool isInterfaceOpen();

#endif