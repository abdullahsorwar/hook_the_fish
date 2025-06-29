#include "Common.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>

SDL_Texture* fishRightClosedTexture = nullptr;
SDL_Texture* fishRightOpenTexture = nullptr;
SDL_Texture* fishLeftClosedTexture = nullptr;
SDL_Texture* fishLeftOpenTexture = nullptr;
SDL_Texture* cloudTexture = nullptr;
SDL_Texture* hookleftTexture = nullptr;
SDL_Texture* hookrightTexture = nullptr;
SDL_Texture* copyright = nullptr;
SDL_Texture* rodTexture = nullptr;
SDL_Window *interfaceWindow = nullptr;
SDL_Renderer *interfaceRenderer = nullptr;
ObjectiveFish objectiveFishes[6] = {0};
std::vector<FloatingText> floatingTexts;
int fishScore = 0;
int targetScore = 0;
Uint32 remaining = 120000;
int lives = 3;
bool crocodiletouch = false;


Mix_Music* intro = nullptr;
Mix_Music* game_music = nullptr;
Mix_Chunk* rightfish = nullptr;
Mix_Chunk* wrongfish = nullptr;
Mix_Chunk* bonuscatch = nullptr;
Mix_Chunk* crocodile = nullptr;

void renderFadedText(int type, Uint32 init_time, int obj_type, int obj_count)
{
    FloatingText text;
    if(hardinterfaceOpen){
            if (type == 0)
            {
                text.text = "X";
                text.color = {255, 0, 0, 255};
            }
            else if (type == 1)
            {
                text.text = "+10";
                text.color = {0, 255, 0, 255};
            }
            else if (type == obj_type && obj_count > 0)
            {
                text.text = "+1";
                text.color = {0, 255, 0, 255};
            }
            else if (type == obj_type && obj_count == 0)
            {
                text.text = "-1";
                text.color = {255, 0, 0, 255};
            }
            else if (obj_type == -1 && obj_count == -1 && targetScore == 0)
            {
                text.text = "+1";
                text.color = {0, 255, 0, 255};
            }
            else
            {
                text.text = "-1";
                text.color = {255, 0, 0, 255};   
            }
        }else if(EasyinterfaceOpen){
            if (type == 0)
            {
                text.text = "+10";
                text.color = {0, 255, 0, 255};
            }
            else if (type == obj_type && obj_count > 0)
            {
                text.text = "+1";
                text.color = {0, 255, 0, 255};
            }
            else if (type == obj_type && obj_count == 0)
            {
                text.text = "X";
                text.color = {255, 0, 0, 255};
            }
            else if (obj_type == -1 && obj_count == -1 && targetScore == 0)
            {
                text.text = "+1";
                text.color = {0, 255, 0, 255};
            }
            else
            {
                text.text = "X";
                text.color = {255, 0, 0, 255};   
            }
        }        
    // Position will be set to fish position when called
    // So leave default for now
    text.startTime = init_time;
    floatingTexts.push_back(text);
}

void rendermediumFadedText(int type, Uint32 init_time, int obj_type, int obj_count)
{
    FloatingText text;

    if (type == 10)
    {
        text.text = "X";
        text.color = {255, 0, 0, 255};
    }
    else if (type == 11)
    {
        text.text = "+15";
        text.color = {0, 255, 0, 255};
    }
    else if (type == obj_type && obj_count > 0)
    {
        text.text = "+2";
        text.color = {0, 255, 0, 255};
    }
    /*else if (type == obj_type && obj_count == 0)
    {
        text.text = "-1";
        text.color = {255, 0, 0, 255};
    }*/
    else if (obj_type == -1 && obj_count == -1 && targetScore == 0)
    {
        text.text = "+1";
        text.color = {0, 255, 0, 255};
    }
    else
    {
        text.text = "-1";
        text.color = {255, 0, 0, 255};   
    }

    // Position will be set to fish position when called
    // So leave default for now
    text.startTime = init_time;
    floatingTexts.push_back(text);
}

