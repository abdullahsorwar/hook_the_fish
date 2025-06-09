#include "Common.h"
#include "HardInterface.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iostream>
#include <cmath>
#include <cstdlib>

#define MAX_FISH 10
#define PI acos(-1)

static SDL_Rect pond = {0, 250, 1280, 470};
static SDL_Rect pond2 = {-1279, 250, 1280, 470};
static SDL_Rect mountain = {0, 0, 1280, 250};

Mix_Music* game_music = nullptr;

SDL_Window* interfaceWindow = nullptr;
static SDL_Renderer* interfaceRenderer = nullptr;
static SDL_Renderer* objectiveRenderer = nullptr;

static SDL_Texture* pondTexture = nullptr;
static SDL_Texture* pond2Texture = nullptr;
static SDL_Texture* mountainTexture = nullptr;
static SDL_Texture* scoreTexture = nullptr;
static SDL_Texture* heartTexture = nullptr;
static SDL_Texture* fishTextures[12] = {nullptr};
static SDL_Texture* objectiveTextures[6] = {nullptr};
static SDL_Texture* rippleTextures[4] = {nullptr};

static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static TTF_Font* textFont = nullptr;

static int fishScore = 0;
static int targetScore = 0;
static int lives = 3;
static Uint32 timerStartTime = 0;
static bool timerRunning = false;
static const Uint32 TIMER_DURATION = 120000;

bool hardinterfaceOpen = false;

struct PondFish {
    SDL_Rect rect;
    float arcHeight;
    float t;
    bool goingUp;
    bool active;
    bool rippleActive;
    int rippleFrame;
    int direction; 
    int type;
    int baseX, baseY;
    bool clicked;
};

struct ObjectiveFish {
    int type;
    int count;
};

static ObjectiveFish objectiveFishes[6];
static bool objectivesInitialized = false;
static PondFish fishes[MAX_FISH];
static std::vector<int> availableTypes;

std::string getFormattedTime() {
    if (!timerRunning) return "02:00";
    
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsed = currentTime - timerStartTime;
    Uint32 remaining = (TIMER_DURATION > elapsed) ? (TIMER_DURATION - elapsed) : 0;
    
    if (remaining == 0) {
        timerRunning = false;
        // Add game over logic here if needed
    }
    
    int minutes = remaining / 60000;
    int seconds = (remaining % 60000) / 1000;
    
    char timeStr[6];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", minutes, seconds);
    
    return std::string(timeStr);
}

void loadHardFishAssets() {
    SDL_Surface* surf;
    
    surf = IMG_Load("png/piranha.png");
    fishTextures[0] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/golden.png");
    fishTextures[1] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/brown.png");
    fishTextures[2] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/emerald.png");
    fishTextures[3] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/green.png");
    fishTextures[4] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/lavender.png");
    fishTextures[5] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/olive.png");
    fishTextures[6] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/orange.png");
    fishTextures[7] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/purple.png");
    fishTextures[8] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/red.png");
    fishTextures[9] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/silver.png");
    fishTextures[10] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/teal.png");
    fishTextures[11] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/heart.png");
    heartTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    for (int i = 0; i < 4; ++i) {
        std::string filename = "bmp/ripple" + std::to_string(i) + ".bmp";
        SDL_Surface* rippleSurf = SDL_LoadBMP(filename.c_str());
        rippleTextures[i] = SDL_CreateTextureFromSurface(interfaceRenderer, rippleSurf);
        SDL_FreeSurface(rippleSurf);
    }
}

