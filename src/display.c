#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "display.h"
#include "format.h"

#define FRAME_RATE 50

long long current_timestamp_milliseconds() {
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec * 1000LL + te.tv_usec / 1000;
    return milliseconds;
}

void display(const char* inputFolder, int num_entries, struct dirent **namelist) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return;
    }

    char *inputFileName = malloc(sizeof(char) * 1000);
    sprintf(inputFileName, "%s/%s", inputFolder, "0.pgm");
    FILE *inputFile = fopen(inputFileName, "rb");
    int width, height;
    char magicNumber[3];
    fscanf(inputFile, "%2c", magicNumber);
    fscanf(inputFile, "%d %d", &width, &height);

    SDL_Window *window = SDL_CreateWindow("Image Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height * 2 / 3, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return;
    }

    const char* outputFileName = "tmp.ppm";
    long long timestamp_ms = current_timestamp_milliseconds();
    for (int i = 0; i < num_entries; ++i) {
        if (namelist[i]->d_type == DT_REG) {
            if (current_timestamp_milliseconds() - timestamp_ms < 1000 / FRAME_RATE) {
                SDL_Delay(1000 / FRAME_RATE - (current_timestamp_milliseconds() - timestamp_ms));
            }
            timestamp_ms = current_timestamp_milliseconds();
            char *inputFilename = malloc(sizeof(char) * 1000);
            sprintf(inputFilename, "%s/%s", inputFolder, namelist[i]->d_name);
            
            pgmToPpm(inputFilename, outputFileName);

            SDL_Surface *imageSurface = IMG_Load(outputFileName);
            if (imageSurface == NULL) {
                fprintf(stderr, "IMG_Load Error: %s\n", IMG_GetError());
                free(inputFilename);
                continue;
            }

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, imageSurface);
            SDL_FreeSurface(imageSurface);

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);

            SDL_DestroyTexture(texture);
            free(inputFilename);

            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    SDL_DestroyRenderer(renderer);
                    SDL_DestroyWindow(window);
                    SDL_Quit();
                    return;
                }
            }
        }
        free(namelist[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}