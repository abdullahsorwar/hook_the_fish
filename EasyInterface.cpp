#include "Common.h"
#include "EasyInterface.h"  // use Easy version
#include "HighScores.h"
#include "Pause.h"
#include "Rain.h"
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

#define MAX_FISH 10//11
#define PI acos(-1)

// Scene layout
static SDL_Rect pond = {0, 250, 1280, 470};
static SDL_Rect pond2 = {-1279, 250, 1280, 470};
static SDL_Rect mountain = {0, 0, 1280, 250};
static SDL_Rect inputBox = {250, 200, 300, 50};
static SDL_Rect confirmButton = {300, 380, 200, 60};
static SDL_Rect pauseBtn = {1205, 15, 60, 60};

// Windows and renderers
//SDL_Window* interfaceWindow = nullptr;
SDL_Window* EasyobjectiveWindow = nullptr;
SDL_Window* EasygamewinWindow = nullptr;

//SDL_Renderer* interfaceRenderer = nullptr;
static SDL_Renderer* EasyobjectiveRenderer = nullptr;
static SDL_Renderer* gamewinRenderer = nullptr;

// Textures (use different fish/backgrounds in Easy mode)
static SDL_Texture* pondTexture = nullptr;
static SDL_Texture* pond2Texture = nullptr;
static SDL_Texture* mountainTexture = nullptr;
static SDL_Texture* heartTexture = nullptr;
static SDL_Texture* fishTextures[12] = {nullptr};          // easy-specific fish
static SDL_Texture* objectiveTextures[4] = {nullptr};      // for displaying targets
static SDL_Texture* rippleTextures[4] = {nullptr};         // same ripple animation

// Fonts
static TTF_Font* titleFont = nullptr;
static TTF_Font* smalltitleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static TTF_Font* textFont = nullptr;
static TTF_Font* typeFont = nullptr;
static TTF_Font* messageFont = nullptr;

//static int lives = 3;
static Uint32 timerStartTime = 0;
static Uint32 congratsStartTime = 0;
static bool timerRunning = false;
static bool congratulationsFlag = false;
static const Uint32 TIMER_DURATION = 120000; // 2 minutes

bool EasyinterfaceOpen = false;  // renamed for Easy

// Game Win Input State
std::string EasyuserInput = "";
std::string EasyfinalText = "";
std::string Easyconf = "";

static bool inputActive = false;
static bool showCursor = true;
static bool gamewinOpen = false;

static Uint32 lastCursorToggle = 0;

// Fish structure
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

// Objectives structure
struct ObjectiveFish {
    int type;
    int count;
};

static ObjectiveFish objectiveFishes[4]; // can reduce number of objectives for easy
static bool objectivesInitialized = false;
static PondFish fishes[MAX_FISH];
//static Uint32 remaining = TIMER_DURATION;
static std::vector<int> availableTypes(10); // could be adjusted (fewer types for easier mode)

std::string EasygetFormattedTime() {
    if (!timerRunning) return "02:00";
    
    Uint32 currentTime = SDL_GetTicks();

    Uint32 effectiveTime = isPaused? pauseStartTime : currentTime;
    Uint32 elapsed = currentTime - (timerStartTime + totalPaused);
    remaining = (TIMER_DURATION > elapsed) ? (TIMER_DURATION - elapsed) : 0;
    
    int minutes = remaining / 60000;
    int seconds = (remaining % 60000) / 1000;
    
    char timeStr[9];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", minutes, seconds);
    
    return std::string(timeStr);
}

void loadEasyFishAssets() {
    SDL_Surface* surf;

    surf = IMG_Load("png/golden.png");
    fishTextures[0] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/brown.png");
    fishTextures[1] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/emerald.png");
    fishTextures[2] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/green.png");
    fishTextures[3] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/lavender.png");
    fishTextures[4] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/olive.png");
    fishTextures[5] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/orange.png");
    fishTextures[6] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/purple.png");
    fishTextures[7] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/red.png");
    fishTextures[8] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/silver.png");
    fishTextures[9] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/teal.png");
    fishTextures[10] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    // Load ripple animations
    for (int i = 0; i < 4; ++i) { //why 4?
        std::string filename = "bmp/ripple" + std::to_string(i) + ".bmp";
        SDL_Surface* rippleSurf = SDL_LoadBMP(filename.c_str());
        rippleTextures[i] = SDL_CreateTextureFromSurface(interfaceRenderer, rippleSurf);
        SDL_FreeSurface(rippleSurf);
    }
}

