
/* huffman.h */

#ifndef _HUFFMAN_
#define _HUFFMAN_

#include <stdint.h>
#include <unordered_map>
#include <iostream>
#include <bitset>
#include <string>


class Symbol {

    public:

        class BadSymbolSize { };

        Symbol() : size(0), word(0) { }

        /* Constructor */
        Symbol( uint8_t size, uint64_t word )
            : size(size), word(word) {
            uint64_t mask = 0x03FFFFFFFFFFFFFF;
            word &= ( mask >> (58-size) );
            if ( size > 58 )
                throw BadSymbolSize();
        }

        Symbol( const Symbol & s ) {
            size = s.size;
            word = s.word;
        }

        Symbol & add0() {
            if ( size == 58 )
                throw BadSymbolSize();
            size++;
            word <<= 1;
            return *this;
        }
        Symbol & add1() {
            if ( size == 58 )
                throw BadSymbolSize();
            word <<= 1;
            word |= 1;
            size++;
            return *this;
        }

        /* Get Size of Enocoded String */
        uint8_t getSize() const {
            return size;
        }
        /* Get Data of Enocoded String */
        uint64_t getWord() const {
            return word;
        }
        /* Get All Bits of Symbol Object */
        const uint64_t & to_unit64_t() const {
            return *( uint64_t * ) ( ( void * ) this );
        }

        /* For Hash Table */
        bool operator== ( const Symbol & s ) const {
            return to_unit64_t() == s.to_unit64_t() ;
        }

        /* Print */
        void print() const {
            if ( to_unit64_t() )
                std::cout << std::bitset<58>( word ).to_string().substr( 58 - size ) << std::endl;
            else
                std::cout << "x\n";
        }

    private:
        uint64_t word : 58 ;
        uint64_t size :  6 ;

};

struct SymbolHash {
    size_t operator() ( const Symbol & s ) const ;
};


struct node {
    node( const Symbol & s, size_t frequency )
        : left(0), right(0),
          frequency(frequency),
          name(s) { }

    node( node * left, node * right );

    ~node();

    node * left;
    node * right;
    size_t frequency;
    const Symbol name;
};
struct node_compare {
    bool operator()( const node * n1, const node * n2 ) const;
};


class Huffman {

    public:
        typedef std::unordered_map< Symbol, size_t, SymbolHash > frequency_table;
        typedef std::unordered_map< Symbol, Symbol, SymbolHash > lookup_table;

        Huffman();
        Huffman( const std::string & filename, const uint8_t & encoding_size = 16 );
        Huffman( const frequency_table & );

        void print();

    private:
        void fill_tree( const frequency_table & );
        void fill_tables( node * n, const Symbol & encoding );
        uint8_t encoding_size;
        lookup_table encoding_table;
        lookup_table decoding_table;

};

#endif
