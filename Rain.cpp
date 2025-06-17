#include "Common.h"
#include "HardInterface.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <algorithm>
#include <string>

struct Raindrop
{
    float x, y;
    float length;
    float speed;
    float tilt;
};

struct Splash
{
    float x, y;
    int life;
    float radius;
};

struct Lightning
{
    std::vector<SDL_Point> points;
    int life;
};

bool thunderActive = false;
int thunderDuration = 0;
int thunderCooldown = 0;
std::vector<Raindrop> raindrops;
std::vector<Splash> splashes;
std::vector<Lightning> lightnings;

void initRain(int count)
{
    raindrops.clear();
    for (int i = 0; i < count; ++i)
    {
        Raindrop drop;
        drop.x = rand() % 1280;
        drop.y = rand() % 720;
        drop.length = 10.0f + rand() % 10;
        drop.speed = 10.0f + rand() % 15;
        drop.tilt = -5.0f;
        raindrops.push_back(drop);
    }
}

void generateLightning()
{
    Lightning bolt;
    int x = rand() % 720;
    int y = 0;
    bolt.points.push_back({x, y});
    for (int i = 0; i < 10; ++i)
    {
        x += (rand() % 21) - 10; // slight horizontal zigzag
        y += 1280 / 10;
        if (x < 0)
            x = 0;
        if (x > 1280)
            x = 1280;
        bolt.points.push_back({x, y});
    }
    bolt.life = 5; // display for 5 frames
    lightnings.push_back(bolt);
}

void updateRain()
{
    for (auto &drop : raindrops)
    {
        drop.y += drop.speed;
        drop.x += 0.2f * drop.tilt;
        if (drop.y > 1280)
        {
            drop.y = -drop.length;
            drop.x = rand() % 1280;
            float y = 250 + rand() % 470;
            splashes.push_back({drop.x, y, 15, 1.0f});
        }
    }

    for (size_t i = 0; i < splashes.size();)
    {
        splashes[i].life--;
        splashes[i].radius += 0.3f;
        if (splashes[i].life <= 0)
        {
            splashes.erase(splashes.begin() + i);
        }
        else
        {
            ++i;
        }
    }

    for (size_t i = 0; i < lightnings.size();)
    {
        lightnings[i].life--;
        if (lightnings[i].life <= 0)
        {
            lightnings.erase(lightnings.begin() + i);
        }
        else
        {
            ++i;
        }
    }

    if (!thunderActive && thunderCooldown <= 0 && (rand() % 1000) < 5)
    {
        thunderActive = true;
        thunderDuration = 6;
        thunderCooldown = 300 + rand() % 200;
        generateLightning();
    }
    else if (thunderActive)
    {
        thunderDuration--;
        if (thunderDuration <= 0)
        {
            thunderActive = false;
        }
    }
    else
    {
        thunderCooldown--;
    }
}

void renderRain()
{
    if (thunderActive)
    {
        SDL_SetRenderDrawColor(hardInterfaceRenderer, 230, 230, 255, 255);
        SDL_RenderClear(hardInterfaceRenderer);
    }

    SDL_SetRenderDrawColor(hardInterfaceRenderer, 255, 255, 255, 255);
    for (auto &drop : raindrops)
    {
        SDL_RenderDrawLine(hardInterfaceRenderer,
                           static_cast<int>(drop.x),
                           static_cast<int>(drop.y),
                           static_cast<int>(drop.x + drop.tilt),
                           static_cast<int>(drop.y + drop.length));
    }

    // Draw lightning bolts
    SDL_SetRenderDrawColor(hardInterfaceRenderer, 255, 255, 255, 255);
    for (auto &bolt : lightnings)
    {
        for (size_t i = 1; i < bolt.points.size(); ++i)
        {
            SDL_RenderDrawLine(hardInterfaceRenderer,
                               bolt.points[i - 1].x, bolt.points[i - 1].y,
                               bolt.points[i].x, bolt.points[i].y);
        }
    }

    // Draw splashes
    SDL_SetRenderDrawColor(hardInterfaceRenderer, 180, 180, 255, 255);
    for (auto &splash : splashes)
    {
        for (int angle = 0; angle < 360; angle += 30)
        {
            float rad = angle * M_PI / 180.0f;
            int x = static_cast<int>(splash.x + splash.radius * cos(rad));
            int y = static_cast<int>(splash.y + splash.radius * sin(rad));
            SDL_RenderDrawPoint(hardInterfaceRenderer, x, y);
        }
    }
}

void destroyRain()
{
    bool thunderActive = false;
    int thunderDuration = 0;
    int thunderCooldown = 0;
    raindrops.clear();
    splashes.clear();
    lightnings.clear();
}