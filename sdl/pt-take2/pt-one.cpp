#include <SDL2/SDL.h>
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include <bitset>
#include <string>
#include <cstdint>
#include <cstdlib>



struct Piece {

  bool isOriginal;
  uint64_t start;
  uint32_t length;
  Piece* next;
  Piece* prev;

};

struct Cursor {
    Piece* piece;  // currentPiece we are in
    uint64_t offset; // position within. 0 = before first char, = length+1 = at the end of the file
};

class PieceTable {
private:
  char* originalBuffer;
  char* editBuffer;
  uint64_t originalSize;
  uint64_t editSize;
  uint64_t editMaxSize;
  Piece* head;
  Piece* tail;
  Piece* currentPiece;
  Cursor* cursor;
  char* tempEditBuffer;  // Holds user input before commit
  uint64_t tempEditLength = 0;  // Number of chars in tempEditBuffer
    
  

  void resizeAddBuffer(uint64_t newSize) {
    printf("not implemented safely, this should not be run");
    if(newSize <= editMaxSize) return; // its fine within the current allocated PieceTable
    //uint64_t oldSize = editMaxSize;
    editMaxSize = newSize * 2;
    char* newBuffer = (char*) malloc(editMaxSize * sizeof(*newBuffer));
    
    if (editBuffer != nullptr){
      memCopier(editSize, newBuffer, editBuffer);  
      free(editBuffer);
    }  
    editBuffer = newBuffer;

  }

  void memHelperChar(uint64_t len, void *src){

  }

  void memCopier(uint64_t len, char *dest, char *src){
    //char *charSrc = (char *)src;
    //char *charDest = (char *)dest
    while(len--){
      *dest++ = *src++;
    }
  }

  //Piece* makePiece
  
  Piece* splitPiece(Piece* toSplit, uint64_t position) {
    //passer position 
    printf("Attempting to split piece at position %lu\n", position);

    // TODO: huh?    
    if (!toSplit || position >= toSplit->length) {
        printf("Invalid split position or null piece\n");
        return nullptr;
    }

    Piece* newPiece = (Piece*) malloc(sizeof(Piece));
    if (!newPiece) {
        printf("Memory allocation failed\n");
        return nullptr;
    }


    if(toSplit == tail){ // because head would stay, ensure new piece is set as tail
      tail = newPiece;
    }

    // original = front half, new = end half
    // orig prev   = old prev      no need to touch
    // orig start  = old start    no need to touch
    // orig length = cursor 
    // orig next   = the middle piece, can temp set it to the end half  and handle later
    // 
    // new prev   = the middle piece, temp set to front half
    // new start  = old start + cursor + 1
    // new length = old offset - cursor
    // new next = old next
    

//    if (toSplit->next) {
//        toSplit->next->prev = newPiece;
//    }

    
     // Update original piece to its new truncated length
      

    newPiece->start = toSplit->start + position + 1;  // New piece starts where the split occurs
    newPiece->length = toSplit->length - position; // New piece gets the remaining length
    newPiece->isOriginal = toSplit->isOriginal;    // Keep the same source (original/edit)
    newPiece->next = toSplit->next;
    newPiece->prev = toSplit;

    toSplit->next = newPiece;
    toSplit->length = position; 
    return newPiece;
  }

  
public:
  PieceTable() : originalBuffer(nullptr), editBuffer(nullptr), originalSize(0), editSize(0), editMaxSize(0), head(nullptr), tail(nullptr), currentPiece(nullptr){
    cursor = (Cursor*) malloc(sizeof(Cursor));
    cursor->piece= nullptr;
    cursor->offset=0;
    tempEditBuffer=(char*) malloc(256);
    tempEditLength=0;
    editBuffer = (char*) malloc(0);
    resizeAddBuffer(256);
  }
  

  ~PieceTable(){
    printf("hi");
    if(originalBuffer != nullptr) free(originalBuffer); 
    if(editBuffer != nullptr) free(editBuffer);
    if(tempEditBuffer != nullptr) free(tempEditBuffer);
    free(cursor);
    Piece* tmp = head;
    while (tmp != nullptr) {
        Piece* nTmp = tmp->next;  // Store next node
        free(tmp);  // Free current node
        tmp = nTmp;  // Move to next
    }  
  }

  void loadFile(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    originalSize = size;
    // Allocate memory using malloc
    char* file_data = (char*)malloc(size + 1); // +1 for null-terminator
    if (!file_data) {
        std::cerr << "Error: Memory allocation failed\n";
        return;
    }

    if (!file.read(file_data, size)) {
        std::cerr << "Error: Could not read file\n";
        free(file_data);
        return;
    }

    file_data[size] = '\0'; // Null-terminate for safety
    originalBuffer = file_data;
    //std::cout.write(originalBuffer, 100);
    //want to insert the piece now
    // Todo ello 
    // TODO horriblyinitinit
    insertPiece(0,size, true);
}

