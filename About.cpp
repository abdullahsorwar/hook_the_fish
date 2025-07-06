#include "About.h"
#include "Common.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fstream>
#include <stdlib.h>

SDL_Window* aboutWindow = nullptr;
static SDL_Renderer* aboutRenderer = nullptr;
static TTF_Font* titleFont = nullptr;
static TTF_Font* buttonFont = nullptr;
static TTF_Font* nameFont = nullptr;
static TTF_Font* textFont = nullptr;

static SDL_Texture* LeftHook = nullptr;
static SDL_Texture* RightHook = nullptr;
static SDL_Texture* frameTexture = nullptr;

static SDL_Rect backBtn = {400, 500, 200, 60};
static SDL_Rect ArrowLeftBtn = {10, 300, 50, 50};
static SDL_Rect ArrowRightBtn = {940, 300, 50, 50};

bool aboutOpen = false;
static int currentPage = 0;
const int totalPages = 4;
static SDL_Point mp = {0, 0};
static std::string pageText;

About dev;

std::vector<std::string> aboutFiles = {
    "about/shadman.txt",
    "about/abdullah.txt",
    "about/tansim.txt",
    "about/arnob.txt"
};

std::vector<std::string> aboutPhoto = {
    "jpg/shadman.jpg",
    "jpg/abdullah.jpg",
    "jpg/tansim.jpg",
    "jpg/arnob.jpg"
};

void loadAbout (About& dev)
{
    std::ifstream infile(aboutFiles[currentPage]);
    getline(infile, dev.name);
    getline(infile, dev.org);
    getline(infile, dev.git);
    getline(infile, dev.mail);
    getline(infile, dev.designation);
    infile.close();

    SDL_Surface* surf = nullptr;
    surf = IMG_Load(aboutPhoto[currentPage].c_str());
    dev.photo = SDL_CreateTextureFromSurface(aboutRenderer, surf);
    SDL_FreeSurface(surf);
}

void initAbout() {
    if (aboutWindow != nullptr) return;

    aboutWindow = SDL_CreateWindow("About", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 600, SDL_WINDOW_BORDERLESS);
    aboutRenderer = SDL_CreateRenderer(aboutWindow, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* surf = nullptr;
    surf = SDL_LoadBMP("bmp/hook2.bmp");
    LeftHook = SDL_CreateTextureFromSurface(aboutRenderer, surf);
    SDL_FreeSurface(surf);
    
    surf = SDL_LoadBMP("bmp/hook3.bmp");
    RightHook = SDL_CreateTextureFromSurface(aboutRenderer, surf);
    SDL_FreeSurface(surf);

    surf = IMG_Load("png/frame.png");
    frameTexture = SDL_CreateTextureFromSurface(aboutRenderer, surf);
    SDL_FreeSurface(surf);

    titleFont = TTF_OpenFont("fonts/LuckiestGuy-Regular.ttf", 96);
    buttonFont = TTF_OpenFont("fonts/OpenSans-Bold.ttf", 32);
    nameFont = TTF_OpenFont("fonts/Exo2.ttf", 48);
    textFont = TTF_OpenFont("fonts/ShareTech-Regular.ttf", 24);

    loadAbout(dev);
}

void renderAbout() {
    if (!aboutRenderer) return;

    SDL_SetRenderDrawColor(aboutRenderer, 255, 255, 255, 255);
    SDL_RenderClear(aboutRenderer);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Color black = {0, 0, 0, 255};

    SDL_Rect frameBtn = {180, 200, 280, 280};
    SDL_Rect photoBtn = {180, 200, 280, 280};
    SDL_RenderCopy (aboutRenderer, dev.photo, NULL, &photoBtn);
    SDL_RenderCopy (aboutRenderer, frameTexture, NULL, &frameBtn);

    renderText(aboutRenderer, titleFont, "About", black, 500, 80);
    
    if (currentPage > 0) {
        SDL_RenderCopy(aboutRenderer, LeftHook, NULL, &ArrowLeftBtn);
    }

    if (currentPage < totalPages - 1) {
        SDL_RenderCopy(aboutRenderer, RightHook, NULL, &ArrowRightBtn);
    }
    
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = {mx, my};

    Button back = {backBtn, "Close", false};
    back.hovered = SDL_PointInRect(&mousePoint, &back.rect);

    drawParallelogram(aboutRenderer, back, back.hovered);
    renderText(aboutRenderer, buttonFont, back.text, white, back.rect.x + back.rect.w / 2, back.rect.y + back.rect.h / 2);
    
    //filledCircleRGBA(aboutRenderer, 310, 330, 130, 0, 0, 0, 255);

    renderText(aboutRenderer, nameFont, dev.name, black, 500, 150);
    renderLeftText(aboutRenderer, textFont, "Organization:", black, 480, 200);
    renderLeftText(aboutRenderer, textFont, dev.org, black, 480, 230);
    renderLeftText(aboutRenderer, textFont, "GitHub Account:", black, 480, 270);
    renderLeftText(aboutRenderer, textFont, dev.git, black, 480, 300);
    renderLeftText(aboutRenderer, textFont, "email:", black, 480, 340);
    renderLeftText(aboutRenderer, textFont, dev.mail, black, 480, 370);
    renderLeftText(aboutRenderer, textFont, "Worked as:", black, 480, 410);
    renderLeftText(aboutRenderer, textFont, dev.designation, black, 480, 440);

    std::string pageNumber = std::to_string(currentPage + 1) + "/4";
    renderText(aboutRenderer, buttonFont, pageNumber.c_str(), black, 920, 550);

    SDL_RenderPresent(aboutRenderer);
}

void handleAboutEvents(SDL_Event& e, bool& AboutOpen) {
    if (!aboutWindow || e.window.windowID != SDL_GetWindowID(aboutWindow)) return;

    if (e.type == SDL_WINDOWEVENT) {
        if (e.window.event == SDL_WINDOWEVENT_CLOSE) {
            aboutOpen = false;
            destroyAbout();
        }
    }

    if (e.type == SDL_MOUSEBUTTONDOWN) {
        mp = {e.button.x, e.button.y};

        if (SDL_PointInRect(&mp, &ArrowLeftBtn) && currentPage > 0) {
            currentPage--;
            loadAbout(dev);
        }

        if (SDL_PointInRect(&mp, &ArrowRightBtn) && currentPage < totalPages - 1) {
            currentPage++;
            loadAbout(dev);
        }

        if (SDL_PointInRect(&mp, &backBtn)) {
            aboutOpen = false;
            destroyAbout();
        }
    }
}

void destroyAbout() {
    if (titleFont) {
        TTF_CloseFont(titleFont);
        titleFont = nullptr;
    }
    if (buttonFont) {
        TTF_CloseFont(buttonFont);
        buttonFont = nullptr;
    }
    if (nameFont) {
        TTF_CloseFont(nameFont);
        nameFont = nullptr;
    }
    if (textFont) {
        TTF_CloseFont(textFont);
        textFont = nullptr;
    }
    if (LeftHook) {
        SDL_DestroyTexture(LeftHook);
        LeftHook = nullptr;
    }
    if (RightHook) {
        SDL_DestroyTexture(RightHook);
        RightHook = nullptr;
    }
    if (aboutRenderer) {
        SDL_DestroyRenderer(aboutRenderer);
        aboutRenderer = nullptr;
    }
    if (aboutWindow) {
        SDL_DestroyWindow(aboutWindow);
        aboutWindow = nullptr;
    }
    currentPage = 0;
}

bool isAboutOpen() {
    return aboutWindow != nullptr;
}