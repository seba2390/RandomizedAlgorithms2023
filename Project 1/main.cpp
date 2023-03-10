//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "HashingWithChaining.hpp"
#include "RedBlackTree.hpp"
#include "PerfectHashing.hpp"
#include "Utilities.hpp"



// Checking that local environment 'key_type' type bit-size is as expected.
static_assert((CHAR_BIT * sizeof(key_type) == KEY_BIT_SIZE), "Adjust key_type to ensure using 32 bit sized integers.");


int main()
{
    //// ----------------- Testing Hashing With Chaining implementation ----------------- ////


    using hash_table = HashingWithChaining<key_type, array_type , linked_list_type>;

    unsigned int nr_seeds = 10;
    std::string folder_path = "../../Data/HashingWithChaining";
    for(unsigned int seed = 0; seed < nr_seeds; seed++)
    {
        std::cout << "Iteration nr.: " << seed << std::endl;

        // Timing insertion and query for various n
        std::string filename = "HWC_insertion_timing_"+std::to_string(3*seed)+".txt";
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
            output_data_type insertion_duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Getting size of the longest linked list in hash table
            unsigned int max_size = my_hash_table.max_bucket_size();

            // Testing query complexity
            array_type random_keys = generate_random_keys(n,3*seed);
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


    //// ----------------- Testing std::set AKA Red-Black Tree ----------------- ////
    using red_black_tree = RedBlackTree<key_type, array_type>;

    nr_seeds = 10;
    folder_path = "../../Data/RedBlackTree";
    for(unsigned int seed = 0; seed < nr_seeds; seed++)
    {
        std::cout << "Iteration nr.: " << seed << std::endl;

        // Timing insertion and query for various n
        std::string filename = "RBT_insertion_timing_"+std::to_string(3*seed)+".txt";
        remove_file(filename,folder_path); // Removing possibly already existing file with name 'filename' from drive.
        const unsigned int iterations = 19;
        for(key_type w = 5; w <= (key_type)(5+iterations); w++)
        {
            // Defining number of keys as power of 2 to enable use of Multiply-Shift hash function
            key_type n = std::pow(2,w);

            // Generating Red Black tree and keys
            red_black_tree my_red_black_tree = red_black_tree();
            array_type my_keys = generate_ordered_keys(n);

            // Inserting keys and timing the execution
            auto start = std::chrono::high_resolution_clock::now();
            my_red_black_tree.insert_keys(my_keys);
            auto stop = std::chrono::high_resolution_clock::now();
            output_data_type insertion_duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Testing query complexity
            array_type random_keys = generate_random_keys(n,3*seed);
            start = std::chrono::high_resolution_clock::now();
            for(key_type key: random_keys) bool _ = my_red_black_tree.holds(key);
            stop = std::chrono::high_resolution_clock::now();
            output_data_type query_duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Saving time and sizes
            append_to_file(filename, folder_path, {(output_data_type)n,
                                                              insertion_duration,
                                                              query_duration});
        }

    }

    //// ----------------- Testing Perfect Hashing implementation ----------------- ////

    using PerfectHashing = PerfectHashing<key_type, array_type, linked_list_type>;
    nr_seeds = 10;
    folder_path = "../../Data/PerfectHashing";
    for(unsigned int seed = 0; seed < nr_seeds; seed++)
    {
        std::cout << "Iteration nr.: " << seed << std::endl;

        // Timing insertion and query for various n
        std::string filename = "PH_insertion_timing_"+std::to_string(3*seed)+".txt";
        remove_file(filename,folder_path); // Removing possibly already existing file with name 'filename' from drive.
        const unsigned int iterations = 19;
        for(key_type w = 5; w <= (key_type)(5+iterations); w++)
        {
            // Defining number of keys as power of 2 to enable use of Multiply-Shift hash function
            key_type n = std::pow(2,w);

            // Generating Red Black tree and keys
            PerfectHashing my_perfect_hash_table = PerfectHashing (n, 3*seed);
            array_type my_keys = generate_ordered_keys(n);

            // Inserting keys and timing the execution
            auto start = std::chrono::high_resolution_clock::now();
            my_perfect_hash_table.insert_keys(my_keys,3*seed);
            auto stop = std::chrono::high_resolution_clock::now();
            output_data_type insertion_duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Testing query complexity
            array_type random_keys = generate_random_keys(n,3*seed);
            start = std::chrono::high_resolution_clock::now();
            for(key_type key: random_keys) bool _ = my_perfect_hash_table.holds(key);
            stop = std::chrono::high_resolution_clock::now();
            output_data_type query_duration = duration_cast<std::chrono::nanoseconds>(stop - start).count();

            // Saving time and sizes
            append_to_file(filename, folder_path, {(output_data_type)n,
                                                               insertion_duration,
                                                               query_duration});
        }

    }

}