//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "Hashing_with_chaining.hpp"
#include "Utilities.hpp"


// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{
    using hash_table = HashingWithChaining<key_type, array_type , linked_list_type>;

    unsigned int nr_seeds = 100;
    std::string folder_path = "../../Data/HashingWithChaining";
    for(unsigned int seed = 0; seed < nr_seeds; seed++)
    {
        std::cout << "Iteration nr.: " << seed << std::endl;
        // Timing insertion for various n
        std::string filename = "HWC_insertion_timing_"+std::to_string(seed)+".txt";
        remove_file(filename,folder_path); // Removing possibly already existing file with name 'filename' from drive.
        const unsigned int iterations = 19;
        for(key_type w = 5; w <= (key_type)(5+iterations); w++)
        {
            // Defining number of keys as power of 2 to enable use of Multiply-Shift hash function
            key_type n = std::pow(2,w);

            // Generating hash_table and keys
            hash_table my_hash_table = hash_table(n, seed);
            array_type my_keys = generate_ordered_keys(n);

            // Inserting keys and timing the execution
            auto start = std::chrono::high_resolution_clock::now();
            my_hash_table.insert_keys(my_keys);
            auto stop = std::chrono::high_resolution_clock::now();
            output_data_type duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Getting size of the longest linked list in hash table
            unsigned int max_size = my_hash_table.max_bucket_size();

            // Saving time and sizes
            append_to_file(filename, folder_path, {(output_data_type)n, duration, (output_data_type)max_size});
        }

        // Generating random keys for testing query complexity

    }



}