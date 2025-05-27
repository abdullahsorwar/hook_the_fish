#include "Controls.h"
#include "Settings.h"
#include "Common.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <sstream>

SDL_Window* ControlWindow = nullptr;
static SDL_Renderer* ControlRenderer = nullptr;
static TTF_Font* TextFont = nullptr;
static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static SDL_Rect closeBtn = {300, 380, 200, 60};
static std::string controlText;
bool ControlOpen = false;

std::string ControlFile = "texts/Control.txt";

std::string loadControlContent(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "Failed to load " + path;

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void initControls(SDL_Renderer* settingsRenderer) {
    if (ControlWindow != nullptr) return;

    ControlWindow = SDL_CreateWindow("Controls", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    ControlRenderer = SDL_CreateRenderer(ControlWindow, -1, SDL_RENDERER_ACCELERATED);
    
    controlText = loadControlContent(ControlFile);
    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    TextFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 40);
    if (!titleFont || !buttonFont) {
      SDL_Log("Failed to load font: %s", TTF_GetError());
    }
    
}

void handleControlsEvents(SDL_Event& e, bool& ControlsOpen) {
    if (!ControlWindow || e.window.windowID != SDL_GetWindowID(ControlWindow)) return;

    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            ControlsOpen = false;
            destroyControls();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &closeBtn)) {
            ControlsOpen = false;
            destroyControls();
        }
    }
}

void renderControls() {
    if (!ControlRenderer) return;

    SDL_SetRenderDrawColor(ControlRenderer, 255, 100, 100, 255);
    SDL_RenderClear(ControlRenderer);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    
    renderText(ControlRenderer, titleFont, "Controls", black, 400, 80);
    
    renderWrappedText(ControlRenderer, TextFont, controlText.c_str(), black, 420, 200, 700);
    
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    Button back = {closeBtn, "Close", false};
    back.hovered = SDL_PointInRect(&mousePoint, &back.rect);

    drawParallelogram(ControlRenderer, back, back.hovered);
    renderText(ControlRenderer, buttonFont, back.text, white, back.rect.x + back.rect.w / 2, back.rect.y + back.rect.h / 2);

    SDL_RenderPresent(ControlRenderer);
}

void destroyControls() {
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
    if (ControlRenderer) {
        SDL_DestroyRenderer(ControlRenderer);
        ControlRenderer = nullptr;
    }
    if (ControlWindow) {
        SDL_DestroyWindow(ControlWindow);
        ControlWindow = nullptr;
    }
}

bool isControlsOpen() {
    return ControlWindow != nullptr;
}

