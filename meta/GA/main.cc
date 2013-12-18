#include "Chromosome.h"
#include "mutation.h"
#include "crossover.h"
#include "../ObjFunc.h"
#include "../ParseDatFile.hpp"
#include <opencv2/opencv.hpp>
#include <functional>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <cmath>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <SDL2/SDL.h>

/// GA

// Parameters

int width;  // Chromosome width.
int height; // Chromosome height.
int blockSizeMin; // Minimum mutation block size.
int blockSizeMax; // Maximum mutation block size.

int population_size = 20;    // Number of chromosomes.
int steps = 10000;           // Number of steps to perform.
int nElite = 3;              // Maximum number of chromosomes that can be classified as elite.
double pMutElite = 0.3;      // Probability of mutating a secondary (not the first) elite chromosome.
double pMutNonElite = 0.5;   // Probability of mutating a non-elite block.
int nMutBlocks = 4;          // Number of blocks to mutate in a chromosome.
int nMutants = 0;            // Number of mutants to inject at each step.
double childAbias = 0.8;     // The probability that a gene is selected from the elite parent in crossover.
double rBlockSizeMax = 0.1;  // blockSizeMax = max(width, height) * rBlockSizeMax

//CrossFunc cross = cross_single_dual;

// Objective function parameters
cv::Mat d;
cv::Mat w;
ObjFunc::R lambda = 0.5;

// Variables

typedef std::vector<Chromosome*> CVector;

CVector curGen;   // Current generation of chromosomes.
CVector nextGen;  // Next generation of chromosomes.
CVector elite;    // Current generation chromosomes classified as elite.
CVector nonElite; // Current generation, non-elite chromosomes.
CVector tmpCVector;

double avg_rank;
double min_rank;
double max_rank;
double rank_stdev;

// Initialisation

void setupGA (int width, int height, const cv::Mat& d, const cv::Mat& w)
{
    ::width = width;
    ::height = height;
    ::blockSizeMin = 2;
    ::blockSizeMax = (int) ((double)std::max(width, height) * (double)rBlockSizeMax);
    ::d = d;
    ::w = w;
    initialiseChromosomePool(population_size*2, width, height);
}

void destroyGA ()
{
    destroyChromosomePool();
}

// Functions

double rand01 ()
{
    return (double) std::rand() / (double) RAND_MAX;
}

int ilerp(int a, int b, double t)
{
    return (int) ((double)a + t*((double)(b-a)));
}

void binarise (const cv::Mat& d, Chromosome& c, float th)
{
    for (int i = 0; i < d.rows; ++i) {
        for (int j = 0; j < d.cols; ++j) {
            if (d.at<float>(i,j) < th) c.set(i,j,0);
            else c.set(i,j,1);
        }
    }
}

void randomise (Chromosome& c)
{
    const int N = width*height;
    for (int i = 0; i < N; ++i) c.set(i, std::rand()%2);
}

void zero (Chromosome& c)
{
    const int N = width*height;
    for (int i = 0; i < N; ++i) c.set(i, 0);
}

// Initialise the population.
void initialise ()
{
    curGen.push_back(new Chromosome(width, height, 0));
    //zero(*curGen[0]);
    //randomise(*curGen[0]);
    binarise(d, *curGen[0], 0.0f);
    float th_step = 1.0f / (float) population_size;
    float th = th_step;
    for (int i = 1; i < population_size; ++i, th += th_step) {
        /*curGen.push_back(new Chromosome(*curGen[0]));
        mutate(*curGen[i], pMutElite);*/
        curGen.push_back(new Chromosome(width, height, 0));
        //zero(*curGen[i]);
        //randomise(*curGen[i]);
        binarise(d, *curGen[0], th);
    }
}

bool compare_chromosomes (const Chromosome* c1, const Chromosome* c2)
{
    return c1->rank < c2->rank;
}

// Evaluate the given chromosome.
void evaluate (Chromosome& c)
{
    c.rank = 0.0;
    c.rank = ObjFunc::eval(width, height, lambda, d, w, c);
    // Flag
    /*for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            if (j < c.width/2) c.rank -= c(i,j); // ding
            else c.rank += c(i,j); // penalty
        }
    }*/
    // y = x
    /*for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            if (i == j) c.rank -= c(i,j); // ding
            else c.rank += 2*c(i,j); // penalty
        }
    }*/
    // y = x^2
    /*for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            if (i == j*j) c.rank -= c(i,j); // ding
            else c.rank += 1.2*c(i,j); // penalty
        }
    }*/
    // Ones problem
    /*c.rank = 0.0;
    for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            c.rank += c(i,j) ? 0 : 1;
        }
    }*/
}

