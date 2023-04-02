//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "lib/HashingWithChaining.hpp"
#include "lib/Utilities.hpp"
#include "lib/Sketch.hpp"


// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");

int main()
{


    const unsigned int NR_SEEDS = 5, MULTIPLIER = 11235;

    // create a progress bar that displays a message
    boost::timer::progress_display progress(NR_SEEDS, std::cout, "Processing: \n");

    for(unsigned int seed_count = 0; seed_count < NR_SEEDS; seed_count++)
    {

        // Setting new seed
        const unsigned int seed = seed_count*MULTIPLIER;

        // Define return types for hash functions
        using mersenne_4_independent_return_type = std::pair<int64_t, int64_t>;

        // Define sketch and hashing classes with corresponding template arguments
        using sketch_type = Sketch<value_type, pair_type, array_type, mersenne_4_independent_return_type,
                int64_t, uint64_t, hashing_constants>;

        // Constants
        const uint32_t R_MIN = 3;
        const uint32_t R_MAX = 20;
        const auto N_UPDATES = static_cast<int64_t>(std::pow(10, 3));
        const auto N_REPETITIONS = static_cast<int64_t>(std::pow(10, 3));

        // initialize vector inline with a loop using lambda function to generate values
        array_type array_sizes = {
                []() {
                    array_type v;
                    for (uint64_t i = R_MIN; i <= R_MAX; ++i) {
                        v.push_back(static_cast<value_type>(fast_uint64_pow_2(i)));
                    }
                    return v;
                }()
        };

        // Create a vector to hold the variances for each value of 'r' and the correct value of the squared norm-square.
        std::vector<output_data_type> variances(array_sizes.size());
        std::vector<output_data_type> bounds(array_sizes.size());

        // Loop over each value of 'r' and perform the experiments.
        for (unsigned int r_idx = 0; r_idx < array_sizes.size(); r_idx++) {

            const value_type r = array_sizes[r_idx];

            // Create a vector to hold the estimates for the current value of 'r'.
            std::vector<int64_t> estimates(N_REPETITIONS);

            // Loop over each experiment for the current value of 'r'.
            for (unsigned int experiment_idx = 0; experiment_idx < N_REPETITIONS; experiment_idx++) {

                // Create a new sketch for the current experiment using the current value of 'r'.
                sketch_type my_sketch = sketch_type(r, seed*(r_idx + experiment_idx * N_REPETITIONS),
                                                    mersenne_4_independent_hash);

                // Perform the 'N_UPDATES' updates on the sketch, and record the estimates.
                double true_value = 0;
                for (int64_t update = 1; update < N_UPDATES; update++) {
                    auto delta = static_cast<value_type>(std::pow(update, 2));
                    auto key = static_cast<key_type>(update);
                    true_value += static_cast<double>(delta*delta);
                    my_sketch.update(std::make_pair(key, delta));
                }
                estimates.at(experiment_idx) = my_sketch.query();
                bounds.at(r_idx) = 2*true_value*true_value / static_cast<double>(r);
            }

            // Compute the sample variance of the estimates for the current value of 'r'.
            variances.at(r_idx) = variance(estimates);
        }


        // Saving to drive
        std::string filename = "variance_test_seed"+std::to_string(seed)+".txt";
        std::string folder_path = "../../../../Data/Extra";
        remove_file(filename, folder_path); // Removing possibly already existing file with name 'filename' from drive.
        for (unsigned int r = 0; r < variances.size(); r++) {
            append_to_file(filename, folder_path, {
                    static_cast<output_data_type>(array_sizes[r]),
                    static_cast<output_data_type>(variances[r]),
                    static_cast<output_data_type>(bounds[r])});
        }

        ++progress; // increment the progress bar
    }



}

