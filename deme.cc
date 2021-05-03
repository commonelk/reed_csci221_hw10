/*
 * Declarations for Deme class to evolve a genetic algorithm for the
 * travelling-salesperson problem.  A deme is a population of individuals.
 */

#include "chromosome.hh"
#include "deme.hh"

#include <cmath>
#include <cfloat>
#include <algorithm>

using chrom_pair_t = std::pair<Chromosome*, Chromosome*>;
using chrom_pairs_t = std::vector<chrom_pair_t>;

// Generate a Deme of the specified size with all-random chromosomes.
// Also receives a mutation rate in the range [0-1].
Deme::Deme(const Cities* cities_ptr, unsigned pop_size, double mut_rate) : mut_rate_(mut_rate) {
    std::uniform_real_distribution<double> distr(0.0, std::nextafter(1.0, DBL_MAX)); // Effectively creates inclusive distribution on [0,1]
    for(unsigned int i = 0; i < pop_size; i++) {
        auto chrom = new Chromosome(cities_ptr);
        if(distr(generator_) < mut_rate_) { chrom -> mutate(); }
        pop_.push_back(chrom);
    }
}

// Deletes each Chromosome* in pop_ to prevent memory leaks
Deme::~Deme() { clean_pop(); }

// Helper function that deletes all Chromosome* in pop_ and clears pop_ (used in destructor and compute_next_generation())
void Deme::clean_pop() {
    std::for_each(pop_.begin(), pop_.end(), [](Chromosome* chrom) { delete chrom; } );
    pop_.clear();
}

// Helper function to clean / free memory from a chrom_pairs_t vector (used in compute_next_generation())
void clean_chrom_pairs_t(chrom_pairs_t& pairs) {
    std::for_each(pairs.begin(), pairs.end(), [](chrom_pair_t pair) { delete pair.first; delete pair.second; } );
    pairs.clear();
}

// Evolve a single generation of new chromosomes, as follows:
// We select pop_size/2 pairs of chromosomes (using the select() method below).
// Each chromosome in the pair can be randomly selected for mutation, with
// probability mut_rate, in which case it calls the chromosome mutate() method.
// Then, the pair is recombined once (using the recombine() method) to generate
// a new pair of chromosomes, which are stored in the Deme.
// After we've generated pop_size new chromosomes, we delete all the old ones.
void Deme::compute_next_generation() {
    std::vector<std::pair<Chromosome*, Chromosome*>> parent_pairs;
    const int pop_size = pop_.size(); // We will be changing pop_ over each iteration.

    // Get pairs of parents
    chrom_pair_t parent_pair;
    for(int i = 0; i < pop_size/2; i++) {
        for(unsigned int j = 0; j < 2; j++) {
            auto parent = select_parent();
            auto parent_iter = std::find(pop_.begin(), pop_.end(), parent);
            pop_.erase(parent_iter);
            if(j == 0) { parent_pair.first = parent; }
            else if(j == 1) { parent_pair.second = parent; }
        }
        parent_pairs.push_back(parent_pair);
    }

    // Randomly mutate parents based on mut_rate_
    std::uniform_real_distribution<double> distr(0.0, std::nextafter(1.0, DBL_MAX));
    for(chrom_pair_t pair : parent_pairs) {
        double r1 = distr(generator_), r2 = distr(generator_);
        if(r1 < mut_rate_) { pair.first -> mutate(); }
        if(r2 < mut_rate_) { pair.second -> mutate(); }
    }

    // Generate new children and create new pop_
    clean_pop(); // Cleans out any remaining chromosomes that weren't selected as parents (happens if pop_size is odd)
    for(chrom_pair_t parent_pair : parent_pairs) {
        auto child_pair = parent_pair.first -> recombine(parent_pair.second);
        pop_.push_back(child_pair.first);
        pop_.push_back(child_pair.second);
    }
    
    // Lastly, unallocate memory of previous generation Chromosome*
    clean_chrom_pairs_t(parent_pairs);
}

// Return a pointer (not a clone(), clarification from Eitan) of the chromosome with the highest fitness.
const Chromosome* Deme::get_best() const {
    Chromosome* best_chrom;
    double min_dist = DBL_MAX;
    for(Chromosome* chrom : pop_) {
        auto dist = chrom -> calculate_total_distance();
        if(dist < min_dist) { min_dist = dist; best_chrom = chrom; }
    }
    return best_chrom;
}

// Randomly select a chromosome in the population based on fitness and
// return a pointer to that chromosome.
Chromosome* Deme::select_parent() {
    double fitness_sum = 0.0, partial_sum = 0.0;
    for(Chromosome* chrom : pop_) { fitness_sum += chrom -> get_fitness(); } // Compute fitness_sum

    std::uniform_real_distribution<double> distr(0.0, std::nextafter(fitness_sum, DBL_MAX));
    double r = distr(generator_);
    for(Chromosome* chrom : pop_) {
        partial_sum += chrom -> get_fitness();
        if(r < partial_sum) { return chrom; }
    }
    // Maybe throw error here???? **************************************************************************
    return nullptr; // Should never return nullptr, but if so... there's an issue...
}
