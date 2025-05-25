#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
//Screen dimension constants
//const uint32_t SCREEN_WIDTH = 320;
//const uint32_t SCREEN_HEIGHT = 240;
//76800
//  /8 = 9600
//const uint32_t SCALE = 2;
//const uint32_t WINDOW_WIDTH = (SCREEN_WIDTH ) * SCALE;
//const uint32_t WINDOW_HEIGHT = (SCREEN_HEIGHT) * SCALE;
const uint32_t BYTES_PER_PIXEL = 4;

const uint32_t WINDOW_WIDTH = 640;
const uint32_t WINDOW_HEIGHT = 480;
uint32_t *vram = new uint32_t[WINDOW_WIDTH*WINDOW_HEIGHT];
//Starts up SDL and creates window


bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

bool init()
{
  //Initialization flag
  bool success = true;

  //Initialize SDL
  if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
  {
    printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    success = false;
  }
  else
{
    //Create window
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN );
    if( gWindow == NULL )
    {
      printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
      success = false;
    }
    else
  {
      //Get window surface
      gScreenSurface = SDL_GetWindowSurface( gWindow );
    }
  }

  return success;
}


void close()
{
  //Deallocate surface

  SDL_FreeSurface( gHelloWorld );
  gHelloWorld = NULL;
  //Destroy window
  SDL_DestroyWindow( gWindow );
  gWindow = NULL;
  //Quit SDL subsystems
  SDL_Quit();
  delete[] vram;
}

bool loadMedia(){
  bool success = true; 
  return success;
}


void drawSinglePixel()
{
  // Lock the surface to get direct access to pixels
  if (SDL_MUSTLOCK(gScreenSurface))
  {
    SDL_LockSurface(gScreenSurface);
  }
  //std::cout<<gScreenSurface->pixels<<std::endl;
  // Copy pixel data from vram to the surface's pixels
  uint32_t* pixels = (uint32_t*)gScreenSurface->pixels;
  int pitch = gScreenSurface->pitch / sizeof(uint32_t);
  std::cout<<pitch<<std::endl;

  int centerX = gScreenSurface->w /2;
  int centerY = gScreenSurface->h /2;
  uint32_t toSet= 0;
  pixels[centerY * pitch + centerX] = (255 << 16) | (255 << 8) | 255;
  //for (uint32_t y = 0; y < SCREEN_HEIGHT; ++y)
  //{
  //    for (uint32_t x = 0; x < SCREEN_WIDTH; ++x)
  //    {
  //        pixels[y * gScreenSurface->pitch / BYTES_PER_PIXEL + x] = vram[y * SCREEN_WIDTH + x];
  //    }
  //}

  // Unlock the surface
  if (SDL_MUSTLOCK(gScreenSurface))
  {
    SDL_UnlockSurface(gScreenSurface);
  }

  // Update the surface
  SDL_UpdateWindowSurface(gWindow);

}

uint32_t rgbToInt(uint8_t r=0, uint8_t g=0, uint8_t b=0){
  uint32_t returnr = 0;
  returnr = (r << 16) | (g << 8) | (b);
  //std::cout<<returnr<<std::endl;
  return returnr;
}
void drawFramebuffer()
{
  // Lock the surface to get direct access to pixels
  if (SDL_MUSTLOCK(gScreenSurface))
  {
    SDL_LockSurface(gScreenSurface);
  }
  //std::cout<<gScreenSurface->pixels<<std::endl;
  // Copy pixel data from vram to the surface's pixels
  uint32_t* pixels = (uint32_t*)gScreenSurface->pixels;
  //int pitch = gScreenSurface->pitch / sizeof(uint32_t);
  //std::cout<<pitch<<std::endl;

  //int centerX = gScreenSurface->w /2;
  //int centerY = gScreenSurface->h /2;
  //uint32_t toSet= 0;
  //pixels[centerY * pitch + centerX] = (255 << 16) | (255 << 8) | 255;

  for (uint32_t y = 0; y < WINDOW_HEIGHT; y++)
  {
    for (uint32_t x = 0; x < WINDOW_WIDTH; x++)
    {
      //(255 << 16) | (255 << 8) | 255;
      //uint32_t color = vram[y * WINDOW_WIDTH + x];
      //printf("%d -- ",color);
      //std::cout<<pixels[y * gScreenSurface->pitch/ BYTES_PER_PIXEL + x]<<std::endl;
      //COLOR : 8 bits red, 8 bits green, 8 bits blue
      pixels[y * gScreenSurface->pitch/ BYTES_PER_PIXEL + x] = vram[y * WINDOW_WIDTH + x];
    }
  }

  // Unlock the surface
  if (SDL_MUSTLOCK(gScreenSurface))
  {
    SDL_UnlockSurface(gScreenSurface);
  }

  // Update the surface
  SDL_UpdateWindowSurface(gWindow);
}


int main( int argc, char* args[] )
{
  printf("rgbtest: %d\n",rgbToInt(255,0,0));
  //Start up SDL and create window
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
{
    //Load loadMedia
    //printf("y/WH: ");
    for (uint32_t y = 0; y < WINDOW_HEIGHT; ++y)
    {
      for (uint32_t x = 0; x < WINDOW_WIDTH; ++x)
      {
        uint8_t red = 255*((float)(x+y)/(WINDOW_HEIGHT+WINDOW_WIDTH));
        uint8_t green = 255*((float)(x+y)/(WINDOW_HEIGHT+WINDOW_WIDTH));
        uint8_t blue = 255*((float)(x+y)/(WINDOW_HEIGHT+WINDOW_WIDTH));
        //printf("color: %d\n",((x%255) << 16) | ((y%255) << 8) | (0b00000000<<0));
        //uint32_t col = 0;
        //col = ((x%255) << 16) | ((y%255) << 8) | (0b00000000<<0);
        //printf("%f",(static_cast< float >(y)/WINDOW_HEIGHT));

        vram[y * WINDOW_WIDTH + x] = red << 16 | green << 8 | blue;
        //static_cast<uint8_t>((0*(static_cast<float>(x)/WINDOW_WIDTH))) << 16 | static_cast<uint8_t>((0*(static_cast<float>(y)/WINDOW_HEIGHT))) << 8 | static_cast<uint8_t>((blue*(static_cast<float>(y+x)/(WINDOW_HEIGHT+WINDOW_WIDTH))));
        //((x%255) << 16) | ((y%255) << 8) | (0b00000000<<0); // ARGB format
        //printf("\033[%dD",1);
        //vram[y * WINDOW_WITDH + x] = ((red*(y/SCREEN_HEIGHT)) << 16) | (green << 8) | blue; // ARGB format
      }
    }
    //Apply the image
    //SDL_BlitSurface( gHelloWorld, NULL, gScreenSurface, NULL );

    //Update the surface
    //SDL_UpdateWindowSurface( gWindow );
    drawFramebuffer();
    //Hack to get window to stay up
    SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }

  }

  //Free resources and close SDL
  close();

  return 0;
}