void loadEasyObjectiveAssets(int type, int index) {
    SDL_Surface* surf = nullptr;

    if (type == 1) {
        surf = IMG_Load("png/brown.png");
    }
    else if (type == 2) {
        surf = IMG_Load("png/emerald.png");
    }
    else if (type == 3) {
        surf = IMG_Load("png/green.png");
    }
    else if (type == 4) {
        surf = IMG_Load("png/lavender.png");
    }
    else if (type == 5) {
        surf = IMG_Load("png/olive.png");
    }
    else if (type == 6) {
        surf = IMG_Load("png/orange.png");
    }
    else if (type == 7) {
        surf = IMG_Load("png/purple.png");
    }
    else if (type == 8) {
        surf = IMG_Load("png/red.png");
    }
    else if (type == 9) {
        surf = IMG_Load("png/silver.png");
    }
    else if (type == 10) {
        surf = IMG_Load("png/teal.png");
    }

    if (surf != nullptr) {
        objectiveTextures[index] = SDL_CreateTextureFromSurface(EasyobjectiveRenderer, surf);
        SDL_FreeSurface(surf);
    }
}

void spawnEasyFish() {
    for (int i = 0; i < 4; ++i) { //4
        if (!fishes[i].active && rand() % 20 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % 200 + 400;
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
    for (int i = 4; i < 5; ++i) { //4, i<5
        if (!fishes[i].active && rand() % 50 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = i-4; //i-4
            
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
    for (int i = 5; i < MAX_FISH; ++i) { //5 - MAX_FISH
        if (!fishes[i].active && rand() % 200 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = rand() % 11; //11
            
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

void updateEasyFishMotion() {
    for (int i = 0; i < MAX_FISH; ++i) {
        if (!fishes[i].active) continue;

        float angle = fishes[i].t;
        float radius = fishes[i].arcHeight;

        float x = fishes[i].baseX + fishes[i].direction * radius * cos(angle);
        float y = fishes[i].baseY - radius * sin(angle);

        fishes[i].rect = {static_cast<int>(x), static_cast<int>(y), 80, 80};
        fishes[i].t += 0.060f;

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

void renderEasyFishAndRipples() {
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

void handleEasyFishClick(int x, int y)
{
    for (int i = 0; i < MAX_FISH; i++)
    {
        if (fishes[i].active && !fishes[i].clicked &&
            x >= fishes[i].rect.x && x <= fishes[i].rect.x + fishes[i].rect.w &&
            y >= fishes[i].rect.y && y <= fishes[i].rect.y + fishes[i].rect.h)
            {
                if (fishes[i].type == 0) //type 0 from 1
                {
                    fishScore+=10;
                    fishes[i].clicked = true;
                    if (soundOn) Mix_PlayChannel(-1, bonuscatch, 0);
                    break;
                }
                else if (targetScore > 0)
                {
                    for (int j=0; j < 4 && !fishes[i].clicked; j++) //chnaged to 4 frm 6
                    {
                        if (fishes[i].type == objectiveFishes[j].type && objectiveFishes[j].count > 0)
                        {
                            fishScore++;
                            targetScore--;
                            objectiveFishes[j].count--;
                            fishes[i].clicked = true;
                            if (soundOn) Mix_PlayChannel(-1, rightfish, 0);
                            if (targetScore == 0) congratsStartTime = SDL_GetTicks();
                            break;
                        }
                        else if (fishes[i].type == objectiveFishes[j].type && objectiveFishes[j].count == 0)
                        {
                            //if (fishScore > 0) fishScore--;
                            fishes[i].clicked = true;
                            if (soundOn) Mix_PlayChannel(-1, wrongfish, 0);
                            break;
                        }
                    }
                    if (fishes[i].clicked) break;
                }
                else if (targetScore == 0)
                {
                    fishScore++;
                    fishes[i].clicked = true;
                    if (soundOn) Mix_PlayChannel(-1, rightfish, 0);
                    break;
                }
                else
                {
                    //if (fishScore > 0) fishScore--;
                    fishes[i].clicked = true;
                    if (soundOn) Mix_PlayChannel(-1, wrongfish, 0); // should remove it??
                    break;
                }
            }
    }
}

void initEasyInterface() {
    

    if (interfaceWindow != nullptr) return;

    game_music = Mix_LoadMUS("music/game_music.mp3");
    bonuscatch = Mix_LoadWAV("music/bonuscatch.wav");
    rightfish = Mix_LoadWAV("music/rightfish.wav");
    wrongfish = Mix_LoadWAV("music/wrongfish.wav");



    interfaceWindow = SDL_CreateWindow("Easy Mode", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        1280, 720, SDL_WINDOW_BORDERLESS);
    interfaceRenderer = SDL_CreateRenderer(interfaceWindow, -1, SDL_RENDERER_ACCELERATED);
    
    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    smalltitleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 64);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 32);
    messageFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 32);
        
    SDL_Surface* surf;
    
    if(sunnyOn)
    {surf = SDL_LoadBMP("bmp/pond.bmp");
    pondTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/pond2.bmp");
    pond2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/mountain.bmp");
    mountainTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);
    }else
    {surf = SDL_LoadBMP("bmp/rainypond.bmp");
    pondTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/rainypond2.bmp");
    pond2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/rainymountain.bmp");
    mountainTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    initRain(500);
    }
    loadEasyFishAssets();
    initEasyObjective();
}

void initEasyObjective() {
    if (EasyobjectiveWindow != nullptr) return;

    EasyobjectiveWindow = SDL_CreateWindow("Objective", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    EasyobjectiveRenderer = SDL_CreateRenderer(EasyobjectiveWindow, -1, SDL_RENDERER_ACCELERATED);

    if (!objectivesInitialized) {
        for (int i = 1; i <= 10; ++i) { //chnged frm i=2 to i=1, i<=11 to i<=10  
            availableTypes[i-1] = i; //i-1 frm i-2
        }
        std::mt19937 seed(std::chrono::system_clock::now().time_since_epoch().count());
        std::shuffle(availableTypes.begin(), availableTypes.end(), seed);
        for (int i = 0; i < 4; ++i) { //4
            objectiveFishes[i].type = availableTypes[i];
            objectiveFishes[i].count = rand() % 3 + 8;
            targetScore += objectiveFishes[i].count;
            loadEasyObjectiveAssets(objectiveFishes[i].type, i);
        }

        objectivesInitialized = true;
    }
}

void renderEasyInterface() {
    if (!interfaceRenderer || isPaused) return;
    
    SDL_RenderCopy(interfaceRenderer, pondTexture, NULL, &pond);
    SDL_RenderCopy(interfaceRenderer, pond2Texture, NULL, &pond2);
    SDL_RenderCopy(interfaceRenderer, mountainTexture, NULL, &mountain);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    if (!sunnyOn){
        renderRain();
    }

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor, int radius, int alpha) {
        roundedBoxRGBA(interfaceRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, alpha);
        renderText(interfaceRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };

    SDL_Rect infoBox = {10, 10, 400, 200};
    drawRoundedButton(infoBox, "", white, 5, 100);

    std::string timerText = "Time: " + EasygetFormattedTime();
    SDL_Surface* textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    SDL_Rect timerRect = {45, 15, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);

    timerText = "Score: " + std::to_string(fishScore) + ", Rem: " + std::to_string(targetScore);
    textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    timerRect = {45, 55, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);

    // 2x2 Grid for 4 objective fishes
    for (int i = 0; i < 4; ++i) {
        int row = i / 2;
        int col = i % 2;

        int centerX = 45 + col * 170;  // horizontal spacing
        int centerY = 95 + row * 80;   // vertical spacing

        SDL_Rect fishRect = {centerX, centerY, 60, 60};
        SDL_RenderCopy(interfaceRenderer, fishTextures[objectiveFishes[i].type], NULL, &fishRect);

        std::string countText = "x " + std::to_string(objectiveFishes[i].count);
        renderText(interfaceRenderer, buttonFont, countText, black, centerX + 90, centerY + 25);
    }

    renderEasyFishAndRipples();

    if (targetScore == 0 && !congratulationsFlag) {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - congratsStartTime;
        float progress = elapsed / 3000.0f;

        if (progress >= 1.0f) {
            congratulationsFlag = true;
        } else {
            SDL_Color color = {0, 0, 0, 255};
            SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(
                textFont,
                "      OBJECTIVES COMPLETED\nCatch as much as you can!!",
                color,
                0
            );
            SDL_Texture* texture = SDL_CreateTextureFromSurface(interfaceRenderer, surface);
            int textW = surface->w;
            int textH = surface->h;
            SDL_FreeSurface(surface);

            int windowW, windowH;
            SDL_GetRendererOutputSize(interfaceRenderer, &windowW, &windowH);
            float startY = windowH / 2.0f;
            float endY = windowH / 2.0f - 100;
            float yPos = startY + (endY - startY) * progress;

            Uint8 alpha = 255;
            if (elapsed < 500) {
                alpha = (Uint8)(255.0f * (elapsed / 500.0f));
            } else if (elapsed > 2500) {
                alpha = (Uint8)(255.0f * (1.0f - ((elapsed - 2500.0f) / 500.0f)));
            }
            SDL_SetTextureAlphaMod(texture, alpha);

            SDL_Rect dstRect = {
                (windowW - textW) / 2,
                (int)yPos,
                textW,
                textH
            };

            SDL_RenderCopy(interfaceRenderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
        }
    }

    if (gamewinOpen && targetScore == 0) {
        EasyrenderGameWin();
    }

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mp = {mx, my};
    SDL_Color color1 = {255, 180, 100, 255};
    SDL_Color color2 = {255, 150, 0, 255};
    drawRoundedButton(pauseBtn, "", SDL_PointInRect(&mp, &pauseBtn) ? color1 : color2, 0, 255);
    renderText(interfaceRenderer, smalltitleFont, "| |", white, 1235, 52);

    SDL_RenderPresent(interfaceRenderer);

    if (!objectiveClose) renderEasyObjective();
}

void renderEasyObjective() {
    if (!EasyobjectiveRenderer) return;

    SDL_SetRenderDrawColor(EasyobjectiveRenderer, 255, 255, 255, 255);
    SDL_RenderClear(EasyobjectiveRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    renderText(EasyobjectiveRenderer, titleFont, "Objectives", black, 400, 80);

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    SDL_Rect backBtnRect = {300, 380, 200, 60};
    Button backBtn = {backBtnRect, "Confirm", false};
    backBtn.hovered = SDL_PointInRect(&mousePoint, &backBtn.rect);

    drawParallelogram(EasyobjectiveRenderer, backBtn, backBtn.hovered);
    renderText(EasyobjectiveRenderer, buttonFont, backBtn.text, white, backBtn.rect.x + backBtn.rect.w / 2, backBtn.rect.y + backBtn.rect.h / 2);

    // 2x2 Grid layout for 4 fishes
    for (int i = 0; i < 4; ++i) {
        int col = i % 2;
        int row = i / 2;

        int centerX = 250 + col * 300;  // 250, 550
        int centerY = 180 + row * 100;  // 180, 280

        SDL_Rect fishRect = {centerX - 60, centerY - 30, 60, 60};
        SDL_RenderCopy(EasyobjectiveRenderer, objectiveTextures[i], NULL, &fishRect);

        std::string countText = " x " + std::to_string(objectiveFishes[i].count);
        renderText(EasyobjectiveRenderer, buttonFont, countText, black, centerX + 30, centerY - 5);
    }

    SDL_RenderPresent(EasyobjectiveRenderer);
}

void EasyinitgameWin()
{
    if (EasygamewinWindow != nullptr) return;

    EasygamewinWindow = SDL_CreateWindow("Winner!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    gamewinRenderer = SDL_CreateRenderer(EasygamewinWindow, -1, SDL_RENDERER_ACCELERATED);

    typeFont = TTF_OpenFont ("fonts/Arial.ttf", 24);
    SDL_StartTextInput();
}

void EasyrenderGameWin() {
    if (!gamewinRenderer) return;

    SDL_SetRenderDrawColor(gamewinRenderer, 20, 20, 40, 255);
    SDL_RenderClear(gamewinRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    renderText(gamewinRenderer, smalltitleFont, "Congratulations!", white, 400, 80);

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    Easyconf = (EasyfinalText == "0") ? "Exit" : "Confirm";
    Button confirmBtn = {confirmButton, Easyconf, false};
    confirmBtn.hovered = SDL_PointInRect(&mousePoint, &confirmBtn.rect);

    drawParallelogram(gamewinRenderer, confirmBtn, confirmBtn.hovered);
    renderText(gamewinRenderer, buttonFont, confirmBtn.text, white, confirmBtn.rect.x + confirmBtn.rect.w / 2, confirmBtn.rect.y + confirmBtn.rect.h / 2);

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor) {
        int radius = 5;
        roundedBoxRGBA(gamewinRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 100);
        renderText(gamewinRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    SDL_Color faded = {255, 255, 255, 255};
    drawRoundedButton(inputBox, "", faded);

    if (SDL_GetTicks() - lastCursorToggle > 500) {
        showCursor = !showCursor;
        lastCursorToggle = SDL_GetTicks();
    }

    renderText(gamewinRenderer, textFont, "Enter your name: ", white, 400, 150);

    // --- Centered and Scrolling Text ---
    std::string displayText = EasyuserInput;
    if (inputActive && showCursor) {
        displayText += "|";
    }

    // Measure full text width
    int textWidth = 0, textHeight = 0;
    TTF_SizeText(typeFont, displayText.c_str(), &textWidth, &textHeight);

    // Scroll if text is wider than box
    int maxVisibleWidth = inputBox.w - 20;
    std::string visibleText = displayText;
    while (!visibleText.empty()) {
        TTF_SizeText(typeFont, visibleText.c_str(), &textWidth, nullptr);
        if (textWidth <= maxVisibleWidth) break;
        visibleText.erase(0, 1);  // Scroll left
    }

    // Center visible text inside inputBox
    TTF_SizeText(typeFont, visibleText.c_str(), &textWidth, &textHeight);
    int textX = inputBox.x + inputBox.w / 2;
    int textY = inputBox.y + inputBox.h / 2;

    // Render the user input
    renderText(gamewinRenderer, typeFont, visibleText, white, textX, textY);

    // Success message
    if (EasyfinalText == "0") {
        renderText(gamewinRenderer, messageFont, "Entry Successful!", white, 400, 300);
    }
    else if (EasyfinalText == "18") {
        renderText(gamewinRenderer, messageFont, "Invalid name: must not exceed 18 characters.", white, 400, 300);
    }
    else if (EasyfinalText == "-1") {
        renderText(gamewinRenderer, messageFont, "Invalid name: only A-Z, a-z, 0-9, and", white, 400, 300);
        renderText(gamewinRenderer, messageFont, "underscores (_) allowed. No spaces!", white, 400, 350);
    }

    SDL_RenderPresent(gamewinRenderer);
}

void handleEasyInterfaceEvents(SDL_Event& e, bool& interfaceOpen) {
    if (!interfaceWindow) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(interfaceWindow)) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        handleEasyFishClick(mouseX, mouseY);
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mp = {mouseX, mouseY};
        if (SDL_PointInRect(&mp, &pauseBtn) && !isPaused && objectiveClose) {
            initPauseMenu();
            pauseStartTime = SDL_GetTicks();
            isPaused = true;
        }
        SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(EasyobjectiveWindow)) {
        SDL_Rect backBtnRect = {300, 380, 200, 60};
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePoint = {mx, my};
        if (SDL_PointInRect(&mousePoint, &backBtnRect)) {
            SDL_DestroyRenderer(EasyobjectiveRenderer);
            SDL_DestroyWindow(EasyobjectiveWindow);
            EasyobjectiveRenderer = nullptr;
            EasyobjectiveWindow = nullptr;
            objectiveClose = true;
            if (!timerRunning) {
                timerStartTime = SDL_GetTicks();
                timerRunning = true;
            }
            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(EasygamewinWindow)) {
        if (gamewinOpen) {
            int mx = e.button.x;
            int my = e.button.y;

            if (mx >= inputBox.x && mx <= inputBox.x + inputBox.w &&
                my >= inputBox.y && my <= inputBox.y + inputBox.h) {
                inputActive = true;
            } else {
                inputActive = false;
            }

            if (mx >= confirmButton.x && mx <= confirmButton.x + confirmButton.w &&
                my >= confirmButton.y && my <= confirmButton.y + confirmButton.h && Easyconf == "Confirm") {
                EasyfinalText = checkAndAddHighScore("files/easy.txt", EasyuserInput, fishScore);
            }

            SDL_Rect backBtnRect = {300, 380, 200, 60};
            SDL_GetMouseState(&mx, &my);
            SDL_Point mousePoint = {mx, my};

            if (SDL_PointInRect(&mousePoint, &backBtnRect) && Easyconf == "Exit") {
                SDL_DestroyRenderer(gamewinRenderer);
                SDL_DestroyWindow(EasygamewinWindow);
                gamewinOpen = false;
                gamewinRenderer = nullptr;
                EasygamewinWindow = nullptr;
                destroyEasyInterface();
            }
        }
    }

    if (gamewinOpen) {
        if (e.type == SDL_TEXTINPUT && inputActive) {
            EasyuserInput += e.text.text;
        }

        if (e.type == SDL_KEYDOWN && inputActive) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !EasyuserInput.empty()) {
                EasyuserInput.pop_back();
            }
        }
    }

    if (isPaused) {
        renderPauseMenu();
        handlePauseMenuEvents(e, isPaused);
    }
}


void handleEasyInterfaceLogics() {
    if (!interfaceWindow || isPaused) return;

    spawnEasyFish();
    updateEasyFishMotion();
    updateRain();

    pond.x += 1;
    pond2.x += 1;

    if (pond.x > 1279) pond.x = -1279;
    if (pond2.x > 1279) pond2.x = -1279;

    if (remaining == 0)
    {
        if (targetScore == 0 && !gamewinOpen) {
            EasyinitgameWin();
            gamewinOpen = true;
        }
        else if (targetScore != 0) {
            destroyEasyInterface();
            timerRunning = false;
        }
    }
}

void EasyendGame(int targetScore)
{
    if (targetScore == 0)
    {
        
    }
}

void destroyEasyInterface() {
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
    for (int i = 0; i < 12; ++i) {
        if (fishTextures[i]) {
            SDL_DestroyTexture(fishTextures[i]);
            fishTextures[i] = nullptr;
        }
    }
    for (int i = 0; i < 4; ++i) {
        if (rippleTextures[i]) {
            SDL_DestroyTexture(rippleTextures[i]);
            rippleTextures[i] = nullptr;
        }
    }

    if (soundOn) {
        Mix_PauseMusic();
        Mix_PlayMusic(intro, -1);
    } else {
        Mix_PauseMusic();
    }

    objectiveClose = false;
    objectivesInitialized = false;
    //heartTexture = nullptr;
    titleFont = nullptr;
    smalltitleFont = nullptr;
    buttonFont = nullptr;
    textFont = nullptr;
    typeFont = nullptr;
    fishScore = 0;
    targetScore = 0;
    //lives = 3;
    timerStartTime = 0;
    congratsStartTime = 0;
    pauseStartTime = 0;
    totalPaused = 0;
    isPaused = false;
    congratulationsFlag = false;

    for (int i = 0; i < MAX_FISH; ++i) {
        fishes[i] = PondFish();
    }

    EasyuserInput = "";
    EasyfinalText = "";
    Easyconf = "";
    inputActive = false;
    showCursor = true;
    gamewinOpen = false;
    lastCursorToggle = 0;
    remaining = 120000;

    Mix_FreeChunk(bonuscatch);
    //Mix_FreeChunk(crocodile);
    Mix_FreeChunk(rightfish);
    Mix_FreeChunk(wrongfish);

    if(!sunnyOn){
        destroyRain();
    }
    

    if (interfaceRenderer) {
        SDL_DestroyRenderer(interfaceRenderer);
        interfaceRenderer = nullptr;
    }
    if (interfaceWindow) {
        SDL_DestroyWindow(interfaceWindow);
        interfaceWindow = nullptr;
    }

    EasyinterfaceOpen = false;
}

bool isEasyInterfaceOpen() {
    return interfaceWindow != nullptr;
}















