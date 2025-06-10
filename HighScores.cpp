#include "Common.h"
#include "HighScores.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

SDL_Window* menuWindow = nullptr;
static SDL_Renderer* menuRenderer = nullptr;

static SDL_Window* subWindow = nullptr;
static SDL_Renderer* subRenderer = nullptr;

static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static TTF_Font* textFont = nullptr;

static std::string subWindowTitle = "";

struct ScoreButton {
    SDL_Rect rect;
    SDL_Color color;
    std::string label;
    std::string filename;
    bool hovered;
};

static std::vector<ScoreButton> difficultyButtons = {
    {{65, 200, 200, 80}, {100, 255, 100, 255}, "Easy", "files/easy.txt", false},
    {{305, 200, 200, 80}, {255, 255, 100, 255}, "Medium", "files/medium.txt", false},
    {{545, 200, 200, 80}, {255, 100, 100, 255}, "Hard", "files/hard.txt", false}
};

static SDL_Rect closeBtnRect = {300, 380, 200, 60};


std::vector<pair<string, int>> loadedScores = {};

void loadScoresFromFile(const std::string& filename) {
    loadedScores.clear();
    std::ifstream file(filename);
    std::string name;
    int score;
    while (file >> name >> score) {
        loadedScores.emplace_back(name, score);
    }
    sort(loadedScores.begin(), loadedScores.end(), [](const auto& a, const auto& b) {
        return b.second < a.second;
    });
    if (loadedScores.size() > 5) {
        loadedScores.pop_back();

        std::ofstream outFile(filename, std::ios::trunc);
        for (const auto& entry : loadedScores) {
            outFile << entry.first << " " << entry.second << "\n";
        }
    }
}
std::string checkAndAddHighScore(const std::string& filename, const std::string& playerName, int newScore) {

    if (playerName.length() > 18) {
        return "18";
    }

    for (char c : playerName) {
        if (!(std::isalnum(c) || c == '_')) {
            return "-1";
        }
    }

    loadScoresFromFile(filename);

    if (loadedScores.size() < 5 || newScore > loadedScores.back().second) {
        std::cout << "You made a High Score!\n";
        loadedScores.emplace_back(playerName, newScore);

        std::sort(loadedScores.begin(), loadedScores.end(), [](const auto& a, const auto& b) {
            return b.second < a.second;
        });

        if (loadedScores.size() > 5) {
            loadedScores.pop_back();
        }

        std::ofstream outFile(filename, std::ios::trunc);
        if (!outFile) {
            std::cerr << "Error: Could not open file " << filename << " for writing.\n";
            return "-2";
        }
        for (const auto& entry : loadedScores) {
            outFile << entry.first << " " << entry.second << "\n";
            std::cout << "Writing: " << entry.first << " " << entry.second << std::endl;
        }
    }
    return "0";
}


void initHighscores(SDL_Renderer* mainRenderer) {
    if (menuWindow != nullptr) return;

    menuWindow = SDL_CreateWindow("Select Difficulty", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    menuRenderer = SDL_CreateRenderer(menuWindow, -1, SDL_RENDERER_ACCELERATED);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/PTSerif-Regular.ttf", 32);
}

void openSubWindow(const std::string& title, const std::string& filename) {
    if (subWindow) return;

    subWindow = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS);
    subRenderer = SDL_CreateRenderer(subWindow, -1, SDL_RENDERER_ACCELERATED);
    
    subWindowTitle = title;
    loadScoresFromFile(filename);
    SDL_StartTextInput();
}

void handleHighscoreEvents(SDL_Event& e, bool& highscoreOpen) {
    if (menuWindow && e.window.windowID == SDL_GetWindowID(menuWindow)) {
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            highscoreOpen = false;
            destroyHighscores();
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point mp = {e.button.x, e.button.y};
            for (auto& btn : difficultyButtons) {
                if (SDL_PointInRect(&mp, &btn.rect)) {
                    openSubWindow(btn.label, btn.filename);
                }
            }
            if (SDL_PointInRect(&mp, &closeBtnRect)) {
                highscoreOpen = false;
                destroyHighscores();
            }
        }
    }

    if (subWindow && e.window.windowID == SDL_GetWindowID(subWindow)) {
        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            SDL_DestroyRenderer(subRenderer);
            SDL_DestroyWindow(subWindow);
            subRenderer = nullptr;
            subWindow = nullptr;
            SDL_StopTextInput();
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point mp = {e.button.x, e.button.y};
            if (SDL_PointInRect(&mp, &closeBtnRect)) {
                SDL_DestroyRenderer(subRenderer);
                SDL_DestroyWindow(subWindow);
                subRenderer = nullptr;
                subWindow = nullptr;
                SDL_StopTextInput();
            }
        }
    }
}

