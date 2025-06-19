#include "Common.h"
#include "HardInterface.h"
#include "HighScores.h"
#include "Pause.h"
#include "Rain.h"
#include "GameOver.h"
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
static SDL_Rect pauseBtn = {1205, 15, 60, 60};

//SDL_Window *interfaceWindow = nullptr;
SDL_Window *objectiveWindow = nullptr;
SDL_Window *gamewinWindow = nullptr;
//SDL_Renderer *interfaceRenderer = nullptr;
static SDL_Renderer *objectiveRenderer = nullptr;
static SDL_Renderer *gamewinRenderer = nullptr;

static SDL_Texture *pondTexture = nullptr;
static SDL_Texture *pond2Texture = nullptr;
static SDL_Texture *mountainTexture = nullptr;
static SDL_Texture *heartTexture = nullptr;
static SDL_Texture *fishTextures[12] = {nullptr};
static SDL_Texture *objectiveTextures[6] = {nullptr};
static SDL_Texture *rippleTextures[4] = {nullptr};

static TTF_Font *titleFont = nullptr;
static TTF_Font *smalltitleFont = nullptr;
static TTF_Font *buttonFont = nullptr;
static TTF_Font *textFont = nullptr;
static TTF_Font *typeFont = nullptr;
static TTF_Font *messageFont = nullptr;

static int lives = 3;
static Uint32 timerStartTime = 0;
static Uint32 congratsStartTime = 0;
static bool timerRunning = false;
static bool congratulationsFlag = false;
static const Uint32 TIMER_DURATION = 120000;
bool hardinterfaceOpen = false;

struct PondFish
{
    SDL_Rect rect;
    float arcHeight;
    float t;
    float rotation;
    bool goingUp;
    bool active;
    bool rippleActive;
    int rippleFrame;
    int direction;
    int type;
    int baseX, baseY;
    bool clicked;
};

struct FloatingText
{
    std::string text;
    SDL_Color color;
    SDL_Point position;
    Uint32 startTime;
    int duration = 1000;
};

static bool objectivesInitialized = false;
static PondFish fishes[MAX_FISH];
static Uint32 remaining = 120000;
static std::vector<int> availableTypes(10);
static std::vector<FloatingText> floatingTexts;

std::string getFormattedTime()
{
    if (!timerRunning)
        return "02:00";

    Uint32 currentTime = SDL_GetTicks();

    Uint32 effectiveTime = isPaused ? pauseStartTime : currentTime;
    Uint32 elapsed = effectiveTime - (timerStartTime + totalPaused);
    remaining = (TIMER_DURATION > elapsed) ? (TIMER_DURATION - elapsed) : 0;

    int minutes = remaining / 60000;
    int seconds = (remaining % 60000) / 1000;

    char timeStr[9];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", minutes, seconds);

    return std::string(timeStr);
}

void loadHardFishAssets()
{
    SDL_Surface *surf;

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

    for (int i = 0; i < 4; ++i)
    {
        std::string filename = "bmp/ripple" + std::to_string(i) + ".bmp";
        SDL_Surface *rippleSurf = SDL_LoadBMP(filename.c_str());
        rippleTextures[i] = SDL_CreateTextureFromSurface(interfaceRenderer, rippleSurf);
        SDL_FreeSurface(rippleSurf);
    }
}

void loadObjectiveAssets(int type, int index)
{
    SDL_Surface *surf = nullptr;

    if (type == 2)
    {
        surf = IMG_Load("png/brown.png");
    }
    else if (type == 3)
    {
        surf = IMG_Load("png/emerald.png");
    }
    else if (type == 4)
    {
        surf = IMG_Load("png/green.png");
    }
    else if (type == 5)
    {
        surf = IMG_Load("png/lavender.png");
    }
    else if (type == 6)
    {
        surf = IMG_Load("png/olive.png");
    }
    else if (type == 7)
    {
        surf = IMG_Load("png/orange.png");
    }
    else if (type == 8)
    {
        surf = IMG_Load("png/purple.png");
    }
    else if (type == 9)
    {
        surf = IMG_Load("png/red.png");
    }
    else if (type == 10)
    {
        surf = IMG_Load("png/silver.png");
    }
    else if (type == 11)
    {
        surf = IMG_Load("png/teal.png");
    }

    if (surf != nullptr)
    {
        objectiveTextures[index] = SDL_CreateTextureFromSurface(objectiveRenderer, surf);
        SDL_FreeSurface(surf);
    }
}

