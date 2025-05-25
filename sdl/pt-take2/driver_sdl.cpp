
#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <bitset>
#include <string>
#include <cstdint>


SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;
const uint32_t WINDOW_WIDTH = 800;//640;
const uint32_t WINDOW_HEIGHT = 600;//480;

bool running = true;
const std::string FONTFILE = "myfont.wff";
uint16_t FONT_NUM_PER_CHAR = 0;
uint16_t FONT_WIDTH = 0;
uint16_t FONT_HEIGHT =0;
uint16_t NUM_CHARS_IN_FONT =0;
char FONT_DATA_OFFSET=0x40;
uint8_t FONT_SIZE = 2;
const int BUFFER_SIZE = 1024;
uint32_t PREV_LINE_X=0;
uint8_t LETTER_PADDING = 4;
const uint32_t BYTES_PER_PIXEL = 4;
int cmd_tracker=-1;
uint8_t* fontData = nullptr;
unsigned int CURSOR[2] = {0,0};
unsigned int PREV_CURSOR[2] = {0,0};
void drawCharacter(int offset, SDL_Renderer *renderer, unsigned int x, unsigned int y, unsigned int scale, uint32_t color=0xFFFFFFFF);
void loadFontData(const std::string& binaryFile) {
  std::ifstream inputFile(binaryFile, std::ios::binary);
  if (!inputFile) {
    std::cerr << "Failed to open binary file.\n";
    return; 
  }
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

  inputFile.close();
}



void drawCharacter(int offset, SDL_Renderer *renderer, unsigned int x, unsigned int y, unsigned int scale, uint32_t color){  

  //width of 5, height of 6. render from top left corner;
  //std::cout<<((color&(0xFF<<24))>>24)<<" "<<(color&(0xFF<<16))<<" "<<(color&(0xFF<<8))<<" "<<(color&(0xFF))<<std::endl;

  SDL_SetRenderDrawColor(renderer, (color&(0xFF<<24))>>24, (color&(0xFF<<16))>>16, (color&(0xFF<<8))>>8,(color&(0xFF)));
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
      if (offset==0){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawPoint(renderer, x+j,y+i);
      }
      else if (offset<0){//draw inverse on neg offset
        if (fontData[byteIndex] & (0 << bitPosition)) {
          if(x+j<=WINDOW_WIDTH && y+i<=WINDOW_HEIGHT){ //ensure bounds
            SDL_RenderDrawPoint(renderer, x+j,y+i);
          }
        }
      }else{
        if (fontData[byteIndex] & (1 << bitPosition)) {
          if(x+j<=WINDOW_WIDTH && y+i<=WINDOW_HEIGHT){ //ensure bounds
            SDL_RenderDrawPoint(renderer, x+j,y+i);

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


void freeFontData() {
  delete[] fontData;
  fontData = nullptr;
  std::cout << "Font data memory freed.\n";
}



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
}

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
    else if (textBuffer[cmd_tracker+1]=='p'){
      //print buffer for Debug
      printf("\n\n");

      for (char* ptr = textBuffer; *ptr != '\0'; ++ptr) {
        printf("%c", *ptr);
      }
    }
  }
  return -1;
}

/*
void drawString(const char* text, SDL_Renderer* renderer, int x, int y) {
  // Iterate through each character in the C-string (until null-terminator)
  uint16_t defX = x;
  for (size_t i = 0; text[i] != '\0'; ++i) {
    // Call drawCharacter on each character
    if(i%edInfo->maxCharsPerLine == 0 && i!=0){
      std::cout<<"++"<<std::endl;
      y+=(FONT_SIZE*FONT_HEIGHT);
    }  
    drawCharacter(text[i], renderer,(x + ((i%edInfo->maxCharsPerLine) * (FONT_WIDTH + FONT_SIZE + LETTER_PADDING))), y, FONT_SIZE);
    
  }
}
*/

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
      PREV_CURSOR[0]=CURSOR[0];
      PREV_CURSOR[1]=CURSOR[1];
      CURSOR[0] += (FONT_SIZE*FONT_WIDTH+2);

       
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
            PREV_CURSOR[0]=CURSOR[0];
            PREV_CURSOR[1]=CURSOR[1];
            CURSOR[1]+=(FONT_HEIGHT*FONT_SIZE+4);
            CURSOR[0]=0;
            textBuffer[bufferPos] = '\0';  // Null-terminate the buffer
          }
        }
        break;
      case SDLK_TAB:
        if (bufferPos + 2 < BUFFER_SIZE - 1) {
          strcat(textBuffer, "  "); // Add four spaces for Tab
          bufferPos += 2;
          PREV_CURSOR[0]=CURSOR[0];
          CURSOR[0] += 2*(FONT_SIZE*FONT_WIDTH+2);
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








int main( int argc, char* args[] )
{
  //printf("rgbtest: %d\n",rgbToInt(255,0,0));
  //Start up SDL and create window
  SDL_Renderer *renderer;
  if( !init() )
  {
    printf( "Failed to initialize!\n" );
  }
  else
  {
    renderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_SOFTWARE);
    //SDL_SetRenderDrawColor(renderer,0,0,0,255);

    SDL_RenderClear(renderer);
    //SDL_RenderPresent(renderer); 
    //assuming blank file  

    loadFontData(FONTFILE);
    if (fontData==nullptr) {
      return 1;
    }
    printf("paasses!");



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



    unsigned int prevSize = 1;
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
  ///////////////////////
  //SDL_Delay(4000);
  printf("hit");

  }
  //Free resources and close SDL
  close();

  return 0;
}
