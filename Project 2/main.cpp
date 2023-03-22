//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "HashingWithChaining.hpp"
#include "Utilities.hpp"



// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{
    key_type seed = 123;
    key_type upper_bound = std::pow(2,MERSENNE_PRIME_EXPONENT) - 1;
    key_type array_size = 3;
    hashing_constants test_constants;
    test_constants.a = get_random_uint64(seed, upper_bound);
    test_constants.b = get_random_uint64(seed+11, upper_bound);
    test_constants.c = get_random_uint64(seed+2, upper_bound);
    test_constants.d = get_random_uint64(seed+7, upper_bound);

    key_type key = 2;
    auto result = mersenne_4_independent_hash(key,array_size,test_constants);
    std::cout << "g: " << result.first << " | h: " << result.second << std::endl;



}