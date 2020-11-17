
/* huffman.cpp */


#include "huffman.h"

#include <algorithm>
#include <queue>
#include <math.h>

#include <iostream>
#include <fstream>
#include <bitset>


using namespace std;



/* ====== Symbol ====== */

Symbol::Symbol( const uint8_t & size, const uint64_t & word )
    : size(size), word(word) {

    uint64_t mask = 0x03FFFFFFFFFFFFFF;
    this->word &= ( mask >> (58-size) );
    if ( size > 58 ) throw BadSymbolSize();
}
Symbol::Symbol( const Symbol & s ) {
    size = s.size;
    word = s.word;
}

Symbol & Symbol::add0() {
    if ( size == 58 )
        throw BadSymbolSize();
    size++;
    word <<= 1;
    return *this;
}
Symbol & Symbol::add1() {
    if ( size == 58 )
        throw BadSymbolSize();
    word <<= 1;
    word |= 1;
    size++;
    return *this;
}

void Symbol::print() const {
    if ( to_unit64_t() )
        std::cout << std::bitset<58>( word ).to_string().substr( 58 - size ) << std::endl;
    else
        std::cout << "x\n";
}

size_t SymbolHash::operator() ( const Symbol & s ) const {
    return hash<uint64_t>() (
        s.to_unit64_t()
    );
}



/* ====== node ====== */

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

bool node_compare::operator()( const node * n1, const node * n2 ) const {
    return n1->frequency > n2->frequency;
}




/* ====== Huffman ====== */


Huffman::Huffman() { }

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

Symbol Huffman::encode( const Symbol & s ) const {
    auto it = encoding_table.find( s );
    if ( it == encoding_table.end() )
        throw NoSuchSymbol();
    return it->first;
}
Symbol Huffman::decode( const Symbol & s ) const {
    auto it = decoding_table.find( s );
    if ( it == decoding_table.end() )
        throw NoSuchSymbol();
    return it->first;
}

void Huffman::print() {
    for ( auto i : encoding_table ) {
        cout << "name: "; i.first.print();
        cout << "enco: "; i.second.print();
        cout << '\n';
    }
}
