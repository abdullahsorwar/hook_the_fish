#ifndef WEATHER_H
#define WEATHER_H

#include <SDL2/SDL.h>
#include <stdbool.h>

void initWeather(SDL_Renderer* settingsRenderer);
void handleWeatherEvents(SDL_Event& e, bool& WeatherOpen);
void renderWeather();
void destroyWeather();
bool isWeatherOpen();

#endif
