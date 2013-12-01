#include "Chromosome.h"
#include "Pool.h"
#include <cstring> // memcpy

Pool* pool;

static int chromosome_size (int width, int height) {
    int odd = (width/height) % 8 == 0 ? 0 : 1;
    int n = width*height/8 + odd;
    return sizeof(Chromosome)-1 + n;
}

void initialiseChromosomePool (int numChromosomes, int width, int height)
{
    pool = new Pool(chromosome_size(width, height), numChromosomes);
}

void destroyChromosomePool ()
{
    delete pool;
}

Chromosome::Chromosome (int _width, int _height, double _rank)
    : rank(_rank), width(_width), height(_height) {}

Chromosome::Chromosome (const Chromosome& c)
{
    rank = c.rank;
    width = c.width;
    height = c.height;
    int odd = (width/height) % 8 == 0 ? 0 : 1;
    int n = width*height/8 + odd;
    memcpy(data, c.data, n);
}

void* Chromosome::operator new (size_t)
{
    return pool->alloc();
}

void Chromosome::operator delete (void* p)
{
    pool->free(p);
}
