//
//  main.c
//  hft-sdl
//
//  Created by GREGG TAVARES on 5/20/15.
//  Copyright (c) 2015 greggman. All rights reserved.
//

#include <stdio.h>
#include <SDL2/SDL.h>
#include "run.h"

int main(int argc, const char * argv[]){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        puts("SDL_Init error");
        return EXIT_FAILURE;
    } else {
        puts("SDL_Init success!");
        Run();
        return EXIT_SUCCESS;
    }
}
