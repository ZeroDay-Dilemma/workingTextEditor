
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

int main( int argc, char* args[] )
{
  //printf("rgbtest: %d\n",rgbToInt(255,0,0));
  //Start up SDL and create window


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

  //Free resources and close SDL
  close();

  return 0;
}
