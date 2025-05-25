#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
//Screen dimension constants
//const uint32_t SCREEN_WIDTH = 320;
//const uint32_t SCREEN_HEIGHT = 240;
//76800
//  /8 = 9600
//const uint32_t SCALE = 2;
//const uint32_t WINDOW_WIDTH = (SCREEN_WIDTH ) * SCALE;
//const uint32_t WINDOW_HEIGHT = (SCREEN_HEIGHT) * SCALE;

const int BUFFER_SIZE = 1024;
bool running = true;
const uint32_t BYTES_PER_PIXEL = 4;
int cmd_tracker=-1;
const uint32_t WINDOW_WIDTH = 640;
const uint32_t WINDOW_HEIGHT = 480;
uint32_t *vram = new uint32_t[WINDOW_WIDTH*WINDOW_HEIGHT];
//Starts up SDL and creates window

int handleCmd(SDL_Event& event, char* textBuffer, int& bufferPos) {
  printf("hit\n");
  if(textBuffer[cmd_tracker] == ':'){ // no error here
    printf("a\n");
    if(textBuffer[cmd_tracker+1]=='q'){
      printf("b\n");
      if(cmd_tracker+2 == bufferPos){
        //end of command;
        return 0;
      }
    }
  }
  return -1;
}

void handleInput(SDL_Event& event, char* textBuffer, int& bufferPos) {
  if (event.type == SDL_TEXTINPUT) {
    if (strcmp(event.text.text, ":") == 0){
      //is command start
      printf("ypooorgmjkr\n");
      cmd_tracker=bufferPos;
    }
    // Append text to the buffer if there's space
    if (bufferPos + strlen(event.text.text) < BUFFER_SIZE - 1) {
      strcat(textBuffer, event.text.text); // Concatenate input to the buffer
      bufferPos += strlen(event.text.text); // Update buffer position
      //check for command attempt

    }
  } else if (event.type == SDL_KEYDOWN) {
    // Handle special keys
    switch (event.key.keysym.sym) {
      case SDLK_BACKSPACE:
        if (bufferPos > 0) {
          textBuffer[--bufferPos] = '\0'; // Remove the last character
        }
        break;
      case SDLK_RETURN:
        if (bufferPos < BUFFER_SIZE - 1) {
          if(cmd_tracker != -1){
            int todo = -2;
            todo = handleCmd(event, textBuffer, bufferPos);
            if(todo==0)
              exit(0);
          }
          else{
            textBuffer[bufferPos++] = '\n'; // Add a newline character
            textBuffer[bufferPos] = '\0';  // Null-terminate the buffer
          }
        }
        break;
      case SDLK_TAB:
        if (bufferPos + 4 < BUFFER_SIZE - 1) {
          strcat(textBuffer, "    "); // Add four spaces for Tab
          bufferPos += 4;
        }
        break;
      //case SDLK_ESCAPE:
      //  std::cout << "Escape pressed, exiting." << std::endl;
      //  exit(0); // Exit program
      default:
        break;
    }
  }
}

void drawFriend(SDL_Renderer *renderer, int x, int y,int scale){
  //width of 5, height of 6. render from top left corner;
  uint8_t myGuy[] = {
    0b00001110,
    0b00011101,
    0b00011111,
    0b00011111,
    0b00001111,
    0b00001001
  };
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  //0b00011101
  //mask= row&(1<<POS)
  for (int i = 0; i<(6*scale);i++){ //print a row
    for (int j = 0; j<(5*scale); j++){ //print each in row
      if(myGuy[i/scale]&(1<<4-(j/scale))){ //if bit = 0;
        if(x+j<=WINDOW_WIDTH && y+i<=WINDOW_HEIGHT){ //ensure bounds
          SDL_RenderDrawPoint(renderer, x+j,y+i);
        }
      }
    }

  }


}

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
  //printf("rgbtest: %d\n",rgbToInt(255,0,0));
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
    //drawFramebuffer();
    SDL_Renderer *renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
    //SDL_RenderPreent
    printf("hi\n");
    //SDL_SetRenderDrawColor(renderer,0,0,255,255);
    //SDL_RenderClear(renderer);
    drawFriend(renderer, WINDOW_WIDTH/2, WINDOW_HEIGHT/2,20);
    SDL_RenderPresent(renderer);


    // Main loop
    char textBuffer[BUFFER_SIZE]; // Character buffer for text input
    int bufferPos = 0;            // Current position in the buffer
    memset(textBuffer, 0, BUFFER_SIZE); // Initialize the buffer to all null characters

    // Main loop
    int prevLoc = 0;
    while (running) {
        SDL_Event event;

        // Poll events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else {
                // Pass events to the input handler
                handleInput(event, textBuffer, bufferPos);
            }

        }

        // Render or print the buffer (for testing)
        if (prevLoc != bufferPos){
          system("clear"); // Clear the terminal (optional)
          std::cout << textBuffer << std::endl;
          prevLoc=bufferPos;
        }
    }
    /*
    bool running = true;
    bool key_down = false;

    while (running) {
        SDL_Event event;

        // Poll events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Get the current state of the keyboard
        const Uint8* key_states = SDL_GetKeyboardState(nullptr);

        // Check for 'A' key press/release
        if (key_states[SDL_SCANCODE_A] && !key_down) {
            std::cout << "A key pressed" << std::endl;
            key_down = true;
        } else if (!key_states[SDL_SCANCODE_A] && key_down) {
            std::cout << "A key released" << std::endl;
            key_down = false;
        }

        // Optional: Refresh window or perform rendering
        SDL_UpdateWindowSurface(gWindow);
    }
    */

    //SDL_Delay(3000);
    //Hack to get window to stay up
    //SDL_Event e; bool quit = false; while( quit == false ){ while( SDL_PollEvent( &e ) ){ if( e.type == SDL_QUIT ) quit = true; } }

  }

  //Free resources and close SDL
  close();

  return 0;
}
