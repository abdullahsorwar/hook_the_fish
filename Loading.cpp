#include "Common.h"
#include "NewGame.h"
#include "HardInterface.h"
#include "MediumInterface.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <cstdlib>
#include <cmath>

SDL_Window* loadingWindow = nullptr;
static SDL_Renderer* loadingRenderer = nullptr;
static TTF_Font* titleFont = nullptr;

void Loading()
{
    if (loadingWindow != nullptr) return;

    loadingWindow = SDL_CreateWindow("Loading", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                     1280, 720, SDL_WINDOW_BORDERLESS);
    loadingRenderer = SDL_CreateRenderer(loadingWindow, -1, SDL_RENDERER_ACCELERATED);
    if (!loadingRenderer) return;

    titleFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 64);
    if (!titleFont) return;

    int totalDuration = 5000 + std::rand() % 10001;
    Uint32 startTime = SDL_GetTicks();

    while (1)
    {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsed = currentTime - startTime;
        if (elapsed >= static_cast<Uint32>(totalDuration))
            break;

        float t = (elapsed % 5000) / 2500.0;
        float alphaF;
        if (t<1.0) alphaF = 250.0*(1.0-t);
        else alphaF = 250.0*(t-1.0);
        Uint8 alpha = static_cast<Uint8>(alphaF + 5.0);

        SDL_SetRenderDrawColor(loadingRenderer, 0, 0, 0, 255);
        SDL_RenderClear(loadingRenderer);

        SDL_SetRenderDrawBlendMode(loadingRenderer, SDL_BLENDMODE_BLEND);

        SDL_Color flashingColor = {255, 255, 255, alpha};
        renderText(loadingRenderer, titleFont, "LOADING...", flashingColor, 640, 360);

        SDL_RenderPresent(loadingRenderer);
    }

    TTF_CloseFont(titleFont);
    SDL_DestroyRenderer(loadingRenderer);
    SDL_DestroyWindow(loadingWindow);
    if (soundOn)
    {
        Mix_PauseMusic();
        Mix_PlayMusic(game_music, -1);
    }
    else Mix_PauseMusic();
    titleFont = nullptr;
    loadingRenderer = nullptr;
    loadingWindow = nullptr;
}