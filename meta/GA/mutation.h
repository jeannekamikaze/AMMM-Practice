#pragma once

#include <functional>

class Chromosome;

//typedef void (*MutationFunc)(Chromosome&, double);

typedef std::function<void(Chromosome&, double)> MutationFunc;

// Block mutation.
// N is the number of blocks to mutate.
// B is the block size.
void mutate_block (Chromosome&, int N, int B);

// Block mutation.
// Mutated blocks are set to 1.
// N is the number of blocks to mutate.
// B is the block size.
void mutate_block_ones (Chromosome&, int N, int B);

// Gene by gene mutation.
// p is the probability of mutating a single gene.
void mutate_gene2gene (Chromosome&, double p);
