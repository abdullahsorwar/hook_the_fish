#include "Common.h"
#include "NewGame.h"
#include "GameOver.h"
#include "MediumInterface.h"
#include "Rain.h"
#include "Pause.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iostream>

#define MAX_FISH 10
#define PI acos(-1)

static SDL_Rect pond = {0, 250, 1280, 470}, pond2 = {-1279, 250, 1280, 470};
static SDL_Rect mountain = {0, 0, 1280, 250}, mountain2 = {-1279, 0, 1280, 250};
static SDL_Rect inputBox = {250, 200, 300, 50};
static SDL_Rect boat = {500, 100, 200, 200};
static SDL_Rect confirmButton = {300, 380, 200, 60};

Mix_Music* medium_game_music = nullptr;

//SDL_Window* interfaceWindow = nullptr;
//static SDL_Renderer* interfaceRenderer = nullptr;
static SDL_Renderer* objectiveRenderer = nullptr;
static SDL_Window* mediumGameWinWindow = nullptr;
//SDL_Renderer* hardInterfaceRenderer = nullptr;
static SDL_Renderer* gamewinRenderer = nullptr;

static SDL_Texture* pondTexture = nullptr;
static SDL_Texture* pond2Texture = nullptr;
static SDL_Texture* mountainTexture = nullptr;
static SDL_Texture* mountain2Texture = nullptr;
static SDL_Texture *boatTexture = nullptr;
static SDL_Texture* heartTexture = nullptr;
static std::vector<SDL_Texture*> fishTextures, objectiveTextures, rippleTextures;

static TTF_Font* titleFont = nullptr, *buttonFont = nullptr, *textFont = nullptr, *typeFont = nullptr;
static TTF_Font* smalltitleFont = nullptr, *messageFont = nullptr;

//int lives = 3;
uint32_t timerStartTime = 0;
static Uint32 congratsStartTime = 0;
bool timerRunning = false;
static const Uint32 TIMER_DURATION = 120000;
static Uint32 remaining_time = 120000;

bool MediuminterfaceOpen = false;
bool isLifeLost = false;
static bool timerStarted = false;
static bool congratulationsFlag = false;

std::string mediumuserInput = "";
std::string mediumfinalText = "";
std::string mediumconf = "";

static bool inputActive = false;
static bool showCursor = true;
static bool gamewinOpen = false;

static Uint32 lastCursorToggle = 0;

struct PondFish {
    SDL_Rect rect;
    float arcHeight;
    float t;
    bool goingUp;
    bool active;
    bool rippleActive;
    int rippleFrame;
    int direction; 
    int type;
    int baseX, baseY;
    bool clicked;
};
/*
struct ObjectiveFish {
    int type;
    int count;
};*/

//static ObjectiveFish objectiveFishes[5];
static bool objectivesInitialized = false;
static PondFish fishes[MAX_FISH];
static std::vector<int> availableTypes(10);

std::string getMediumFormattedTime() {
    if (!timerRunning || isPaused) return "02:00";  // Return default time if timer is not running
    
    Uint32 currentTime = SDL_GetTicks();
    Uint32 elapsed = currentTime - timerStartTime - totalPaused;
    remaining_time = (TIMER_DURATION>elapsed) ? (TIMER_DURATION-elapsed) : 0;
    // If the elapsed time exceeds 120 seconds (02:00), stop the timer
    if (elapsed >= 120000) {
        elapsed = 120000;  
        timerRunning = false;
        // Add game over logic here if needed 
        /*if(GameOverOpen){
            SDL_Event e;
            while(SDL_PollEvent(&e)){
                handleGameOverEvents(e, GameOverOpen);
            }
            renderGameOver();
            SDL_Delay(16);    
        }*/
    }
    
    remaining = 120000-elapsed;
    int minutes = remaining / 60000;
    int seconds = (remaining % 60000) / 1000;

    char timeStr[9];
    snprintf(timeStr, sizeof(timeStr), "%02d:%02d", minutes, seconds);

    return std::string(timeStr);
}



