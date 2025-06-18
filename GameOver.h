#ifndef GAMEOVER_H
#define GAMEOVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

struct HighScoreEntry {
    std::string name;
    int score;
};

enum GameOverReason {
    OUT_OF_LIVES,
    OUT_OF_TIME,
    LOW_SCORE,
    HIGH_SCORE
};

struct GameOverState {
    GameOverReason reason;
    int score;
    std::string playerName;
    bool nameEntered;
    bool active;
};

void initGameOver(GameOverState& state, int score, int lives, int timeRemaining, bool objectivesCompleted, const std::vector<HighScoreEntry>& highScores);

void handleGameOverInput(SDL_Event& e, GameOverState& state, std::vector<HighScoreEntry>& highScores);
void renderGameOverScreen(SDL_Renderer* renderer, TTF_Font* font, const GameOverState& state);

#endif