void renderHighscores() {
    if (!menuRenderer) return;

    SDL_SetRenderDrawColor(menuRenderer, 20, 20, 40, 255);
    SDL_RenderClear(menuRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    renderText(menuRenderer, titleFont, "HighScores", white, 400, 80);

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    for (auto& btn : difficultyButtons) {
        btn.hovered = SDL_PointInRect(&mousePoint, &btn.rect);
        SDL_Color renderColor = btn.color;
        renderColor.a = btn.hovered ? 255 : 200;

        roundedBoxRGBA(menuRenderer, btn.rect.x, btn.rect.y, btn.rect.x + btn.rect.w, btn.rect.y + btn.rect.h, 40,
                       renderColor.r, renderColor.g, renderColor.b, renderColor.a);
        renderText(menuRenderer, buttonFont, btn.label, black, btn.rect.x + btn.rect.w / 2, btn.rect.y + btn.rect.h / 2);
    }

    Button closeBtn = {closeBtnRect, "Close", SDL_PointInRect(&mousePoint, &closeBtnRect)};
    drawParallelogram(menuRenderer, closeBtn, closeBtn.hovered);
    renderText(menuRenderer, buttonFont, closeBtn.text, white, closeBtn.rect.x + closeBtn.rect.w / 2, closeBtn.rect.y + closeBtn.rect.h / 2);

    SDL_RenderPresent(menuRenderer);

    if (subRenderer) {
        SDL_SetRenderDrawColor(subRenderer, 20, 20, 40, 255);
        SDL_RenderClear(subRenderer);

        renderText(subRenderer, titleFont, subWindowTitle , white, 400, 80);
        
        renderText(subRenderer, buttonFont, "Pos.", white, 100, 150);
        renderText(subRenderer, buttonFont, "Name", white, 400, 150);
        renderText(subRenderer, buttonFont, "Score", white, 700, 150);

        int y = 185;
        for (size_t i = 0; i < loadedScores.size(); ++i) {
            string pos;
            if (i == 0) pos = "1st";
            else if (i == 1) pos = "2nd";
            else if (i == 2) pos = "3rd";
            else pos = to_string(i + 1) + "th";

            string name = loadedScores[i].first;
            int score = loadedScores[i].second;

            renderText(subRenderer, buttonFont, pos, white, 100, y);
            renderText(subRenderer, buttonFont, name, white, 400, y);
            renderText(subRenderer, buttonFont, to_string(score), white, 700, y);

            y += 40;
        }

        Button closeSubBtn = {closeBtnRect, "Close", false};
        int mx2, my2;
        SDL_GetMouseState(&mx2, &my2);
        SDL_Point mp2 = {mx2, my2};
        closeSubBtn.hovered = SDL_PointInRect(&mp2, &closeBtnRect);
        drawParallelogram(subRenderer, closeSubBtn, closeSubBtn.hovered);
        renderText(subRenderer, buttonFont, closeSubBtn.text, white, closeBtnRect.x + closeBtnRect.w / 2, closeBtnRect.y + closeBtnRect.h / 2);

        SDL_RenderPresent(subRenderer);
    }
}

void destroyHighscores() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (menuRenderer) {
        SDL_DestroyRenderer(menuRenderer);
        menuRenderer = nullptr;
    }
    if (menuWindow) {
        SDL_DestroyWindow(menuWindow);
        menuWindow = nullptr;
    }
    if (subRenderer) {
        SDL_DestroyRenderer(subRenderer);
        subRenderer = nullptr;
    }
    if (subWindow) {
        SDL_DestroyWindow(subWindow);
        subWindow = nullptr;
    }
    SDL_StopTextInput();
}

bool isHighscoreOpen() {
    return menuWindow != nullptr;
}