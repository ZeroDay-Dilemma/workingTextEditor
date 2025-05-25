#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <cstdint>

const std::string FONTFILE = "myfont.wff";
uint16_t FONT_NUM_PER_CHAR = 0;
uint16_t FONT_WIDTH = 0;
uint16_t FONT_HEIGHT =0;
uint16_t NUM_CHARS_IN_FONT =0;
char FONT_DATA_OFFSET=0x40;
uint8_t FONT_SIZE = 2;
uint32_t PREV_LINE_X=0;
const int BUFFER_SIZE = 1024;
bool running = true;
const uint32_t BYTES_PER_PIXEL = 4;
int cmd_tracker=-1;
const uint32_t WINDOW_WIDTH = 800;//640;
const uint32_t WINDOW_HEIGHT = 600;//480;
unsigned int CURSOR[2] = {0,0};
unsigned int PREV_CURSOR[2] = {0,0};

uint32_t *vram = new uint32_t[WINDOW_WIDTH*WINDOW_HEIGHT];
uint8_t* fontData = nullptr;
void drawCharacter(int offset, SDL_Renderer *renderer, unsigned int x, unsigned int y, unsigned int scale, uint32_t color=0xFFFFFFFF);


// Function to read raw binary into a vector
void loadFontData(const std::string& binaryFile) {
    std::ifstream inputFile(binaryFile, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Failed to open binary file.\n";
        return; 
    }

    // Read the entire file into a vector
    //std::vector<uint8_t> binaryData((std::istreambuf_iterator<char>(inputFile)),
    //                                std::istreambuf_iterator<char>());
    //inputFile.close();
    inputFile.seekg(0, std::ios::beg); // Go to the start of the file
    uint8_t byte1, byte2;
    inputFile.read(reinterpret_cast<char*>(&byte1), 1); // Read first byte
    inputFile.read(reinterpret_cast<char*>(&byte2), 1); // Read second byte
    FONT_NUM_PER_CHAR = (static_cast<uint16_t>(byte1) << 8) | byte2; // Combine bytes to form uint16_t

    // Read FONT_WIDTH (2 bytes)
    inputFile.read(reinterpret_cast<char*>(&byte1), 1); 
    inputFile.read(reinterpret_cast<char*>(&byte2), 1);
    FONT_WIDTH = (static_cast<uint16_t>(byte1) << 8) | byte2;

    // Read FONT_HEIGHT (2 bytes)
    inputFile.read(reinterpret_cast<char*>(&byte1), 1); 
    inputFile.read(reinterpret_cast<char*>(&byte2), 1);
    FONT_HEIGHT = (static_cast<uint16_t>(byte1) << 8) | byte2;

    // Read NUM_CHARS_IN_FONT (2 bytes)
    inputFile.read(reinterpret_cast<char*>(&byte1), 1); 
    inputFile.read(reinterpret_cast<char*>(&byte2), 1);
    NUM_CHARS_IN_FONT = (static_cast<uint16_t>(byte1) << 8) | byte2;

    // Step 2: Debug output for the font header data
    std::cout << "FONT_NUM_PER_CHAR: " << FONT_NUM_PER_CHAR << std::endl;
    std::cout << "FONT_WIDTH: " << FONT_WIDTH << std::endl;
    std::cout << "FONT_HEIGHT: " << FONT_HEIGHT << std::endl;
    std::cout << "NUM_CHARS_IN_FONT: " << NUM_CHARS_IN_FONT << std::endl;    // Step 2: Skip the header offset and go to the font data offset
    inputFile.seekg(0, std::ios::beg);

    // Step 3: Dynamically allocate memory for font data (each character has 18 bytes = 144 bits)
    int totalFontSize = NUM_CHARS_IN_FONT * 18 + 8;  // Each character is 18 bytes (144 bits)
    fontData = new uint8_t[totalFontSize];

    // Step 4: Read the font data into the allocated memory
    inputFile.read(reinterpret_cast<char*>(fontData), totalFontSize);

    /*
    for (int i = 0; i < totalFontSize; i++) {
        std::cout << std::hex << (int)fontData[i] << " ";
        if ((i + 1) % 18 == 0) {
            std::cout << std::endl;  // Print new line after every 18 bytes (one character)
        }
    }
    */

    inputFile.close();
}


void freeFontData() {
    delete[] fontData;
    fontData = nullptr;
    std::cout << "Font data memory freed.\n";
}

// Function to print binary as 1s and 0s
void printBinary(const std::vector<uint8_t>& binaryData) {
    for (uint8_t byte : binaryData) {
        std::cout << std::bitset<8>(byte) << " ";
    }
    std::cout << std::endl;
}


