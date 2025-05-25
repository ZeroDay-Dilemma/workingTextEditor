#ifndef PIECE_TABLE_H
#define PIECE_TABLE_H
//#include <SDL2/SDL.h>
#include <stdio.h>
//#include <iostream>
#include <cstring>
//#include <fstream>
//#include <vector>
//#include <bitset>
//#include <string>
#include <cstdint>


//HEADER
enum PieceType {ORIGINAL, EDIT};
struct Piece {
  PieceType type; 
  uint64_t offset;
  uint64_t length;
};
class PieceTable{
  private:
    const char* ORIGINAL_TEXT; // the original text, read only!
    const uint64_t ORIGINAL_LENGTH;
    char* editBuffer; // the edit array, append only
    Piece** pieces; // array of pieces
    uint64_t editBufferMax; // max size alloc'd
    uint64_t editBufferLength; // current length written to edit buffer
    uint32_t piecesMax; // max pieces alloc'd
    uint32_t piecesCurrAmt; // current amount of pieces 
    
    bool _checkExpandNeeded(uint64_t &len); // true = success, expanded or didnt need to 
    bool _expandEditBuffer();
    bool _expandPieceBuffer();
    uint32_t makeSlot(uint64_t pos, uint64_t length);
    
  public:
    PieceTable(const char* originalText, uint64_t textLength);
    ~PieceTable();
    void insert(uint64_t pos, const char* text, uint64_t length);
    void deleteRange(uint64_t pos, uint64_t length);

};




#endif
