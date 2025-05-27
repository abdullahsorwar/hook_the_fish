#include "GameRules.h"
#include "Settings.h"
#include "Common.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <sstream>

static SDL_Texture* LeftHook = nullptr;
static SDL_Texture* RightHook = nullptr;
SDL_Window* GameRulesWindow = nullptr;
static SDL_Renderer* GameRulesRenderer = nullptr;
static SDL_Rect ArrowLeftBtn = {10, 240, 50, 50};
static SDL_Rect ArrowRightBtn = {740, 240, 50, 50};
static SDL_Rect backBtn = {300, 380, 200, 60};
static TTF_Font* titleFont = nullptr;
static TTF_Font* GameRulesFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static int currentPage = 0;
const int totalPages = 4;
static SDL_Point mp = {0, 0};
static std::string pageText; 

bool GameRulesOpen = false;

std::vector<std::string> ruleFiles = {
    "texts/Intro.txt",
    "texts/Easy.txt",
    "texts/medium.txt",
    "texts/Hard.txt"
};

std::string loadPageContent(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "Failed to load " + path;

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void updatePageContent() {
    pageText = loadPageContent(ruleFiles[currentPage]);
    if (pageText.find("Failed to load") != std::string::npos) {
    pageText = "This rule page could not be loaded. Please check the file path.";
    }
}

void initGameRules(SDL_Renderer* settingsRenderer) {
    if (GameRulesWindow != nullptr) return;

    GameRulesWindow = SDL_CreateWindow("GameRules", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    GameRulesRenderer = SDL_CreateRenderer(GameRulesWindow, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* surf = nullptr;
    surf = SDL_LoadBMP("bmp/hook2.bmp");
    LeftHook = SDL_CreateTextureFromSurface(GameRulesRenderer, surf);
    SDL_FreeSurface(surf);
    
    surf = SDL_LoadBMP("bmp/hook3.bmp");
    RightHook = SDL_CreateTextureFromSurface(GameRulesRenderer, surf);
    SDL_FreeSurface(surf);
    
    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    GameRulesFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 32);
    updatePageContent();
}

void handleGameRulesEvents(SDL_Event& e, bool& GameRulesOpen) {
    if (!GameRulesWindow || e.window.windowID != SDL_GetWindowID(GameRulesWindow)) return;

    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            GameRulesOpen = false;
            destroyGameRules();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &ArrowLeftBtn) && currentPage > 0) {
            currentPage--;
            updatePageContent();
        }

        if (SDL_PointInRect(&mp, &ArrowRightBtn) && currentPage < totalPages - 1) {
            currentPage++;
            updatePageContent();
        }

        if (SDL_PointInRect(&mp, &backBtn)) {
            GameRulesOpen = false;
            destroyGameRules();
        }
    }
}

void renderGameRules() {
    if (!GameRulesRenderer) return;

    SDL_SetRenderDrawColor(GameRulesRenderer, 100, 100, 255, 255);
    SDL_RenderClear(GameRulesRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color bhondo = {255, 255, 255, 255};
    
    renderText(GameRulesRenderer, titleFont, "Game Rules", bhondo, 400, 80);
    renderWrappedText(GameRulesRenderer, GameRulesFont, pageText.c_str(), black, 420, 245, 650);
    
    if (currentPage > 0) {
        SDL_RenderCopy(GameRulesRenderer, LeftHook, NULL, &ArrowLeftBtn);
    }

    if (currentPage < totalPages - 1) {
        SDL_RenderCopy(GameRulesRenderer, RightHook, NULL, &ArrowRightBtn);
    }
    
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    Button back = {backBtn, "Close", false};
    back.hovered = SDL_PointInRect(&mousePoint, &back.rect);

    drawParallelogram(GameRulesRenderer, back, back.hovered);
    renderText(GameRulesRenderer, buttonFont, back.text, white, back.rect.x + back.rect.w / 2, back.rect.y + back.rect.h / 2);
    
    std::string pageNumber = std::to_string(currentPage + 1) + "/4";
    renderText(GameRulesRenderer, buttonFont, pageNumber.c_str(), black, 720, 430);

    SDL_RenderPresent(GameRulesRenderer);
}

void destroyGameRules() {
    if (GameRulesFont) {
        TTF_CloseFont(GameRulesFont);
        GameRulesFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (LeftHook) {
        SDL_DestroyTexture(LeftHook);
        LeftHook = nullptr;
    }
    if (RightHook) {
        SDL_DestroyTexture(RightHook);
        RightHook = nullptr;
    }
    if (GameRulesRenderer) {
        SDL_DestroyRenderer(GameRulesRenderer);
        GameRulesRenderer = nullptr;
    }
    if (GameRulesWindow) {
        SDL_DestroyWindow(GameRulesWindow);
        GameRulesWindow = nullptr;
    }
}

bool isGameRulesOpen() {
    return GameRulesWindow != nullptr;
}