void renderFaded()
{
    TTF_Font* textFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 32);

    Uint32 now = SDL_GetTicks();
    for (int i = 0; i < floatingTexts.size();)
    {
        FloatingText &text = floatingTexts[i];
        float progress = (now - text.startTime) / (float)text.duration;

        if (progress >= 1.0f)
        {
            // Remove expired text
            floatingTexts.erase(floatingTexts.begin() + i);
            continue;
        }
        Uint8 alpha = (Uint8)(255 * (1.0f - progress));

        SDL_Color renderColor = text.color;
        renderColor.a = alpha;

        int offsetY = (int)(-30.0f * progress);
        int textX = text.position.x;
        int textY = text.position.y + offsetY;

        // Render centered
        SDL_Surface *surf = TTF_RenderText_Blended(textFont, text.text.c_str(), renderColor);
        SDL_Texture *tex = SDL_CreateTextureFromSurface(interfaceRenderer, surf);
        SDL_Rect dst = {
            textX - surf->w / 2,
            textY - surf->h / 2,
            surf->w,
            surf->h};
        SDL_FreeSurface(surf);
        SDL_SetTextureAlphaMod(tex, alpha);
        SDL_RenderCopy(interfaceRenderer, tex, NULL, &dst);
        SDL_DestroyTexture(tex);

        ++i;
    }
    if (textFont)
    {
        TTF_CloseFont(textFont);
    }
}

void loadTextures(SDL_Renderer* renderer) {
    SDL_Surface* surf;

    surf = SDL_LoadBMP("bmp/fish_right_close.bmp");
    fishRightClosedTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/fish_right_open.bmp");
    fishRightOpenTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/fish_left_close.bmp");
    fishLeftClosedTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/fish_left_open.bmp");
    fishLeftOpenTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/cloud.bmp");
    cloudTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/hook2.bmp");
    hookleftTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    
    surf = SDL_LoadBMP("bmp/hook3.bmp");
    hookrightTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    
    surf = SDL_LoadBMP("bmp/copyright.bmp");
    copyright = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);

    surf = SDL_LoadBMP("bmp/rod.bmp");
    rodTexture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
}

void freeTextures() {
    SDL_DestroyTexture(fishRightClosedTexture);
    SDL_DestroyTexture(fishRightOpenTexture);
    SDL_DestroyTexture(fishLeftClosedTexture);
    SDL_DestroyTexture(fishLeftOpenTexture);
    SDL_DestroyTexture(cloudTexture);
    SDL_DestroyTexture(hookleftTexture);
    SDL_DestroyTexture(hookrightTexture);
    SDL_DestroyTexture(copyright);
    SDL_DestroyTexture(rodTexture);
}

void drawPondBackground(SDL_Renderer* renderer, int width, int height) {
    SDL_Color topColor = {10, 30, 100, 255};
    SDL_Color bottomColor = {80, 180, 255, 255};

    for (int y = 0; y < height; ++y) {
        float t = static_cast<float>(y) / height;
        Uint8 r = static_cast<Uint8>(topColor.r + t * (bottomColor.r - topColor.r));
        Uint8 g = static_cast<Uint8>(topColor.g + t * (bottomColor.g - topColor.g));
        Uint8 b = static_cast<Uint8>(topColor.b + t * (bottomColor.b - topColor.b));

        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }
}

void drawParallelogram(SDL_Renderer* renderer, Button& btn, bool hovered) {
    int offset = 20;
    Sint16 vx[4] = {
        static_cast<Sint16>(btn.rect.x + offset),
        static_cast<Sint16>(btn.rect.x + btn.rect.w + offset),
        static_cast<Sint16>(btn.rect.x + btn.rect.w - offset),
        static_cast<Sint16>(btn.rect.x - offset)
    };
    Sint16 vy[4] = {
        static_cast<Sint16>(btn.rect.y),
        static_cast<Sint16>(btn.rect.y),
        static_cast<Sint16>(btn.rect.y + btn.rect.h),
        static_cast<Sint16>(btn.rect.y + btn.rect.h)
    };

    if (hovered)
        filledPolygonRGBA(renderer, vx, vy, 4, 255, 180, 100, 255);
    else
        filledPolygonRGBA(renderer, vx, vy, 4, 255, 150, 0, 255);
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int x, int y) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstRect = { x - texW / 2, y - texH / 2, texW, texH };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderWrappedText(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, int x, int y, int wrapLength) {
    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, wrapLength);
    if (textSurface) {
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect dst = {x - textSurface->w / 2, y - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_FreeSurface(textSurface);
        SDL_RenderCopy(renderer, textTexture, nullptr, &dst);
        SDL_DestroyTexture(textTexture);
    }
}

