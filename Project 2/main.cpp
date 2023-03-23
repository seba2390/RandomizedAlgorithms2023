//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "HashingWithChaining.hpp"
#include "Utilities.hpp"



// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{

    /// ----------- TESTING FAST POW2 FUNCTIONS ----------- ///
    for(uint32_t pow = 0; pow < sizeof(uint32_t) * BITS_PR_BYTE; pow++)
    {
        uint32_t std_impl = std::pow(2,pow);
        uint32_t fast_impl = fast_uint32_pow_2(pow);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint32_pow_2 doesn't give same output as std::pow.");
    }
    for(uint64_t pow = 0; pow < sizeof(uint64_t) * BITS_PR_BYTE; pow++)
    {
        uint64_t std_impl = std::pow(2,pow);
        uint64_t fast_impl = fast_uint64_pow_2(pow);
        if(std_impl!=fast_impl) throw std::runtime_error("fast_uint64_pow_2 doesn't give same output as std::pow.");
    }

    /// ----------- TESTING SLOW vs. FAST 4-INDEPENDENT HASH FUNCTIONS ----------- ///
    uint64_t n_keys = std::pow(10,6) - 1;
    std::vector<int64_t> keys{};
    for(int64_t i = 0; i < n_keys; i++)
    {
        keys.push_back(i);
    }

    int64_t array_size =  std::pow(2,24);
    uint64_t seed = 4331;
    uint64_t mersenne_upper_bound = std::pow(2,31) - 1;
    hashing_constants constants = {get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound),
                                   get_random_uint64(seed, mersenne_upper_bound)};

    key_type multiply_shift_upper_bound = std::pow(2,32) - 1;
    key_type a = get_random_odd_uint32(seed,multiply_shift_upper_bound);
    key_type l = std::log2(array_size);

    double avg_time_1 = 0;
    double avg_time_2 = 0;
    double avg_time_3 = 0;
    for(const auto& key: keys)
    {
        auto start_1 = std::chrono::high_resolution_clock::now();
        auto _ = mersenne_4_independent_hash(key, array_size, constants);
        auto stop_1 = std::chrono::high_resolution_clock::now();
        auto duration_1 = duration_cast<std::chrono::nanoseconds>(stop_1 - start_1).count();
        avg_time_1 += duration_1 / ((double)n_keys);

        auto start_2 = std::chrono::high_resolution_clock::now();
        auto __ = slow_mersenne_4_independent_hash(key, array_size, constants);
        auto stop_2 = std::chrono::high_resolution_clock::now();
        auto duration_2 = duration_cast<std::chrono::nanoseconds>(stop_2 - start_2).count();
        avg_time_2 += duration_2 / ((double)n_keys);

        if(_.first!=__.first) throw std::runtime_error("mersenne_4_independent_hash doesn't give same output as slow_mersenne_4_independent_hash");
        if(_.second!=__.second) throw std::runtime_error("mersenne_4_independent_hash doesn't give same output as slow_mersenne_4_independent_hash");

        key_type casted_key = key;
        auto start_3 = std::chrono::high_resolution_clock::now();
        auto ___ = multiply_shift_hash(casted_key,a,l);
        auto stop_3 = std::chrono::high_resolution_clock::now();
        auto duration_3 = duration_cast<std::chrono::nanoseconds>(stop_3 - start_3).count();
        avg_time_3 += duration_3 / ((double)n_keys);

    }

    std::cout << "--- Avg. pr. key hashing times --- " << std::endl;
    std::cout << "Fast: " << avg_time_1 << " [ns]" << std::endl;
    std::cout << "Slow: " << avg_time_2 << " [ns]" << std::endl;
    std::cout << "Multiply-shift: " << avg_time_3 << " [ns]" << std::endl;









}