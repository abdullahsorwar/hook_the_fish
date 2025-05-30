#include "Common.h"
#include "Interface.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

static SDL_Rect pond = {0, 250, 1280, 470};
static SDL_Rect pond2 = {-1279, 250, 1280, 470};
static SDL_Rect mountain = {0, 0, 1280, 250};
static SDL_Rect mountain2 = {-1279, 0, 1280, 250};

SDL_Window* interfaceWindow = nullptr;
static SDL_Renderer* interfaceRenderer = nullptr;

static SDL_Texture* pondTexture = nullptr;
static SDL_Texture* pond2Texture = nullptr;
static SDL_Texture* mountainTexture = nullptr;
static SDL_Texture* mountain2Texture = nullptr;

static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static TTF_Font* textFont = nullptr;

bool interfaceOpen = false;

void initInterface(SDL_Renderer* newgameRenderer){
    if (interfaceWindow != nullptr) return;
    interfaceWindow = SDL_CreateWindow("Easy Mode", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        1280, 720, 
        SDL_WINDOW_BORDERLESS);
    interfaceRenderer = SDL_CreateRenderer(interfaceWindow, -1, 
        SDL_RENDERER_ACCELERATED);

    SDL_Surface* surf;

    surf = SDL_LoadBMP("bmp/pond.bmp");
    pondTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/pond2.bmp");
    pond2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/mountain.bmp");
    mountainTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/mountain2.bmp");
    mountain2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

}
void renderInterface(){
    if (!interfaceRenderer) return;

    SDL_RenderCopy(interfaceRenderer, pondTexture, NULL, &pond);
    SDL_RenderCopy(interfaceRenderer, pond2Texture, NULL, &pond2);
    SDL_RenderCopy(interfaceRenderer, mountainTexture, NULL, &mountain);
    SDL_RenderCopy(interfaceRenderer, mountain2Texture, NULL, &mountain2);

    SDL_RenderPresent(interfaceRenderer);
}
void handleInterfaceEvents(SDL_Event& e, bool& interfaceOpen){
    if (!interfaceWindow) return;
    pond.x+=3;
    pond2.x+=3;
    mountain.x+=1;
    mountain2.x+=1;
    if (pond.x>1279) pond.x = -1279;
    if (pond2.x>1279) pond2.x = -1279;
    if (mountain.x>1279) mountain.x = -1279;
    if (mountain2.x>1279) mountain2.x = -1279;
}
void destroyInterface(){
    if (interfaceRenderer) {
        SDL_DestroyRenderer(interfaceRenderer);
        interfaceRenderer = nullptr;
    }
    if (interfaceWindow) {
        SDL_DestroyWindow(interfaceWindow);
        interfaceWindow = nullptr;
    }
    if (pondTexture) {
        SDL_DestroyTexture(pondTexture);
        pondTexture = nullptr;
    }
    if (pond2Texture) {
        SDL_DestroyTexture(pond2Texture);
        pond2Texture = nullptr;
    }
    if (mountainTexture) {
        SDL_DestroyTexture(mountainTexture);
        mountainTexture = nullptr;
    }
    if (mountain2Texture) {
        SDL_DestroyTexture(mountain2Texture);
        mountain2Texture = nullptr;
    }
}
bool isInterfaceOpen(){
    return interfaceWindow != nullptr;
}