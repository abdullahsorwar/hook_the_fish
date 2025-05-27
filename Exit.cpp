#include "Common.h"
#include "Exit.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

SDL_Window* exitWindow = nullptr;
static SDL_Renderer* exitRenderer = nullptr;
static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static TTF_Font* textFont = nullptr;

static SDL_Rect YesBtn = {150, 350, 200, 80};
static SDL_Rect NoBtn = {450, 350, 200, 80};

void initExit(SDL_Renderer* mainRenderer) {
    if (exitWindow != nullptr) return;

    exitWindow = SDL_CreateWindow("Exit", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    exitRenderer = SDL_CreateRenderer(exitWindow, -1, SDL_RENDERER_ACCELERATED);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/PTSerif-Regular.ttf", 48);
}

void handleExitEvents(SDL_Event& e, bool& exitOpen) {
    if (!exitWindow) return;

    if (e.type == SDL_WINDOWEVENT && e.window.windowID == SDL_GetWindowID(exitWindow)) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            exitOpen = false;
            running = false;
            destroyExit();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &NoBtn)) {
            exitOpen = false;
            destroyExit();
        }
        if (SDL_PointInRect(&mp, &YesBtn)) {
            exitOpen = false;
            running = false;
            destroyExit();
        }
    }
}

void renderExit() {
    if (!exitRenderer) return;

    SDL_SetRenderDrawColor(exitRenderer, 20, 20, 40, 255);
    SDL_RenderClear(exitRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color goku = {80, 160, 240, 255};

    renderText(exitRenderer, titleFont, "Exit", white, 400, 80);
    renderText(exitRenderer, textFont, "Are you sure?", goku, 400, 200);

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor, bool hovered) {
        int radius = 25;
        if (hovered)
        roundedBoxRGBA(exitRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 255);
        else
        roundedBoxRGBA(exitRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 200);
        renderText(exitRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};
    drawRoundedButton(YesBtn, "Yes", {255, 0, 0, 255}, SDL_PointInRect(&mousePoint, &YesBtn));
    drawRoundedButton(NoBtn, "No", {0, 255, 0, 255}, SDL_PointInRect(&mousePoint, &NoBtn));
    SDL_RenderPresent(exitRenderer);
}

void destroyExit() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (exitRenderer) {
        SDL_DestroyRenderer(exitRenderer);
        exitRenderer = nullptr;
    }
    if (exitWindow) {
        SDL_DestroyWindow(exitWindow);
        exitWindow = nullptr;
    }
}

bool isExitOpen() {
    return exitWindow != nullptr;
}
