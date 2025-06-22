#include "Common.h"
#include "Pause.h"
#include "HardInterface.h"
#include "MediumInterface.h"
#include "EasyInterface.h"
#include "GameRules.h"
#include "Settings.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <algorithm>
#include <string>

SDL_Window *pauseWindow = nullptr;
SDL_Window *pauseExitWindow = nullptr;
SDL_Window *pauseObjectiveWindow = nullptr;
static SDL_Renderer *pauseRenderer = nullptr;
static SDL_Renderer *pauseExitRenderer = nullptr;
static SDL_Renderer *pauseObjectiveRenderer = nullptr;
bool isPaused = false;
static bool isExit = false;
static bool objectiveOpen = false;
Uint32 pauseStartTime = 0;
Uint32 totalPaused = 0;
static SDL_Texture *objectiveTextures[6] = {nullptr};

TTF_Font *titleFont = nullptr;
TTF_Font *buttonFont = nullptr;
TTF_Font *textFont = nullptr;

static SDL_Rect resumeBtn = {200, 145, 400, 50};
static SDL_Rect objectiveBtn = {200, 205, 400, 50};
static SDL_Rect soundBtn = {200, 265, 400, 50};
static SDL_Rect toggleRect = {500, 275, 40, 30};
static SDL_Rect gameruleBtn = {200, 325, 400, 50};
static SDL_Rect exitBtn = {200, 385, 400, 50};
static SDL_Rect YesBtn = {150, 350, 200, 80};
static SDL_Rect NoBtn = {450, 350, 200, 80};

void loadEasyPauseObjectiveAssets(int type, int index)
{
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
        objectiveTextures[index] = SDL_CreateTextureFromSurface(pauseObjectiveRenderer, surf);
        SDL_FreeSurface(surf);
    }
}

void loadHardPauseObjectiveAssets(int type, int index)
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
        objectiveTextures[index] = SDL_CreateTextureFromSurface(pauseObjectiveRenderer, surf);
        SDL_FreeSurface(surf);
    }
}

void initPauseMenu()
{
    if (pauseWindow != nullptr)
        return;

    pauseWindow = SDL_CreateWindow("Pause", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    pauseRenderer = SDL_CreateRenderer(pauseWindow, -1, SDL_RENDERER_ACCELERATED);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 32);
}