//Screen dimension constants
//const uint32_t SCREEN_WIDTH = 320;
//const uint32_t SCREEN_HEIGHT = 240;
//76800
//  /8 = 9600
//const uint32_t SCALE = 2;
//const uint32_t WINDOW_WIDTH = (SCREEN_WIDTH ) * SCALE;
//const uint32_t WINDOW_HEIGHT = (SCREEN_HEIGHT) * SCALE;


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
void drawCursor(SDL_Renderer *renderer, char* textBuffer, int& bufferPos){
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  for (int i = 0; i < FONT_HEIGHT*FONT_SIZE; i++){ 
    SDL_RenderDrawPoint(renderer, CURSOR[0], CURSOR[1]+i);
  }
  uint8_t toPrint = textBuffer[bufferPos];
   if( (toPrint>=65 && toPrint<=90) || (toPrint>=97 && toPrint<=122) ){
    //its a letter

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 0; i < FONT_HEIGHT*FONT_SIZE; i++){ 
      SDL_RenderDrawPoint(renderer, PREV_CURSOR[0]-(FONT_WIDTH*FONT_SIZE+2), PREV_CURSOR[1]+i);
    }  
  }
  
  else if(textBuffer[bufferPos]=='\0' || textBuffer[bufferPos-1]=='\n'){
    SDL_SetRenderDrawColor(renderer, 0,0, 0, 255);
    for (int i = 0; i < FONT_HEIGHT*FONT_SIZE; i++){
      SDL_RenderDrawPoint(renderer, PREV_CURSOR[0], PREV_CURSOR[1]+i);
    }
  }

}
void drawCharacter_Helper(uint8_t toPrint, char* textBuffer, int& bufferPos, SDL_Renderer *renderer){
  //handle special characters first:
   
  if( (toPrint>=33 && toPrint<=126) ){
    //its a letter
    
    drawCharacter((toPrint), renderer, CURSOR[0], CURSOR[1], FONT_SIZE);
  } 
  else if (toPrint == '\0'){
printf("letter");
    drawCharacter(0, renderer, CURSOR[0], CURSOR[1], FONT_SIZE, 0x000000FF);
    
  }
}
void handleInput(SDL_Event& event, char* textBuffer, int& bufferPos, SDL_Renderer *renderer) {
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
      drawCharacter_Helper(*event.text.text,textBuffer, bufferPos,renderer);
      PREV_CURSOR[0]=CURSOR[0];
      PREV_CURSOR[1]=CURSOR[1];
      CURSOR[0] += (FONT_SIZE*FONT_WIDTH+2);
      drawCursor(renderer,textBuffer, bufferPos);

      //check for command attempt

    }
  } else if (event.type == SDL_KEYDOWN) {
    // Handle special keys
    switch (event.key.keysym.sym) {
      case SDLK_BACKSPACE:
        if (bufferPos > 0) {
          if (textBuffer[bufferPos-1]=='\n'){
                SDL_Rect rect;
    rect.x = 250;
    rect.y = 150;
    rect.w = 200;
    rect.h = 200;
            SDL_SetRenderDrawColor(renderer,0,255,0,255);
            SDL_RenderDrawRect(renderer,&rect);
            uint32_t tmp=0;
            for(int l = bufferPos-2; l>=0; l--){
              if (textBuffer[l]=='\n'){
                break;
              }
              else{
                tmp++;
              }
            }
            PREV_CURSOR[0]=CURSOR[0];
            PREV_CURSOR[1]=CURSOR[1];
            CURSOR[0]=tmp*(FONT_SIZE*FONT_WIDTH+2);
            CURSOR[1]-=(FONT_HEIGHT*FONT_SIZE+4);
          } else{
            PREV_CURSOR[0]=CURSOR[0];
            PREV_CURSOR[1]=CURSOR[1];
            CURSOR[0]-=(FONT_SIZE*FONT_WIDTH+2); 
          }
          drawCharacter_Helper('\0',textBuffer, bufferPos,renderer);
          textBuffer[--bufferPos] = '\0'; // Remove the last character
          drawCursor(renderer ,textBuffer, bufferPos);

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
            PREV_CURSOR[0]=CURSOR[0];
            PREV_CURSOR[1]=CURSOR[1];
            CURSOR[1]+=(FONT_HEIGHT*FONT_SIZE+4);
            CURSOR[0]=0;
            drawCursor(renderer, textBuffer, bufferPos);
            textBuffer[bufferPos] = '\0';  // Null-terminate the buffer
          }
        }
        break;
      case SDLK_TAB:
        if (bufferPos + 2 < BUFFER_SIZE - 1) {
          strcat(textBuffer, "  "); // Add four spaces for Tab
          bufferPos += 2;
          drawCharacter_Helper('\0',textBuffer, bufferPos,renderer);
          PREV_CURSOR[0]=CURSOR[0];
          CURSOR[0] += 2*(FONT_SIZE*FONT_WIDTH+2);
          drawCursor(renderer,textBuffer, bufferPos);

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

void redraw(char* textBuffer, int& bufferPos){

}

void drawCharacter(int offset, SDL_Renderer *renderer, unsigned int x, unsigned int y, unsigned int scale, uint32_t color){  

  //width of 5, height of 6. render from top left corner;
  std::cout<<((color&(0xFF<<24))>>24)<<" "<<(color&(0xFF<<16))<<" "<<(color&(0xFF<<8))<<" "<<(color&(0xFF))<<std::endl;
  
  SDL_SetRenderDrawColor(renderer, (color&(0xFF<<24))>>24, (color&(0xFF<<16))>>16, (color&(0xFF<<8))>>8,(color&(0xFF)));
  
  //0b00011101
  //mask= row&(1<<POS)
  //printf("\n");
  for (unsigned int i = 0; i<(FONT_HEIGHT*scale);i++){ //print a row
    for (unsigned int j = 0; j<(FONT_WIDTH*scale); j++){ //print each in row
      int row = i/scale;
      int col = j/scale;

      // Calculate the bit index in the font data
      int bitIndex = row * FONT_WIDTH + col;
      //std::cout<<bitIndex%8<<std::endl;
      //starting array = (offset*FONT_NUM_PER_CHAR + FONT_DATA_OFFSET)/8; 

      // Access the corresponding byte from fontData
      int byteIndex=(offset * FONT_NUM_PER_CHAR + FONT_DATA_OFFSET)/8 + (bitIndex / 8);

      //std::cout<<"  "<<byteIndex<<" ";
      
      int bitPosition = 7 - (bitIndex % 8); // bit position within the byte (0-7)
      //std::cout<<bitIndex<<" "<<byteIndex<<" "<<bitPosition<<" "<<std::endl;

      //if(fontData[(offset*FONT_NUM_PER_CHAR+FONT_DATA_OFFSET)+(row*FONT_WIDTH+col)]&(1<<(FONT_NUM_PER_CHAR-(row*FONT_WIDTH+col)))){ //if bit = 0;
      if (offset==0){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawPoint(renderer, x+j,y+i);
      }
      else if (offset<0){//draw inverse on neg offset
        if (fontData[byteIndex] & (0 << bitPosition)) {
        if(x+j<=WINDOW_WIDTH && y+i<=WINDOW_HEIGHT){ //ensure bounds
          SDL_RenderDrawPoint(renderer, x+j,y+i);
          //printf("1"); 
        }
        }

        
      }else{
        if (fontData[byteIndex] & (1 << bitPosition)) {
        if(x+j<=WINDOW_WIDTH && y+i<=WINDOW_HEIGHT){ //ensure bounds
          SDL_RenderDrawPoint(renderer, x+j,y+i);
          //printf("1");

          
        }
        else{
          printf("failed: on %d\n",offset);
        }
      }
      else{
        //printf("0");
      }
    }
  //printf("\n");
  }


  }
}


void drawFriend(SDL_Renderer *renderer, unsigned int x, unsigned int y, unsigned int scale){
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
  for (unsigned int i = 0; i<(6*scale);i++){ //print a row
    for (unsigned int j = 0; j<(5*scale); j++){ //print each in row
      if(myGuy[i/scale]&(1<<(4-(j/scale)))){ //if bit = 0;
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
  freeFontData();
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
  uint32_t toSet= 0; //EWHAT //int NUM_CHARS_IN_FONT =0;0;
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

    //load in text file if passed
    //

    char* file_data;
    if (argc == 2){
      printf("arg passed\n");
      std::cout<<"yo"<<std::endl;
      //something was passed:
      const char* filename = args[1];
      std::ifstream file(filename, std::ios::binary | std::ios::ate);
      if (!file) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return 1;
      }

      std::streamsize size = file.tellg();
      file.seekg(0, std::ios::beg);

      char* file_data = new char[size];

      if (!file.read(file_data, size)) {
        std::cerr << "Error: Could not read file\n";
        delete[] file_data;  // Clean up memory
        return 1;
      }
      std::cout.write(file_data, (size < 100) ? size : 100);
    

      delete[] file_data;

    }
    else{
      printf("nothing passed, new file");
      char a[]="";
      file_data=a;
    }

    const char* const_data = file_data;

    //Update the surface
    //SDL_UpdateWindowSurface( gWindow );
    //drawFramebuffer();
    SDL_Renderer *renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
    //SDL_RenderPreent
    printf("hi\n");
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    
    SDL_RenderClear(renderer);
    

    //drawFriend(renderer, WINDOW_WIDTH/2, WINDOW_HEIGHT/2,20);
    
    loadFontData(FONTFILE);
    if (fontData==nullptr) {
        return 1;
    }
    printf("paasses!");
    
for (int i = 0; i < 10; i++) {  // print the first 10 bytes for debugging
    std::cout << std::hex << (int)fontData[i] << " ";
}
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
                handleInput(event, textBuffer, bufferPos, renderer);
            }

        }

        // Render or print the buffer (for testing)
        if (prevLoc != bufferPos){
          system("clear"); // Clear the terminal (optional)
          std::cout << textBuffer << std::endl;
          //SDL_RenderClear(renderer);
          //drawCharacter_Helper(textBuffer, bufferPos); 
          SDL_RenderPresent(renderer);
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


   ///////////////////////////////////// 
  }
  ///////////////////////
  //SDL_Delay(4000);

  //Free resources and close SDL
  close();

  return 0;
}

