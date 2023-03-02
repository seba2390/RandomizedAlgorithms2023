//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "Hashing_with_chaining.hpp"
#include "Utilities.hpp"


// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{

    const unsigned int n_test = 16; // Should be a power of 2.
    key_type a = get_random_odd_uint32(SEED);
    key_type l = std::log2(n_test);
    using hash_table = HashingWithChaining<key_type, array_type, linked_list_type>;
    hash_table test_hash_table = hash_table(n_test);

    array_type test_keys = generate_keys(n_test);
    test_hash_table.insert_keys(test_keys);

    // Testing that elements are actually inserted
    for(key_type key : test_keys)
    {
        key_type idx = hash(key, n_test, a, l);
        std::cout << "At index: " << idx << std::endl;
        for(auto item: test_hash_table.hash_table[idx])
        {
            std::cout << "key: " << item << std::endl;
        }
        std::cout << "------------" << std::endl;
    }

    // Testing query functionality
    key_type some_key = 123;
    if(test_hash_table.holds(some_key)) std::cout << "Hash table holds: " << some_key;
    else std::cout << "Hash table does not hold: " << some_key << std::endl;

    // Testing writing to hard drive
    append_to_file("test.txt", {0.1, 2.0});


    // Timing insertion for various n
    std::string filename = "HWC_insertion_timing.txt";
    remove_file(filename); // Clearing possible file with name 'filename' from drive.
    const unsigned int iterations = 19;
    for(key_type w = 5; w <= (key_type)(5+iterations); w++)
    {
        // Defining number of keys as power of 2 to enable use of Multiply-Shift hash function
        key_type n = std::pow(2,w);

        std::cout << "w:" << w << " | n: " << n << std::endl;

        // Generating hash_table and keys
        hash_table my_hash_table = hash_table(n);
        array_type my_keys = generate_keys(n);

        // Inserting keys and timing the execution
        auto start = std::chrono::high_resolution_clock::now();
        my_hash_table.insert_keys(my_keys);
        auto stop = std::chrono::high_resolution_clock::now();
        output_data_type duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

        // Getting size of the longest linked list in hash table
        unsigned int max_size = my_hash_table.max_bucket_size();
        std::cout << "max size: " << max_size << std::endl;

        // Saving time and sizes
        append_to_file(filename, {(output_data_type)n, duration, (output_data_type)max_size});
    }

}