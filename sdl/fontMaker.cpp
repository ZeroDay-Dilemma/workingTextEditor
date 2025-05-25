#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>

// Function to write binary from a text file
void writeRawBinary(const std::string& inputTextFile, const std::string& outputBinaryFile) {
    std::ifstream inputFile(inputTextFile);
    if (!inputFile) {
        std::cerr << "Failed to open input text file.\n";
        return;
    }

    std::ofstream outputFile(outputBinaryFile, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Failed to open output binary file.\n";
        return;
    }

    char bitChar;
    uint8_t byte = 0;
    int bitCount = 0;

    while (inputFile.get(bitChar)) {
        if (bitChar == '0' || bitChar == '1') {
            // Shift the bit into the byte
            byte = (byte << 1) | (bitChar - '0');
            ++bitCount;

            // Write a full byte when we have 8 bits
            if (bitCount == 8) {
                outputFile.put(byte);
                byte = 0;
                bitCount = 0;
            }
        }
    }

    // Write the remaining bits if they don't fill a full byte
    if (bitCount > 0) {
        byte <<= (8 - bitCount); // Pad remaining bits with zeros
        outputFile.put(byte);
    }

    inputFile.close();
    outputFile.close();
    std::cout << "Binary file written successfully.\n";
}


int main(){
    const std::string inputTextFile = "inputfont.txt";   // Text file containing '0's and '1's
    const std::string outputBinaryFile = "myfont.wff"; // Raw binary file
    writeRawBinary(inputTextFile, outputBinaryFile);
}
