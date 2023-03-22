//
// Created by Sebastian Yde Madsen on 01/03/2023.
//

#include "Utilities.hpp"


template <typename key_type, typename pair_type, typename array_type, typename list_type>
class HashingWithChaining
{
private:
    // Typedefs
    using hash_table_type = std::vector<list_type>;
    using sum_type = int64_t;

    // Attributes
    unsigned int m;
    key_type a, l;
    bool empty;


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
        this->empty = true;
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
    void insert(const pair_type& pair)
    {
        key_type key = pair.first;
        this->hash_table[hash(key, this->a, this->l)].push_back(std::move(pair));
    }

    void insert_key_value_pairs(const array_type& key_value_pairs)
    {
        for(pair_type pair : key_value_pairs) insert(pair);
    }

    std::tuple<key_type, key_type, bool> holds(const key_type& key)
    {
        /*
         * Checks whether the provided key is stored in the hash table.
         */
        key_type array_index = hash(key, this->a, this->l);

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
    unsigned int max_bucket_size()
    {
        unsigned int max_size = 0;
        for(int m_i = 0; m_i < this->m; m_i++)
        {
            if(this->hash_table[m_i].size() > max_size) max_size = this->hash_table[m_i].size();
        }
        return max_size;
    }

    void add(key_type key, int32_t delta)
    {

        // Only performing checks if table holds any pairs
        if(!this->empty){
            // Checking if key is already in table
            std::tuple<key_type,key_type,bool> result = holds(key);
            if(std::get<2>(result))
            {
                // Then just add delta to value in pair
                key_type array_index = std::get<0>(result);
                key_type list_index = std::get<1>(result);
                (this->hash_table[array_index])[list_index].second += delta;
            } else
            {
                // Then just append pair to end of list
                key_type array_index = hash(key, this->a, this->l);
                (this->hash_table[array_index]).push_back(std::make_pair(key, 0+delta));
            }
        } else
        {
            // Then just append pair to end of list
            key_type array_index = hash(key, this->a, this->l);
            (this->hash_table[array_index]).push_back(std::make_pair(key, 0+delta));
            // Set table not empty
            this->empty = false;
        }

    }

    sum_type sum_of_squares()
    {
        sum_type result = 0;
        for(const list_type& list: this->hash_table)
            if(!list.empty())
            {
                for(const pair_type& pair: list)
                {
                    sum_type v = static_cast<sum_type>(pair.second); // TODO: Only necessary if key_type is 32-bit.
                    result += v*v;
                }
            }
        return result;
    }
};