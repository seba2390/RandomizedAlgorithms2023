//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "HashingWithChaining.hpp"
#include "Utilities.hpp"



// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{
    print(2);

}