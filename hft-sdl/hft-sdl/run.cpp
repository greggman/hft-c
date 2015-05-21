//
//  run.cpp
//  hft-sdl
//
//  Created by GREGG TAVARES on 5/20/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//
#include "run.h"

#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

#include "websocket.h"

#ifndef RES_PATH_H
#define RES_PATH_H

//#include <iostream>
//#include <string>
//#include <SDL.h>

/*
 * Get the resource path for resources located in res/subDir
 * It's assumed the project directory is structured like:
 * bin/
 *  the executable
 * res/
 *  Lesson1/
 *  Lesson2/
 *
 * Paths returned will be Lessons/res/subDir
 */
std::string getResourcePath(const std::string &subDir = ""){
    //We need to choose the path separator properly based on which
    //platform we're running on, since Windows uses a different
    //separator than most systems
#ifdef _WIN32
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif
    //This will hold the base resource path: Lessons/res/
    //We give it static lifetime so that we'll only need to call
    //SDL_GetBasePath once to get the executable path
    static std::string baseRes;
    if (baseRes.empty()){
        //SDL_GetBasePath will return NULL if something went wrong in retrieving the path
        char *basePath = SDL_GetBasePath();
        if (basePath){
            baseRes = basePath;
            SDL_free(basePath);
        }
        else {
            std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
            return "";
        }
        //We replace the last bin/ with res/ to get the the resource path
        size_t pos = baseRes.rfind("bin");
        baseRes = baseRes.substr(0, pos) + "res" + PATH_SEP;
    }
    //If we want a specific subdirectory path in the resource directory
    //append it to the base path. This would be something like Lessons/res/Lesson0
    return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
}

#endif

int Run() {
    SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (win == nullptr){
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (ren == nullptr){
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    std::string imagePath = getResourcePath() + "f-texture.png";
    SDL_Surface *bmp = IMG_Load(imagePath.c_str());
    if (bmp == nullptr){
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cout << "SDL_LoadPNG Error: " << SDL_GetError() << ": " << imagePath << std::endl;
        SDL_Quit();
        return 1;
    }
    
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
    SDL_FreeSurface(bmp);
    if (tex == nullptr){
        SDL_DestroyRenderer(ren);
        SDL_DestroyWindow(win);
        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    foobarmain();
    
    //A sleepy rendering loop, wait for 3 seconds and render and present the screen each time
    for (int i = 0; i < 3; ++i){
        //First clear the renderer
        SDL_RenderClear(ren);
        //Draw the texture
        SDL_RenderCopy(ren, tex, NULL, NULL);
        //Update the screen
        SDL_RenderPresent(ren);
        //Take a quick break after all that hard work
        SDL_Delay(1000);
    }
    
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    return 0;
}