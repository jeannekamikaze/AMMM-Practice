#include "crossover.h"
#include "Chromosome.h"
#include <cstdlib>

static double rand01 ()
{
    return (double) std::rand() / (double) RAND_MAX;
}

static void copy_block
(const Chromosome& src, Chromosome& dest, int row, int col, int B)
{
    for (int i = row; i < row+B; ++i) {
        for (int j = col; j < col+B; ++j) {
            dest.set(i, j, src(i,j));
        }
    }
}

// Single cross overs

void cross_single
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs)
{
    Chromosome* c = new Chromosome(c1.width, c1.height, 0);
    const int N = c1.width*c1.height;
    int X = std::rand() % N;
    for (int i = 0; i < X; ++i) c->set(i, c1[i]);
    for (int i = X; i < N; ++i) c->set(i, c2[i]);
    cs.push_back(c);
}

void cross_multi
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs)
{
    Chromosome* c = new Chromosome(c1.width, c1.height, 0);
    const Chromosome* p = &c1;
    const Chromosome* q = &c2;
    int X = 0;
    int N = c1.width*c1.height;
    while (X < N) {
        int P = X + std::rand()%(N-X+1);
        for (int i = X; i < P; ++i) c->set(i, (*p)[i]);
        const Chromosome* t = p;
        p = q;
        q = t;
        X = P;
    }
    cs.push_back(c);
}

void cross_gene2gene
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs)
{
    cross_gene2gene_biased(c1, c2, cs, 0.5);
}

void cross_gene2gene_biased
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs, double pc1)
{
    Chromosome* c = new Chromosome(c1.width, c1.height, 0);
    for (int i = 0; i < c1.height; ++i) {
        for (int j = 0; j < c1.width; ++j) {
            const Chromosome& src = rand01() < pc1 ? c1 : c2;
            c->set(i, j, src(i,j));
        }
    }
    cs.push_back(c);
}

void cross_grid
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs, int B)
{
    cross_grid_biased(c1, c2, cs, B, 0.5);
}

void cross_grid_biased
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs, int B, double p)
{
    Chromosome* c = new Chromosome(c1.width, c1.height, 0);
    for (int i = 0; i <= c1.height-B; i += B) {
        for (int j = 0; j <= c1.width-B; j += B) {
            const Chromosome* x1 = rand01() < p ? &c1 : &c2;
            copy_block(*x1, *c, i, j, B);
            /*if (x1 == &c1) printf("1 ");
            else printf("2 ");*/
        }
        //printf("\n");
    }
    cs.push_back(c);
}

void cross_grid_variable
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs, int B)
{
    Chromosome* c = new Chromosome(c1.width, c1.height, 0);
    int row = 0;
    while (row <= c1.height-B)
    {
        row += B + 2*(std::rand() % B);
        row = std::min(c1.height-B, row);
        int col = 0;
        while (col <= c1.width-B)
        {
            col += B + 2*(std::rand() % B);
            col = std::min(c1.width-B, col);
            const Chromosome* x1 = rand01() < 0.5 ? &c1 : &c2;
            copy_block(*x1, *c, row, col, B);
        }
    }
    cs.push_back(c);
}

void cross_grid_variable_fit
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs, int B, const FitnessFunc& fitness)
{
    // Evaluate obj func at block for each chromosome and choose the best.
    Chromosome* c = new Chromosome(c1.width, c1.height, 0);
    int row = 0;
    while (row <= c1.height-B)
    {
        row += B + 2*(std::rand() % B);
        row = std::min(c1.height-B, row);
        int col = 0;
        while (col <= c1.width-B)
        {
            col += B + 2*(std::rand() % B);
            col = std::min(c1.width-B, col);
            double fit1 = fitness(c1, col, row, col+B, row+B);
            double fit2 = fitness(c2, col, row, col+B, row+B);
            if (fit1 < fit2) copy_block(c1, *c, row, col, B);
            else copy_block(c2, *c, row, col, B);
        }
    }
    cs.push_back(c);
}

// Dual cross overs

void cross_single_dual
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs)
{
    Chromosome* p1 = new Chromosome(c1.width, c1.height, 0);
    Chromosome* p2 = new Chromosome(c1.width, c1.height, 0);
    const int N = c1.width*c1.height;
    int X = std::rand() % N;
    for (int i = 0; i < X; ++i) {
        p1->set(i, c1[i]);
        p2->set(i, c2[i]);
    }
    for (int i = X; i < N; ++i) {
        p1->set(i, c2[i]);
        p2->set(i, c1[i]);
    }
    cs.push_back(p1);
    cs.push_back(p2);
}

void cross_gene2gene_dual
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs)
{
    Chromosome* p1 = new Chromosome(c1.width, c1.height, 0);
    Chromosome* p2 = new Chromosome(c1.width, c1.height, 0);
    for (int i = 0; i < c1.height; ++i) {
        for (int j = 0; j < c1.width; ++j) {
            const Chromosome* x1 = rand01() < 0.5 ? &c1 : &c2;
            const Chromosome* x2 = x1 == &c1 ? &c2 : &c1;
            p1->set(i, j, (*x1)(i,j));
            p2->set(i, j, (*x2)(i,j));
        }
    }
    cs.push_back(p1);
    cs.push_back(p2);
}

void cross_grid_dual
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs, int B)
{
    Chromosome* p1 = new Chromosome(c1.width, c1.height, 0);
    Chromosome* p2 = new Chromosome(c1.width, c1.height, 0);
    for (int i = 0; i <= c1.height-B; i+=B)
    {
        for (int j = 0; j <= c1.width-B; j+=B)
        {
            const Chromosome* x1 = rand01() < 0.5 ? &c1 : &c2;
            const Chromosome* x2 = x1 == &c1 ? &c2 : &c1;
            copy_block(*x1, *p1, i, j, B);
            copy_block(*x2, *p2, i, j, B);
        }
    }
    cs.push_back(p1);
    cs.push_back(p2);
}

void cross_grid_variable_dual
(const Chromosome& c1, const Chromosome& c2, std::vector<Chromosome*>& cs, int B)
{
    Chromosome* p1 = new Chromosome(c1.width, c1.height, 0);
    Chromosome* p2 = new Chromosome(c1.width, c1.height, 0);
    int row = 0;
    while (row <= c1.height-B)
    {
        row += B + 2*(std::rand() % B);
        row = std::min(c1.height-B, row);
        int col = 0;
        while (col <= c1.width-B)
        {
            col += B + 2*(std::rand() % B);
            col = std::min(c1.width-B, col);
            const Chromosome* x1 = rand01() < 0.5 ? &c1 : &c2;
            const Chromosome* x2 = x1 == &c1 ? &c2 : &c1;
            copy_block(*x1, *p1, row, col, B);
            copy_block(*x2, *p2, row, col, B);
        }
    }
    cs.push_back(p1);
    cs.push_back(p2);
}
