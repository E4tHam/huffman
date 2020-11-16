
/* encoder.cpp */


using namespace std;

#include "encoder.h"
#include <fstream>
#include <math.h>
#include <iostream>
#include <bitset>

const string Encoder::extension = "huff";


Encoder::Encoder( const std::string & filename, const uint8_t & encoding_size )
    : filename(filename),
      encoding_size(encoding_size) {

    load_buffer();
    bufferToSymbols();
    symbolsToHuffman();
}

void Encoder::fileIn( const std::string & filename, const uint8_t & encoding_size ) {
    this->filename = filename;
    this->encoding_size = encoding_size;

    load_buffer();
    bufferToSymbols();
    symbolsToHuffman();
}

void Encoder::load_buffer() {
    ifstream ifs( filename );
    
    if ( !ifs )
        throw CouldNotOpenFile();

    // get length of file:
    ifs.seekg ( 0, ifs.end );
    size_t length = ifs.tellg();
    ifs.seekg ( 0, ifs.beg );


    // set buffer size
    buffer_size = length;
    while ( ( buffer_size * 8 ) % encoding_size )
        buffer_size++;

    // load into buffer
    buffer = new char[ buffer_size ];
    ifs.read( buffer, length );
    ifs.close();

    // fill remaining spots in buffer
    for ( size_t i = length; i < buffer_size; i++ )
        buffer[ i ] = 0xFF;

    // cout << "Printing Buffer:\n";
    // for ( size_t i = 0; i < buffer_size; i++ )
    //     cout << std::bitset<8>( buffer[i] ).to_string() << std::endl;
    // cout << "\n";
}

void Encoder::bufferToSymbols() {
    
    size_t i = ( encoding_size-1 ) / 8;
    uint8_t offset = ( 8 - ( encoding_size ) % 8 ) % 8 ;
    /*  00000000 offset: 0
            0xxxxxxx offset: 7  */

    while ( i < buffer_size ) {

        // set buffer focus length
        uint8_t buffer_focus = ceil( (float) encoding_size / 8 );
        buffer_focus += ( offset > 8 - ( encoding_size % 8 ) );


        // fill word
        uint64_t word = 0;
        for ( size_t j = 0; j < buffer_focus; j++ ) {
            uint64_t toAdd = (unsigned char) buffer[ i - j ];

            toAdd <<= 8*j;
            toAdd >>= offset;

            word |= toAdd;
        }

        // add word to file_symbols
        file_symbols.emplace_back( encoding_size, word );

        // increment i
        i += encoding_size / 8;
        i += ( offset <= ( encoding_size % 8 ) - 1 );

        // increment offset
        offset += 8 - ( encoding_size ) % 8 ;
        offset %= 8;

    }

    // cout << "Printing Symbols:\n";
    // for ( auto i : file_symbols )
    //     i.print();
    // cout << "\n";
}

void Encoder::symbolsToHuffman() {
    Huffman::frequency_table ft; 
    for ( auto i : file_symbols ) {
        if ( ft.find( i ) != ft.end() )
            ft[ i ]++;
        else
            ft[ i ] = 1;
    }
    h = Huffman( ft );
}

void Encoder::fileOut() const {
    if ( buffer == 0 )
        throw FileNotLoaded();
    
    ofstream ofs( filename + "." + extension );

    writeEncoding( ofs );
    writeMessage( ofs );

    ofs.close();
}

void Encoder::writeEncoding( std::basic_ostream<char>& os ) const {

}
void Encoder::writeMessage( std::basic_ostream<char>& os ) const {
    
}