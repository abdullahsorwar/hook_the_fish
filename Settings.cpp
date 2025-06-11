#include "Settings.h"
#include "Weather.h"
#include "GameRules.h"
#include "Controls.h"
#include "Common.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

SDL_Window* settingsWindow = nullptr;
static SDL_Renderer* settingsRenderer = nullptr;
static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;

static SDL_Rect controlBtn = {200, 130, 400, 50};
static SDL_Rect soundBtn = {200, 190, 400, 50};
static SDL_Rect howBtn = {200, 250, 400, 50};
static SDL_Rect weatherBtn = {200, 310, 400, 50};
static SDL_Rect toggleRect = {500, 200, 40, 30};

void initSettings(SDL_Renderer* mainRenderer) {
    if (settingsWindow != nullptr) return;

    settingsWindow = SDL_CreateWindow("Settings", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    settingsRenderer = SDL_CreateRenderer(settingsWindow, -1, SDL_RENDERER_ACCELERATED);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
}



void handleSettingsEvents(SDL_Event& e, bool& settingsOpen) {
    if (!settingsWindow) return;
    if (!settingsWindow || e.window.windowID != SDL_GetWindowID(settingsWindow)) return;

    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            settingsOpen = false;
            destroySettings();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &soundBtn)) {
            soundOn = !soundOn;
            saveSoundState(soundOn);
            if (soundOn) {
                Mix_PlayMusic(intro, -1);
            } else {
                Mix_PauseMusic();
            }
        }

        if (SDL_PointInRect(&mp, &howBtn)) {
            if (!GameRulesOpen) {
                initGameRules(settingsRenderer);
                GameRulesOpen = true;
            } else {
                SDL_RaiseWindow(GameRulesWindow);
            }
        }
        
        if (SDL_PointInRect(&mp, &controlBtn)) {
            if (!ControlOpen) {
                initControls(settingsRenderer);
                ControlOpen = true;
            } else {
                SDL_RaiseWindow(ControlWindow);
            }
        }
        
        if (SDL_PointInRect(&mp, &weatherBtn)) {
            if (!WeatherOpen) {
                initWeather(settingsRenderer);
                WeatherOpen = true;
            } else {
                SDL_RaiseWindow(WeatherWindow);
            }
        }

        SDL_Rect backBtnRect = {300, 380, 200, 60};
        if (SDL_PointInRect(&mp, &backBtnRect)) {
            settingsOpen = false;
            destroySettings();
        }
    }
}

void renderSettings() {
    if (!settingsRenderer) return;

    SDL_SetRenderDrawColor(settingsRenderer, 20, 20, 40, 255);
    SDL_RenderClear(settingsRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    renderText(settingsRenderer, titleFont, "Settings", white, 400, 80);

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor, bool hovered) {
        int radius = 25;
        if (hovered)
        roundedBoxRGBA(settingsRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 255);
        else
        roundedBoxRGBA(settingsRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 200);
        renderText(settingsRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};
    drawRoundedButton(controlBtn, "Controls", {255, 100, 100, 255}, SDL_PointInRect(&mousePoint, &controlBtn));
    drawRoundedButton(soundBtn, "Sound", {100, 255, 255, 255}, SDL_PointInRect(&mousePoint, &soundBtn));
    drawRoundedButton(howBtn, "Game Rules", {100, 100, 255, 255}, SDL_PointInRect(&mousePoint, &howBtn));
    drawRoundedButton(weatherBtn, "Weather", {100, 100, 100, 255}, SDL_PointInRect(&mousePoint, &weatherBtn));

    SDL_SetRenderDrawColor(settingsRenderer, 60, 60, 60, 255);
    SDL_RenderFillRect(settingsRenderer, &toggleRect);

    SDL_Rect toggleKnob = {toggleRect.x + (soundOn ? 20 : 0), toggleRect.y, 20, 30};
    SDL_SetRenderDrawColor(settingsRenderer, soundOn ? 0 : 255, soundOn ? 255 : 0, 100, 255);
    SDL_RenderFillRect(settingsRenderer, &toggleKnob);

    SDL_Rect backBtnRect = {300, 380, 200, 60};
    Button backBtn = {backBtnRect, "Close", false};
    backBtn.hovered = SDL_PointInRect(&mousePoint, &backBtn.rect);

    drawParallelogram(settingsRenderer, backBtn, backBtn.hovered);
    renderText(settingsRenderer, buttonFont, backBtn.text, white, backBtn.rect.x + backBtn.rect.w / 2, backBtn.rect.y + backBtn.rect.h / 2);

    SDL_RenderPresent(settingsRenderer);
}

void destroySettings() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (settingsRenderer) {
        SDL_DestroyRenderer(settingsRenderer);
        settingsRenderer = nullptr;
    }
    if (settingsWindow) {
        SDL_DestroyWindow(settingsWindow);
        settingsWindow = nullptr;
    }
}

bool isSettingsOpen() {
    return settingsWindow != nullptr;
}
