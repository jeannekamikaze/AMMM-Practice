#pragma once

#include <cstddef> // size_t

class Chromosome
{
public:

    double rank;
    int width;
    int height;
    
private:
    
    char data[1];
    
    Chromosome& operator= (const Chromosome&);
    
public:
    
    Chromosome (int _width, int _height, double _rank);
    
    Chromosome (const Chromosome& c);
    
    // Return the bit at (row, col).
    char operator() (int row, int col) const {
        return (*this)[row*width + col];
    }
    
    // Return the ith bit.
    char operator[] (int i) const {
        char shift = i & 7;
        return (data[i>>3] & (1 << shift)) >> shift;
    }
    
    // Set the bit at (row, col).
    void set (int row, int col, char bit) {
        set(row*width + col, bit);
    }
    
    // Set the ith bit.
    void set (int i, char bit) {
        char mask = 1 << (i & 7);
        if (bit) data[i>>3] |= mask;
        else     data[i>>3] &= ~mask;
    }
    
    // Flip the bit at (row, col).
    void flip (int row, int col) {
        flip(row*width + col);
    }
    
    // Flip the ith bit.
    void flip (int i) {
        char mask = 1 << (i & 7);
        data[i>>3] ^= mask;
    }
    
    void* operator new (size_t);
    
    void operator delete (void*);
};

void initialiseChromosomePool (int numChromosomes, int width, int height);

void destroyChromosomePool ();
