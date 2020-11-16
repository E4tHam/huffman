
/* huffman.h */


#ifndef _HUFFMAN_
#define _HUFFMAN_

#include <stdint.h>
#include <unordered_map>
#include <string>


class Symbol {

    public:

        class BadSymbolSize { };

        /* Default Constructor */
        Symbol() : size(0), word(0) { }

        /* Constructor */
        Symbol( const uint8_t & size, const uint64_t & word );

        /* Copy Constructor */
        Symbol( const Symbol & s );

        Symbol & add0();
        Symbol & add1();

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
        void print() const;

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
        class CouldNotOpenFile { };
        class BadEncodingSize { };

        typedef std::unordered_map< Symbol, size_t, SymbolHash > frequency_table;
        typedef std::unordered_map< Symbol, Symbol, SymbolHash > lookup_table;

        Huffman();
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