void loadMediumFishAssets(){

    fishTextures.resize(12);     
    rippleTextures.resize(4);

    const char* fishPaths[12] = {
        "png/brown.png", "png/emerald.png", "png/green.png",
        "png/lavender.png", "png/olive.png", "png/orange.png", "png/purple.png",
        "png/red.png", "png/silver.png", "png/teal.png", "png/piranha.png", "png/golden.png"
    };

    SDL_Surface* surf;

    for(int i = 0; i<12; ++i){
        surf = IMG_Load(fishPaths[i]);
        fishTextures[i] = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);
    
    }

    surf = IMG_Load("png/heart.png");
    heartTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/fisherman.png");
    boatTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
    SDL_FreeSurface(surf);

}

void loadMediumObjectiveAssets(int type, int index)
{
    objectiveTextures.resize(5);
    const char* fishPaths[12] = {
        "png/brown.png", "png/emerald.png", "png/green.png",
        "png/lavender.png", "png/olive.png", "png/orange.png", "png/purple.png",
        "png/red.png", "png/silver.png", "png/teal.png", "png/piranha.png", "png/golden.png"
    };

    SDL_Surface* surf = nullptr;

    surf = IMG_Load(fishPaths[type]);

    if (surf != nullptr) {
        objectiveTextures[index] = SDL_CreateTextureFromSurface(objectiveRenderer, surf);
        SDL_FreeSurface(surf);
    }

}

void spawnMediumFish() {
    if (isPaused) return;

    const char* fishPaths[12] = {
        "png/brown.png", "png/emerald.png", "png/green.png",
        "png/lavender.png", "png/olive.png", "png/orange.png", "png/purple.png",
        "png/red.png", "png/silver.png", "png/teal.png", "png/piranha.png", "png/golden.png"
    };

    for (int i = 0; i < 7; ++i) {
        if (!fishes[i].active && rand() % 150 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;

            // Randomly assign type, ensuring special fish types (golden and piranha) are handled separately
            int type = rand() % 7;  // Normal fish types (0-4)
            if (rand() % 100 < 10) {  // 10% chance for golden fish
                type = 11;  // Golden fish type (last one in fishPaths)
            } else if (rand() % 100 < 20) {  // 20% chance for piranha fish
                type = 10;  // Piranha fish type (second last in fishPaths)
            }

            // Assign fish properties
            fishes[i].arcHeight = rand() % 60 + 70;
            fishes[i].rect.x = fishes[i].baseX;
            fishes[i].rect.y = fishes[i].baseY;
            fishes[i].t = 0;
            fishes[i].goingUp = true;
            fishes[i].active = true;
            fishes[i].rippleActive = false;
            fishes[i].rippleFrame = 0;
            fishes[i].direction = direction;
            fishes[i].type = type;
            fishes[i].clicked = false;
        }
    }

    // Handle objective fishes (for the next group)
    for (int i = 7; i < MAX_FISH; ++i) {
        if (!fishes[i].active && rand() % 150 == 0) {
            fishes[i].baseX = rand() % (1240 - 40 + 1) + 40;
            fishes[i].baseY = rand() % (720 - 400) + 400;
            int direction = (rand() % 2 == 0) ? 1 : -1;
            int type = i;  // Objective fish types (based on availableTypes, adjust logic here)

            fishes[i].arcHeight = rand() % 60 + 70;
            fishes[i].rect.x = fishes[i].baseX;
            fishes[i].rect.y = fishes[i].baseY;
            fishes[i].t = 0;
            fishes[i].goingUp = true;
            fishes[i].active = true;
            fishes[i].rippleActive = false;
            fishes[i].rippleFrame = 0;
            fishes[i].direction = direction;
            fishes[i].type = type;
            fishes[i].clicked = false;
        }
    }
    
}


