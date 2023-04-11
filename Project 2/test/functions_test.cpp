//
// Created by Sebastian Yde Madsen on 02/04/2023.
//

#include <catch2/catch_all.hpp>

#include "lib/HashingWithChaining.hpp"
#include "lib/Utilities.hpp"
#include "lib/Sketch.hpp"


TEST_CASE("Relative Error", "[Fast functions]")
{
    /// ----------- TESTING FAST REL_ERR FUNCTION ----------- ///
    for(uint64_t a = 1; a < sizeof(uint64_t) * BITS_PR_BYTE; a++)
    {
        for(uint64_t b = 1; b < sizeof(uint64_t) * BITS_PR_BYTE; b++)
        {
            const double std_impl = slow_relative_err(a,b);
            const double fast_impl = fast_relative_err(a,b);
            REQUIRE(fast_impl==std_impl);
        }
    }
    std::cout << "## ====== RELATIVE ERROR FUNCTIONS TEST SUCCESSFUL ====== ##" << std::endl;
}

TEST_CASE("Pow2", "[Fast functions]")
{
    /// ----------- TESTING FAST POW2 FUNCTIONS ----------- ///
    for(uint32_t pow = 0; pow < sizeof(uint32_t) * BITS_PR_BYTE; pow++)
    {
        auto std_impl = static_cast<uint32_t>(std::pow(2,pow));
        const uint32_t fast_impl = fast_uint32_pow_2(pow);
        REQUIRE(fast_impl==std_impl);    }

    for(uint64_t pow = 0; pow < sizeof(uint64_t) * BITS_PR_BYTE; pow++)
    {
        auto std_impl = static_cast<uint64_t>(std::pow(2,pow));
        const uint64_t fast_impl = fast_uint64_pow_2(pow);
        REQUIRE(fast_impl==std_impl);    }

    std::cout << "## ====== POW2 FUNCTIONS TEST SUCCESSFUL ======= ##" << std::endl;
    }

TEST_CASE("Log2", "[Fast functions]")
{
    /// ----------- TESTING FAST LOG2 FUNCTIONS ----------- ///
    for(uint32_t pow = 1; pow < sizeof(uint32_t) * BITS_PR_BYTE; pow++)
    {
        const uint32_t x = fast_uint32_pow_2(pow);
        auto std_impl = static_cast<uint32_t>(std::log2(x));
        const uint32_t fast_impl = fast_uint32_log_2(x);
        REQUIRE(fast_impl==std_impl);
    }
    for(uint64_t pow = 1; pow < sizeof(uint64_t) * BITS_PR_BYTE; pow++)
    {
        const uint64_t x = fast_uint64_pow_2(pow);
        auto std_impl = static_cast<uint64_t>(std::log2(x));
        const uint64_t fast_impl = fast_uint64_log_2(x);
        REQUIRE(fast_impl==std_impl);
    }
    std::cout << "## ====== LOG2 FUNCTIONS TEST SUCCESSFUL ====== ##" << std::endl;
}

TEST_CASE("Slow_VS_Fast", "[Hash functions]")
{
    // Get the current time
    auto now = std::chrono::high_resolution_clock::now();

    // Get the current time as a long integer
    auto seed = now.time_since_epoch().count();

    const value_type power = 24;
    const unsigned int array_size = fast_uint32_pow_2(power);
    auto n_keys = static_cast<int64_t>(std::pow(10, 7));
    std::vector<int64_t> keys{};
    for (int64_t i = 1; i <= n_keys; i++) {
        keys.push_back(i);
    }
    const uint64_t mersenne_upper_bound = fast_uint64_pow_2(31) - 1;
    const hashing_constants constants = {get_random_uint64(seed+0, mersenne_upper_bound),
                                         get_random_uint64(seed+1114, mersenne_upper_bound),
                                         get_random_uint64(seed+78, mersenne_upper_bound),
                                         get_random_uint64(seed+31, mersenne_upper_bound)};

    for (const auto &key: keys) {
        auto result1 = mersenne_4_independent_hash(key, array_size, constants);
        auto result2 = slow_mersenne_4_independent_hash(key, array_size, constants);


        REQUIRE(result1.first == result2.first);
        REQUIRE(result1.second == result2.second);
    }
    std::cout << "## ====== SLOW_VS_FAST HASH TEST SUCCESSFUL ====== ##" << std::endl;

}


TEST_CASE("Independent 2 (impl. 1) vs. independent 2 (impl. 2)", "[Hash functions]")
{

    // Get the current time as a long integer
    const uint64_t N = 1000;
    const uint64_t ARRAY_SIZE = 12345;

    for(uint64_t a = 0; a < N; a++)
    {
        for(uint64_t b = 0; a < N; a++)
        {
            uint64_t c = 0;
            uint64_t d = 0;
            hashing_constants constants = {a,b,c,d};
            for(int64_t key = 0; key < static_cast<int64_t>(N); key++)
            {
                auto v1 = multiply_shift_2_independent(key,ARRAY_SIZE,constants);
                auto v2 = multiply_shift_2_independent_2(key,ARRAY_SIZE,constants);
                REQUIRE(v1.first == v2.first);
                REQUIRE(v1.second == v2.second);
            }
        }
    }
    std::cout << "## ====== Independent 2 (impl. 1) vs. independent 2 (impl. 2) ====== ##" << std::endl;

}