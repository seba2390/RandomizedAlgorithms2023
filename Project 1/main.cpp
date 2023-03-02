//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "Hashing_with_chaining.hpp"
#include "Utilities.hpp"


// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{

    const unsigned int n = 16; // Should be a power of 2.
    using hash_table = HashingWithChaining<key_type, array_type, linked_list_type>;
    hash_table my_hash_table = hash_table(n);

    array_type my_keys = generate_keys(n);
    my_hash_table.insert_keys(my_keys);

    // Testing that elements are actually inserted
    for(key_type key : my_keys)
    {
        key_type idx = hash(key,n);
        std::cout << "At index: " << idx << std::endl;
        for(auto item: my_hash_table.hash_table[idx])
        {
            std::cout << "key: " << item << std::endl;
        }
        std::cout << "------------" << std::endl;
    }

    // Testing query functionality
    key_type some_key = 123;
    if(my_hash_table.has(some_key)) std::cout << "Hash table holds: " << some_key;
    else std::cout << "Hash table does not hold: " << some_key;

    // Testing writing to hard drive
    save("test.txt", {0.1, 2.0});



}