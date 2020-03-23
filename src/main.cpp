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

void render(SDL_Renderer* renderer, std::vector<PixelScanline> pixels)
{
    for (int y = 0; y < pixels.size(); y++) {
        for (int x = 0; x < pixels[y].size(); x++) {
            RGBPixel pixel = pixels[y][x];
            //printf("R: %d | G: %d | B: %d\n", pixel.red, pixel.green, pixel.blue);
            SDL_SetRenderDrawColor(renderer, pixel.red, pixel.green, pixel.blue, pixel.alpha);
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }
    
    SDL_RenderPresent(renderer);
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
    SDL_Renderer *renderer;
    PNG_header pngHdr = decoder.getPNGHeader();
    SDL_CreateWindowAndRenderer(pngHdr.width, pngHdr.height, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "PNG Decoder");
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    render(renderer, pixels);

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