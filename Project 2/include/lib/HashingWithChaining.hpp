//
// Created by Sebastian Yde Madsen on 01/03/2023.
//

#include "Utilities.hpp"


template <typename value_type, typename pair_type, typename list_type, typename hash_return_type, typename... hash_args>
class HashingWithChaining
{
private:
    // Typedefs
    using array_type = std::vector<list_type>;
    using hash_table_type = array_type;
    using sum_type = int64_t;
    using hash_func_type = std::function<hash_return_type(hash_args...)>;

    // Attributes
    uint32_t array_size;
    uint32_t a, l;
    hashing_constants my_hash_constants;
    bool empty;

    // Equivalent to 2^KEY_BIT_SIZE - 1
    uint32_t multiply_shift_upper_bound = static_cast<uint32_t>(std::pow(2,KEY_BIT_SIZE) - 1);
    value_type mersenne_upper_bound = MERSENNE_PRIME; // TODO: Should this be the same for alle the hash funcs w. multiple constants?



    // Methods
    /**
     * Initializes the hash table by allocating memory for the array/vector, setting its size, and
     * setting empty lists in each array/vector element.
     *
     * @param none
     *
     * @return none
     */
    void initialize_hash_table()
    {
        hash_table.reserve(this->array_size);    // allocate memory for the array/vector
        hash_table.resize(this->array_size);        // initialize the array/vector with the given size
        for(key_type i = 0; i < static_cast<key_type>(this->array_size); i++) hash_table[i] = list_type{}; // Setting lists in array/vector.
    }

    /**
     * Sets the constants for the 4-wise independent hash function using the Mersenne Twister algorithm with the given seed.
     *
     * @param seed The seed used to generate the hash function constants.
     */
    void set_hash_constants(const unsigned int& seed)
    {
        // Constant for other 4-wise independent hash function  (remember to use different seeds).
        this->my_hash_constants.a = get_random_uint64(seed+0, this->mersenne_upper_bound);
        this->my_hash_constants.b = get_random_uint64(seed+11, this->mersenne_upper_bound);
        this->my_hash_constants.c = get_random_uint64(seed+431, this->mersenne_upper_bound);
        this->my_hash_constants.d = get_random_uint64(seed+78, this->mersenne_upper_bound);
    }



    /**
     * Initializes the constants for the other 4-wise independent hash function with the given seed.
     *
     * @param seed The seed used to generate the hash function constants.
     */
    void initialize_consts(const unsigned int& seed)
    {
        // Constant for other 4-wise independent hash function  (remember to use different seeds).
        set_hash_constants(seed);
    }


public:

    // Attributes
    hash_table_type hash_table;
    hash_func_type hash_function;

    // Parameterized C-tor
    /**
     * Constructs a HashingWithChaining object with the specified parameters.
     *
     * @param array_size An unsigned integer representing the size of the hash table array. Must be a power of 2.
     * @param seed An unsigned integer used as the seed for the multiply-shift hash function.
     *
     * @throws std::runtime_error if the value_type used in the Sketch template is not 64-bit.
     * @throws std::runtime_error if the array_size parameter is not a power of 2.
     */
    [[maybe_unused]] explicit HashingWithChaining(const unsigned int& array_size, const unsigned int& seed, hash_func_type hash_func)
    {
     // Checking that 64-bit numbers are used c.f. exercise 6.
     if(!sizeof(value_type) * BITS_PR_BYTE == 64) throw std::runtime_error("'value_type' used in Sketch template should be 64-bit.");
     // For this purpose we assume 'array_size' to be r=2^R (i.e. power of 2).
     if((array_size & (array_size - 1)) != 0) throw std::runtime_error("Array size given to Sketch C-tor should be power of 2.");
     this->array_size = array_size;

     // Constant for multiply-shift hash function.
     this->a = get_random_odd_uint32(seed, this->multiply_shift_upper_bound);
     this->l = fast_uint64_log_2(this->array_size); // if m = 2^l then l = log2(m).
     this->empty = true;
     initialize_hash_table();
     initialize_consts(seed);

     // Initializing hash function as attribute.
        this->hash_function = hash_func;
    }

    // Methods
    /**
     * Computes a hash value using the provided arguments and the hash function
     * specified in the constructor.
     *
     * @tparam Args The types of the arguments to hash (see class template).
     * @param args The values to hash.
     * @return The hash value computed by the hash function.
     */
    hash_return_type hash(hash_args... args) {
        return this->hash_function(args...);
    }

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
        key_type array_index;
        if constexpr (is_pair<hash_return_type>::value) {
            // if hash_return_type is std::pair
            auto result = hash(static_cast<int64_t>(key),
                                                   static_cast<uint64_t>(this->array_size),
                                                   this->my_hash_constants);
            array_index = static_cast<key_type>(result.second);
        }
        else {
            // if hash_return_type is not std::pair but single int
            array_index = hash(static_cast<uint32_t>(key),
                                     static_cast<uint32_t>(this->a),
                                     static_cast<uint32_t>(this->l));
        }

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
        /// THIS PART MUST RETURN FALSE OTHERWISE VERY BAD THINGS WILL HAPPEN ///////
        return std::make_tuple(NAN_TOKEN, NAN_TOKEN, false);
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
     * Computes the sum of squares of all values stored in the hash table.
     *
     * @return The sum of squares of all values stored in the hash table.
     */
    sum_type query() {

        sum_type result = 0;
        // Iterate over each linked list in array
        for(const list_type& linked_list : this->hash_table)
        {
            // Only sum over entries if list holds any values - if bucket is not empty.
            if(!linked_list.empty())
            {
                // Use std::accumulate and lambda function to compute the sum of squares of each list
                auto list_result = std::accumulate(linked_list.begin(), linked_list.end(), static_cast<sum_type>(0),
                                                   [](const auto& acc, const auto& pair) {return acc + pair.second * pair.second;});
                result += list_result;
            }
        }
        return result;
    }
};