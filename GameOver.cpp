#include "GameOver.h"
#include <iostream>

void initGameOver(GameOverState& state, int score, int lives, int timeRemaining, bool objectivesCompleted, const std::vector<HighScoreEntry>& highScores) {
    state.score = score;
    state.playerName = "";
    state.nameEntered = false;
    state.active = true;

    if (lives <= 0) {
        state.reason = OUT_OF_LIVES;
    } else if (timeRemaining <= 0 && !objectivesCompleted) {
        state.reason = OUT_OF_TIME;
    } else if (objectivesCompleted) {
        bool qualifies = false;
        for (const auto& entry : highScores) {
            if (score > entry.score) {
                qualifies = true;
                break;
            }
        }
        if (qualifies || highScores.size() < 5) {
            state.reason = HIGH_SCORE;
        } else {
            state.reason = LOW_SCORE;
        }
    }
}

void handleGameOverInput(SDL_Event& e, GameOverState& state, std::vector<HighScoreEntry>& highScores) {
    if (!state.active) return;

    if (state.reason == HIGH_SCORE) {
        if (e.type == SDL_TEXTINPUT && state.playerName.length() < 18) {
            state.playerName += e.text.text;
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE && !state.playerName.empty()) {
            state.playerName.pop_back();
        } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN && !state.nameEntered) {
            highScores.push_back({state.playerName, state.score});
            state.nameEntered = true;
            state.active = false;  // optionally deactivate screen
        }
    } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
        state.active = false;
    }
}

void renderGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, const GameOverState& state) {
    if (!state.active) return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Color red = {255, 0, 0, 255};

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    std::string message;
    switch (state.reason) {
        case OUT_OF_LIVES: message = "Oops! Your Lives Are Over"; break;
        case OUT_OF_TIME: message = "Oops! Your Time Has Run Out"; break;
        case LOW_SCORE: message = "Uh Oh! You Couldn't Make It To The High Scores"; break;
        case HIGH_SCORE: message = "Congratulations! You've Entered The Top 5"; break;
    }

    SDL_Surface* msgSurface = TTF_RenderText_Solid(font, message.c_str(), white);
    SDL_Texture* msgTexture = SDL_CreateTextureFromSurface(renderer, msgSurface);
    SDL_Rect msgRect = {100, 100, msgSurface->w, msgSurface->h};
    SDL_RenderCopy(renderer, msgTexture, NULL, &msgRect);
    SDL_FreeSurface(msgSurface);
    SDL_DestroyTexture(msgTexture);

    std::string scoreText = "Your Score is : " + std::to_string(state.score);
    SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText.c_str(), yellow);
    SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {100, 150, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    if (state.reason == HIGH_SCORE && !state.nameEntered) {
        SDL_Surface* namePrompt = TTF_RenderText_Solid(font, "Enter Your Name:", white);
        SDL_Texture* nameTexture = SDL_CreateTextureFromSurface(renderer, namePrompt);
        SDL_Rect nameRect = {100, 220, namePrompt->w, namePrompt->h};
        SDL_RenderCopy(renderer, nameTexture, NULL, &nameRect);
        SDL_FreeSurface(namePrompt);
        SDL_DestroyTexture(nameTexture);

        std::string typed = state.playerName + "|";
        SDL_Surface* inputSurface = TTF_RenderText_Solid(font, typed.c_str(), red);
        SDL_Texture* inputTexture = SDL_CreateTextureFromSurface(renderer, inputSurface);
        SDL_Rect inputRect = {100, 260, inputSurface->w, inputSurface->h};
        SDL_RenderCopy(renderer, inputTexture, NULL, &inputRect);
        SDL_FreeSurface(inputSurface);
        SDL_DestroyTexture(inputTexture);
    }

    SDL_RenderPresent(renderer);
}