void loadObjectiveAssets(int type, int index) {
    SDL_Surface* surf = nullptr;

    if (type == 2) {
        surf = IMG_Load("png/brown.png");
    }
    else if (type == 3) {
        surf = IMG_Load("png/emerald.png");
    }
    else if (type == 4) {
        surf = IMG_Load("png/green.png");
    }
    else if (type == 5) {
        surf = IMG_Load("png/lavender.png");
    }
    else if (type == 6) {
        surf = IMG_Load("png/olive.png");
    }
    else if (type == 7) {
        surf = IMG_Load("png/orange.png");
    }
    else if (type == 8) {
        surf = IMG_Load("png/purple.png");
    }
    else if (type == 9) {
        surf = IMG_Load("png/red.png");
    }
    else if (type == 10) {
        surf = IMG_Load("png/silver.png");
    }
    else if (type == 11) {
        surf = IMG_Load("png/teal.png");
    }

    if (surf != nullptr) {
        objectiveTextures[index] = SDL_CreateTextureFromSurface(objectiveRenderer, surf);
        SDL_FreeSurface(surf);
    }
}

void spawnHardFish() {
    for (int i = 0; i < MAX_FISH - 6; ++i) {
        if (!fishes[i].active && rand() % 200 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = availableTypes[i];
            
            fishes[i].arcHeight = rand() % 60 + 70;
            fishes[i].rect.x = fishes[i].baseX;
            fishes[i].rect.y = fishes[i].baseY;
            fishes[i].t = 0;
            fishes[i].goingUp = true;
            fishes[i].active = true;
            fishes[i].rippleActive = false;
            fishes[i].rippleFrame = 0;
            fishes[i].direction = direction;
            fishes[i].type = type;
            fishes[i].clicked = false;
        }
    }
    for (int i = 6; i < 8; ++i) {
        if (!fishes[i].active && rand() % 200 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = i-6;
            
            fishes[i].arcHeight = rand() % 60 + 70;
            fishes[i].rect.x = fishes[i].baseX;
            fishes[i].rect.y = fishes[i].baseY;
            fishes[i].t = 0;
            fishes[i].goingUp = true;
            fishes[i].active = true;
            fishes[i].rippleActive = false;
            fishes[i].rippleFrame = 0;
            fishes[i].direction = direction;
            fishes[i].type = type;
            fishes[i].clicked = false;
        }
    }
    for (int i = 8; i < MAX_FISH; ++i) {
        if (!fishes[i].active && rand() % 200 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = rand() % 12;
            
            fishes[i].arcHeight = rand() % 60 + 70;
            fishes[i].rect.x = fishes[i].baseX;
            fishes[i].rect.y = fishes[i].baseY;
            fishes[i].t = 0;
            fishes[i].goingUp = true;
            fishes[i].active = true;
            fishes[i].rippleActive = false;
            fishes[i].rippleFrame = 0;
            fishes[i].direction = direction;
            fishes[i].type = type;
            fishes[i].clicked = false;
        }
    }
}

void updateHardFishMotion() {
    for (int i = 0; i < MAX_FISH; ++i) {
        if (!fishes[i].active) continue;

        float angle = fishes[i].t;
        float radius = fishes[i].arcHeight;

        float x = fishes[i].baseX + fishes[i].direction * radius * cos(angle);
        float y = fishes[i].baseY - radius * sin(angle);

        fishes[i].rect = {static_cast<int>(x), static_cast<int>(y), 80, 80};
        fishes[i].t += 0.085f;

        if (fishes[i].t >= PI) {
            fishes[i].active = false;
            fishes[i].rippleActive = true;
            fishes[i].rippleFrame = 0;
        }

        if (fishes[i].rippleActive) {
            fishes[i].rippleFrame++;
            if (fishes[i].rippleFrame > 10) {
                fishes[i].rippleActive = false;
            }
        }
    }
}