void updateMediumFishMotion() {
    if (isPaused) return;

    for (int i = 0; i < MAX_FISH; ++i) {
        if (!fishes[i].active) continue;

        float angle = fishes[i].t;
        float radius = fishes[i].arcHeight;

        float x = fishes[i].baseX + fishes[i].direction * radius * cos(angle);
        float y = fishes[i].baseY - radius * sin(angle);

        fishes[i].rect = {static_cast<int>(x), static_cast<int>(y), 80, 80};
        fishes[i].t += 0.060f;

        if (fishes[i].t >= PI) {
            fishes[i].active = false;
            fishes[i].rippleActive = true;
            fishes[i].rippleFrame = 0;
        }

        if (fishes[i].rippleActive) {
            fishes[i].rippleFrame++;
            if (fishes[i].rippleFrame > 10) {
                fishes[i].rippleActive = false;
            }
        }
    }
}

void renderMediumFishAndRipples() {
    for (int i = 0; i < MAX_FISH; ++i) {
        if (fishes[i].active) {
            SDL_RendererFlip flip = (fishes[i].direction == -1) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            SDL_RenderCopyEx(interfaceRenderer, fishTextures[fishes[i].type], NULL, &fishes[i].rect, 0, NULL, flip);
        }

        if (fishes[i].rippleActive) {
            int frame = fishes[i].rippleFrame / 3;
            if (frame < 4) {
                SDL_Rect rippleRect = { fishes[i].baseX + 20, fishes[i].baseY + 10, 80, 80 };
                SDL_RenderCopy(interfaceRenderer, rippleTextures[frame], NULL, &rippleRect);
            }
        }
    }
}

void handleMediumFishClick(int x, int y) {
    for (int i = 0; i < MAX_FISH; i++) {
        if (fishes[i].active && !fishes[i].clicked &&
            x >= fishes[i].rect.x && x <= fishes[i].rect.x + fishes[i].rect.w &&
            y >= fishes[i].rect.y && y <= fishes[i].rect.y + fishes[i].rect.h) {

            if (fishes[i].type == 11) {  // Golden fish type (assuming 11 represents golden fish)
                fishScore += 15;  // Add 15 points for golden fish
                fishes[i].clicked = true;
                rendermediumFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                floatingTexts.back().position = {
                    fishes[i].rect.x + fishes[i].rect.w / 2,
                    fishes[i].rect.y - 20};
                if (soundOn) Mix_PlayChannel(-1, bonuscatch, 0);
                break;
            }

            else if (fishes[i].type == 10) {  // Piranha fish type (assuming 10 represents piranha fish)
                lives--;  // Decrease life by 1
                fishes[i].clicked = true;
                rendermediumFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                floatingTexts.back().position = {
                    fishes[i].rect.x + fishes[i].rect.w / 2,
                    fishes[i].rect.y - 20};

                if (lives == 0) {                   
                    initGameOver();
                    /*if (GameOverOpen) {
                        SDL_Event e;
                        while (SDL_PollEvent(&e)) {
                            handleGameOverEvents(e, GameOverOpen);
                        }
                        renderGameOver();
                        SDL_Delay(16);
                    }*/
                }
                break;
                
            }
            // Handle regular and objective fishes
            else {
                for (int j = 0; j < 5 && !fishes[i].clicked; j++) {
                    if (fishes[i].type == objectiveFishes[j].type) {
                        if(objectiveFishes[j].count>0){
                            fishScore += 2;  // Increment score for objective fish
                            rendermediumFadedText(fishes[i].type, SDL_GetTicks(), objectiveFishes[j].type, objectiveFishes[j].count);
                            floatingTexts.back().position = {
                                fishes[i].rect.x + fishes[i].rect.w / 2,
                                fishes[i].rect.y - 20};
                            if (soundOn) Mix_PlayChannel(-1, rightfish, 0);
                            if (targetScore == 0 && congratsStartTime == 0) {
                                congratsStartTime = SDL_GetTicks();
                            }
                            
                        }
                        else{
                            fishScore++;
                            rendermediumFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                            floatingTexts.back().position = {
                                fishes[i].rect.x + fishes[i].rect.w / 2,
                                fishes[i].rect.y - 20};
                            if (soundOn) Mix_PlayChannel(-1, rightfish, 0);
                            
                        }
                        if (targetScore > 0 && objectiveFishes[j].count>0) {
                            targetScore--;  // Decrease target score
                        }
                        if(objectiveFishes[j].count>0){
                            objectiveFishes[j].count--;  // Decrease the objective fish count
                        }
                        fishes[i].clicked = true;
                        
                    }
                }

                // If it's not an objective fish, it's a regular fish
                if (!fishes[i].clicked) {
                    if(fishScore>=0){
                        rendermediumFadedText(fishes[i].type, SDL_GetTicks(), -1, -2);
                        floatingTexts.back().position = {
                            fishes[i].rect.x + fishes[i].rect.w / 2,
                            fishes[i].rect.y - 20};
                        if(fishScore>0) fishScore--;
                        if (soundOn) Mix_PlayChannel(-1, wrongfish, 0);
                    
                    }
                    else if(fishScore>0 && targetScore==0){
                        rendermediumFadedText(fishes[i].type, SDL_GetTicks(), -1, -1);
                        floatingTexts.back().position = {
                            fishes[i].rect.x + fishes[i].rect.w / 2,
                            fishes[i].rect.y - 20};
                        fishScore++;
                        if (soundOn) Mix_PlayChannel(-1, rightfish, 0);
                    
                    }
                    fishes[i].clicked = true;
                }
            }
        }
    }
}


