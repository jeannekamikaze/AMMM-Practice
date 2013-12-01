#include "mutation.h"
#include "Chromosome.h"
#include <cstdlib>
#include <algorithm> // max

static double rand01 ()
{
    return (double) std::rand() / (double) RAND_MAX;
}

// Flip SxS block at (i,j).
static void flip_block (Chromosome& c, int i, int j, int B)
{
    for (int y = 0; y < B; ++y) {
        for (int x = 0; x < B; ++x) {
            if (rand01() < 0.5) c.flip(i+y,j+x);
        }
    }
}

// One-out SxS block at (i,j).
static void one_out_block (Chromosome& c, int i, int j, int B)
{
    for (int y = 0; y < B; ++y) {
        for (int x = 0; x < B; ++x) {
            c.set(i+y, j+x, 1);
        }
    }
}

void mutate_block (Chromosome& c, int N, int B)
{
    for (int n = 0; n < N; ++n) {
        // Choose a random block
        int xmin = std::rand() % (c.width - B);
        int ymin = std::rand() % (c.height - B);
        // Flip
        flip_block(c, xmin, ymin, B);
    }
}

void mutate_block_ones (Chromosome& c, int N, int B)
{
    for (int n = 0; n < N; ++n) {
        // Choose a random block
        int xmin = std::rand() % (c.width - B);
        int ymin = std::rand() % (c.height - B);
        // One-out
        one_out_block(c, xmin, ymin, B);
    }
}

void mutate_gene2gene (Chromosome& c, float p)
{
    int N = c.width*c.height;
    for (int i = 0; i < N; ++i) {
        if (rand01() < p) c.flip(i);
    }
}
