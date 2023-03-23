//
// Created by Sebastian Yde Madsen on 01/03/2023.
//

#include "Utilities.hpp"


template <typename value_type, typename pair_type, typename list_type>
class HashingWithChaining
{
private:
    // Typedefs
    using array_type = std::vector<list_type>;
    using hash_table_type = array_type;
    using sum_type = int64_t;

    // Attributes
    key_type array_size;
    key_type a, l;
    bool empty;

    // Equivalent to 2^KEY_BIT_SIZE - 1
    uint32_t multiply_shift_upper_bound = fast_uint32_pow_2(KEY_BIT_SIZE) - 1;



    // Methods
    void initialize_hash_table()
    {
        hash_table.reserve(this->array_size);    // allocate memory for the array/vector
        hash_table.resize(this->array_size);        // initialize the array/vector with the given size
        for(key_type i = 0; i < this->array_size; i++) hash_table[i] = list_type{}; // Setting lists in array/vector.
    }


    void initialize_consts(const unsigned int& seed)
    {
        /*
         * Initializing constants for hash function here
         * to avoid continuous recalculation when
         * calling hash function.
         * */

        // Constant for multiply-shift hash function.
        this->a = get_random_odd_uint32(seed, this->multiply_shift_upper_bound);
        this->l = fast_uint64_log_2(this->array_size); // if m = 2^l then l = log2(m).
        this->empty = true;
    }

public:

    // Attributes
    hash_table_type hash_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit HashingWithChaining(const unsigned int& array_size, const unsigned int& seed)
    {
     // Checking that 64-bit numbers are used c.f. exercise 6.
     if(!sizeof(value_type) * BITS_PR_BYTE == 64) throw std::runtime_error("'value_type' used in Sketch template should be 64-bit.");


     this->array_size = array_size;
     initialize_hash_table();
     initialize_consts(seed);
    }

    // Methods

    /**
     * Checks whether the provided key is stored in the hash table.
     *
     * @param key The key to search for in the hash table.
     *
     * @return A std::tuple containing three values:
     *         1. The index of the bucket in the hash table where the key is found (or NAN_TOKEN if not found)
     *         2. The index of the key-value pair in the linked list within the bucket where the key is found (or NAN_TOKEN if not found)
     *         3. A boolean value indicating whether the key is found in the hash table or not.
     */
    std::tuple<key_type, key_type , bool> holds(const key_type& key)
    {
        /*
         * Checks whether the provided key is stored in the hash table.
         */
        key_type array_index = multiply_shift_hash(key, this->a, this->l);

        // Only start iterating through linked list if bucket is not empty
        if(!this->hash_table[array_index].empty())
        {
            // Using lambda function in iterator to check if the first value in the pair (the key value) equals given key.
            auto iterator = std::find_if(this->hash_table[array_index].begin(),
                                         this->hash_table[array_index].end(),
                                         [=](const pair_type& p){return p.first == key;}
                                         );
            if(iterator != this->hash_table[array_index].end())
            {
                key_type list_index = std::distance(this->hash_table[array_index].begin(), iterator);
                return std::make_tuple(array_index, list_index, true);
            }
        }
        return std::make_tuple(NAN_TOKEN, NAN_TOKEN, true);
    }

    /**
     * Updates the hash table with a given key-value pair. If the key already exists in the hash table,
     * the value is incremented by the given delta. Otherwise, a new key-value pair is added to the hash table.
     *
     * @param pair A key-value pair to update the hash table with.
     *
     * @returns void
     */
    void update(const pair_type& pair)
    {
        key_type key = pair.first;
        value_type delta = pair.second;

        // Only performing checks if table holds any pairs
        if(!this->empty){
            // Checking if key is already in table
            std::tuple<key_type,value_type,bool> result = holds(key);
            if(std::get<2>(result))
            {
                // Then just add delta to value in pair
                key_type array_index = std::get<0>(result);
                key_type list_index = std::get<1>(result);

                // iterate to the 'list_index' element using a list iterator.
                auto it = std::next((this->hash_table[array_index]).begin(), list_index);

                // change the .second value of the pair at the 'list_index' position.
                it->second += delta;
            } else
            {
                // Then just append pair to end of list
                key_type array_index = multiply_shift_hash(key, this->a, this->l);
                (this->hash_table[array_index]).push_back(std::make_pair(key, 0+delta));
            }
        } else
        {
            // Then just append pair to end of list
            key_type array_index = multiply_shift_hash(key, this->a, this->l);
            (this->hash_table[array_index]).push_back(std::make_pair(key, 0+delta));
            // Set table not empty
            this->empty = false;
        }

    }

    /**
     * Compute the norm square of the hash table.
     * If array_type is std::vector, use std::inner_product to compute the sum of squares.
     * If array_type is not std::vector, use std::accumulate to compute the sum of squares.
     *
     * @return The norm square of the hash table.
     */
    sum_type query() {
        // Check if array_type is std::vector
        if constexpr (std::is_same_v<array_type, std::vector<typename array_type::value_type, typename array_type::allocator_type>>) {
            // If array_type is std::vector, use std::inner_product to compute the sum of squares
            return std::inner_product(this->hash_table.begin(), this->hash_table.end(), this->hash_table.begin(), 0);
        } else {
            // If array_type is not std::vector, use std::accumulate and lambda function to compute the sum of squares
            auto result = std::accumulate(this->hash_table.begin(), this->hash_table.end(), static_cast<sum_type>(0), [](const auto& acc, const auto& val) {
                return acc + val * val;
            });
            return result;
        }
    }
};