void renderHardFishAndRipples() {
    for (int i = 0; i < MAX_FISH; ++i) {
        if (fishes[i].active) {
            SDL_RendererFlip flip = (fishes[i].direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(interfaceRenderer, fishTextures[fishes[i].type], NULL, &fishes[i].rect, 0, NULL, flip);
        }

        if (fishes[i].rippleActive) {
            int frame = fishes[i].rippleFrame / 3;
            if (frame < 4) {
                SDL_Rect rippleRect = { fishes[i].baseX + 20, fishes[i].baseY + 10, 80, 80 };
                SDL_RenderCopy(interfaceRenderer, rippleTextures[frame], NULL, &rippleRect);
            }
        }
    }
}

void handleHardFishClick(int x, int y)
{
    for (int i = 0; i < MAX_FISH; i++)
    {
        if (fishes[i].active && !fishes[i].clicked &&
            x >= fishes[i].rect.x && x <= fishes[i].rect.x + fishes[i].rect.w &&
            y >= fishes[i].rect.y && y <= fishes[i].rect.y + fishes[i].rect.h)
            {
                if (fishes[i].type == 0)
                {
                    lives--;
                    fishes[i].clicked = true;
                    break;
                }
                else if (fishes[i].type == 1)
                {
                    fishScore+=10;
                    fishes[i].clicked = true;
                    break;
                }
                else
                {
                    for (int j=0; j < 6 && !fishes[i].clicked; j++)
                    {
                        if (fishes[i].type == objectiveFishes[j].type)
                        {
                            fishScore++;
                            targetScore--;
                            objectiveFishes[i].count--;
                            fishes[i].clicked = true;
                            break;
                        }
                    }
                    if (fishes[i].clicked) break;
                }
            }
    }
}

void initHardInterface() {
    if (interfaceWindow != nullptr) return;

    game_music = Mix_LoadMUS("music/game_music.mp3");

    interfaceWindow = SDL_CreateWindow("Hard Mode", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        1280, 720, SDL_WINDOW_BORDERLESS);
    interfaceRenderer = SDL_CreateRenderer(interfaceWindow, -1, SDL_RENDERER_ACCELERATED);
    
    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 32);
        
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

    loadHardFishAssets();
    initHardObjective();
}

