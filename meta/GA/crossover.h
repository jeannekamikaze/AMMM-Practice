#pragma once

#include <functional>
#include <vector>

class Chromosome;

// Single crossing

typedef void (*CrossFunc)(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&);

typedef std::function<double (const Chromosome&, int xmin, int ymin, int xmax, int ymax)> FitnessFunc;

void cross_single
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&);

void cross_multi
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&);

void cross_gene2gene
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&);

void cross_gene2gene_biased
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&, double p);

void cross_grid
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&, int B);

void cross_grid_biased
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&, int B, double p);

void cross_grid_variable
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&, int B);

void cross_grid_variable_fit
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&, int B, const FitnessFunc&);

// Dual crossing

void cross_single_dual
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&);

void cross_gene2gene_dual
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&);

// Dual grid crossing.
// B is the block size.
void cross_grid_dual
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&, int B);

void cross_grid_variable_dual
(const Chromosome&, const Chromosome&, std::vector<Chromosome*>&, int B);
