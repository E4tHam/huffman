
#include "huffman.h"

#include <unordered_map>

using namespace std;

int main() {

    Huffman::frequency_table ft;

    ft[ Symbol( 8, 0b1010011 ) ] = 3;
    ft[ Symbol( 8, 0b0011000 ) ] = 2;
    ft[ Symbol( 8, 0b0111000 ) ] = 21;
    ft[ Symbol( 8, 0b0000010 ) ] = 10;
    ft[ Symbol( 8, 0b1101001 ) ] = 6;
    ft[ Symbol( 8, 0b1100000 ) ] = 211;
    ft[ Symbol( 8, 0b0110101 ) ] = 101;
    ft[ Symbol( 8, 0b11101011 ) ] = 61;

    Huffman h( ft );

    // Huffman h( "file.txt", 8 );

    h.print();

    return 0;
}