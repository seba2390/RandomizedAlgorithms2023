//
// Created by Sebastian Yde Madsen on 23/03/2023.
//

#include "Utilities.hpp"

template <typename value_type, typename pair_type, typename array_type, typename hash_return_type, typename... hash_args>
class Sketch
{
private:
    // Typedefs
    using hash_table_type = array_type;
    using sum_type = int64_t;
    using hash_func_type = std::function<hash_return_type(hash_args...)>;

    // Attributes
    unsigned int array_size;
    value_type mersenne_upper_bound = MERSENNE_PRIME;
    hashing_constants my_hash_constants;
    hash_func_type hash_function;


    // Methods
    /**
     * Initializes the hash table by allocating memory for the array/vector and filling it with nullified pairs.
     *
     * @return void
     */
    void initialize_hash_table()
    {
        this->hash_table.reserve(this->array_size);        // allocate memory for the array/vector
        this->hash_table.resize(this->array_size);        // initialize the array/vector with the given size
        // Filling with nullified pairs.
        for(unsigned int entry = 0; entry < this->array_size; entry++) this->hash_table[entry] = (value_type)0;
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
public:

    // Attributes
    hash_table_type hash_table;

    /**
     * Constructs a new Sketch object with the given array size and seed.
     *
     * @param array_size The size of the array for the hash table. Must be a power of 2.
     * @param seed The seed used to generate the hash function constants.
     * @throws std::runtime_error if
     *                              1 - The array size is not a power of 2.
     *                              2 - The value_type is not 64-bit.
     *                              3 - The return type of hash func is not std::pair<some_type_1, some_type_2>.
     */
    [[maybe_unused]] explicit Sketch(const unsigned int& array_size, const unsigned int& seed, hash_func_type hash_func)
    {
        // For this purpose we assume 'array_size' to be r=2^R (i.e. power of 2).
        if((array_size & (array_size - 1)) != 0) throw std::runtime_error("Array size given to Sketch C-tor should be power of 2.");
        // Checking that 64-bit numbers are used c.f. exercise 6.
        if(!sizeof(value_type) * BITS_PR_BYTE == 64) throw std::runtime_error("'value_type' used in Sketch template should be 64-bit.");
        // Checking that return type of provided hash function is always std::pair<type_1,type_2>
        if constexpr (!is_pair<hash_return_type>::value) throw std::runtime_error("Return type of provided hash function is not std::pair<some_type_1, some_type_2>.");


        this->array_size = array_size;
        initialize_hash_table();
        initialize_consts(seed);
        this->hash_function = hash_func;
    }

    // Methods
    void update(const pair_type& pair)
    {
        // First value in pair is key 'i' and second is update-value 'delta'.
        pair_type result = hash(static_cast<int64_t>(pair.first),
                                static_cast<uint64_t>(this->array_size),
                                this->my_hash_constants);
        (this->hash_table)[result.second] += result.first * pair.second;  // A[h(i)] += g(i) * delta;

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
        sum_type result = 0;
        if constexpr (std::is_same_v<array_type, std::vector<typename array_type::value_type, typename array_type::allocator_type>>) {
            // If array_type is std::vector, use std::inner_product to compute the sum of squares
            return std::inner_product(this->hash_table.begin(), this->hash_table.end(), this->hash_table.begin(), static_cast<sum_type>(0));
        } else {
            // If array_type is not std::vector, use std::accumulate and lambda function to compute the sum of squares
            result += std::accumulate(this->hash_table.begin(), this->hash_table.end(), static_cast<sum_type>(0), [](const auto& acc, const auto& val) {
                return acc + val * val;
            });
            return result;
        }
    }
};