void spawnHardFish()
{
    for (int i = 0; i < 6; ++i)
    {
        if (!fishes[i].active && rand() % 20 == 0)
        {
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
    for (int i = 6; i < 8; ++i)
    {
        if (!fishes[i].active && rand() % 50 == 0)
        {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = i - 6;

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
    for (int i = 8; i < MAX_FISH; ++i)
    {
        if (!fishes[i].active && rand() % 200 == 0)
        {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = availableTypes[i-2];

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

void updateHardFishMotion()
{
    for (int i = 0; i < MAX_FISH; ++i)
    {
        if (!fishes[i].active)
            continue;

        float angle = fishes[i].t;
        float radius = fishes[i].arcHeight;

        float x = fishes[i].baseX + fishes[i].direction * radius * cos(angle);
        float y = fishes[i].baseY - radius * sin(angle);

        fishes[i].rect = {static_cast<int>(x), static_cast<int>(y), 80, 80};
        fishes[i].t += 0.075f;

        if (fishes[i].t >= PI)
        {
            fishes[i].active = false;
            fishes[i].rippleActive = true;
            fishes[i].rippleFrame = 0;
        }

        if (fishes[i].rippleActive)
        {
            fishes[i].rippleFrame++;
            if (fishes[i].rippleFrame > 10)
            {
                fishes[i].rippleActive = false;
            }
        }
    }
}

void renderHardFishAndRipples()
{
    for (int i = 0; i < MAX_FISH; ++i)
    {
        if (fishes[i].active)
        {
            SDL_RendererFlip flip = (fishes[i].direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(interfaceRenderer, fishTextures[fishes[i].type], NULL, &fishes[i].rect, 0, NULL, flip);
            SDL_RenderCopyEx(interfaceRenderer, fishTextures[fishes[i].type], NULL, &fishes[i].rect, fishes[i].rotation, NULL, flip);
        }

        if (fishes[i].rippleActive)
        {
            int frame = fishes[i].rippleFrame / 3;
            if (frame < 4)
            {
                SDL_Rect rippleRect = {fishes[i].baseX + 20, fishes[i].baseY + 10, 80, 80};
                SDL_RenderCopy(interfaceRenderer, rippleTextures[frame], NULL, &rippleRect);
            }
        }
    }
}

void renderFadedText(int type, Uint32 init_time, int obj_type, int obj_count)
{
    FloatingText text;

    if (type == 0)
    {
        text.text = "X";
        text.color = {255, 0, 0, 255};
    }
    else if (type == 1)
    {
        text.text = "+10";
        text.color = {0, 255, 0, 255};
    }
    else if (type == obj_type && obj_count > 0)
    {
        text.text = "+1";
        text.color = {0, 255, 0, 255};
    }
    else if (type == obj_type && obj_count == 0)
    {
        text.text = "-1";
        text.color = {255, 0, 0, 255};
    }
    else if (obj_type == -1 && obj_count == -1 && targetScore == 0)
    {
        text.text = "+1";
        text.color = {0, 255, 0, 255};
    }
    else
    {
        text.text = "-1";
        text.color = {255, 0, 0, 255};   
    }

    // Position will be set to fish position when called
    // So leave default for now
    text.startTime = init_time;
    floatingTexts.push_back(text);
}

void handleHardFishClick(int x, int y)
{
    for (int i = 0; i < MAX_FISH; i++)
    {
        if (!isPaused && fishes[i].active && !fishes[i].clicked &&
            x >= fishes[i].rect.x && x <= fishes[i].rect.x + fishes[i].rect.w &&
            y >= fishes[i].rect.y && y <= fishes[i].rect.y + fishes[i].rect.h)
        {
            if (fishes[i].type == 0)
            {
                lives--;
                fishes[i].clicked = true;
                renderFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                floatingTexts.back().position = {
                    fishes[i].rect.x + fishes[i].rect.w / 2,
                    fishes[i].rect.y - 20};
                if (lives == 0)
                    destroyHardInterface();
                break;
            }
            else if (fishes[i].type == 1)
            {
                fishScore += 10;
                fishes[i].clicked = true;
                renderFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                floatingTexts.back().position = {
                    fishes[i].rect.x + fishes[i].rect.w / 2,
                    fishes[i].rect.y - 20};
                if (soundOn)
                    Mix_PlayChannel(-1, bonuscatch, 0);
                break;
            }
            else if (targetScore > 0)
            {
                for (int j = 0; j < 6 && !fishes[i].clicked; j++)
                {
                    if (fishes[i].type == objectiveFishes[j].type && objectiveFishes[j].count > 0)
                    {
                        fishScore++;
                        targetScore--;
                        renderFadedText(fishes[i].type, SDL_GetTicks(), objectiveFishes[j].type, objectiveFishes[j].count);
                        floatingTexts.back().position = {
                            fishes[i].rect.x + fishes[i].rect.w / 2,
                            fishes[i].rect.y - 20};
                        objectiveFishes[j].count--;
                        fishes[i].clicked = true;
                        if (soundOn)
                            Mix_PlayChannel(-1, rightfish, 0);
                        if (targetScore == 0)
                            congratsStartTime = SDL_GetTicks();
                        break;
                    }
                    else if (fishes[i].type == objectiveFishes[j].type && objectiveFishes[j].count == 0)
                    {
                        if (fishScore > 0)
                        {
                            fishScore--;
                            fishes[i].clicked = true;
                            renderFadedText(fishes[i].type, SDL_GetTicks(), objectiveFishes[j].type, objectiveFishes[j].count);
                            floatingTexts.back().position = {
                                fishes[i].rect.x + fishes[i].rect.w / 2,
                                fishes[i].rect.y - 20};
                            if (soundOn)
                                Mix_PlayChannel(-1, wrongfish, 0);
                            break;
                        }
                    }
                }
                if (fishes[i].clicked)
                    break;
            }
            else if (targetScore == 0)
            {
                fishScore++;
                fishes[i].clicked = true;
                renderFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                floatingTexts.back().position = {
                    fishes[i].rect.x + fishes[i].rect.w / 2,
                    fishes[i].rect.y - 20};
                if (soundOn)
                    Mix_PlayChannel(-1, rightfish, 0);
                break;
            }
            else
            {
                if (fishScore > 0)
                    fishScore--;
                fishes[i].clicked = true;
                renderFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                floatingTexts.back().position = {
                    fishes[i].rect.x + fishes[i].rect.w / 2,
                    fishes[i].rect.y - 20};
                if (soundOn)
                    Mix_PlayChannel(-1, wrongfish, 0);
                break;
            }
        }
    }
}

void initHardInterface()
{
    if (interfaceWindow != nullptr)
        return;

    game_music = Mix_LoadMUS("music/game_music.mp3");
    bonuscatch = Mix_LoadWAV("music/bonuscatch.wav");
    crocodile = Mix_LoadWAV("music/crocodile.wav");
    rightfish = Mix_LoadWAV("music/rightfish.wav");
    wrongfish = Mix_LoadWAV("music/wrongfish.wav");

    interfaceWindow = SDL_CreateWindow("Hard Mode",
                                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                           1280, 720, SDL_WINDOW_BORDERLESS);
    interfaceRenderer = SDL_CreateRenderer(interfaceWindow, -1, SDL_RENDERER_ACCELERATED);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    smalltitleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 64);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 32);
    messageFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 32);

    SDL_Surface *surf;

    if (sunnyOn)
    {
        surf = SDL_LoadBMP("bmp/pond.bmp");
        pondTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        surf = SDL_LoadBMP("bmp/pond2.bmp");
        pond2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        surf = SDL_LoadBMP("bmp/mountain.bmp");
        mountainTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);
    }
    else if (!sunnyOn)
    {
        surf = SDL_LoadBMP("bmp/rainypond.bmp");
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

    loadHardFishAssets();
    initHardObjective();
}

void initHardObjective()
{
    if (objectiveWindow != nullptr)
        return;

    objectiveWindow = SDL_CreateWindow("Objective", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    objectiveRenderer = SDL_CreateRenderer(objectiveWindow, -1, SDL_RENDERER_ACCELERATED);

    if (!objectivesInitialized)
    {
        for (int i = 2; i <= 11; ++i)
        {
            availableTypes[i - 2] = i;
        }
        std::mt19937 seed(std::chrono::system_clock::now().time_since_epoch().count());
        std::shuffle(availableTypes.begin(), availableTypes.end(), seed);
        for (int i = 0; i < 6; ++i)
        {
            objectiveFishes[i].type = availableTypes[i];
            objectiveFishes[i].count = rand() % 11 + 5;
            targetScore += objectiveFishes[i].count;
            loadObjectiveAssets(objectiveFishes[i].type, i);
        }

        objectivesInitialized = true;
    }
}

void renderHardInterface()
{
    if (!interfaceRenderer || isPaused)
        return;

    SDL_RenderCopy(interfaceRenderer, pondTexture, NULL, &pond);
    SDL_RenderCopy(interfaceRenderer, pond2Texture, NULL, &pond2);
    SDL_RenderCopy(interfaceRenderer, mountainTexture, NULL, &mountain);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    if (!sunnyOn)
    {
        renderRain();
    }

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string &text, SDL_Color fillColor, int radius, int alpha)
    {
        roundedBoxRGBA(interfaceRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, alpha);
        renderText(interfaceRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };

    SDL_Rect infoBox = {10, 10, 400, 200};
    drawRoundedButton(infoBox, "", white, 5, 100);

    std::string timerText = "Time: " + getFormattedTime();
    SDL_Surface *textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    SDL_Texture *timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
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

    for (int i = 0; i < lives; ++i)
    {
        int x = 130 + i * 30;
        SDL_Rect liveRect = {x, 45, 30, 30};
        SDL_RenderCopy(interfaceRenderer, heartTexture, NULL, &liveRect);
    }

    timerText = "Score: " + std::to_string(fishScore) + ", Rem: " + std::to_string(targetScore);
    textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    timerRect = {45, 75, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);

    for (int i = 0; i < 6; ++i)
    {
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
    if (targetScore == 0 && !congratulationsFlag)
    {
        Uint32 now = SDL_GetTicks();
        Uint32 elapsed = now - congratsStartTime;
        float progress = elapsed / (float)3000;

        if (progress >= 1.0f)
        {
            congratulationsFlag = true;
        }
        else
        {
            SDL_Color color = {0, 0, 0, 255};
            SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(
                textFont,
                "      OBJECTIVES COMPLETED\nCatch as much as you can!!",
                color,
                0);
            SDL_Texture *texture = SDL_CreateTextureFromSurface(interfaceRenderer, surface);
            int textW = surface->w;
            int textH = surface->h;
            SDL_FreeSurface(surface);

            int windowW, windowH;
            SDL_GetRendererOutputSize(interfaceRenderer, &windowW, &windowH);
            float startY = windowH / 2.0f;
            float endY = windowH / 2.0f - 100;
            float yPos = startY + (endY - startY) * progress;

            Uint8 alpha = 255;
            if (elapsed < 500)
            {
                alpha = (Uint8)(255.0f * (elapsed / 500.0f));
            }
            else if (elapsed > 2500)
            {
                alpha = (Uint8)(255.0f * (1.0f - ((elapsed - 2500.0f) / 500.0f)));
            }
            SDL_SetTextureAlphaMod(texture, alpha);

            SDL_Rect dstRect = {
                (windowW - textW) / 2,
                (int)yPos,
                textW,
                textH};

            SDL_RenderCopy(interfaceRenderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
        }
    }
    
    Uint32 now = SDL_GetTicks();
    for (int i = 0; i < floatingTexts.size();)
    {
        FloatingText &text = floatingTexts[i];
        float progress = (now - text.startTime) / (float)text.duration;

        if (progress >= 1.0f)
        {
            // Remove expired text
            floatingTexts.erase(floatingTexts.begin() + i);
            continue;
        }
        Uint8 alpha = (Uint8)(255 * (1.0f - progress));

        SDL_Color renderColor = text.color;
        renderColor.a = alpha;

        int offsetY = (int)(-30.0f * progress);
        int textX = text.position.x;
        int textY = text.position.y + offsetY;

        // Render centered
        SDL_Surface *surf = TTF_RenderText_Blended(textFont, text.text.c_str(), renderColor);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_Rect dst = {
            textX - surf->w / 2,
            textY - surf->h / 2,
            surf->w,
            surf->h};
        SDL_FreeSurface(surf);
        SDL_SetTextureAlphaMod(tex, alpha);
        SDL_RenderCopy(interfaceRenderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);

        ++i;
    }
    if (gameoverOpen)
    {
        renderGameOver();
    }
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mp = {mx, my};
    SDL_Color color1 = {255, 180, 100, 255};
    SDL_Color color2 = {255, 150, 0, 255};
    drawRoundedButton(pauseBtn, "", SDL_PointInRect(&mp, &pauseBtn) ? color1 : color2, 0, 255);
    renderText(interfaceRenderer, smalltitleFont, "| |", white, 1235, 52);
    SDL_RenderPresent(interfaceRenderer);
    if (!objectiveClose)
        renderHardObjective();
}

void renderHardObjective()
{
    if (!objectiveRenderer)
        return;

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

    for (int i = 0; i < 6; ++i)
    {
        int col = (i < 3) ? 0 : 1;
        int row = i % 3;

        int centerX = (col == 0) ? 200 : 600;
        int centerY = 160 + row * 90;

        SDL_Rect fishRect = {centerX - 60, centerY - 30, 60, 60};
        SDL_RenderCopy(objectiveRenderer, objectiveTextures[i], NULL, &fishRect);

        std::string countText = " x " + std::to_string(objectiveFishes[i].count);
        renderText(objectiveRenderer, buttonFont, countText, black, centerX + 35, centerY - 5);
    }
    SDL_RenderPresent(objectiveRenderer);
}

void handleHardInterfaceEvents(SDL_Event &e, bool &interfaceOpen)
{
    if (!interfaceWindow)
        return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(interfaceWindow))
    {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        handleHardFishClick(mouseX, mouseY);
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point mp = {mouseX, mouseY};
        if (SDL_PointInRect(&mp, &pauseBtn) && !isPaused && objectiveClose)
        {
            initPauseMenu();
            pauseStartTime = SDL_GetTicks();
            isPaused = true;
        }
        else if (SDL_PointInRect(&mp, &pauseBtn) && isPaused && objectiveClose)
        {
            SDL_RaiseWindow(pauseWindow);
        }
        SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
    }
    if (e.type == SDL_KEYDOWN && e.window.windowID == SDL_GetWindowID(interfaceWindow))
        {
            if (e.key.keysym.sym == SDLK_ESCAPE && !isPaused)
            {
                initPauseMenu();
                pauseStartTime = SDL_GetTicks();
                isPaused = true;
            }
            else if (e.key.keysym.sym == SDLK_ESCAPE && isPaused)
            {
               SDL_RaiseWindow(pauseWindow);
            }
        }
    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(objectiveWindow))
    {
        SDL_Rect backBtnRect = {300, 380, 200, 60};
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePoint = {mx, my};

        if (SDL_PointInRect(&mousePoint, &backBtnRect))
        {
            SDL_DestroyRenderer(objectiveRenderer);
            SDL_DestroyWindow(objectiveWindow);
            objectiveRenderer = nullptr;
            objectiveWindow = nullptr;
            objectiveClose = true;
            if (!timerRunning)
            {
                timerStartTime = SDL_GetTicks();
                timerRunning = true;
            }
            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
        }
    }
    if (isPaused)
    {
        renderPauseMenu();
        handlePauseMenuEvents(e, isPaused);
    }
    if (gameoverOpen)
    {
        handleGameOverEvents(e, gameoverOpen);
    }
}

void handleHardInterfaceLogics()
{
    if (!interfaceWindow || isPaused)
        return;

    spawnHardFish();
    updateHardFishMotion();
    updateRain();

    pond.x += 1;
    pond2.x += 1;

    if (pond.x > 1279)
        pond.x = -1279;
    if (pond2.x > 1279)
        pond2.x = -1279;

    if (remaining == 0)
    {
        if (!gameoverOpen)
        {
            initGameOver();
            gameoverOpen = true;
        }
        /*if (targetScore != 0)
        {
            destroyHardInterface();
            timerRunning = false;
        }*/
    }
}

void gameCondition()
{
    
}

void destroyHardInterface()
{
    if (pondTexture)
    {
        SDL_DestroyTexture(pondTexture);
        pondTexture = nullptr;
    }
    if (pond2Texture)
    {
        SDL_DestroyTexture(pond2Texture);
        pond2Texture = nullptr;
    }
    if (mountainTexture)
    {
        SDL_DestroyTexture(mountainTexture);
        mountainTexture = nullptr;
    }
    for (int i = 0; i < 12; ++i)
    {
        if (fishTextures[i])
        {
            SDL_DestroyTexture(fishTextures[i]);
            fishTextures[i] = nullptr;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        if (rippleTextures[i])
        {
            SDL_DestroyTexture(rippleTextures[i]);
            rippleTextures[i] = nullptr;
        }
    }
    if (soundOn)
    {
        Mix_PauseMusic();
        Mix_PlayMusic(intro, -1);
    }
    else
        Mix_PauseMusic();
    objectiveClose = false;
    timerRunning = false;
    objectivesInitialized = false;
    heartTexture = nullptr;
    titleFont = nullptr;
    smalltitleFont = nullptr;
    buttonFont = nullptr;
    textFont = nullptr;
    typeFont = nullptr;
    fishScore = 0;
    targetScore = 0;
    lives = 3;
    timerStartTime = 0;
    congratsStartTime = 0;
    pauseStartTime = 0;
    totalPaused = 0;
    isPaused = false;
    congratulationsFlag = false;
    for (int i = 0; i < MAX_FISH; ++i)
    {
        fishes[i] = PondFish();
    }
    remaining = 120000;
    Mix_FreeChunk(bonuscatch);
    Mix_FreeChunk(crocodile);
    Mix_FreeChunk(rightfish);
    Mix_FreeChunk(wrongfish);
    if (!sunnyOn) destroyRain();
    if (interfaceRenderer)
    {
        SDL_DestroyRenderer(interfaceRenderer);
        interfaceRenderer = nullptr;
    }
    if (interfaceWindow)
    {
        SDL_DestroyWindow(interfaceWindow);
        interfaceWindow = nullptr;
    }
    hardinterfaceOpen = false;
}

bool isHardInterfaceOpen()
{
    return interfaceWindow != nullptr;
}