  void addEdit(char c, bool forceEdit = false){
    //when is the edit committed?
    //cursor manually moved
    //edit buffer is full
    //newline? 
    if ((tempEditLength < sizeof(tempEditBuffer) - 1) || forceEdit == false) {
        tempEditBuffer[tempEditLength++] = c;
        tempEditBuffer[tempEditLength] = '\0'; 
    } 
    if (c == '\n' || tempEditLength >= 255 || forceEdit == true) {
      //commit
      //horribly unsafe i fear, need to ensure within buffer and resize if needed 
      //push temp to editbuffer, then insert the piece
      printf("PUSHING NEW PIECE!");
      memCopier(tempEditLength, editBuffer+editSize, tempEditBuffer);
      insertPiece(editSize,tempEditLength,false);
    }

  }

  void insertPiece(uint64_t start, uint64_t length, bool isOriginal=false){
    printf("Ran insert");
    Piece* newPiece = (Piece*) malloc(sizeof(Piece));
    if (!newPiece) {
        printf("Memory allocation failed\n");
        return;
    }
    newPiece->start = start;
    newPiece->length = length;
    newPiece->isOriginal = isOriginal;
    newPiece->next = nullptr;
    newPiece->prev = nullptr;

  //determine how to actually insert; 4 cases
  //1. first piece, just set it to head, tail, & currentPiece. case basically only when no input passed
  //2-4 are based around the currentPiece, and position within it. worry later
  //case 1:
    if(head==nullptr){
      head=newPiece;
      tail=newPiece;
      currentPiece=newPiece;
      cursor->piece=currentPiece; 
      //TODO: this might want to be changed in the future, but just places cursor at end of file for now 
      cursor->offset=currentPiece->length+1;//length+1;
    }
    
    else{
      
      int TODO;
      
      
      if(cursor->offset == 0){
        printf("inserting in front of piece");
        if(cursor->currentPiece == head) head = newPiece;
        newPice->next = currentPiece;
        newPiece->prev = currentPiece->prev;
        currentPiece->prev = newPiece;
        cursor->piece = newPiece;
        cursor->offset=newPiece->length;

      }
//TODO : CHECK IF RIGHT FORMULA!
      // NOTE: Should only really happen if we're at tail, so maybe consider changing 
      // it to be a check for if curr = tail
      
      else if((cursor->piece->length == cursor->offset)&& (curor->piece==tail)){
        printf("inserting after piece NOT IMPEMENTED");
        tail = newPiece;
        cursor->piece->next = newPiece;
        cursor->piece = newPiece;
        cursor->offset=0;
      }
      

      else{
        //TODO: this 
        
        Piece* newEnd = splitPiece(cursor->piece, cursor->position);
        if(cursor->piece == tail) tail = newEnd;
        cursor->piece= 

      }
      
      //ok so it needs to handle: if first or last piece, 
      //case 2-4 based around cursor
      //2: cursor offset = 0 //insert before piece
      //3: cursor offset = length+1 //insert after piece
      //4: else: split current piece, and insert new piece in the middle

    }
  
  }
  
  void debugOutputText() const{
    
    printf("running debug out");
    system("clear");
    
    Piece* current = head;
    while (current) {
      const char* buffer = current->isOriginal ? originalBuffer : editBuffer;
      if(cursor->piece == current){
        uint64_t iter = 0;
        while(iter<cursor->offset){
          printf("%c", buffer[current->start + iter]);
          iter++;
        }
        //display cursor where it exists!
        //printf("%c[%dm",'\033',42);
        //printf("%c",' ');
        //printf("%c[%dm",'\033',0);      
        printf("%s", tempEditBuffer);
        printf("\033[42m \033[0m"); 

        printf("%.*s", (int)(current->length - cursor->offset), buffer + current->start + cursor->offset);
      }
      else{
        printf("%.*s", (int)current->length, buffer + current->start);
      }
      current = current->next;
    }
    printf("\n"); // Ensure a newline after printing
  }

  // void debug_PrintPiecesDev() const{
  //   printf("original array: ");
  //   printf("%.*s", (int)current->length, buffer + current->start);
  //
  // }
};


int main( int argc, char* args[] ){
  //determine if new blank file or has input
  PieceTable* tablerrrr = new PieceTable;


  if (argc == 2) {
    printf("arg passed\n");
    tablerrrr->loadFile(args[1]);
  } else {
    printf("nothing passed, new file\n");
  }
  
 tablerrrr->addEdit('a');
 tablerrrr->addEdit('a');
 tablerrrr->addEdit('a');
 tablerrrr->addEdit('a');
 tablerrrr->addEdit('a');
 tablerrrr->addEdit('a');
  tablerrrr->addEdit('\n');
  tablerrrr->debugOutputText(); 


  delete tablerrrr;
  return 0;
}
