//
// Created by Sebastian Yde Madsen on 01/03/2023.
//

#include "Utilities.hpp"


template <typename key_type, typename array_type, typename list_type>
class HashingWithChaining
{
private:
    using hash_table_type = std::vector<list_type>;

    // Attributes
    unsigned int m;

    key_type a, l;


    // Methods
    void initialize_hash_table()
    {
        hash_table.reserve(this->m);    // allocate memory for the array/vector
        hash_table.resize(this->m);        // initialize the array/vector with the given size
        for(key_type i = 0; i < this->m; i++) hash_table[i] = std::list<key_type>{}; // Setting lists in array/vector.
    }

    void initialize_consts(const unsigned int& seed)
    {
        /*
         * Initializing constants for hash function here
         * to avoid continuous recalculation when
         * calling hash function.
         * */

        this->a = get_random_odd_uint32(seed);
        this->l = std::log2(this->m); // if m = 2^l then l = log2(m)
    }

public:

    // Attributes
    hash_table_type hash_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit HashingWithChaining(const unsigned int& n, const unsigned int& seed)
    {
     this->m = n;
     initialize_hash_table();
     initialize_consts(seed);
    }

    // Methods
    void insert(const key_type& key)
    {
        this->hash_table[hash(key, this->a, this->l)].push_back(std::move(key));
    }

    void insert_keys(const array_type& keys)
    {
        for(key_type key : keys) insert(key);
    }

    bool holds(const key_type& key)
    {
        /*
         * Checks whether the provided key is stored in the hash table.
         */
        key_type index = hash(key, this->a, this->l);

        // Only start iterating through linked list if bucket is not empty
        if(!this->hash_table[index].empty())
        {
            auto iterator = std::find(this->hash_table[index].begin(), this->hash_table[index].end(), key);
            if(iterator != this->hash_table[index].end())
            {
                return true;
            }
        }
        return false;
    }
    unsigned int max_bucket_size()
    {
        unsigned int max_size = 0;
        for(int m_i = 0; m_i < this->m; m_i++)
        {
            if(this->hash_table[m_i].size() > max_size) max_size = this->hash_table[m_i].size();
        }
        return max_size;
    }


};