void initMediumInterface(){

    if (interfaceWindow || interfaceRenderer) {
        destroyMediumInterface();
    }
    if (interfaceWindow != nullptr) return;

    game_music = Mix_LoadMUS("music/game_music.mp3");
    bonuscatch = Mix_LoadWAV("music/bonuscatch.wav");
    rightfish = Mix_LoadWAV("music/rightfish.wav");
    wrongfish = Mix_LoadWAV("music/wrongfish.wav");

    interfaceWindow = SDL_CreateWindow("Medium Mode", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        1280, 720, 
        SDL_WINDOW_BORDERLESS);
    interfaceRenderer = SDL_CreateRenderer(interfaceWindow ,-1, 
        SDL_RENDERER_ACCELERATED);

    if (!interfaceWindow) {
        printf("Failed to create interface window: %s\n", SDL_GetError());
    }

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    smalltitleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 64);
    messageFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 32);

    SDL_Surface* surf;
    if(sunnyOn){
        surf = SDL_LoadBMP("bmp/pond.bmp");
        pondTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        surf = SDL_LoadBMP("bmp/pond2.bmp");
        pond2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        surf = SDL_LoadBMP("bmp/mountain.bmp");
        mountainTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        surf = SDL_LoadBMP("bmp/mountain2.bmp");
        mountain2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);
    }
    else{
        surf = SDL_LoadBMP("bmp/rainymountain.bmp");
        mountainTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        surf = SDL_LoadBMP("bmp/rainypond.bmp");
        pondTexture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        surf = SDL_LoadBMP("bmp/rainypond2.bmp");
        pond2Texture = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_FreeSurface(surf);

        initRain(500);
    }
    
    loadMediumFishAssets();
    initMediumObjective();

}

