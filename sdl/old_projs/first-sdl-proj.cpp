#include <iostream>
#include <thread>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
using namespace std;

int main(int argc, char *argv[])
{

  // returns zero on success else non-zero
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
  }

  SDL_Window* win = SDL_CreateWindow("GAME",
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     1000, 1000, 0);

  // triggers the program that controls
  // your graphics hardware and sets flags
  Uint32 render_flags = SDL_RENDERER_ACCELERATED;

  // creates a renderer to render our images
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

  // creates a surface to load an image into the main memory
  //SDL_Surface* surface;

  // please provide a path for your image
  //surface = IMG_Load("path");

  // loads image to our graphics hardware memory.
  //SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

  // clears main-memory
  //SDL_FreeSurface(surface);

  // let us control our image position
  // so that we can move it with our keyboard.
  SDL_Rect dest;
  int close = 0;
  // connects our texture with dest to control position
  //SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);



  while (!close){
    SDL_Event event;

    // Events management
    while (SDL_PollEvent(&event)) {
      switch (event.type) {

        case SDL_QUIT:
          // handling of close button
          close = 1;
          break;
        default:
          break;
      }
    }
  }
  //SDL_DestroyTexture(tex);

  // destroy renderer
  SDL_DestroyRenderer(rend);

  // destroy window
  SDL_DestroyWindow(win);

  // close SDL
  SDL_Quit();

  return 0;


  return 0;
}
