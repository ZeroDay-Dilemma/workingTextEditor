#include "piece_table.h"



PieceTable::PieceTable(const char* originalText, const uint64_t textLength): ORIGINAL_TEXT(originalText), ORIGINAL_LENGTH(textLength), editBufferLength(0), piecesCurrAmt(0) {
  // Initialize edit buffer and piece list
  editBufferMax = 512;  // Example size
  editBuffer = new char[editBufferMax];

  piecesMax = 128;       // Example size
  pieces = new Piece*[piecesMax];

    // Start with the whole original text as a single piece
    pieces[piecesCurrAmt++] = new Piece {ORIGINAL, 0, textLength};
}

PieceTable::~PieceTable() {
  for (uint64_t i = 0; i < piecesCurrAmt; ++i) {
    delete pieces[i];
  }
  delete[] editBuffer;
  delete[] pieces;
}

bool PieceTable::_checkExpandNeeded(uint64_t &len){
  bool r1 = false;
  bool r2 = false;
  if(editBufferLength + len > editBufferMax){
    //do need to resize!
    r1 = _expandEditBuffer(); 
  }
  if(piecesCurrAmt + 2 > piecesMax){
    r2 = _expandPieceBuffer();  
  }
  return (r1 && r2);

}

bool PieceTable::_expandEditBuffer(){
  //doubles length of edit buffer
  int tmpNewMax = editBufferMax * 2;
  char* newBuffer = new char[tmpNewMax];
  memcpy(newBuffer, editBuffer, editBufferLength);
  delete[] editBuffer;
  editBuffer = newBuffer;
  editBufferMax = tmpNewMax;
  return true;
}

bool PieceTable::_expandPieceBuffer(){
  //doubles length of edit buffer
  int tmpNewMax = piecesMax * 2;
  Piece** newBuffer = new Piece*[tmpNewMax];
  memcpy(newBuffer, pieces, piecesMax);
  for (uint64_t i = 0; i < piecesCurrAmt; ++i) {
    delete pieces[i];
  }
  delete[] pieces;
  pieces = newBuffer;
  piecesMax = tmpNewMax;
  return true;
}
/*
uint32_t PieceTable::makeSlot(uint64_t pos,uint64_t length){
    // Step 1: Check if expansion is needed for the pieces array
    //handled in _checkExpandNeeded();
    uint64_t currentPos = 0; // Tracks the current logical position of the pieces
    for (uint32_t i = 0; i < piecesCurrAmt; ++i) {
        Piece& piece = *pieces[i];
        uint64_t pieceEnd = currentPos + piece.length;
        // Case 1: Insertion at the start (before the first piece)
        if (pos <= currentPos) {
            // Push all pieces down by 1
            for (uint32_t j = piecesCurrAmt; j > i; --j) {
                pieces[j] = pieces[j - 1];
            }
            // Create a new piece and insert it at the start
            // Update the pieces count
            ++piecesCurrAmt;
            return i; // Return the index where the piece was inserted
        }
        // Case 2: Insertion at the end (after the last piece)
        if (pos >= pieceEnd) {
            // No need to shift; just insert the piece at the end
            currentPos = pieceEnd;
            if (i == piecesCurrAmt - 1) {
                // If it's the last piece, insert it after this
                Piece newPiece = {EDIT, currentPos, length};
                pieces[piecesCurrAmt] = new Piece(newPiece);
                ++piecesCurrAmt;
                return piecesCurrAmt - 1;
            }
        }
        // Case 3: Insertion between pieces (need to split a piece)
        if (pos > currentPos && pos < pieceEnd) {
            // Split the current piece at pos
            uint64_t splitOffset = pos - currentPos;
            // Create the "before" piece (start of the current piece up to the insertion point)
            Piece beforePiece = {piece.type, piece.offset, splitOffset};
            // Create the "after" piece (the remaining part of the current piece after the insertion point)
            Piece afterPiece = {piece.type, piece.offset + splitOffset, piece.length - splitOffset};
            // Replace the current piece with the "before" piece
            pieces[i] = new Piece(beforePiece);
            // Shift subsequent pieces down by 1
            for (uint32_t j = piecesCurrAmt; j > i + 1; --j) {
                pieces[j] = pieces[j - 1];
            }
            // Insert the new piece after the "before" piece
            Piece newPiece = {EDIT, pos, length};
            pieces[i + 1] = new Piece(newPiece);
            // Insert the "after" piece after the new piece
            pieces[i + 2] = new Piece(afterPiece);
            // Update the pieces count
            piecesCurrAmt += 2;
            return i + 1; // Return the index where the new piece was inserted
        }
        // Move the current position to the next piece's end
        currentPos = pieceEnd;
    }
    // If no case triggered, return an error (shouldn't happen with a valid position)
    return UINT32_MAX;
}
*/


uint32_t makeSlot(uint64_t pos, uint64_t textLength){
  //space already ensured via _checkExpandNeeded(), so can savely shift down when called via insert of delete
  uint64_t currentPos = 0; 
  for (uint32_t i = 0; i < piecesCurrAmt; ++i) {
    Piece& piece = *pieces[i];
    uint64_t pieceEnd = currentPos + piece.length;
    if(pos <= currentPos){ //before a piece, shift down 1 and insert
      for (uint32_t j = piecesCurrAmt; j > i; --j) {
        //start at end piece
        pieces[j] = pieces[j - 1];
      }
      //i SHOULD be empty
      return i;
    } 
    // start = piece.offset, end = pieceEnd
    // i = current piece index
    // to iter thru, want a total sum
    
  }
}

void PieceTable::insert(uint64_t pos, const char* text, uint64_t length){
  if(_checkExpandNeeded(length)){
    //&dest, &src, num bytes. &array + int = array address offset where empty starts!
    memcpy(editBuffer+editBufferLength, text, length);
    //pieces[piecesCurrAmt++] = new Piece {EDIT, editBufferLength, length};
    //editBufferLength += length;
    uint32_t theSlot = makeSlot(pos, length);
    piece[theSlot] = new Piece {EDIT, editBufferLength, length};
    editBufferLength+=length;
  }
  else{
    printf("FAILED TO EXPAND BUFFER WHEN NEEDED\nTHIS REALLY SHOULD BE A THROW!!!");
  }
}


void PieceTable::deleteRange(uint64_t pos, uint64_t length){

}
