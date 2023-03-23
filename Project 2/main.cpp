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
        int32_t fast_impl = fast_uint32_log_2(x);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint32_log_2 doesn't give same output as std::log2.");
    }
    for(uint64_t pow = 1; pow < sizeof(uint64_t) * BITS_PR_BYTE; pow++)
    {
        uint64_t x = fast_uint64_pow_2(pow);
        auto std_impl = static_cast<int64_t>(std::log2(x));
        int64_t fast_impl = fast_uint64_log_2(x);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint64_log_2 doesn't give same output as std::log2.");
    }

    /// ----------- EXERCISE 5 ----------- ///
    const uint32_t seed = 4331;
    const value_type power = 24;
    const unsigned int array_size =  fast_uint32_pow_2(power);
    auto n_keys = static_cast<uint64_t>(std::pow(10,6));
    std::vector<int64_t> keys{};
    for(int64_t i = 0; i < n_keys; i++)
    {
        keys.push_back(i);
    }

    auto mersenne_upper_bound = static_cast<uint64_t>(std::pow(2,31) - 1);
    hashing_constants constants = {get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound)};


    uint32_t multiply_shift_upper_bound = static_cast<uint32_t>(std::pow(2,32)) - 1;
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

    }

    std::cout << "--- Avg. pr. key hashing times --- " << std::endl;
    std::cout << "Fast: " << avg_time_1 << " [ns]" << std::endl;
    std::cout << "Slow: " << avg_time_2 << " [ns]" << std::endl;
    std::cout << "Multiply-shift: " << avg_time_3 << " [ns]" << std::endl;


    /// ----------- EXERCISE 7 ----------- ///
    using sketch_type = Sketch<value_type, pair_type, array_type>;
    using hashing_with_chaining_type = HashingWithChaining<value_type, pair_type, linked_list_type>;

    const uint32_t N_MAX = 28;
    const uint32_t N_MIN = 6;
    const auto N_UPDATES = static_cast<int64_t>(std::pow(10,9));
    const array_type array_sizes = {(value_type)fast_uint64_pow_2(7),
                                    (value_type)fast_uint64_pow_2(10),
                                    (value_type)fast_uint64_pow_2(20)};

    print(std::string("hehe"));
    for(const value_type& r : array_sizes)
    {
        sketch_type my_sketch = sketch_type(r, seed);
        hashing_with_chaining_type my_hashing_with_chaining(r, seed);

        for(uint32_t N = N_MIN; N <= N_MAX; N++)
        {
            std::cout << "N: " << N << std::endl;
            uint32_t n = fast_uint32_pow_2(N);
            for(int64_t i = 1; i < N_UPDATES; i++)
            {
                value_type delta = 1;
                auto key = static_cast<key_type>(i & (n-1)); // Fast i mod n, when n=2^N.

                my_sketch.update(std::make_pair(key,delta));
                my_hashing_with_chaining.update(std::make_pair(key,delta));
            }
        }
    }







}