#include "NewGame.h"
#include "Interface.h"
#include "Common.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

SDL_Window* newgameWindow = nullptr;
static SDL_Renderer* newgameRenderer = nullptr;
static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;

static SDL_Rect easyBtn = {200, 140, 400, 60};
static SDL_Rect mediumBtn = {200, 220, 400, 60};
static SDL_Rect hardBtn = {200, 300, 400, 60};

void initNewGame(SDL_Renderer* mainRenderer) {
    if (newgameWindow != nullptr) return;

    newgameWindow = SDL_CreateWindow("New Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    newgameRenderer = SDL_CreateRenderer(newgameWindow, -1, SDL_RENDERER_ACCELERATED);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
}

void handleNewGameEvents(SDL_Event& e, bool& newgameOpen) {
    if (!newgameWindow) return;

    if (e.type == SDL_WINDOWEVENT && e.window.windowID == SDL_GetWindowID(newgameWindow)) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            newgameOpen = false;
            destroyNewGame();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &easyBtn) && !interfaceOpen) {
            initInterface (newgameRenderer);
            interfaceOpen = true;
        }
        else if (SDL_PointInRect(&mp, &easyBtn) && interfaceOpen) {
            SDL_RaiseWindow(interfaceWindow);
        }
        else if (SDL_PointInRect(&mp, &mediumBtn)) {
            
        }
        else if (SDL_PointInRect(&mp, &hardBtn)) {
            
        }

        SDL_Rect backBtnRect = {300, 380, 200, 60};
        if (SDL_PointInRect(&mp, &backBtnRect)) {
            newgameOpen = false;
            destroyNewGame();
        }
    }
}

void renderNewGame() {
    if (!newgameRenderer) return;

    SDL_SetRenderDrawColor(newgameRenderer, 20, 20, 40, 255);
    SDL_RenderClear(newgameRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    renderText(newgameRenderer, titleFont, "Set Difficulty", white, 400, 80);

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor, bool hovered) {
        int radius = 25;
        roundedBoxRGBA(newgameRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b,
                       hovered ? 255 : 200);
        renderText(newgameRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    drawRoundedButton(easyBtn, "Easy", {100, 255, 100, 255}, SDL_PointInRect(&mousePoint, &easyBtn));
    drawRoundedButton(mediumBtn, "Medium", {255, 255, 100, 255}, SDL_PointInRect(&mousePoint, &mediumBtn));
    drawRoundedButton(hardBtn, "Hard", {255, 100, 100, 255}, SDL_PointInRect(&mousePoint, &hardBtn));

    SDL_Rect backBtnRect = {300, 380, 200, 60};
    Button backBtn = {backBtnRect, "Close", false};
    backBtn.hovered = SDL_PointInRect(&mousePoint, &backBtn.rect);

    drawParallelogram(newgameRenderer, backBtn, backBtn.hovered);
    renderText(newgameRenderer, buttonFont, backBtn.text, white, backBtn.rect.x + backBtn.rect.w / 2, backBtn.rect.y + backBtn.rect.h / 2);

    SDL_RenderPresent(newgameRenderer);
}

void destroyNewGame() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (newgameRenderer) {
        SDL_DestroyRenderer(newgameRenderer);
        newgameRenderer = nullptr;
    }
    if (newgameWindow) {
        SDL_DestroyWindow(newgameWindow);
        newgameWindow = nullptr;
    }
}

bool isNewGameOpen() {
    return newgameWindow != nullptr;
}
