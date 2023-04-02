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

    // Define constants
    const uint32_t N_SEEDS = 30;
    const uint32_t SEED_MULTIPLIER = 11;

    // create a progress bar that displays a message
    boost::timer::progress_display progress(N_SEEDS, std::cout, "Processing... \n");
    for(uint32_t seed = 0; seed < N_SEEDS; seed++) {


        /// ----------- EXERCISE 5 ----------- ///
        std::string folder_path = "../../../../Data/Exercise_5";
        std::string filename = "Exercise_5_" + std::to_string(0+seed*SEED_MULTIPLIER) + ".txt";
        remove_file(filename, folder_path); // Removing possibly already existing file with name 'filename' from drive.



        const value_type power = 24;
        const unsigned int array_size = fast_uint32_pow_2(power);
        auto n_keys = static_cast<int64_t>(std::pow(10, 5));// TODO: Should be 10^6
        std::vector<int64_t> keys{};
        for (int64_t i = 1; i <= n_keys; i++) {
            keys.push_back(i);
        }
        const uint64_t mersenne_upper_bound = fast_uint64_pow_2(31) - 1;
        const hashing_constants constants = {get_random_uint64(0+seed*SEED_MULTIPLIER, mersenne_upper_bound),
                                       get_random_uint64(0+seed*SEED_MULTIPLIER, mersenne_upper_bound),
                                       get_random_uint64(0+seed*SEED_MULTIPLIER, mersenne_upper_bound),
                                       get_random_uint64(0+seed*SEED_MULTIPLIER, mersenne_upper_bound)};

        auto multiply_shift_upper_bound = static_cast<uint32_t>(std::pow(2, KEY_BIT_SIZE) - 1);
        const auto a = static_cast<int32_t>(get_random_odd_uint32(0+seed*SEED_MULTIPLIER, multiply_shift_upper_bound));
        const auto l = static_cast<uint32_t>(std::log2(array_size));
        double avg_time_1 = 0;
        double avg_time_2 = 0;
        double avg_time_3 = 0;
        for (const auto &key: keys) {
            auto start_1 = std::chrono::high_resolution_clock::now();
            auto result1 = mersenne_4_independent_hash(key, array_size, constants);
            auto stop_1 = std::chrono::high_resolution_clock::now();
            auto duration_1 = duration_cast<std::chrono::nanoseconds>(stop_1 - start_1).count();
            avg_time_1 += static_cast<double>(duration_1) / (static_cast<double>(n_keys));

            auto start_2 = std::chrono::high_resolution_clock::now();
            auto result2 = slow_mersenne_4_independent_hash(key, array_size, constants);
            auto stop_2 = std::chrono::high_resolution_clock::now();
            auto duration_2 = duration_cast<std::chrono::nanoseconds>(stop_2 - start_2).count();
            avg_time_2 += static_cast<double>(duration_2) / (static_cast<double>(n_keys));

            if (result1.first != result2.first)
                throw std::runtime_error(
                        "mersenne_4_independent_hash doesn't give same output as slow_mersenne_4_independent_hash");
            if (result1.second != result2.second)
                throw std::runtime_error(
                        "mersenne_4_independent_hash doesn't give same output as slow_mersenne_4_independent_hash");

            auto casted_key = static_cast<key_type>(key);
            auto start_3 = std::chrono::high_resolution_clock::now();
            [[maybe_unused]] auto result3 = multiply_shift_hash(casted_key, a, l);
            auto stop_3 = std::chrono::high_resolution_clock::now();
            auto duration_3 = duration_cast<std::chrono::nanoseconds>(stop_3 - start_3).count();
            avg_time_3 += static_cast<double>(duration_3) / (static_cast<double>(n_keys));

            // Saving time and sizes
            append_to_file(filename, folder_path, {static_cast<output_data_type>(key),
                                                   static_cast<output_data_type>(duration_1),
                                                   static_cast<output_data_type>(duration_2),
                                                   static_cast<output_data_type>(duration_3)});

        }

        //std::cout << "--- Avg. pr. key hashing times --- " << std::endl;
        //std::cout << "Fast: " << avg_time_1 << " [ns]" << std::endl;
        //std::cout << "Slow: " << avg_time_2 << " [ns]" << std::endl;
        //std::cout << "Multiply-shift: " << avg_time_3 << " [ns]" << std::endl;


        /// ----------- EXERCISE 7 ----------- ///
        // Define return types for hash functions
        using mersenne_4_independent_return_type = std::pair<int64_t, int64_t>;
        using multiply_shift_return_type = uint32_t;

        // Define sketch and hashing classes with corresponding template arguments
        using sketch_type_1 = Sketch<value_type, pair_type, array_type, mersenne_4_independent_return_type,
                int64_t, uint64_t, hashing_constants>;

        using hashing_with_chaining_type_1 = HashingWithChaining<value_type, pair_type, linked_list_type,
                multiply_shift_return_type, uint32_t, uint32_t, uint32_t>;

        // Define constants for the experiment
        const uint32_t N_POWER_MAX = 26;  // Maximum power of 2 to test (TODO: Should be 28)
        const uint32_t N_POWER_MIN = 6;   // Minimum power of 2 to test
        const uint32_t N_UPDATES_POWER = 6;   // Power of 10 for number of updates (TODO: Should be 9)
        const auto N_UPDATES = static_cast<int64_t>(std::pow(10, N_UPDATES_POWER)); // Total number of updates
        const array_type array_sizes = {(value_type) fast_uint64_pow_2(7),
                                        (value_type) fast_uint64_pow_2(10),
                                        (value_type) fast_uint64_pow_2(20)};   // Array sizes to test
        const array_type n_values = {
                []() {
                    array_type v;
                    for (uint64_t i = N_POWER_MIN; i <= N_POWER_MAX; ++i) {
                        v.push_back(static_cast<int64_t>(fast_uint64_pow_2(i)));
                    }
                    return v;
                }()
        }; // Vector of values of n to test

        std::vector<output_data_type> average_HWC_update_times(n_values.size());
        std::vector<std::vector<output_data_type>> average_sketch_update_times;
        average_sketch_update_times.resize(array_sizes.size(), std::vector<output_data_type>(n_values.size()));

        // iterate over array of n values
        for (unsigned int n_idx = 0; n_idx < n_values.size(); n_idx++) {
            const value_type n = n_values[n_idx];
            // TODO: Investigate and determine if hashing with chaining should have m=n

            // create a new hashing_with_chaining_type_1 object with given n, seed and multiply_shift_hash
            hashing_with_chaining_type_1 my_hashing_with_chaining = hashing_with_chaining_type_1(n, 0+seed*SEED_MULTIPLIER,
                                                                                                 multiply_shift_hash);
            output_data_type HWC_time = 0.0;

            // iterate over N_UPDATES, update hashing_with_chaining_type_1 object with each update
            for (int64_t update = 1; update <= N_UPDATES; update++) {
                const value_type delta = 1;
                auto key = static_cast<key_type>(update & (n - 1)); // Fast i mod n, when n=2^N.

                // record start time and update object
                auto start = std::chrono::high_resolution_clock::now();
                my_hashing_with_chaining.update(std::make_pair(key, delta));
                auto stop = std::chrono::high_resolution_clock::now();

                // calculate duration and add to total time
                auto duration = static_cast<output_data_type>(duration_cast<std::chrono::nanoseconds>(
                        stop - start).count());
                HWC_time += duration;
            }

            // add average update time for this value of n to the vector
            average_HWC_update_times[n_idx] = HWC_time / static_cast<output_data_type>(N_UPDATES);

            // iterate over array_sizes, for each size create a new sketch_type_1 object with given r, seed and mersenne_4_independent_hash
            for (unsigned int r_idx = 0; r_idx < array_sizes.size(); r_idx++) {
                const value_type r = array_sizes[r_idx];
                sketch_type_1 my_sketch = sketch_type_1(r, 0+seed*SEED_MULTIPLIER, mersenne_4_independent_hash);
                output_data_type sketch_time = 0.0;

                // iterate over N_UPDATES, update sketch_type_1 object with each update
                for (int64_t update = 1; update <= N_UPDATES; update++) {
                    const value_type delta = 1;
                    auto key = static_cast<key_type>(update & (n - 1)); // Fast i mod n, when n=2^N.

                    // record start time and update object
                    auto start = std::chrono::high_resolution_clock::now();
                    my_sketch.update(std::make_pair(key, delta));
                    auto stop = std::chrono::high_resolution_clock::now();

                    // calculate duration and add to total time
                    auto duration = static_cast<output_data_type>(duration_cast<std::chrono::nanoseconds>(
                            stop - start).count());
                    sketch_time += duration;
                }

                // add average update time for this value of r and n to the vector
                average_sketch_update_times[r_idx][n_idx] = sketch_time / static_cast<output_data_type>(N_UPDATES);
            }
        }

        // Saving times to drive
        filename = "Exercise_7_seed_" + std::to_string(0+seed*SEED_MULTIPLIER) + ".txt";
        folder_path = "../../../../Data/Exercise_7";

        remove_file(filename, folder_path); // Removing possibly already existing file with name 'filename' from drive.

        for (unsigned int n = 0; n < n_values.size(); n++) {
            append_to_file(filename, folder_path, {
                    static_cast<output_data_type>(n_values[n]),
                    static_cast<output_data_type>(average_HWC_update_times[n]),
                    static_cast<output_data_type>(average_sketch_update_times[0][n]),
                    static_cast<output_data_type>(average_sketch_update_times[1][n]),
                    static_cast<output_data_type>(average_sketch_update_times[2][n])});
        }



        /// ----------- EXERCISE 8 ----------- ///
        const uint32_t r_min = 3;
        const uint32_t r_max = 20; // TODO: Should be 20
        const auto N_UPDATES_2 = static_cast<int64_t>(std::pow(10, 3)); // TODO: Should be 10^3
        const auto N_REPETITIONS = static_cast<int64_t>(std::pow(10, 3)); // TODO: Should be 10^3

        // initialize vector inline with a loop using lambda function to generate values
        array_type array_sizes_2 = {
                []() {
                    array_type v;
                    for (uint64_t i = r_min; i <= r_max; ++i) {
                        v.push_back(static_cast<value_type>(fast_uint64_pow_2(i)));
                    }
                    return v;
                }()
        };

        std::vector<output_data_type> avg_relative_errs(array_sizes_2.size());
        std::vector<output_data_type> max_relative_errs(array_sizes_2.size());

        // Perform the experiments for each value of 'r'.
        for (unsigned int r_idx = 0; r_idx < array_sizes_2.size(); r_idx++) {
            const value_type r = array_sizes_2[r_idx];

            double avg_error_sum = 0;
            double max_error = 0;

            // Repeat the experiment N_REPETITIONS times
            for (uint32_t experiment = 0; experiment <= N_REPETITIONS; experiment++) {
                // Initialize new Sketch
                sketch_type_1 my_sketch = sketch_type_1(r, 0+seed*SEED_MULTIPLIER + (r_idx), mersenne_4_independent_hash);
                uint64_t true_value = 0;
                // Performing the 'N_UPDATES_2' updates, i.e. inserting (key, delta) pairs.
                for (int64_t update = 1; update < N_UPDATES_2; update++) {
                    auto delta = static_cast<value_type>(std::pow(update, 2));
                    auto key = static_cast<key_type>(update);
                    true_value += static_cast<uint64_t>(delta*delta);
                    my_sketch.update(std::make_pair(key, delta));
                }
                // Calculate the relative error for this experiment
                auto estimated_value = my_sketch.query();
                // Updating avg. err.
                const double rel_err = slow_relative_err(estimated_value, true_value);

                avg_error_sum += rel_err;
                // Checking for new max. err.
                if (rel_err > max_error) max_error = rel_err;
            }
            avg_relative_errs[r_idx] = avg_error_sum / static_cast<output_data_type>(N_REPETITIONS);
            max_relative_errs[r_idx] = max_error;
        }


        // Saving errors to drive
        filename = "Exercise_8_seed_" + std::to_string(0+seed*SEED_MULTIPLIER) + ".txt";
        folder_path = "../../../../Data/Exercise_8";
        remove_file(filename, folder_path); // Removing possibly already existing file with name 'filename' from drive.
        for (unsigned int r = 0; r < avg_relative_errs.size(); r++) {
            append_to_file(filename, folder_path, {
                    static_cast<output_data_type>(array_sizes_2[r]),
                    static_cast<output_data_type>(avg_relative_errs[r]),
                    static_cast<output_data_type>(max_relative_errs[r])});
        }


        /// ----------- EXERCISE 9 ----------- ///
        // TODO: determine why error is so much bigger for 2-wise multiply shift (this exercise) than 4-wise (exercise 8)
        // TODO: Numerically off by approx factor 555 (equivalent to hash function mapping all-to-one entry) - see overleaf doc.
        using multiply_shift_2_independent_return_type_2 = std::pair<int64_t, int64_t>;

        using sketch_type_2 = Sketch<value_type, pair_type, array_type, multiply_shift_2_independent_return_type_2,
                int64_t, uint64_t, hashing_constants>;

        std::vector<output_data_type> avg_relative_errs_2(array_sizes_2.size());
        std::vector<output_data_type> max_relative_errs_2(array_sizes_2.size());

        // Perform the experiments for each value of 'r'.
        for (unsigned int r_idx = 0; r_idx < array_sizes_2.size(); r_idx++) {
            const value_type r = array_sizes_2[r_idx];

            double avg_error_sum = 0;
            double max_error = 0;

            // Repeat the experiment N_REPETITIONS times
            for (uint32_t experiment = 0; experiment <= N_REPETITIONS; experiment++) {
                // Initialize new Sketch
                sketch_type_2 my_sketch = sketch_type_2(r, 0+seed*SEED_MULTIPLIER + (r_idx), multiply_shift_2_independent);
                uint64_t true_value = 0;
                // Performing the 'N_UPDATES_2' updates, i.e. inserting (key, delta) pairs.
                for (int64_t update = 1; update < N_UPDATES_2; update++) {
                    auto delta = static_cast<value_type>(std::pow(update, 2));
                    auto key = static_cast<key_type>(update);
                    true_value += static_cast<uint64_t>(delta*delta);
                    my_sketch.update(std::make_pair(key, delta));
                }
                // Calculate the relative error for this experiment
                auto estimated_value = my_sketch.query();
                // Updating avg. err.
                const double rel_err = slow_relative_err(estimated_value, true_value);
                avg_error_sum += rel_err;
                // Checking for new max. err.
                if (rel_err > max_error) max_error = rel_err;
            }
            avg_relative_errs_2[r_idx] = avg_error_sum / static_cast<output_data_type>(N_REPETITIONS);
            max_relative_errs_2[r_idx] = max_error;
        }

        // Saving errors to drive
        filename = "Exercise_9_seed_" + std::to_string(0+seed*SEED_MULTIPLIER) + ".txt";
        folder_path = "../../../../Data/Exercise_9";
        remove_file(filename, folder_path); // Removing possibly already existing file with name 'filename' from drive.
        for (unsigned int r = 0; r < avg_relative_errs_2.size(); r++) {
            append_to_file(filename, folder_path, {
                    static_cast<output_data_type>(array_sizes_2[r]),
                    static_cast<output_data_type>(avg_relative_errs_2[r]),
                    static_cast<output_data_type>(max_relative_errs_2[r])});
        };

        ++progress; // Increment progress bar

    }


}