// Evaluate the population.
void evaluate ()
{
    for (Chromosome* c : curGen) evaluate(*c);
    std::sort(curGen.begin(), curGen.end(), compare_chromosomes);
    
    // Compute average, min, and max ranks
    avg_rank = 0.0;
    min_rank = curGen[0]->rank;
    max_rank = curGen[0]->rank;
    for (Chromosome* c : curGen) {
        avg_rank += c->rank;
        min_rank = std::min(min_rank, c->rank);
        max_rank = std::max(max_rank, c->rank);
    }
    avg_rank /= (double) population_size;
    
    // Compute stdev
    rank_stdev = 0.0;
    for (Chromosome* c : curGen) {
        double d = c->rank-avg_rank;
        rank_stdev += d*d;
    }
    rank_stdev = std::sqrt(rank_stdev);
}

// Classify chromosomes as elite or non-elite.
void classify ()
{
    // Classify elite chromosomes.
    
    tmpCVector.clear();
    for (int i = 0; i < population_size; ++i) {
        Chromosome* c = curGen[i];
        if (c->rank <= avg_rank) tmpCVector.push_back(c);
    }
    
    if (tmpCVector.size() == 0) {
        for (int i = 0; i < nElite; ++i) tmpCVector.push_back(curGen[i]);
    }
    
    std::sort(tmpCVector.begin(), tmpCVector.end(), compare_chromosomes);
    
    elite.clear();
    int numElite = std::min(tmpCVector.size(), (size_t) nElite);
    for (int i = 0; i < numElite; ++i) {
        elite.push_back(tmpCVector[i]);
    }
    tmpCVector.clear();
    
    // Classify non-elite chromosomes.
    
    nonElite.clear();
    for (int i = 0; i < population_size; ++i) {
        Chromosome* c = curGen[i];
        bool isElite = false;
        for (size_t i = 0; i < elite.size(); ++i) {
            if (elite[i] == c) {
                isElite = true;
                break;
            }
        }
        if (!isElite) {
            nonElite.push_back(c);
        }
    }
}

// Copy elite chromosomes to the next generation.
void copy_elite ()
{
    for (Chromosome* c : elite) {
        nextGen.push_back(new Chromosome(*c));
    }
}

// Mutate chromosomes.
void mutate ()
{
    // Mutate secondary elite.
    if (elite.size() > 1) {
        for (size_t i = 1; i < elite.size(); ++i) {
            if (rand01() < pMutElite) {
                int B = ilerp(blockSizeMin, blockSizeMax, rand01());
                //mutate_block_ones(*elite[i], nMutBlocks, B);
                //mutate_block(*elite[i], nMutBlocks, B);
                if (rand01() < 0.5f) mutate_block_ones(*elite[i], nMutBlocks, B);
                else mutate_block_zeroes(*elite[i], nMutBlocks, B);
            }
        }
    }
    
    // Mutate non-elite.
    for (Chromosome* c : nonElite) {
        if (rand01() < pMutNonElite) {
            int B = ilerp(blockSizeMin, blockSizeMax, rand01());
            //mutate_block_ones(*c, nMutBlocks, B);
            //mutate_block_ones(*c, nMutBlocks, B);
            if (rand01() < 0.5f) mutate_block_ones(*c, nMutBlocks, B);
            else mutate_block_zeroes(*c, nMutBlocks, B);
        }
    }
}

// Step the simulation.
// A set of 2N chromosomes are selected and crossed.
// Non-elite chromosomes are mutated.
// Fresh new chromosomes are introduced into the next generation.
void cross_population ()
{
    //int B = ilerp(blockSizeMin, blockSizeMax, 0.5f);
    //int N = (population_size - elite.size() - nMutants)/2;
    int N = population_size - elite.size() - nMutants;
    for (int i = 0; i < N; ++i) {
        Chromosome* c1 = elite[std::rand() % elite.size()];
        Chromosome* c2 = nonElite[std::rand() % nonElite.size()];
        cross_multi(*c1, *c2, nextGen);
        //cross_grid(*c1, *c2, nextGen, B);
        //cross_gene2gene_biased(*c1, *c2, nextGen, childAbias);
        //cross_grid_biased(*c1, *c2, nextGen, B, childAbias);
    }
}

