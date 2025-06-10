#include "Common.h"
#include "NewGame.h"
#include "HardInterface.h"
#include "MediumInterface.h"
#include "Settings.h"
#include "HighScores.h"
#include "GameRules.h"
#include "Controls.h"
#include "Weather.h"
#include "Exit.h"
#include <vector>
#include <cmath>
#include <ctime>

std::vector<Cloud>clouds;
std::vector<Fish>fishes;

void loadTextures(SDL_Renderer* renderer);
void freeTextures();
void updateClouds(std::vector<Cloud>& clouds);
void updateFishes(std::vector<Fish>& fishes);
void renderFishesAndClouds(SDL_Renderer* renderer, const std::vector<Cloud>& clouds, const std::vector<Fish>& fishes);
bool running = true;
bool soundOn = true;
bool sunnyOn = true;
bool objectiveClose = false;
Mix_Music* intro = nullptr;

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Init(SDL_INIT_AUDIO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);
    srand(static_cast<unsigned int>(time(NULL)));

    SDL_Window* mainWindow = SDL_CreateWindow("Hook the Fish", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_BORDERLESS);
    SDL_Renderer* mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_SOFTWARE);

    TTF_Font* titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    TTF_Font* buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    TTF_Font* copyrightFont = TTF_OpenFont("fonts/FiraCode-Regular.ttf", 20);

    loadTextures(mainRenderer);

    soundOn = loadSoundState();
    sunnyOn = loadWeatherState();
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    intro = Mix_LoadMUS("music/intro.mp3");
    if (soundOn) {
        Mix_PlayMusic(intro, -1);
    }
    else {
        Mix_PauseMusic();
    }
    
    for (int i = 0; i < 5; ++i) clouds.push_back(generateRandomCloud());
    for (int i = 0; i < 10; ++i) fishes.push_back(generateRandomFish());

    std::vector<Button> buttons = {
        {{300, 250, 250, 80}, "New Game"},
        {{750, 300, 250, 80}, "High Scores"},
        {{280, 450, 250, 80}, "Settings"},
        {{730, 500, 250, 80}, "Exit"}
    };

    bool newgameOpen = false;
    bool highscoresOpen = false;
    bool settingsOpen = false;
    bool exitOpen = false;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (e.type == SDL_MOUSEMOTION && e.window.windowID == SDL_GetWindowID(mainWindow)) {
                int x = e.motion.x, y = e.motion.y;
                SDL_Point p = {x, y};
                for (auto& btn : buttons) btn.hovered = SDL_PointInRect(&p, &btn.rect);
                for (auto& fish : fishes) fish.hovered = SDL_PointInRect(&p, &fish.rect);
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(mainWindow)) {
                int x = e.button.x, y = e.button.y;
                SDL_Point p = {x, y};
                for (auto& btn : buttons) {
                    if (SDL_PointInRect(&p, &btn.rect)) {
                        if (btn.text == "Exit" && !exitOpen)
                        {
                            initExit(mainRenderer);
                            exitOpen = true;
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        else if (btn.text == "Exit" && exitOpen)
                        {
                            SDL_RaiseWindow(exitWindow);
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        else if (btn.text == "Settings" && !settingsOpen)
                        {
                            initSettings(mainRenderer);
                            settingsOpen = true;
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        else if (btn.text == "Settings" && settingsOpen)
                        {
                            SDL_RaiseWindow(settingsWindow);
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        else if (btn.text == "High Scores" && !highscoresOpen)
                        {
                            initHighscores(mainRenderer);
                            highscoresOpen = true;
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        else if (btn.text == "High Scores" && highscoresOpen)
                        {
                            SDL_RaiseWindow(menuWindow);
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        else if (btn.text == "New Game" && !newgameOpen)
                        {
                            initNewGame(mainRenderer);
                            newgameOpen = true;
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                        else if (btn.text == "New Game" && newgameOpen)
                        {
                            SDL_RaiseWindow(newgameWindow);
                            SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                        }
                    }
                }
            }

            if (settingsOpen) {
                handleSettingsEvents(e, settingsOpen);
            }
            if (highscoresOpen) {
                handleHighscoreEvents(e, highscoresOpen);
            }
            if (exitOpen) {
                handleExitEvents(e, exitOpen);
            }
            if (GameRulesOpen) {
                handleGameRulesEvents(e, GameRulesOpen);
            }
            if (ControlOpen) {
                handleControlsEvents(e, ControlOpen);
            }
            if (WeatherOpen) {
                handleWeatherEvents(e, WeatherOpen);
            }  
            if (newgameOpen) {
                handleNewGameEvents(e, newgameOpen);
            }
            if (hardinterfaceOpen) {
                handleHardInterfaceEvents(e, hardinterfaceOpen);
            } 
            if(MediuminterfaceOpen){
                handleMediumInterfaceEvents(e, MediuminterfaceOpen);
            }
        }

        updateClouds(clouds);
        updateFishes(fishes);

        drawPondBackground(mainRenderer, 1280, 720);
        renderFishesAndClouds(mainRenderer, clouds, fishes);

        SDL_Rect hook1 = {375, 40, 100, 100};
        SDL_Rect hook2 = {415, 40, 100, 100};
        SDL_Rect copy = {530, 685, 30, 30};
        SDL_Rect fishrod = {780, 50, 80, 80};
        SDL_RenderCopy(mainRenderer, hookleftTexture, NULL, &hook1);
        SDL_RenderCopy(mainRenderer, hookrightTexture, NULL, &hook2);
        SDL_RenderCopy(mainRenderer, copyright, NULL, &copy);

        SDL_Color titleColor = {255, 220, 50, 255};
        renderText(mainRenderer, titleFont, "H     k the Fish", titleColor, 640, 100);

        for (auto& btn : buttons) {
            drawParallelogram(mainRenderer, btn, btn.hovered);
            renderText(mainRenderer, buttonFont, btn.text, {255, 255, 255, 255}, btn.rect.x + btn.rect.w / 2, btn.rect.y + btn.rect.h / 2);
        }
        renderText(mainRenderer, copyrightFont, "   the_primes_of_30", {0, 0, 0, 255}, 640, 700);

        SDL_RenderPresent(mainRenderer);

        if (settingsOpen) {
            renderSettings();
        }
        if (highscoresOpen) {
            renderHighscores();
        }
        if (exitOpen) {
            renderExit();
        }
        if (GameRulesOpen)
        {
            renderGameRules();
        }
        if (WeatherOpen)
        {
            renderWeather();
        }
        if (ControlOpen)
        {
            renderControls();
        }
        if (newgameOpen)
        {
            renderNewGame();
        }
        if (hardinterfaceOpen)
        {
            renderHardInterface();
        }
        if (hardinterfaceOpen && objectiveClose) {
            handleHardInterfaceLogics(e, hardinterfaceOpen);
        }
        if (MediuminterfaceOpen)
        {
            renderMediumInterface();
        }
        if (MediuminterfaceOpen && objectiveClose) {
            handleMediumInterfaceLogics(e, MediuminterfaceOpen);
        }
        SDL_Delay(16);
    }
    freeTextures();
    TTF_CloseFont(titleFont);
    TTF_CloseFont(buttonFont);
    TTF_CloseFont(copyrightFont);

    if (isSettingsOpen()) destroySettings();
    if (isHighscoreOpen()) destroyHighscores();
    if (isExitOpen()) destroyExit();
    if (isGameRulesOpen()) destroyGameRules();
    if (isControlsOpen()) destroyControls();
    if (isNewGameOpen()) destroyNewGame();
    if (isHardInterfaceOpen()) destroyHardInterface();
    
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    IMG_Quit();
    TTF_Quit();
    Mix_FreeMusic(intro);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