void initHardObjective() {
    if (objectiveWindow != nullptr) return;

    objectiveWindow = SDL_CreateWindow("Objective", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    objectiveRenderer = SDL_CreateRenderer(objectiveWindow, -1, SDL_RENDERER_ACCELERATED);

    if (!objectivesInitialized) {
        for (int i = 2; i <= 11; ++i) {
            availableTypes.push_back(i);
        }
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(availableTypes.begin(), availableTypes.end(), std::default_random_engine(seed));
        for (int i = 0; i < 6; ++i) {
            objectiveFishes[i].type = availableTypes[i];
            objectiveFishes[i].count = rand() % 6 + 4;
            targetScore += objectiveFishes[i].count;
            loadObjectiveAssets(objectiveFishes[i].type, i);
        }

        objectivesInitialized = true;
    }
}

void renderHardInterface() {
    if (!interfaceRenderer) return;
    
    SDL_RenderCopy(interfaceRenderer, pondTexture, NULL, &pond);
    SDL_RenderCopy(interfaceRenderer, pond2Texture, NULL, &pond2);
    SDL_RenderCopy(interfaceRenderer, mountainTexture, NULL, &mountain);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor) {
        int radius = 5;
        roundedBoxRGBA(interfaceRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 100);
        renderText(interfaceRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };

    SDL_Rect infoBox = {10, 10, 400, 200};
    drawRoundedButton(infoBox, "", white);

    // Timer
    std::string timerText = "Time: " + getFormattedTime();
    SDL_Surface* textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    SDL_Rect timerRect = {45, 15, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);

    timerText = "Lives: ";
    textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    timerRect = {45, 45, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);

    for (int i=0; i<lives; ++i)
    {
        int x = 130 + i*30;
        SDL_Rect liveRect = {x, 45, 30, 30};
        SDL_RenderCopy (interfaceRenderer, heartTexture, NULL, &liveRect);
    }

    timerText = "Score: " + std::to_string(fishScore) + ", Rem: " + std::to_string(targetScore);
    textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    timerRect = {45, 75, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);


    for (int i = 0; i < 6; ++i) {
        int col = (i < 3) ? 0 : 1;
        int row = i % 3;

        int centerX = (col == 0) ? 45 : 215;
        int centerY = 95 + row * 33;

        SDL_Rect fishRect = {centerX, centerY, 60, 60};
        SDL_RenderCopy(interfaceRenderer, fishTextures[objectiveFishes[i].type], NULL, &fishRect);

        std::string countText = "x " + std::to_string(objectiveFishes[i].count);
        renderText(interfaceRenderer, buttonFont, countText, black, centerX + 90, centerY + 25);
    }

    renderHardFishAndRipples();

    SDL_RenderPresent(interfaceRenderer);
    if (!objectiveClose) renderHardObjective();
}

void renderHardObjective() {
    if (!objectiveRenderer) return;

    SDL_SetRenderDrawColor(objectiveRenderer, 255, 255, 255, 255);
    SDL_RenderClear(objectiveRenderer);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    renderText(objectiveRenderer, titleFont, "Objectives", black, 400, 80);

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    SDL_Rect backBtnRect = {300, 380, 200, 60};
    Button backBtn = {backBtnRect, "Confirm", false};
    backBtn.hovered = SDL_PointInRect(&mousePoint, &backBtn.rect);

    drawParallelogram(objectiveRenderer, backBtn, backBtn.hovered);
    renderText(objectiveRenderer, buttonFont, backBtn.text, white, backBtn.rect.x + backBtn.rect.w / 2, backBtn.rect.y + backBtn.rect.h / 2);
    
    for (int i = 0; i < 6; ++i) {
        int col = (i < 3) ? 0 : 1;
        int row = i % 3;

        int centerX = (col == 0) ? 200 : 600;
        int centerY = 160 + row * 90;

        SDL_Rect fishRect = {centerX - 60, centerY - 30, 60, 60};
        SDL_RenderCopy(objectiveRenderer, objectiveTextures[i], NULL, &fishRect);

        std::string countText = "x " + std::to_string(objectiveFishes[i].count);
        renderText(objectiveRenderer, buttonFont, countText, black, centerX + 30, centerY-5);
    }
    SDL_RenderPresent(objectiveRenderer);
}

void handleHardInterfaceEvents(SDL_Event& e, bool& interfaceOpen) {
    if (!interfaceWindow) return;
    
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        handleHardFishClick(mouseX, mouseY);
        
        SDL_Rect backBtnRect = {300, 380, 200, 60};
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePoint = {mx, my};
        
        if (SDL_PointInRect(&mousePoint, &backBtnRect)) {
            SDL_DestroyRenderer(objectiveRenderer);
            SDL_DestroyWindow(objectiveWindow);
            objectiveClose = true;
            
            // Start the timer when confirm is clicked
            timerStartTime = SDL_GetTicks();
            timerRunning = true;
        }
    }
}

void handleHardInterfaceLogics(SDL_Event& e, bool& interfaceOpen) {
    if (!interfaceWindow) return;
    
    spawnHardFish();
    updateHardFishMotion();
    
    pond.x += 1;
    pond2.x += 1;

    if (pond.x > 1279) pond.x = -1279;
    if (pond2.x > 1279) pond2.x = -1279;
}

void destroyHardInterface() {
    if (interfaceRenderer) SDL_DestroyRenderer(interfaceRenderer);
    if (interfaceWindow) SDL_DestroyWindow(interfaceWindow);
    interfaceRenderer = nullptr;
    interfaceWindow = nullptr;

    if (pondTexture) SDL_DestroyTexture(pondTexture);
    if (pond2Texture) SDL_DestroyTexture(pond2Texture);
    if (mountainTexture) SDL_DestroyTexture(mountainTexture);

    pondTexture = nullptr;
    pond2Texture = nullptr;
    mountainTexture = nullptr;

    for (int i = 0; i < 12; ++i) {
        if (fishTextures[i]) SDL_DestroyTexture(fishTextures[i]);
    }
    for (int i = 0; i < 4; ++i) {
        if (rippleTextures[i]) SDL_DestroyTexture(rippleTextures[i]);
    }

    timerRunning = false;
    timerStartTime = 0;
}

bool isHardInterfaceOpen() {
    return interfaceWindow != nullptr;
}