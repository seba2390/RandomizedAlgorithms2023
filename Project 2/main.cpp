//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "HashingWithChaining.hpp"
#include "Utilities.hpp"



// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{
    const unsigned int iterations = 14;
    const unsigned int seed_multiplier = 7;

    //// ----------------- Testing Hashing With Chaining implementation ----------------- ////
    std::cout << " \n-------- Hashing with Chaining --------\n " << std::endl;

    using hash_table = HashingWithChaining<key_type, array_type , linked_list_type>;

    unsigned int nr_seeds = 500;
    std::string folder_path = "../../Data/HashingWithChaining";
    for(unsigned int seed = 0; seed < nr_seeds; seed++)
    {
        std::cout << "Seed iteration nr.: " << seed << std::endl;

        // Timing insertion and query for various n
        std::string filename = "HWC_insertion_timing_"+std::to_string(seed_multiplier*seed)+".txt";
        remove_file(filename,folder_path); // Removing possibly already existing file with name 'filename' from drive.
        for(key_type w = 5; w <= (key_type)(5+iterations); w++)
        {
            //std::cout << "n=2^" << w << std::endl;
            // Defining number of keys as power of 2 to enable use of Multiply-Shift hash function
            key_type n = std::pow(2,w);

            // Generating hash_table and keys
            hash_table my_hash_table = hash_table(n, seed_multiplier*seed);
            array_type my_keys = generate_ordered_keys(n);

            // Inserting keys and timing the execution
            auto start = std::chrono::high_resolution_clock::now();
            my_hash_table.insert_keys(my_keys);
            auto stop = std::chrono::high_resolution_clock::now();
            output_data_type insertion_duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Getting size of the longest linked list in hash table
            unsigned int max_size = my_hash_table.max_bucket_size();

            // Testing query complexity
            array_type random_keys = generate_random_keys(n,seed_multiplier*seed);
            start = std::chrono::high_resolution_clock::now();
            for(key_type key: random_keys) bool _ = my_hash_table.holds(key);
            stop = std::chrono::high_resolution_clock::now();
            output_data_type query_duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Saving time and sizes
            append_to_file(filename, folder_path, {(output_data_type)n,
                                                   insertion_duration,
                                                   (output_data_type)max_size,
                                                   query_duration});
        }

    }







}