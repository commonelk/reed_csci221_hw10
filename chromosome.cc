/*
 * Implementation for Chromosome class
 */

#include <algorithm>
#include <cassert>
#include <random>
#include <limits>

#include "chromosome.hh"

//////////////////////////////////////////////////////////////////////////////
// Generate a completely random permutation from a list of cities
Chromosome::Chromosome(const Cities* cities_ptr)
  : cities_ptr_(cities_ptr),
    order_(random_permutation(cities_ptr->size())),
    generator_(rand())
{
  assert(is_valid());
}

//////////////////////////////////////////////////////////////////////////////
// Clean up as necessary
Chromosome::~Chromosome()
{
  assert(is_valid());
}

//////////////////////////////////////////////////////////////////////////////
// Perform a single mutation on this chromosome
void
Chromosome::mutate()
{
  //vector swap_order = Cities::random_permutation(order_.size()); //Randomly generates a permutation of cities, then takes the first two, to be used as teh indecies that will be swapped. Slow.

  std::pair<int, int> unique_cities = Chromosome::two_random_cities();

  std::iter_swap(order_.begin() + unique_cities.first, order_.begin() + unique_cities.second); //Swaps the values in the indecies from the previous step.

  assert(is_valid());
}

//////////////////////////////////////////////////////////////////////////////
// Return a pair of offsprings by recombining with another chromosome
// Note: this method allocates memory for the new offsprings
std::pair<Chromosome*, Chromosome*>
Chromosome::recombine(const Chromosome* other)
{
  assert(is_valid());
  assert(other->is_valid());

  std::pair<Chromosome*, Chromosome*> pair;
  std::pair<int, int> range = Chromosome::two_random_cities();

  pair.first = Chromosome::create_crossover_child(this, other, range.first, range.second);
  pair.second = Chromosome::create_crossover_child(other, this, range.first, range.second);
  return pair;

}

//////////////////////////////////////////////////////////////////////////////
// For an ordered set of parents, return a child using the ordered crossover.
// The child will have the same values as p1 in the range [b,e),
// and all the other values in the same order as in p2.
Chromosome*
Chromosome::create_crossover_child(const Chromosome* p1, const Chromosome* p2,
                                   unsigned b, unsigned e) const
{
  Chromosome* child = p1->clone();

  // We iterate over both parents separately, copying from parent1 if the
  // value is within [b,e) and from parent2 otherwise
  unsigned i = 0, j = 0;

  for ( ; i < p1->order_.size() && j < p2->order_.size(); ++i) {
    if (i >= b and i < e) {
      child->order_[i] = p1->order_[i];
    }
    else { // Increment j as long as its value is in the [b,e) range of p1
      while (p1->is_in_range(p2->order_[j], b, e)) {
        ++j;
      }
      assert(j < p2->order_.size());
      child->order_[i] = p2->order_[j];
      j++;
    }
  }

  assert(child->is_valid());
  return child;
}

// Return a positive fitness value, with higher numbers representing
// fitter solutions (shorter total-city traversal path).
double
Chromosome::get_fitness() const
{
  return std::numeric_limits<double>::max() - calculate_total_distance(); //Fitness = max double - total length. I'm not 100% confident in this one.
}

// A chromsome is valid if it has no repeated values in its permutation,
// as well as no indices above the range (length) of the chromosome.
// We implement this check with a sort, which is a bit inefficient, but simple
bool
Chromosome::is_valid() const
{
  unsigned length = order_.size();
  auto highest = std::max_element(order_.begin(), order_.end()); //Highest == iterator pointing to highest value in order_.
  if (*highest > length){
    return 0;
  } //Check if there are any values above the length of the chromosome by comparing the highest value to the length

  for (unsigned i = 0; i != length; i++){
    if (std::count(order_.begin(), order_.end(), i) != 1) { //Check if there are any missing/repeated values
      return 0;
    }
  }
  return 1;
}

// Find whether a certain value appears in a given range of the chromosome.
// Returns true if value is within the specified the range specified
// [begin, end) and false otherwise.
bool
Chromosome::is_in_range(unsigned value, unsigned begin, unsigned end) const
{
  //std::ranges::any_of(order_[begin], order_[end], value); // I think this'd work, but I haven't tried it.
  for(unsigned int i = begin; i < end; i++) {
    if(value == order_[i]) { return true; }
  }
  return false;
}

// Returns a pair containing two unique numbers between 0 and number of cities -1.
std::pair<int, int>
Chromosome::two_random_cities()
{
  std::uniform_int_distribution<int> gen(0, cities_ptr_->size() - 1); //Should this have the static keyword?

  int a = gen(generator_); //Randomly chooses two *unique* values. This seems like it'd *almost* always be faster than random_permutation.
  int b = gen(generator_);
  while (a == b){ b = gen(generator_); }
  return std::pair<int, int> (a, b);
}
