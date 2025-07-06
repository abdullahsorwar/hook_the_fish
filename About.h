#ifndef ABOUT_H
#define ABOUT_H

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <string>

struct About {
    std::string name;
    std::string org;
    std::string git;
    std::string mail;
    std::string designation;
    SDL_Texture* photo;
};

void loadAbout(About& dev);
void initAbout();
void handleAboutEvents(SDL_Event& e, bool& aboutOpen);
void renderAbout();
void destroyAbout();
bool isAboutOpen();
extern bool aboutOpen;

#endif