void inject_mutants ()
{
    for (int i = 0; i < nMutants; ++i) {
        Chromosome* c = new Chromosome(width, height, 0.0);
        randomise(*c);
        nextGen.push_back(c);
    }
}

// Swap the chromosome generation buffers.
void swapGenerations ()
{
    for (Chromosome* c : curGen) delete c;
    curGen.clear();
    for (Chromosome* c : nextGen) curGen.push_back(c);
    nextGen.clear();
}

typedef std::function<void
    (int stepNumber,
     const CVector& elite,
     const CVector& nonElite,
     const CVector& generation)> OnNewGenerationFunc;

Chromosome& evolve (const OnNewGenerationFunc& onNewGeneration)
{
    curGen.reserve(population_size);
    nextGen.reserve(population_size);
    elite.reserve(population_size);
    nonElite.reserve(population_size);
    tmpCVector.reserve(population_size);
    
    printf("Initialising GA\n");
    initialise();
    
    printf("Evolving population (steps = %d)\n", steps);
    for (int i = 1; i <= steps; ++i) {
        evaluate();
        classify();
        copy_elite();
        mutate();
        cross_population();
        inject_mutants();
        onNewGeneration(i, elite, nonElite, nextGen);
        swapGenerations();
        fflush(stdout);
    }
    printf("\n");
    evaluate();
    
    return *curGen[0];
}

///
/// MAIN
///

SDL_Window* wnd = 0;
SDL_Renderer* rend = 0;
SDL_Texture* bmpImage = 0;
const int window_width = 512;
const int window_height = 512;
int xscale;
int yscale;
std::string solutionFile;
enum RenderMode { DRAW_IMAGE, DRAW_DEBUG };
RenderMode render_mode = DRAW_IMAGE;
int renderModeKey = 0;

void print_statistics (int stepNumber)
{
    printf("%05d: best %f, worst %f, avg %f, stdev %f\n",
           stepNumber, min_rank, max_rank, avg_rank, rank_stdev);
    
    int n = 1;
    
    // Print chromosomes.
    for (size_t i = 0; i < elite.size(); ++i) {
        printf("%02d: %f (elite)\n", n, elite[i]->rank);
        n++;
    }
    
    for (size_t i = 0; i < nonElite.size(); ++i) {
        printf("%02d: %f\n", n, nonElite[i]->rank);
        n++;
    }
}

void draw_chromosome (SDL_Renderer* rend, const Chromosome& c)
{
    SDL_Rect rect;
    rect.w = xscale;
    rect.h = yscale;
    for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            if (c(i,j)) {
                rect.x = j*xscale;
                rect.y = i*yscale;
                SDL_RenderFillRect(rend, &rect);
            }
        }
    }
}

void draw_chromosome_complementary (SDL_Renderer* rend, const Chromosome& c)
{
    SDL_Rect rect;
    rect.w = xscale;
    rect.h = yscale;
    for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            if (!c(i,j)) {
                rect.x = j*xscale;
                rect.y = i*yscale;
                SDL_RenderFillRect(rend, &rect);
            }
        }
    }
}

void draw_population (SDL_Renderer* rend, const CVector& chroms, int start, int end)
{
    SDL_Rect rect;
    rect.w = xscale;
    rect.h = yscale;
    const int h = chroms[0]->height;
    const int w = chroms[0]->width;
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int val = 0;
            for (int x = start; x < end; ++x) {
                const Chromosome* c = chroms[x];
                if ((*c)(i,j)) {
                    val = 1;
                    break;
                }
            }
            if (val) {
                rect.x = j*xscale;
                rect.y = i*yscale;
                SDL_RenderFillRect(rend, &rect);
            }
        }
    }
}

