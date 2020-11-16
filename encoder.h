
/* encoder.h */


#ifndef _ENCODER_
#define _ENCODER_

#include "huffman.h"

#include <stdint.h>
#include <string>
#include <vector>

class Encoder {
    public:
        Encoder() : buffer(0) { };
        Encoder( const std::string & filename, const uint8_t & encoding_size );

        ~Encoder() { delete [] buffer; }

        /* Read into Buffer then load into file_symbols */
        void fileIn( const std::string & filename, const uint8_t & encoding_size );

        /* Print */
        void fileOut() const;

        static const std::string extension;

    private:
        class FileNotLoaded { };
        class CouldNotOpenFile { };

        void load_buffer();
        void bufferToSymbols();
        void symbolsToHuffman();

        void writeEncoding( std::basic_ostream<char>& os ) const;
        void writeMessage( std::basic_ostream<char>& os ) const;

        std::string filename;
        uint8_t encoding_size;
        
        Huffman h;

        char * buffer;
        size_t buffer_size;
        std::vector<Symbol> file_symbols;
};

#endif