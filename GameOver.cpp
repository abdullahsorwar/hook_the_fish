#include "GameOver.h"
#include "MediumInterface.h"
#include "NewGame.h"
#include "Common.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <sstream>

SDL_Window* GameOverWindow = nullptr;
SDL_Renderer* GameOverRenderer = nullptr;
static TTF_Font* TextFont = nullptr;
static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static SDL_Rect ContinueBtn = {300, 380, 200, 60};
static std::string GameOverText;
bool GameOverOpen;

std::string TextFile;

std::string loadGameOverContent(const std::string& path) {
    if(isLifeLost){
        TextFile = "texts/LifeLost.txt";
    }
    else if(!timerRunning){
        TextFile = "texts/TimeLost.txt";
    }
    std::ifstream file(path);
    if (!file.is_open()) return "Failed to load " + path;

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void initGameOver() {
    if (GameOverWindow != nullptr) return;

    GameOverWindow = SDL_CreateWindow("GameOver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    GameOverRenderer = SDL_CreateRenderer(GameOverWindow, -1, SDL_RENDERER_ACCELERATED);

    GameOverText = loadGameOverContent(TextFile);
    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    TextFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 40);
    if (!titleFont || !buttonFont) {
      SDL_Log("Failed to load font: %s", TTF_GetError());
    }
    
}

void handleGameOverEvents(SDL_Event& e, bool& GameOverOpen) {
    if (!GameOverWindow || e.window.windowID != SDL_GetWindowID(GameOverWindow)) return;

    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            GameOverOpen = false;
            destroyGameOver();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &ContinueBtn)) {
            GameOverOpen = false;

            // Destroy Game Over Screen
            destroyGameOver();
            destroyMediumInterface();
        }
    }
}


void renderGameOver() {
    if (!GameOverRenderer) return;

    SDL_SetRenderDrawColor(GameOverRenderer, 255, 0, 0, 255);
    SDL_RenderClear(GameOverRenderer);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    
    renderText(GameOverRenderer, titleFont, "GAMEOVER", black, 400, 80);
    
    renderWrappedText(GameOverRenderer, TextFont, GameOverText.c_str(), black, 420, 200, 700);
    
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    Button back = {ContinueBtn, "Continue", false};
    back.hovered = SDL_PointInRect(&mousePoint, &back.rect);

    drawParallelogram(GameOverRenderer, back, back.hovered);
    renderText(GameOverRenderer, buttonFont, back.text, white, back.rect.x + back.rect.w / 2, back.rect.y + back.rect.h / 2);

    SDL_RenderPresent(GameOverRenderer);
}

void destroyGameOver() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (TextFont) {
        TTF_CloseFont(TextFont);
        TextFont = nullptr;
    }
    if (GameOverRenderer) {
        SDL_DestroyRenderer(GameOverRenderer);
        GameOverRenderer = nullptr;
    }
    if (GameOverWindow) {
        SDL_DestroyWindow(GameOverWindow);
        GameOverWindow = nullptr;
    }
}

bool isGameOverOpen() {
    return GameOverWindow != nullptr;
}

