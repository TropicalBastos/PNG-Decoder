#include <SDL2/SDL.h>
#include "decoder.h"
#include <iostream>

void event_loop() {
    SDL_Event e;
    bool quit = false;
    while (!quit){
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Please provide the path to a single file as an argument" << std::endl;
        return -1;
    }

    SDL_Init(SDL_INIT_VIDEO);

    PNGDecoder decoder(argv[1]);
    auto pixels = decoder.decode();
    if (pixels.size() < 1) {
        return 0;
    }

    SDL_Window *window;
    PNG_header pngHdr = decoder.getPNGHeader();

    window = SDL_CreateWindow(
        "PNG Decoder",                  // window title
        0,                              // initial x position
        0,                              // initial y position
        pngHdr.width,                            // width, in pixels
        pngHdr.height,                            // height, in pixels
        SDL_WINDOW_OPENGL               // flags
    );

    if (window == NULL) {
        // In the case that the window could not be made...
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_ShowWindow(window);

    event_loop();

    SDL_DestroyWindow(window);

    // Clean up
    SDL_Quit();
    return 0;
}