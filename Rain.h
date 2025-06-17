#ifndef RAIN_H
#define RAIN_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initRain(int count);
void generateLightning();
void updateRain();
void renderRain();
void destroyRain();

#endif