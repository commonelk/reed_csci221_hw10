## **Homework 10**
### By Quinn Hargrove & Evan Sieden

<hr />

#### **Compiling & Usage Instructions:**
+ To compile, execute `make` command in the base directory.
    + Adjust `NUM_ITER` in `tsp.cc` as desired.
+ To use, execute `./tsp <filename> <population size> <mutation rate>` command.

#### **Design Decisions:**
1. <u>Chromosome class.</u>
    + Since there is more than one fucntion in the Chromosome class where getting two unique indecies to cities is useful, the `two_random_cities()` function was added, which returns two numbers in a pair, randomly chosen from 0 to the number of cities -1.
    +  There were a few possibilities for implementing `get_fitness()`. In the end, we subtracted the distance returned by `calculate_total_distance()` from the maximum value of a double. 
2. <u>Deme class.</u>
    + Beyond implementing the algorithms as instructed, the central focus in writing `Deme` involved managing memory and preventing memory leaks. To facilitate this prevention, we use one method and one function, `clean_pop()` and `clean_chrom_pairs_t()`. 
        + `clean_pop()` is used in both the destructor and `compute_next_generation()`; as the name suggests, it unallocates the memory of each `Chromosome*` in `prop_` (and calls `pop_.clear()`). 
        + An additional function `clean_chrom_pairs_t()` was used in `compute_next_generation()` to free memory from vectors of type `std::vector<std::pair<Chromosome*, Chromosome*>>`. It would've been simpler to only have to use `clean_pop()` to unallocate memory,  but since `select_parent()` is implemented to use the instance variable `pop_` as opposed to a vector of similar type given as an argument, we had to modify `pop_` directly in between `select_parent()` calls. This resulted in us having to erase `Chromosome*`'s from `pop_` before unallocating the memory (because the parents had yet to create children). Hence, by the end of `compute_next_generation()`, the `Chromosome*`'s of the previous generation are stored in a vector of type `chrom_pairs_t` (the new children are stored in `prop_`), which is why `clean_chrom_pairs_t()` is needed in addition to `clean_pop()`.
    + On a minor note, `std::uniform_real_distribution<double> distr(0.0, 1.0)` results in a distribution over [0,1). To make this distribution inclusive, we used `std::nextafter(1.0, DBL_MAX)` as the endpoint instead. Doing so results in a distribution over an interval from 0 to the next `double` after 1.0 (exclusive), which effectively gives us an interval of [0.0, 1.0] as desired.
    + Further implementation details can be found among the documentation within `deme.hh` and `deme.cc`.

#### **Attributions:**
+ Base files provided by Eitan via Moodle assignment page.
