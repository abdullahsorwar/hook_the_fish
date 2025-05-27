#include "Weather.h"
#include "Settings.h"
#include "Common.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

SDL_Window* WeatherWindow = nullptr;
static SDL_Renderer* WeatherRenderer = nullptr;
static SDL_Rect sunnyBtn = {150, 200, 200, 60};
static SDL_Rect rainyBtn = {450, 200, 200, 60};
static SDL_Rect backBtnRect = {300, 380, 200, 60};
static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;

bool WeatherOpen = false;

void initWeather(SDL_Renderer* settingsRenderer) {
    if (WeatherWindow != nullptr) return;

    WeatherWindow = SDL_CreateWindow("Weather", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    WeatherRenderer = SDL_CreateRenderer(WeatherWindow, -1, SDL_RENDERER_ACCELERATED);
    
    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
}

void handleWeatherEvents(SDL_Event& e, bool& WeatherOpen) {
    if (!WeatherWindow || e.window.windowID != SDL_GetWindowID(WeatherWindow)) return;

    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            WeatherOpen = false;
            destroyWeather();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &backBtnRect)) {
            WeatherOpen = false;
            destroyWeather();
        }
        if (SDL_PointInRect(&mp, &sunnyBtn) && !sunnyOn) {
            sunnyOn = !sunnyOn;
            saveWeatherState(sunnyOn);
        }
        if (SDL_PointInRect(&mp, &rainyBtn) && sunnyOn) {
            sunnyOn = !sunnyOn;
            saveWeatherState(sunnyOn);
        }
    }
}

void renderWeather() {
    if (!WeatherRenderer) return;

    SDL_SetRenderDrawColor(WeatherRenderer, 100, 100, 100, 255);
    SDL_RenderClear(WeatherRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color green = {0, 255, 0, 255};
    
    renderText(WeatherRenderer, titleFont, "Weather", white, 400, 80);
    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor, bool hovered) {
        int radius = 25;
        if (hovered)
        roundedBoxRGBA(WeatherRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 255);
        else
        roundedBoxRGBA(WeatherRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 200);
        renderText(WeatherRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};
    drawRoundedButton(rainyBtn, "Rainy", (sunnyOn)? red : green, SDL_PointInRect(&mousePoint, &rainyBtn));
    drawRoundedButton(sunnyBtn, "Sunny", (sunnyOn)? green : red, SDL_PointInRect(&mousePoint, &sunnyBtn));
    
    Button backBtn = {backBtnRect, "Close", false};
    backBtn.hovered = SDL_PointInRect(&mousePoint, &backBtn.rect);

    drawParallelogram(WeatherRenderer, backBtn, backBtn.hovered);
    renderText(WeatherRenderer, buttonFont, backBtn.text, white, backBtn.rect.x + backBtn.rect.w / 2, backBtn.rect.y + backBtn.rect.h / 2);
    
    SDL_RenderPresent(WeatherRenderer);
}

void destroyWeather() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (WeatherRenderer) {
        SDL_DestroyRenderer(WeatherRenderer);
        WeatherRenderer = nullptr;
    }
    if (WeatherWindow) {
        SDL_DestroyWindow(WeatherWindow);
        WeatherWindow = nullptr;
    }
}

bool isWeatherOpen() {
    return WeatherWindow != nullptr;
}

