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
}
