//
// Created by Sebastian Yde Madsen on 01/03/2023.
//

#include "Utilities.hpp"


template <typename key_type, typename array_type>
class PerfectHashing
{
private:

    // Attributes
    unsigned int m;

    key_type l;
    key_type a;   // Rng. const for functions hashing to entries in outer table.
    array_type A; // Rng. consts for the m hash functions hashing from outer table -> inner tables.
    column_vector outer_collisions; // j'th entry = nr. keys hashed to j'th entry in outer table.



    // Methods
    void initialize_outer_table()
    {
        outer_table.reserve(this->m);       // allocate memory for the array/vector
        outer_table.resize(this->m);        // initialize the array/vector with the given size
    }

    void initialize_inner_tables()
    {
        array_type inner_table;
        std::fill(this->outer_table.begin(), this->outer_table.end(), array_type{});
    }

    void generate_hash_consts(const unsigned int& seed)
    {
        for(int table_entry = 0; table_entry < this->m; table_entry++)
        {
            this->A[table_entry] = get_random_odd_uint32(seed);
        }
    }

    void initialize_consts(const unsigned int& seed)
    {
        A.reserve(this->m);           // allocate memory for the array/vector
        A.resize(this->m);            // initialize the array/vector with the given size
        this->outer_collisions.resize(this->m);

        this->l = std::log2(this->m); // if m = 2^l then l = log2(m)
        this->a = get_random_odd_uint32(seed);
    }

    unsigned int sum_of_squares()
    {
        unsigned int tot_size = 0;
        for(array_type inner_table : outer_table) tot_size += (unsigned int)std::pow(inner_table.size(),2);
        return tot_size;
    }

public:

    // Attributes
    std::vector<array_type> outer_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit PerfectHashing(const unsigned int& n)
    {
        this->m = n;
        initialize_outer_table();
        initialize_inner_tables();
    }

    // Methods
    void insert(const key_type& key, const key_type& a, array_type& inner_table)
    {
        inner_table[hash(key, this->m, a, this->l)].push_back(std::move(key));
    }

    void insert_keys(const array_type& keys, const unsigned int& seed)
    {

        // Sum of squares should be O(n) (prob 1/2 to be less than 4*c*n).
        unsigned int seed_shift = 0;
        do{
            outer_collisions.setZero();
            this->a = get_random_odd_uint32(seed + seed_shift);
            for(int j = 0; j < this->m; j++){
                this->outer_collisions[hash(keys[j], this->m, this->a, this->l)] += 1;
            }
            seed_shift++;
        }
        while(this->outer_collisions.squaredNorm() >= std::pow(this->m,2));

        // Setting sizes of inner tables
        for(int j = 0; j < this->m; j++)
        {
            if(this->outer_collisions[j] > 0){
                // TODO: How do I make sure this is a power of 2 (for use of multiply-shift hashing)?
                this->outer_table[j].reserve(std::pow(this->outer_collisions[j],2));
                this->outer_table[j].resize(std::pow(this->outer_collisions[j],2));
            }
        }

        // Making sure that there are no collisions in inner tables


    }

    bool holds(const key_type& key)
    {
        /*
         * Checks whether the provided key is stored in the hash table.
         */
        key_type index = hash(key, this->m, this->a, this->l);

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