void initPauseExit()
{
    if (pauseExitWindow != nullptr)
        return;

    pauseExitWindow = SDL_CreateWindow("Pause Exit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    pauseExitRenderer = SDL_CreateRenderer(pauseExitWindow, -1, SDL_RENDERER_ACCELERATED);
}

void initPauseObjective()
{
    if (pauseObjectiveWindow != nullptr)
        return;

    pauseObjectiveWindow = SDL_CreateWindow("Pause Objective", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    pauseObjectiveRenderer = SDL_CreateRenderer(pauseObjectiveWindow, -1, SDL_RENDERER_ACCELERATED);
    if (EasyinterfaceOpen)
    {
        for (int i = 0; i < 4; i++)
        {
            loadEasyPauseObjectiveAssets(objectiveFishes[i].type, i);
        }
    }
    else if (hardinterfaceOpen)
    {
        for (int i = 0; i < 6; i++)
        {
            loadHardPauseObjectiveAssets(objectiveFishes[i].type, i);
        }
    }
}

void renderPauseMenu()
{
    if (!pauseWindow)
        return;
    SDL_SetRenderDrawColor(pauseRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pauseRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    renderText(pauseRenderer, titleFont, "Pause", white, 400, 80);

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string &text, SDL_Color fillColor, bool hovered)
    {
        int radius = 25;
        if (hovered)
        {
            roundedBoxRGBA(pauseRenderer,
                           rect.x, rect.y,
                           rect.x + rect.w, rect.y + rect.h,
                           radius,
                           fillColor.r, fillColor.g, fillColor.b, 255);
        }
        else
        {
            roundedBoxRGBA(pauseRenderer,
                           rect.x, rect.y,
                           rect.x + rect.w, rect.y + rect.h,
                           radius,
                           fillColor.r, fillColor.g, fillColor.b, 200);
        }
        renderText(pauseRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mp = {mx, my};
    drawRoundedButton(resumeBtn, "Resume", {255, 100, 100, 255}, SDL_PointInRect(&mp, &resumeBtn));
    drawRoundedButton(objectiveBtn, "Objectives", {100, 255, 100, 255}, SDL_PointInRect(&mp, &objectiveBtn));
    drawRoundedButton(soundBtn, "Sound", {100, 255, 255, 255}, SDL_PointInRect(&mp, &soundBtn));
    drawRoundedButton(gameruleBtn, "Game Rules", {100, 100, 255, 255}, SDL_PointInRect(&mp, &gameruleBtn));
    drawRoundedButton(exitBtn, "Exit", {100, 100, 100, 255}, SDL_PointInRect(&mp, &exitBtn));

    SDL_SetRenderDrawColor(pauseRenderer, 60, 60, 60, 255);
    SDL_RenderFillRect(pauseRenderer, &toggleRect);

    SDL_Rect toggleKnob = {toggleRect.x + (soundOn ? 20 : 0), toggleRect.y, 20, 30};
    SDL_SetRenderDrawColor(pauseRenderer, soundOn ? 0 : 255, soundOn ? 255 : 0, 100, 255);
    SDL_RenderFillRect(pauseRenderer, &toggleKnob);

    SDL_RenderPresent(pauseRenderer);
}

void renderPauseExit()
{
    if (!pauseExitWindow)
        return;
    SDL_SetRenderDrawColor(pauseExitRenderer, 0, 0, 0, 255);
    SDL_RenderClear(pauseExitRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color undef = {80, 160, 240, 255};

    renderText(pauseExitRenderer, titleFont, "Exit", white, 400, 80);
    renderText(pauseExitRenderer, textFont, "Exit to Difficulty Menu?", undef, 400, 200);

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string &text, SDL_Color fillColor, bool hovered)
    {
        int radius = 25;
        if (hovered)
        {
            roundedBoxRGBA(pauseExitRenderer,
                           rect.x, rect.y,
                           rect.x + rect.w, rect.y + rect.h,
                           radius,
                           fillColor.r, fillColor.g, fillColor.b, 255);
        }
        else
        {
            roundedBoxRGBA(pauseExitRenderer,
                           rect.x, rect.y,
                           rect.x + rect.w, rect.y + rect.h,
                           radius,
                           fillColor.r, fillColor.g, fillColor.b, 200);
        }
        renderText(pauseExitRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mp = {mx, my};
    drawRoundedButton(YesBtn, "Yes", {255, 0, 0, 255}, SDL_PointInRect(&mp, &YesBtn));
    drawRoundedButton(NoBtn, "No", {0, 255, 0, 255}, SDL_PointInRect(&mp, &NoBtn));
    SDL_RenderPresent(pauseExitRenderer);
}

void renderPauseObjective()
{
    if (!pauseObjectiveWindow)
        return;
    SDL_SetRenderDrawColor(pauseObjectiveRenderer, 100, 255, 100, 255);
    SDL_RenderClear(pauseObjectiveRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    renderText(pauseObjectiveRenderer, titleFont, "Objectives", black, 400, 80);

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mp = {mx, my};

    SDL_Rect backBtnRect = {300, 380, 200, 60};
    Button backBtn = {backBtnRect, "Confirm", false};
    backBtn.hovered = SDL_PointInRect(&mp, &backBtn.rect);

    drawParallelogram(pauseObjectiveRenderer, backBtn, backBtn.hovered);
    renderText(pauseObjectiveRenderer, buttonFont, backBtn.text, white, backBtn.rect.x + backBtn.rect.w / 2, backBtn.rect.y + backBtn.rect.h / 2);
    if (EasyinterfaceOpen)
    {
        for (int i = 0; i < 4; ++i)
        {
            int col = i % 2;
            int row = i / 2;

            int centerX = 250 + col * 300; // 250, 550
            int centerY = 180 + row * 100;

            SDL_Rect fishRect = {centerX - 60, centerY - 30, 60, 60};
            SDL_RenderCopy(pauseObjectiveRenderer, objectiveTextures[i], NULL, &fishRect);

            std::string countText = " x " + std::to_string(objectiveFishes[i].count);
            renderText(pauseObjectiveRenderer, buttonFont, countText, black, centerX + 30, centerY - 5);
        }
    }
    else if (hardinterfaceOpen)
    {
        for (int i = 0; i < 6; ++i)
        {
            int col = (i < 3) ? 0 : 1;
            int row = i % 3;

            int centerX = (col == 0) ? 200 : 600;
            int centerY = 160 + row * 90;

            SDL_Rect fishRect = {centerX - 60, centerY - 30, 60, 60};
            SDL_RenderCopy(pauseObjectiveRenderer, objectiveTextures[i], NULL, &fishRect);

            std::string countText = " x " + std::to_string(objectiveFishes[i].count);
            renderText(pauseObjectiveRenderer, buttonFont, countText, black, centerX + 35, centerY - 5);
        }
    }
    SDL_RenderPresent(pauseObjectiveRenderer);
}

void handlePauseMenuEvents(SDL_Event &e, bool &isPaused)
{
    if (pauseWindow && e.window.windowID == SDL_GetWindowID(pauseWindow))
    {
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            destroyPauseMenu();
        }
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_Point mp = {e.button.x, e.button.y};
            if (SDL_PointInRect(&mp, &resumeBtn))
            {
                destroyPauseMenu();
                isPaused = false;
            }
            if (SDL_PointInRect(&mp, &objectiveBtn) && !objectiveOpen)
            {
                initPauseObjective();
                objectiveOpen = true;
            }
            else if (SDL_PointInRect(&mp, &objectiveBtn) && objectiveOpen)
            {
                SDL_RaiseWindow(pauseObjectiveWindow);
            }
            if (SDL_PointInRect(&mp, &soundBtn))
            {
                soundOn = !soundOn;
                saveSoundState(soundOn);
                if (soundOn)
                {
                    Mix_PlayMusic(game_music, -1);
                }
                else
                {
                    Mix_PauseMusic();
                }
            }
            if (SDL_PointInRect(&mp, &gameruleBtn) && !GameRulesOpen)
            {
                initGameRules(pauseRenderer);
                GameRulesOpen = true;
            }
            else if (SDL_PointInRect(&mp, &gameruleBtn) && GameRulesOpen)
            {
                SDL_RaiseWindow(GameRulesWindow);
            }
            if (SDL_PointInRect(&mp, &exitBtn) && !isExit)
            {
                initPauseExit();
                isExit = true;
            }
            else if (SDL_PointInRect(&mp, &exitBtn) && isExit)
            {
                SDL_RaiseWindow(pauseExitWindow);
            }
        }
        if (e.type = SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE && isPaused)
            {
                destroyPauseMenu();
                isPaused = false;
            }
        }
    }
    if (isExit)
    {
        renderPauseExit();
        handlePauseExitEvents(e, isExit);
    }
    if (objectiveOpen)
    {
        renderPauseObjective();
        handlePauseObjectiveEvents(e, objectiveOpen);
    }
}

void handlePauseExitEvents(SDL_Event &e, bool &isExit)
{
    if (!pauseExitWindow)
        return;

    if (e.type == SDL_WINDOWEVENT && e.window.windowID == SDL_GetWindowID(pauseExitWindow))
    {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            isExit = false;
            destroyPauseExit();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(pauseExitWindow))
    {
        SDL_Point mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &NoBtn))
        {
            isExit = false;
            destroyPauseExit();
        }
        if (SDL_PointInRect(&mp, &YesBtn))
        {
            isExit = false;
            destroyPauseExit();
            destroyPauseMenu();
            if (hardinterfaceOpen)
                destroyHardInterface();
            if (MediuminterfaceOpen)
                destroyMediumInterface();
            if (EasyinterfaceOpen)
                destroyEasyInterface();
        }
    }
}

void handlePauseObjectiveEvents(SDL_Event &e, bool &objectiveOpen)
{
    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(pauseObjectiveWindow))
    {
        SDL_Rect backBtnRect = {300, 380, 200, 60};
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePoint = {mx, my};

        if (SDL_PointInRect(&mousePoint, &backBtnRect))
        {
            for (int i = 0; i < 6; i++)
            {
                SDL_DestroyTexture(objectiveTextures[i]);
                objectiveTextures[i] = nullptr;
            }
            SDL_DestroyRenderer(pauseObjectiveRenderer);
            SDL_DestroyWindow(pauseObjectiveWindow);
            pauseObjectiveRenderer = nullptr;
            pauseObjectiveWindow = nullptr;
            objectiveOpen = false;
            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
        }
    }
}

void destroyPauseMenu()
{
    if (pauseRenderer)
    {
        SDL_DestroyRenderer(pauseRenderer);
        pauseRenderer = nullptr;
    }
    if (pauseWindow)
    {
        SDL_DestroyWindow(pauseWindow);
        pauseWindow = nullptr;
    }
    if (titleFont)
    {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont)
    {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (textFont)
    {
        TTF_CloseFont(textFont);
        textFont = nullptr;
    }
    isPaused = false;
    totalPaused += SDL_GetTicks() - pauseStartTime;
}

void destroyPauseExit()
{
    if (pauseExitRenderer)
    {
        SDL_DestroyRenderer(pauseExitRenderer);
        pauseExitRenderer = nullptr;
    }
    if (pauseWindow)
    {
        SDL_DestroyWindow(pauseExitWindow);
        pauseExitWindow = nullptr;
    }
    isExit = false;
}

bool isPauseOpen()
{
    return pauseWindow != nullptr;
}

bool ispauseExitOpen()
{
    return pauseExitWindow != nullptr;
}