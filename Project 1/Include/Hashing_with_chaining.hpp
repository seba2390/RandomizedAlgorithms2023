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

    // Methods
    void initialize_hash_table()
    {
        hash_table.reserve(this->m);    // allocate memory for the array/vector
        hash_table.resize(this->m);        // initialize the array/vector with the given size
        for(key_type i = 0; i < this->m; i++) hash_table[i] = std::list<key_type>{}; // Setting lists in array/vector.
    }

public:

    // Attributes
    hash_table_type hash_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit HashingWithChaining(const unsigned int& n)
    {
     this->m = n;
     initialize_hash_table();
    }

    // Methods
    void insert(const key_type& key)
    {
        this->hash_table[hash(key,this->m)].push_back(key);
    }

    void insert_keys(const array_type& keys)
    {
        for(key_type key : keys) insert(key);
    }

    bool has(const key_type& key)
    {
        /*
         * Check whether the provided keys is stored in the hash table.
         */
        key_type index = hash(key, this->m);

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


};