void onNewGeneration
(int stepNumber, const CVector& elite, const CVector& nonElite, const CVector& gen)
{
    // Console
    
    print_statistics(stepNumber);
    
    // SDL
    
    // Change render mode when key is pressed
    SDL_PumpEvents();
    const Uint8* state = SDL_GetKeyboardState(NULL);
    int renderModeKey_prev = renderModeKey;
    renderModeKey = state[SDL_SCANCODE_F];
    int change = renderModeKey ^ renderModeKey_prev;
    if (renderModeKey && change) render_mode = (RenderMode)(1 - render_mode);
    
    // Clear
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
    SDL_RenderClear(rend);
    if (render_mode == DRAW_IMAGE) {
        // Draw image
        SDL_Rect imgRect = {0, 0, width, height};
        SDL_Rect vp = {0, 0, window_width, window_height};
        SDL_RenderCopy(rend, bmpImage, &imgRect, &vp);
        // Draw best
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_MOD);
        SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
        draw_chromosome_complementary(rend, *elite[0]);
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    }
    else {
        // Draw mutants
        SDL_SetRenderDrawColor(rend, 128, 0, 0, 255);
        draw_population(rend, gen, population_size-nMutants, population_size);
        // Draw non-mutants
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rend, 0, 128, 0, 200);
        draw_population(rend, nonElite, 0, nonElite.size());
        // Draw best
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        draw_chromosome(rend, *elite[0]);
    }
    // Present
    SDL_RenderPresent(rend);
}

void write_pgm (const cv::Mat& mat, const char* file)
{
    std::fstream f(file, fstream::out);
    f << "P2 " << mat.cols << " " << mat.rows << " 1\n";
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            if (mat.at<float>(i,j) == 0.0) f << "0 ";
            else f << "1 ";
        }
    }
    f.close();
}

void write_pgm (const Chromosome& c, const char* file)
{
    std::fstream f(file, fstream::out);
    f << "P2 " << c.width << " " << c.height << " 1\n";
    for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            if (c(i,j) == 0) f << "0 ";
            else f << "1 ";
        }
    }
    f.close();
}

cv::Mat binarise (const cv::Mat& d)
{
    cv::Mat bind = d.clone();
    for (int i = 0; i < bind.rows; ++i) {
        for (int j = 0; j < bind.cols; ++j) {
            if (bind.at<float>(i,j) >= 0.5) bind.at<float>(i,j) = 0.0;
            else bind.at<float>(i,j) = 1.0;
        }
    }
    return bind;
}

void print_chromosome (const Chromosome& c)
{
    for (int i = 0; i < c.height; ++i) {
        for (int j = 0; j < c.width; ++j) {
            printf("%d ", c(i,j));
        }
        printf("\n");
    }
}

void shutdown (const Chromosome& best)
{
    printf("\nWriting solution to %s\n", solutionFile.c_str());
    write_pgm(best, solutionFile.c_str());
    destroyGA();
    if (bmpImage) SDL_DestroyTexture(bmpImage);
    if (rend) SDL_DestroyRenderer(rend);
    if (wnd) SDL_DestroyWindow(wnd);
    SDL_Quit();
}

void on_signal (int)
{
    const Chromosome* best = curGen.size() > 0 ? curGen[0] : nextGen[0];
    shutdown(*best);
    exit(0); 
}

int main (int argc, char** argv)
{
    using namespace std;
    
    srand(time(NULL)*getpid());
    
    if (argc != 2) {
        fprintf(stderr, "%s <file name>\n", argv[0]);
        return 0;
    }
    
    std::string fileName = argv[1];
    std::string dat = fileName + ".dat";
    std::string bmp = fileName + ".bmp";
    solutionFile = fileName + "_sol.pgm";
    
    struct sigaction sigIntHandler;
    
    sigIntHandler.sa_handler = on_signal;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    
    sigaction(SIGINT, &sigIntHandler, NULL);
    
    ParseDatFile parse(dat);
    parse.parseDandZ(d, w);
    
    cv::Mat bind = binarise(d);
    write_pgm(bind, "d.pgm");
    
    // Set parameters
    width  = d.cols;
    height = d.rows;
    setupGA(width, height, d, w);
    
    xscale = window_width / width;
    yscale = window_height / height;
    
    // Setup SDL
    SDL_Init(SDL_INIT_VIDEO);
    wnd = SDL_CreateWindow(
                "BinImg",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                window_width, window_height,
                SDL_WINDOW_SHOWN);
    
    rend = SDL_CreateRenderer(
                wnd,
                -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    SDL_Surface* bmpSurf = SDL_LoadBMP(bmp.c_str());
    bmpImage = SDL_CreateTextureFromSurface(rend, bmpSurf);
    SDL_FreeSurface(bmpSurf);
    
    // Evolve
    const Chromosome& best = evolve(onNewGeneration);
    shutdown(best);
    return 0;
}
