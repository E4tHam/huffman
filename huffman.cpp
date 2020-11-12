
/* huffman.cpp */

#include "huffman.h"

#include <vector>
#include <utility>
#include <algorithm>
#include <queue>
#include <math.h>

#include <fstream>


using namespace std;



bool node_compare::operator()( const node * n1, const node * n2 ) const {
    return n1->frequency > n2->frequency;
}

node::~node() {
    if ( left ) {
        delete left;
        left = 0;
    }
    if ( right ) {
        delete right;
        right = 0;
    }
}

node::node( node * left, node * right )
    : left(left), right(right),
    name() {
        frequency = ( left  ? left->frequency  : 0 )
                  + ( right ? right->frequency : 0 );
}



Huffman::Huffman() { }

class CouldNotOpenFile { };
class BadEncodingSize { };

struct counter {

    counter( const uint8_t & encoding_size, const size_t & filelength )
        : offset1(0),
          offset2( encoding_size - 1 ),
          encoding_size(encoding_size),
          i(0),
          filelength(filelength) {

        if ( ( filelength << 3 ) % encoding_size  != 0 )
            throw BadEncodingSize();

        size = ceil( (float) encoding_size / 8 );

        mask = 0x03FFFFFFFFFFFFFF;
        mask >>= 58 - encoding_size;

    }

    bool hasNext() const {
        return ( i != filelength-1 ) && ( offset2 != 7 );
    }
    void increment() {
        i += ( encoding_size + offset1 ) / 8;
        size = ceil( (float) ( encoding_size + offset1 ) / 8 );
        offset1 = offset2 + 1;
        offset2 = offset1 + encoding_size - 1;
    }

    uint8_t offset1 : 3;
    uint8_t offset2 : 3;

    uint8_t encoding_size : 6;

    size_t i;
    size_t filelength;
    uint8_t size;

    uint64_t mask;

};


Huffman::Huffman( const string & filename, const uint8_t & encoding_size )
    : encoding_size(encoding_size) {
    frequency_table ft;

    ifstream ifs( filename, ifstream::binary );

    if ( !ifs ) throw CouldNotOpenFile();

    // get length of file:
    ifs.seekg ( 0, ifs.end );
    size_t length = ifs.tellg();
    ifs.seekg ( 0, ifs.beg );

    char * buffer = new char[ length ];
    ifs.read( buffer, length );
    ifs.close();


    counter c( encoding_size, length );

    while ( c.hasNext() ) {

        uint64_t word;

        // Set word to all 8 bytes before and including offset2
        word = *(
            ( uint64_t * )(
                ( void * ) ( buffer + ( c.i + c.size - 8 ) )
            )
        );

        // shift word according to offset2
        word >> 7 - c.offset2;

        // or in byte according to offset2
        word |= ( *(
            ( uint8_t * )(
                ( void * ) ( buffer + ( c.i + c.size - 9 ) )
            )
        ) ) << ( 56 + c.offset2 - 7 ) ;

        word &= c.mask;

        Symbol temp( word, encoding_size );

        if ( ft.find( temp ) != ft.end() )
            ft[ temp ] += 1;
        else
            ft[ temp ] = 1;

        c.increment();
    }


    delete [] buffer;

    fill_tree( ft );
}
Huffman::Huffman( const frequency_table & ft ) {
    encoding_size = ft.begin()->first.getSize();
    fill_tree( ft );
}





void Huffman::fill_tree( const frequency_table & ft ) {

    priority_queue< node*, vector<node*>, node_compare > min_heap;

    for ( auto i : ft )
        min_heap.push( new node( i.first, i.second ) );

    while ( min_heap.size() > 1 ) {

        node * n1 = min_heap.top();
        min_heap.pop();
        node * n2 = min_heap.top();
        min_heap.pop();

        min_heap.emplace( new node( n1, n2 ) );
    }

    fill_tables( min_heap.top(), Symbol() );

    delete min_heap.top();
}

void Huffman::fill_tables( node * n, const Symbol & encoding ) {

    if ( n->left )
        fill_tables( n->left, Symbol(encoding).add0() );
    if ( n->right )
        fill_tables( n->right, Symbol(encoding).add1() );

    if ( n->name.to_unit64_t() ) {
        encoding_table[ n->name ] = Symbol(encoding);
        decoding_table[ encoding ] = Symbol(n->name);
    }
}


void Huffman::print() {
    for ( auto i : encoding_table ) {
        cout << "name: "; i.first.print();
        cout << "enco: "; i.second.print();
        cout << '\n';
    }
}



size_t SymbolHash::operator() ( const Symbol & s ) const {
    return hash<uint64_t>() (
        s.to_unit64_t()
    );
}
