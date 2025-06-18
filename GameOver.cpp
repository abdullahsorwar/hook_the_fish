#include "Common.h"
#include "GameOver.h"
#include "HighScores.h"
#include "EasyInterface.h"
#include "MediumInterface.h"
#include "HardInterface.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>

SDL_Window *gameOverWindow = nullptr;
static SDL_Renderer *gameOverRenderer = nullptr;
static SDL_Rect inputBox = {250, 200, 300, 50};
static SDL_Rect confirmButton = {300, 380, 200, 60};

static TTF_Font *titleFont = nullptr;
static TTF_Font *smalltitleFont = nullptr;
static TTF_Font *buttonFont = nullptr;
static TTF_Font *textFont = nullptr;
static TTF_Font *typeFont = nullptr;
static TTF_Font *messageFont = nullptr;

std::string interface = "";
std::string userInput = "";
std::string finalText = "";
std::string conf = "";
bool gameoverOpen = false;
static bool inputActive = false;
static bool showCursor = true;
static Uint32 lastCursorToggle = 0;
static bool verdict = false;

void initGameOver()
{
    if (gameOverWindow != nullptr)
        return;

    gameOverWindow = SDL_CreateWindow("Game Over",
                                      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                      800, 480, SDL_WINDOW_BORDERLESS);
    gameOverRenderer = SDL_CreateRenderer(gameOverWindow, -1, SDL_RENDERER_ACCELERATED);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    smalltitleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 64);
    typeFont = TTF_OpenFont("fonts/Arial.ttf", 24);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    textFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 32);
    messageFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 32);

    if (EasyinterfaceOpen)
        interface = "Easy";
    else if (MediuminterfaceOpen)
        interface = "Medium";
    else if (hardinterfaceOpen)
        interface = "Hard";

    SDL_StartTextInput();
}

void renderGameOver()
{
    if (!gameOverRenderer) return;
    std::cout << "point crossed\n";
    SDL_SetRenderDrawColor(gameOverRenderer, 20, 20, 40, 255);
    SDL_RenderClear(gameOverRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};
    std::string text = "files/" + interface + ".txt";
    if (targetScore == 0 && checkHighScore(text, fishScore))
    {
        verdict = true;
        renderText(gameOverRenderer, smalltitleFont, "Congratulations!", white, 400, 80);

        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePoint = {mx, my};

        conf = (finalText == "0") ? "Exit" : "Confirm";
        Button confirmBtn = {confirmButton, conf, false};
        confirmBtn.hovered = SDL_PointInRect(&mousePoint, &confirmBtn.rect);

        drawParallelogram(gameOverRenderer, confirmBtn, confirmBtn.hovered);
        renderText(gameOverRenderer, buttonFont, confirmBtn.text, white, confirmBtn.rect.x + confirmBtn.rect.w / 2, confirmBtn.rect.y + confirmBtn.rect.h / 2);

        auto drawRoundedButton = [&](SDL_Rect rect, const std::string &text, SDL_Color fillColor)
        {
            int radius = 5;
            roundedBoxRGBA(gameOverRenderer,
                           rect.x, rect.y,
                           rect.x + rect.w, rect.y + rect.h,
                           radius,
                           fillColor.r, fillColor.g, fillColor.b, 100);
            renderText(gameOverRenderer, buttonFont, text, black, rect.x + rect.w / 2, rect.y + rect.h / 2);
        };
        SDL_Color faded = {255, 255, 255, 255};
        drawRoundedButton(inputBox, "", faded);

        if (SDL_GetTicks() - lastCursorToggle > 500)
        {
            showCursor = !showCursor;
            lastCursorToggle = SDL_GetTicks();
        }

        renderText(gameOverRenderer, textFont, "Enter your name: ", white, 400, 150);

        // --- Centered and Scrolling Text ---
        std::string displayText = userInput;
        if (inputActive && showCursor)
        {
            displayText += "|";
        }

        // Measure full text width
        int textWidth = 0, textHeight = 0;
        TTF_SizeText(typeFont, displayText.c_str(), &textWidth, &textHeight);

        // Scroll if text is wider than box
        int maxVisibleWidth = inputBox.w - 20;
        std::string visibleText = displayText;
        while (!visibleText.empty())
        {
            TTF_SizeText(typeFont, visibleText.c_str(), &textWidth, nullptr);
            if (textWidth <= maxVisibleWidth)
                break;
            visibleText.erase(0, 1); // Scroll left
        }

        // Center visible text inside inputBox
        TTF_SizeText(typeFont, visibleText.c_str(), &textWidth, &textHeight);
        int textX = inputBox.x + inputBox.w / 2;
        int textY = inputBox.y + inputBox.h / 2;

        // Render the user input
        renderText(gameOverRenderer, typeFont, visibleText, white, textX, textY);

        // Success message
        if (finalText == "0")
        {
            renderText(gameOverRenderer, messageFont, "Entry Successful!", white, 400, 300);
        }
        else if (finalText == "18")
        {
            renderText(gameOverRenderer, messageFont, "Invalid name: must not exceed 18 characters.", white, 400, 300);
        }
        else if (finalText == "-1")
        {
            renderText(gameOverRenderer, messageFont, "Invalid name: Only A-Z, a-z, 0-9, and", white, 400, 300);
            renderText(gameOverRenderer, messageFont, "Underscores (_) allowed. No spaces!", white, 400, 350);
        }
    }
    else
    {
        renderText(gameOverRenderer, titleFont, "Oops!", white, 400, 80);

        int mx, my;
        SDL_GetMouseState(&mx, &my);
        SDL_Point mousePoint = {mx, my};

        conf = "Exit";
        Button confirmBtn = {confirmButton, conf, false};
        confirmBtn.hovered = SDL_PointInRect(&mousePoint, &confirmBtn.rect);

        drawParallelogram(gameOverRenderer, confirmBtn, confirmBtn.hovered);
        renderText(gameOverRenderer, buttonFont, confirmBtn.text, white, confirmBtn.rect.x + confirmBtn.rect.w / 2, confirmBtn.rect.y + confirmBtn.rect.h / 2);

        renderText(gameOverRenderer, textFont, "Under Construction!", white, 400, 150);

    }
    SDL_RenderPresent(gameOverRenderer);
}

