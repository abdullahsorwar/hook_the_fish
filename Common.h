#ifndef COMMON_H
#define COMMON_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <string>

struct Button {
    SDL_Rect rect;
    std::string text;
    bool hovered = false;
};

struct Cloud {
    SDL_Rect rect;
    int speed;
    int direction;
};

struct Fish {
    SDL_Rect rect;
    int speed;
    int direction;
    int baseY;
    bool hovered = false;
};

void drawPondBackground(SDL_Renderer* renderer, int width, int height);
void drawParallelogram(SDL_Renderer* renderer, Button& btn, bool hovered);
void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int x, int y);
void renderWrappedText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int x, int y, int wrapLength);

Cloud generateRandomCloud();
Fish generateRandomFish();

void loadTextures(SDL_Renderer* renderer);
void freeTextures();

void updateClouds(std::vector<Cloud>& clouds);
void updateFishes(std::vector<Fish>& fishes);
void renderFishesAndClouds(SDL_Renderer* renderer, const std::vector<Cloud>& clouds, const std::vector<Fish>& fishes);
bool loadSoundState();
bool loadWeatherState();
void saveSoundState(bool soundOn);
void saveWeatherState(bool sunnyOn);

extern SDL_Texture* fishRightClosedTexture;
extern SDL_Texture* fishRightOpenTexture;
extern SDL_Texture* fishLeftClosedTexture;
extern SDL_Texture* fishLeftOpenTexture;
extern SDL_Texture* cloudTexture;
extern SDL_Texture* hookleftTexture;
extern SDL_Texture* hookrightTexture;
extern SDL_Texture* copyright;
extern SDL_Texture* rodTexture;
extern bool running;
extern bool soundOn;
extern bool sunnyOn;
extern Mix_Music* intro;
extern Mix_Music* game_music;
extern SDL_Window* exitWindow;
extern SDL_Window* settingsWindow;
extern SDL_Window* menuWindow;
extern SDL_Window* GameRulesWindow;
extern SDL_Window* WeatherWindow;
extern SDL_Window* ControlWindow;
extern SDL_Window* newgameWindow;
extern SDL_Window* loadingWindow;
extern SDL_Window* interfaceWindow;
extern SDL_Window* mediumInterfaceWindow;
extern SDL_Window* objectiveWindow;
extern SDL_Window* gamewinWindow;
extern bool GameRulesOpen;
extern bool WeatherOpen;
extern bool hardinterfaceOpen;
extern bool MediuminterfaceOpen;
extern bool objectiveClose;

#endif