void initMediumObjective() {
    if (objectiveWindow != nullptr) return;

    objectiveWindow = SDL_CreateWindow("Objective", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    objectiveRenderer = SDL_CreateRenderer(objectiveWindow, -1, SDL_RENDERER_ACCELERATED);

    if (!objectivesInitialized) {
        for (int i = 0; i < 10; ++i) {
            availableTypes[i] = i;
        }
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(availableTypes.begin(), availableTypes.end(), std::default_random_engine(seed));
        for (int i = 0; i < 5; ++i) {
            objectiveFishes[i].type = availableTypes[i];
            objectiveFishes[i].count = rand() % 6 + 4;
            targetScore += objectiveFishes[i].count;
            loadMediumObjectiveAssets(objectiveFishes[i].type, i);
        }

        objectivesInitialized = true;
    }
}

/*
void mediuminitgameWin()
{
    if (mediumGameWinWindow != nullptr) return;

    mediumGameWinWindow = SDL_CreateWindow("Winner!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 480, SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
    gamewinRenderer = SDL_CreateRenderer(mediumGameWinWindow, -1, SDL_RENDERER_ACCELERATED);

    typeFont = TTF_OpenFont ("fonts/Arial.ttf", 24);
    SDL_StartTextInput();
}

void mediumrenderGameWin() {
    if (!gamewinRenderer) return;

    SDL_SetRenderDrawColor(gamewinRenderer, 20, 20, 40, 255);
    SDL_RenderClear(gamewinRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    renderText(gamewinRenderer, smalltitleFont, "Congratulations!", white, 400, 80);

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    mediumconf = (mediumfinalText == "0") ? "Exit" : "Confirm";
    Button confirmBtn = {confirmButton, mediumconf, false};
    confirmBtn.hovered = SDL_PointInRect(&mousePoint, &confirmBtn.rect);

    drawParallelogram(gamewinRenderer, confirmBtn, confirmBtn.hovered);
    renderText(gamewinRenderer, buttonFont, confirmBtn.text, white, confirmBtn.rect.x + confirmBtn.rect.w / 2, confirmBtn.rect.y + confirmBtn.rect.h / 2);

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor) {
        int radius = 5;
        roundedBoxRGBA(gamewinRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 100);
        renderText(gamewinRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };
    SDL_Color faded = {255, 255, 255, 255};
    drawRoundedButton(inputBox, "", faded);

    if (SDL_GetTicks() - lastCursorToggle > 500) {
        showCursor = !showCursor;
        lastCursorToggle = SDL_GetTicks();
    }

    renderText(gamewinRenderer, textFont, "Enter your name: ", white, 400, 150);

    // --- Centered and Scrolling Text ---
    std::string displayText = mediumuserInput;
    if (inputActive && showCursor) {
        displayText += "|";
    }

    // Measure full text width
    int textWidth = 0, textHeight = 0;
    TTF_SizeText(typeFont, displayText.c_str(), &textWidth, &textHeight);

    // Scroll if text is wider than box
    int maxVisibleWidth = inputBox.w - 20;
    std::string visibleText = displayText;
    while (!visibleText.empty()) {
        TTF_SizeText(typeFont, visibleText.c_str(), &textWidth, nullptr);
        if (textWidth <= maxVisibleWidth) break;
        visibleText.erase(0, 1);  // Scroll left
    }

    // Center visible text inside inputBox
    TTF_SizeText(typeFont, visibleText.c_str(), &textWidth, &textHeight);
    int textX = inputBox.x + inputBox.w / 2;
    int textY = inputBox.y + inputBox.h / 2;

    // Render the user input
    renderText(gamewinRenderer, typeFont, visibleText, white, textX, textY);

    // Success message
    if (mediumfinalText == "0") {
        renderText(gamewinRenderer, messageFont, "Entry Successful!", white, 400, 300);
    }
    else if (mediumfinalText == "18") {
        renderText(gamewinRenderer, messageFont, "Invalid name: must not exceed 18 characters.", white, 400, 300);
    }
    else if (mediumfinalText == "-1") {
        renderText(gamewinRenderer, messageFont, "Invalid name: only A-Z, a-z, 0-9, and", white, 400, 300);
        renderText(gamewinRenderer, messageFont, "underscores (_) allowed. No spaces!", white, 400, 350);
    }

    SDL_RenderPresent(gamewinRenderer);
}
    */

void renderMediumInterface() {
    if (!interfaceRenderer || isPaused) return;
    SDL_RenderCopy(interfaceRenderer, pondTexture, NULL, &pond);
    SDL_RenderCopy(interfaceRenderer, pond2Texture, NULL, &pond2);
    SDL_RenderCopy(interfaceRenderer, mountainTexture, NULL, &mountain);
    SDL_RenderCopy(interfaceRenderer, boatTexture, NULL, &boat);

    if(sunnyOn){
        SDL_RenderCopy(interfaceRenderer, mountain2Texture, NULL, &mountain2);
    }
    else{
        renderRain();
    }

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    auto drawRoundedButton = [&](SDL_Rect rect, const std::string& text, SDL_Color fillColor) {
        int radius = 5;
        roundedBoxRGBA(interfaceRenderer,
                       rect.x, rect.y,
                       rect.x + rect.w, rect.y + rect.h,
                       radius,
                       fillColor.r, fillColor.g, fillColor.b, 100);
        renderText(interfaceRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
    };

    SDL_Rect infoBox = {10, 10, 400, 220};
    drawRoundedButton(infoBox, "", white);

    // Timer
    std::string timerText = "Time: " + getMediumFormattedTime();
    SDL_Surface* textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    SDL_Texture* timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    SDL_Rect timerRect = {45, 15, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);

    timerText = "Lives: ";
    textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    timerRect = {45, 45, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);

    for (int i=0; i<lives; ++i)
    {
        int x = 130 + i*30;
        SDL_Rect liveRect = {x+5, 52, 30, 30};
        SDL_RenderCopy (interfaceRenderer, heartTexture, NULL, &liveRect);
    }

    timerText = "Score: " + std::to_string(fishScore) + ", Rem: " + std::to_string(targetScore);
    textSurface = TTF_RenderText_Solid(textFont, timerText.c_str(), black);
    timerTexture = SDL_CreateTextureFromSurface(interfaceRenderer, textSurface);
    timerRect = {45, 75, textSurface->w, textSurface->h};
    SDL_RenderCopy(interfaceRenderer, timerTexture, NULL, &timerRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(timerTexture);


    for (int i = 0; i < 5; ++i) {
        int col = (i < 3) ? 0 : 1;
        int row = (i < 3) ? i: i-3;

        int centerX = (col == 0) ? 45 : 215;
        int centerY = 110 + row * 33;

        SDL_Rect fishRect = {centerX, centerY, 60, 60};
        SDL_RenderCopy(interfaceRenderer, fishTextures[objectiveFishes[i].type], NULL, &fishRect);

        std::string countText = "x " + std::to_string(objectiveFishes[i].count);
        renderText(interfaceRenderer, buttonFont, countText, black, centerX + 90, centerY + 25);
    }

    renderMediumFishAndRipples();

    if (targetScore == 0 && !congratulationsFlag) {
        Uint32 now = SDL_GetTicks();
        if (congratsStartTime == 0) {
            congratsStartTime = now;
        }
        Uint32 elapsed = now - congratsStartTime;
        float progress = elapsed / 3000.0f;

        if (progress >= 1.0f) {
            congratulationsFlag = true;
        } else {
            SDL_Color color = {0, 0, 0, 255};
            SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(
                textFont,
                "      OBJECTIVES COMPLETED\nCatch as much as you can!!",
                color,
                0
            );
            SDL_Texture* texture = SDL_CreateTextureFromSurface(interfaceRenderer, surface);
            int textW = surface->w;
            int textH = surface->h;
            SDL_FreeSurface(surface);

            int windowW, windowH;
            SDL_GetRendererOutputSize(interfaceRenderer, &windowW, &windowH);
            float startY = windowH / 2.0f;
            float endY = windowH / 2.0f - 100;
            float yPos = startY + (endY - startY) * progress;

            Uint8 alpha = 255;
            if (elapsed < 500) {
                alpha = (Uint8)(255.0f * (elapsed / 500.0f));
            } else if (elapsed > 2500) {
                alpha = (Uint8)(255.0f * (1.0f - ((elapsed - 2500.0f) / 500.0f)));
            }
            SDL_SetTextureAlphaMod(texture, alpha);

            SDL_Rect dstRect = {
                (windowW - textW) / 2,
                (int)yPos,
                textW,
                textH
            };

            SDL_RenderCopy(interfaceRenderer, texture, NULL, &dstRect);
            SDL_DestroyTexture(texture);
        }
    }

    renderFaded();

    /*if (gamewinOpen && targetScore == 0) {
        mediumrenderGameWin();
    }*/
    if (gameoverOpen)
    {
        renderGameOver();
    }

    SDL_RenderPresent(interfaceRenderer);
    if (!objectiveClose) renderMediumObjective();
}

void renderMediumObjective() {
    if (!objectiveRenderer) return;

    SDL_SetRenderDrawColor(objectiveRenderer, 255, 255, 255, 255);
    SDL_RenderClear(objectiveRenderer);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    renderText(objectiveRenderer, titleFont, "Objectives", black, 400, 80);

    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    SDL_Rect backBtnRect = {300, 380, 200, 60};
    Button backBtn = {backBtnRect, "Confirm", false};
    backBtn.hovered = SDL_PointInRect(&mousePoint, &backBtn.rect);

    drawParallelogram(objectiveRenderer, backBtn, backBtn.hovered);
    renderText(objectiveRenderer, buttonFont, backBtn.text, white, backBtn.rect.x + backBtn.rect.w / 2, backBtn.rect.y + backBtn.rect.h / 2);
    
    for (int i = 0; i < 5; ++i) {
        int col = (i < 3) ? 0 : 1;
        int row = (i < 3) ? i : i-3;

        int centerX = (col == 0) ? 200 : 600;
        int centerY = 160 + row * 90;

        SDL_Rect fishRect = {centerX - 60, centerY - 30, 60, 60};
        SDL_RenderCopy(objectiveRenderer, objectiveTextures[i], NULL, &fishRect);

        std::string countText = "x " + std::to_string(objectiveFishes[i].count);
        renderText(objectiveRenderer, buttonFont, countText, black, centerX + 30, centerY-5);
    }
    SDL_RenderPresent(objectiveRenderer);
}

void handleMediumInterfaceEvents(SDL_Event& e, bool& interfaceOpen) {
    if (!interfaceWindow) return;

    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        // Initialize pause window and set the flag to true
        if (!isPaused) {
            initPauseMenu();  // Initialize pause window
            pauseStartTime = SDL_GetTicks(); 
            isPaused = true;
        }
    }
    
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = e.button.x;
        int mouseY = e.button.y;
        handleMediumFishClick(mouseX, mouseY);
        
        SDL_Rect backBtnRect = {300, 380, 200, 60};
        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePoint = {mx, my};
        
        if (SDL_PointInRect(&mousePoint, &backBtnRect) && !timerStarted) {
            SDL_DestroyRenderer(objectiveRenderer);
            SDL_DestroyWindow(objectiveWindow);
            objectiveClose = true;
            
            // Start the timer when confirm is clicked
            timerStartTime = SDL_GetTicks();
            timerStarted = true;
            timerRunning = true;
        }
    }
    /*
    if (gamewinOpen) {
        if (e.type == SDL_TEXTINPUT && inputActive) {
            mediumuserInput += e.text.text;
        }

        if (e.type == SDL_KEYDOWN && inputActive) {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !mediumuserInput.empty()) {
                mediumuserInput.pop_back();
            }
        }
    }*/

    if (isPaused)
    {
        renderPauseMenu();
        handlePauseMenuEvents(e, isPaused);
    }
    if (gameoverOpen)
    {
        handleGameOverEvents(e, gameoverOpen);
    }
}

void handleMediumInterfaceLogics(SDL_Event& e, bool& interfaceWindow){
    if (!interfaceWindow || isPaused) return;
    
    spawnMediumFish();
    updateMediumFishMotion();
    
    pond.x += 1;
    pond2.x += 1;

    if (pond.x > 1279) pond.x = -1279;
    if (pond2.x > 1279) pond2.x = -1279;

    if(!sunnyOn){
        updateRain();
    }

    if(remaining==0){
        initGameOver();
    }
    
}


void destroyMediumInterface() {
    
    // Reset all static positions
    pond = {0, 250, 1280, 470};
    pond2 = {-1279, 250, 1280, 470};
    mountain = {0, 0, 1280, 250};
    mountain2 = {-1279, 0, 1280, 250};

    if (objectiveRenderer) {
        SDL_DestroyRenderer(objectiveRenderer);
        objectiveRenderer = nullptr;
    }
    if (objectiveWindow) {
        SDL_DestroyWindow(objectiveWindow);
        objectiveWindow = nullptr;
    }
    objectiveClose = false;  // Add this
    // Destroy textures
    if (pondTexture) {
        SDL_DestroyTexture(pondTexture);
        pondTexture = nullptr;
    }
    if (pond2Texture) {
        SDL_DestroyTexture(pond2Texture);
        pond2Texture = nullptr;
    }
    if (mountainTexture) {
        SDL_DestroyTexture(mountainTexture);
        mountainTexture = nullptr;
    }
    if(mountain2Texture){
        SDL_DestroyTexture(mountain2Texture);
    }

    destroyRain();

    if (heartTexture) {
        SDL_DestroyTexture(heartTexture);
        heartTexture = nullptr;
    }

    // Free sound/music if necessary
    if (soundOn)
    {
        Mix_PauseMusic();
        Mix_PlayMusic(intro, -1);
    }
    else
        Mix_PauseMusic();

    for (auto& tex : fishTextures) {
        if (tex) SDL_DestroyTexture(tex);
    }
    fishTextures.clear();

    for (auto& tex : rippleTextures) {
        if (tex) SDL_DestroyTexture(tex);
    }   
    rippleTextures.clear();

    for (auto& tex : objectiveTextures) {
        if (tex) SDL_DestroyTexture(tex);
    }
    objectiveTextures.clear();

    // Add this after your current objective reset
    for (int i = 0; i < 5; ++i) {
        objectiveFishes[i].type = 0;
        objectiveFishes[i].count = 0;
    }
    availableTypes.clear();
    availableTypes.resize(10);

    // Reset game state
    fishScore = 0;
    lives = 3;
    mediumuserInput = "";
    mediumfinalText = "";
    mediumconf = "";
    inputActive = false;
    showCursor = true;
    gamewinOpen = false;
    lastCursorToggle = 0;
    remaining = 120000;
    remaining_time = 120000;
    totalPaused = 0;
    targetScore = 0;
    timerRunning = false;
    timerStarted = false;
    timerStartTime = 0;
    congratulationsFlag = false;
    congratsStartTime = 0;
    isPaused = false;
    objectivesInitialized = false;
    medium_game_music = nullptr;

    for (int i = 0; i < MAX_FISH; ++i) {
        fishes[i] = PondFish(); // Full struct reset
        fishes[i].active = false;
        fishes[i].clicked = false;
        fishes[i].rippleActive = false;
        fishes[i].rippleFrame = 0;
    }

    /*
    for (int i = 0; i < 5; ++i) {
        objectiveFishes[i].count = rand() % 6 + 4;  // Randomize new counts for objectives
    }*/

    // Set the flags back to false
    titleFont = nullptr, buttonFont = nullptr, textFont = nullptr, messageFont = nullptr;
    smalltitleFont = nullptr, typeFont = nullptr;

    if (interfaceRenderer) {
        SDL_DestroyRenderer(interfaceRenderer);
        interfaceRenderer = nullptr;
    }
    if (interfaceWindow) {
        SDL_DestroyWindow(interfaceWindow);
        interfaceWindow = nullptr;
    }
    MediuminterfaceOpen = false;

}


bool isMediumInterfaceOpen() {
    return interfaceWindow != nullptr;
}
