//
// Created by Sebastian Yde Madsen on 23/03/2023.
//

#include "Utilities.hpp"

template <typename value_type, typename pair_type, typename array_type>
class Sketch
{
private:
    // Typedefs
    using hash_table_type = array_type;
    using sum_type = int64_t;

    // Attributes
    unsigned int array_size;

    // Equivalent to 2^MERSENNE_PRIME_EXPONENT - 1
    value_type mersenne_upper_bound = MERSENNE_PRIME;
    hashing_constants mersenne_hashing_constants;



    // Methods
    void initialize_hash_table()
    {
        this->hash_table.reserve(this->array_size);        // allocate memory for the array/vector
        this->hash_table.resize(this->array_size);        // initialize the array/vector with the given size
        // Filling with nullified pairs.
        for(int entry = 0; entry < this->array_size; entry++) this->hash_table[entry] = (value_type)0;
    }

    void set_mersenne_hash_constants(const unsigned int& seed)
    {
        // Constant for other 4-wise independent hash function  (remember to use different seeds).
        this->mersenne_hashing_constants.a = get_random_uint64(seed+0, this->mersenne_upper_bound);
        this->mersenne_hashing_constants.b = get_random_uint64(seed+11, this->mersenne_upper_bound);
        this->mersenne_hashing_constants.c = get_random_uint64(seed+431, this->mersenne_upper_bound);
        this->mersenne_hashing_constants.d = get_random_uint64(seed+78, this->mersenne_upper_bound);
    }



    void initialize_consts(const unsigned int& seed)
    {
        /*
         * Initializing constants for hash function here
         * to avoid continuous recalculation when
         * calling hash function.
         * */

        // Constant for other 4-wise independent hash function  (remember to use different seeds).
        set_mersenne_hash_constants(seed);
    }

public:

    // Attributes
    hash_table_type hash_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit Sketch(const unsigned int& array_size, const unsigned int& seed)
    {
        // For this purpose we assume 'array_size' to be r=2^R (i.e. power of 2).
        if((array_size & (array_size - 1)) != 0) throw std::runtime_error("Array size given to Sketch C-tor should be power of 2.");
        this->array_size = array_size;
        initialize_hash_table();
        initialize_consts(seed);
    }

    // Methods
    void update(const pair_type& pair)
    {
        // First value in pair is key 'i' and second is update-value 'delta'.
        pair_type result = mersenne_4_independent_hash(pair.first,
                                                       this->array_size,
                                                       this->mersenne_hashing_constants);
        (this->hash_table)[result.first] += result.second * pair.second;  // A[h(i)] += g(i) * delta;

    }

    sum_type query()
    {
        // Checking that 64-bit numbers are used c.f. exercise 6.
        if(!sizeof(value_type) * BITS_PR_BYTE == 64) throw std::runtime_error("'value_type' used in Sketch template should be 64-bit.");

        // Using std method for norm square if array_type is std::vector
        if constexpr (std::is_same_v<array_type, std::vector<typename array_type::value_type, typename array_type::allocator_type>>)
        {
            return std::inner_product(this->hash_table.begin(),this->hash_table.end(), this->hash_table, 0);
        }
        // If array_type is not std::vector but some other iterable type container.
        else
        {
            sum_type result = 0;
            for(const value_type& value: this->hash_table) result += value*value;
            return result;
        }

    }
};