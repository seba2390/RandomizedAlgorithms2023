//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "HashingWithChaining.hpp"
#include "Utilities.hpp"
#include "Sketch.hpp"


// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{

    /// ----------- TESTING FAST POW2 FUNCTIONS ----------- ///
    for(uint32_t pow = 0; pow < sizeof(uint32_t) * BITS_PR_BYTE; pow++)
    {
        auto std_impl = static_cast<uint32_t>(std::pow(2,pow));
        uint32_t fast_impl = fast_uint32_pow_2(pow);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint32_pow_2 doesn't give same output as std::pow.");
    }
    for(uint64_t pow = 0; pow < sizeof(uint64_t) * BITS_PR_BYTE; pow++)
    {
        auto std_impl = static_cast<uint64_t>(std::pow(2,pow));
        uint64_t fast_impl = fast_uint64_pow_2(pow);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint64_pow_2 doesn't give same output as std::pow.");
    }

    /// ----------- TESTING FAST LOG2 FUNCTIONS ----------- ///
    for(uint32_t pow = 1; pow < sizeof(uint32_t) * BITS_PR_BYTE; pow++)
    {
        uint32_t x = fast_uint32_pow_2(pow);
        auto std_impl = static_cast<int32_t>(std::log2(x));
        uint32_t fast_impl = fast_uint32_log_2(x);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint32_log_2 doesn't give same output as std::log2.");
    }
    for(uint64_t pow = 1; pow < sizeof(uint64_t) * BITS_PR_BYTE; pow++)
    {
        uint64_t x = fast_uint64_pow_2(pow);
        auto std_impl = static_cast<int64_t>(std::log2(x));
        uint64_t fast_impl = fast_uint64_log_2(x);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint64_log_2 doesn't give same output as std::log2.");
    }

    /// ----------- TESTING FAST REL_ERR FUNCTION ----------- ///
    for(uint64_t a = 1; a < 1000; a++)
    {
        for(uint64_t b = 1; b < 1000; b++)
        {
            double std_impl = slow_relative_err(a,b);
            double fast_impl = fast_relative_err(a,b);
            if(std_impl!=fast_impl) throw std::runtime_error("fast_relative_err doesn't give same output as slow_relative_err.");
        }
    }

    /// ----------- EXERCISE 5 ----------- ///
    std::cout <<"\n ========= Exercise 5 ======== \n";

    std::string folder_path = "../../Data";
    std::string filename = "Exercise_5.txt";
    remove_file(filename,folder_path); // Removing possibly already existing file with name 'filename' from drive.


    const uint32_t seed = 4331;
    const value_type power = 24;
    const unsigned int array_size =  fast_uint32_pow_2(power);
    auto n_keys = static_cast<uint64_t>(std::pow(10,3));// TODO: Should be 10^6
    std::vector<int64_t> keys{};
    for(int64_t i = 0; i < n_keys; i++)
    {
        keys.push_back(i);
    }

    uint64_t mersenne_upper_bound = fast_uint64_pow_2(31) - 1;
    hashing_constants constants = {get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound)};


    uint32_t multiply_shift_upper_bound = static_cast<uint32_t>(std::pow(2,KEY_BIT_SIZE)) - 1;
    const auto a = static_cast<int32_t>(get_random_odd_uint32(seed,multiply_shift_upper_bound));
    const auto l = static_cast<uint32_t>(std::log2(array_size));

    double avg_time_1 = 0;
    double avg_time_2 = 0;
    double avg_time_3 = 0;
    for(const auto& key: keys)
    {
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

        if(result1.first!=result2.first) throw std::runtime_error("mersenne_4_independent_hash doesn't give same output as slow_mersenne_4_independent_hash");
        if(result1.second!=result2.second) throw std::runtime_error("mersenne_4_independent_hash doesn't give same output as slow_mersenne_4_independent_hash");

        auto casted_key = static_cast<key_type>(key);
        auto start_3 = std::chrono::high_resolution_clock::now();
        [[maybe_unused]] auto result3 = multiply_shift_hash(casted_key,a,l);
        auto stop_3 = std::chrono::high_resolution_clock::now();
        auto duration_3 = duration_cast<std::chrono::nanoseconds>(stop_3 - start_3).count();
        avg_time_3 += static_cast<double>(duration_3) / (static_cast<double>(n_keys));

        // Saving time and sizes
        append_to_file(filename, folder_path, {static_cast<output_data_type>(key),
                                               static_cast<output_data_type>(duration_1),
                                               static_cast<output_data_type>(duration_2),
                                               static_cast<output_data_type>(duration_3)});

    }

    std::cout << "--- Avg. pr. key hashing times --- " << std::endl;
    std::cout << "Fast: " << avg_time_1 << " [ns]" << std::endl;
    std::cout << "Slow: " << avg_time_2 << " [ns]" << std::endl;
    std::cout << "Multiply-shift: " << avg_time_3 << " [ns]" << std::endl;


    /// ----------- EXERCISE 7 ----------- ///
    std::cout <<"\n ========= Exercise 7 ======== \n";
    using mersenne_4_independent_return_type = std::pair<int64_t,int64_t>;
    using multiply_shift_return_type = uint32_t;

    using sketch_type_1 = Sketch<value_type, pair_type, array_type, mersenne_4_independent_return_type,
                                 int64_t,uint64_t,hashing_constants>;

    using hashing_with_chaining_type_1 = HashingWithChaining<value_type, pair_type, linked_list_type,
                                                             multiply_shift_return_type, uint32_t,uint32_t,uint32_t>;

    std::cout <<"\n ========= flag ======== \n";

    const uint32_t N_POWER_MAX = 25;
    const uint32_t N_POWER_MIN = 6;
    const uint32_t N_UPDATES_POWER = 5;
    const auto N_UPDATES = static_cast<int64_t>(std::pow(10,N_UPDATES_POWER)); // TODO: Should be 10^9
    const array_type array_sizes = {(value_type)fast_uint64_pow_2(7),
                                    (value_type)fast_uint64_pow_2(10),
                                    (value_type)fast_uint64_pow_2(20)};
    const array_type n_values = {
            []() {
                array_type v;
                for (uint64_t i = N_POWER_MIN; i <= N_POWER_MAX; ++i) {
                    v.push_back(static_cast<int64_t>(fast_uint64_pow_2(i)));
                }
                return v;
            }()
    };
    std::cout <<"\n ========= flag ======== \n";

    std::vector<output_data_type> average_HWC_update_times(n_values.size());
    std::vector<std::vector<output_data_type>> average_sketch_update_times = {{},{},{},{}};
    for(const value_type& n : n_values)
    {
        std::cout <<"n: " << n << std::endl;

        hashing_with_chaining_type_1 my_hashing_with_chaining = hashing_with_chaining_type_1(n, seed, multiply_shift_hash);
        output_data_type HWC_time = 0.0;
        for(int64_t update = 1; update <= N_UPDATES; update++)
        {
            value_type delta = 1;
            auto key = static_cast<key_type>(update & (n-1)); // Fast i mod n, when n=2^N.
            if((update % n) != key) throw std::runtime_error("not correct modulo operation"); // TODO: include line number in exception

            auto start = std::chrono::high_resolution_clock::now();
            my_hashing_with_chaining.update(std::make_pair(key,delta));
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = static_cast<output_data_type>(duration_cast<std::chrono::nanoseconds>(stop - start).count());
            HWC_time+=duration;
        }
        average_HWC_update_times.push_back(HWC_time / static_cast<output_data_type>(N_UPDATES));

        for(const value_type& r: array_sizes)
        {
            sketch_type_1 my_sketch = sketch_type_1(r, seed, mersenne_4_independent_hash);
            output_data_type sketch_time = 0.0;
            for(int64_t update = 1; update <= N_UPDATES; update++)
            {
                value_type delta = 1;
                auto key = static_cast<key_type>(update & (n-1)); // Fast i mod n, when n=2^N.
                if((update % n) != key) throw std::runtime_error("not correct modulo operation"); // TODO: include line number in exception

                auto start = std::chrono::high_resolution_clock::now();
                my_sketch.update(std::make_pair(key,delta));
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = static_cast<output_data_type>(duration_cast<std::chrono::nanoseconds>(stop - start).count());
                sketch_time+=duration;
            }
            average_sketch_update_times[r].push_back(sketch_time / static_cast<output_data_type>(N_UPDATES));
        }
    }


    /// ----------- EXERCISE 8 ----------- ///
    std::cout <<"\n ========= Exercise 8 ======== \n";

    const uint32_t r_min = 3;
    const uint32_t r_max = 10; // TODO: Should be 20
    const auto N_UPDATES_2 = static_cast<int64_t>(std::pow(10,3));
    const auto N_REPETITIONS = static_cast<int64_t>(std::pow(10,3));

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

    for(const value_type& r : array_sizes_2)
    {

        std::cout << "r: " << r << std::endl;
        for(uint32_t repetition = 0; repetition <= N_REPETITIONS; repetition++)
        {
            sketch_type_1 my_sketch = sketch_type_1(r, seed, mersenne_4_independent_hash);
            hashing_with_chaining_type_1 my_hashing_with_chaining = hashing_with_chaining_type_1(r, seed, multiply_shift_hash);

            // Performing updates, i.e. inserting (key, delta) pairs.
            for(int64_t update = 1; update < N_UPDATES_2; update++)
            {
                auto delta = static_cast<value_type>(std::pow(update,2));
                auto key = static_cast<key_type>(update);
                my_sketch.update(std::make_pair(key,delta));
                my_hashing_with_chaining.update(std::make_pair(key,delta));
            }

            // Getting true ||f||^2 and ||f||^2 estimate.
            [[maybe_unused]] uint64_t estimated_value = my_sketch.query();
            [[maybe_unused]] uint64_t true_value = my_hashing_with_chaining.query();

            [[maybe_unused]] double rel_err = fast_relative_err(estimated_value,true_value);

            }

    }

    /// ----------- EXERCISE 9 ----------- ///
    std::cout <<"\n ========= Exercise 9 ======== \n";
    using mersenne_4_independent_return_type_2 = std::pair<int64_t,int64_t>;
    using multiply_shift_2_independent_return_type_2 = std::pair<int64_t,int64_t>;

    using sketch_type_2                = Sketch<value_type, pair_type, array_type, mersenne_4_independent_return_type_2,
                                                int64_t,uint64_t,hashing_constants>;

    using hashing_with_chaining_type_2 = HashingWithChaining<value_type, pair_type, linked_list_type,
                                                        multiply_shift_2_independent_return_type_2, int64_t,uint64_t,hashing_constants>;

    for(const value_type& r : array_sizes_2)
    {

        std::cout << "r: " << r << std::endl;
        for(uint32_t repetition = 0; repetition <= N_REPETITIONS; repetition++)
        {
            sketch_type_2 my_sketch = sketch_type_2(r, seed, multiply_shift_2_independent);
            hashing_with_chaining_type_2 my_hashing_with_chaining = hashing_with_chaining_type_2(r, seed, multiply_shift_2_independent);

            // Performing updates, i.e. inserting (key, delta) pairs.
            for(int64_t update = 1; update < N_UPDATES_2; update++)
            {
                auto delta = static_cast<value_type>(std::pow(update,2));
                auto key = static_cast<key_type>(update);
                my_sketch.update(std::make_pair(key,delta));
                my_hashing_with_chaining.update(std::make_pair(key,delta));
            }

            // Getting true ||f||^2 and ||f||^2 estimate.
            [[maybe_unused]] uint64_t estimated_value = my_sketch.query();
            [[maybe_unused]] uint64_t true_value = my_hashing_with_chaining.query();

            [[maybe_unused]] double rel_err = fast_relative_err(estimated_value,true_value);

        }

    }






}