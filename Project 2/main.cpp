//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "HashingWithChaining.hpp"
#include "Utilities.hpp"



// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{
    key_type mersenne_upper_bound = std::pow(2,MERSENNE_PRIME_EXPONENT) - 1;
    hashing_constants mersenne_hashing_constants;
    key_type test_key = 0;
    key_type seed = 0;
    for(key_type i = 0; i < 10; i++)
    {
        // Constant for other 4-wise independent hash function  (remember to use different seeds).
        mersenne_hashing_constants.a = get_random_uint32(seed+1*i, mersenne_upper_bound);
        mersenne_hashing_constants.b = get_random_uint32(seed+7*i, mersenne_upper_bound);
        mersenne_hashing_constants.c = get_random_uint32(seed+3*i, mersenne_upper_bound);
        mersenne_hashing_constants.d = get_random_uint32(seed+5*i, mersenne_upper_bound);

        key_type a = slow_mersenne_4_independent_hash(test_key,mersenne_hashing_constants);
        key_type b = mersenne_4_independent_hash(test_key,mersenne_hashing_constants);

        std::cout << "Key: " << test_key << std::endl;
        std::cout << "Slow implementation: " << a << std::endl;
        std::cout << "Fast implementation: " << b << std::endl;
        if(a!=b) std::cout << "a!=b  :((((" << std::endl;
        else std::cout << "a==b  :))))" << std::endl;
        std::cout << "=========================================" << std::endl;

        test_key ++;
    }

}