void handleGameOverEvents(SDL_Event &e, bool &gameoverOpen)
{
    if (e.type == SDL_MOUSEBUTTONDOWN && e.window.windowID == SDL_GetWindowID(gameOverWindow))
    {
        if (gameoverOpen && verdict)
        {
            int mx = e.button.x;
            int my = e.button.y;
            if (mx >= inputBox.x && mx <= inputBox.x + inputBox.w &&
                my >= inputBox.y && my <= inputBox.y + inputBox.h)
            {
                inputActive = true;
            }
            else
            {
                inputActive = false;
            }

            if (mx >= confirmButton.x && mx <= confirmButton.x + confirmButton.w &&
                my >= confirmButton.y && my <= confirmButton.y + confirmButton.h && conf == "Confirm")
            {
                finalText = checkAndAddHighScore("files/hard.txt", userInput, fishScore);
            }
            SDL_Rect backBtnRect = {300, 380, 200, 60};
            SDL_GetMouseState(&mx, &my);
            SDL_Point mousePoint = {mx, my};
            if (SDL_PointInRect(&mousePoint, &backBtnRect) && conf == "Exit")
            {
                destroyGameOver();
            }
        }
        if (gameoverOpen)
        {
            int mx = e.button.x;
            int my = e.button.y;
            SDL_Rect backBtnRect = {300, 380, 200, 60};
            SDL_GetMouseState(&mx, &my);
            SDL_Point mousePoint = {mx, my};
            if (SDL_PointInRect(&mousePoint, &backBtnRect) && conf == "Exit")
            {
                destroyGameOver();
            }
        }
    }
    if (gameoverOpen)
    {
        if (e.type == SDL_TEXTINPUT && inputActive)
        {
            userInput += e.text.text;
        }

        if (e.type == SDL_KEYDOWN && inputActive)
        {
            if (e.key.keysym.sym == SDLK_BACKSPACE && !userInput.empty())
            {
                userInput.pop_back();
            }
        }
    }
}

void destroyGameOver()
{
    if (titleFont)
    {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (smalltitleFont)
    {
        TTF_CloseFont(smalltitleFont);
        smalltitleFont = nullptr;
    }
    if (typeFont)
    {
        TTF_CloseFont(typeFont);
        typeFont = nullptr;
    }
    if (buttonFont)
    {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (textFont)
    {
        TTF_CloseFont(textFont);
        textFont = nullptr;
    }
    if (messageFont)
    {
        TTF_CloseFont(messageFont);
        messageFont = nullptr;
    }
    userInput = "";
    finalText = "";
    conf = "";
    inputActive = false;
    showCursor = true;
    lastCursorToggle = 0;
    gameoverOpen = false;
    verdict = false;
    interface = "";
    if (gameOverRenderer)
    {
        SDL_DestroyRenderer(gameOverRenderer);
        gameOverRenderer = nullptr;
    }
    if (gameOverWindow)
    {
        SDL_DestroyWindow(gameOverWindow);
        gameOverWindow = nullptr;
    }
    if (EasyinterfaceOpen) destroyEasyInterface();
    else if (MediuminterfaceOpen) destroyMediumInterface();
    else if (hardinterfaceOpen) destroyHardInterface();
}

bool isGameOverOpen()
{
    return gameOverWindow != nullptr;
}