Cloud generateRandomCloud() {
    int dir = (rand() % 2) ? 1 : -1;
    int x = (dir == 1) ? -100 : 1280;
    int y = rand() % 150;
    return {{x, y, 100 + rand() % 100, 50 + rand() % 30}, 1 + rand() % 2, dir};
}

Fish generateRandomFish() {
    int dir = (rand() % 2) ? 1 : -1;
    int x = (dir == 1) ? -60 : 1280;
    int y = 300 + rand() % (720 - 300);
    return {{x, y, 50 + rand() % 30, 30 + rand() % 20}, 2 + rand() % 3, dir, y};
}

void updateClouds(std::vector<Cloud>& clouds) {
    for (int i = 0; i < (int)clouds.size(); ++i) {
        clouds[i].rect.x += clouds[i].speed * clouds[i].direction;
        if (clouds[i].rect.x > 1280 || clouds[i].rect.x + clouds[i].rect.w < 0) {
            clouds.erase(clouds.begin() + i);
            clouds.push_back(generateRandomCloud());
        }
    }
}

void updateFishes(std::vector<Fish>& fishes) {
    for (int i = 0; i < (int)fishes.size(); ++i) {
        fishes[i].rect.x += fishes[i].speed * fishes[i].direction;
        fishes[i].rect.y = fishes[i].baseY + static_cast<int>(5 * sin(SDL_GetTicks() * 0.005 + fishes[i].rect.x * 0.01));
        if (fishes[i].rect.x > 1280 || fishes[i].rect.x + fishes[i].rect.w < 0) {
            fishes.erase(fishes.begin() + i);
            fishes.push_back(generateRandomFish());
        }
    }
}

void renderFishesAndClouds(SDL_Renderer* renderer, const std::vector<Cloud>& clouds, const std::vector<Fish>& fishes) {
    for (auto& cloud : clouds) SDL_RenderCopy(renderer, cloudTexture, NULL, &cloud.rect);
    for (auto& fish : fishes) {
        if (fish.direction == 1) {
            SDL_RenderCopy(renderer, fish.hovered ? fishRightOpenTexture : fishRightClosedTexture, NULL, &fish.rect);
        }
        else {
            SDL_RenderCopy(renderer, fish.hovered ? fishLeftOpenTexture : fishLeftClosedTexture, NULL, &fish.rect);
        }
    }
}

bool loadSoundState() {
    FILE* f = fopen("files/sound_settings.dat", "rb");
    if (!f) return true;
    uint8_t state;
    size_t read = fread(&state, sizeof(state), 1, f);
    fclose(f);
    return (read == 1) && (state == 1);
}

bool loadWeatherState() {
    FILE* f = fopen("files/weather_settings.dat", "rb");
    if (!f) return true;
    uint8_t state;
    size_t read = fread(&state, sizeof(state), 1, f);
    fclose(f);
    return (read == 1) && (state == 1);
}

void saveSoundState(bool soundOn) {
    FILE* f = fopen("files/sound_settings.dat", "wb");
    if (!f) return;
    uint8_t state = soundOn ? 1 : 0;
    fwrite(&state, sizeof(state), 1, f);
    fclose(f);
}

void saveWeatherState(bool sunnyOn) {
    FILE* f = fopen("files/weather_settings.dat", "wb");
    if (!f) return;
    uint8_t state = sunnyOn ? 1 : 0;
    fwrite(&state, sizeof(state), 1, f);
    fclose(f);
}
