//
// Created by Sebastian Yde Madsen on 01/03/2023.
//

#include "Utilities.hpp"


template <typename key_type, typename array_type, typename list_type>
class PerfectHashing
{
private:
    using inner_hash_table_type = std::vector<list_type>;

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
        outer_table.resize(this->m);           // initialize the array/vector with the given size
        for(key_type i = 0; i < this->m; i++) this->outer_table[i] = inner_hash_table_type{};
    }

    void initialize_inner_table(const unsigned int& m, inner_hash_table_type& inner_table)
    {
        inner_table.reserve(m);    // allocate memory for the array/vector
        inner_table.resize(m);        // initialize the array/vector with the given size
        for(key_type i = 0; i < this->m; i++) inner_table[i] = std::list<key_type>{}; // Setting lists in array/vector.
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

    void clear_lists(const inner_hash_table_type& inner_table)
    {
        for(list_type linked_list: inner_table)
        {
            linked_list.clear();
        }
    }


    bool has_collisions(const inner_hash_table_type& inner_table)
    {
        unsigned int list_length;
        for(list_type linked_list: inner_table)
        {
            list_length = std::distance(linked_list.begin(), linked_list.end());
            if (list_length>1)
            {
                return true;
            }
        }
        return false;
    }

public:

    // Attributes
    std::vector<inner_hash_table_type> outer_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit PerfectHashing(const unsigned int& n, const unsigned int& seed)
    {
        this->m = n;
        initialize_outer_table();
        initialize_consts(seed);
    }

    // Methods
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
                initialize_inner_table(std::pow(this->outer_collisions[j],2), this->outer_table[j]);
            }
        }

        // Initial deposit of keys in inner tables
        unsigned int outer_index, inner_index, m_j, l_j, a_j;
        for(int j = 0; j < this->m; j++)
        {
            outer_index = hash(keys[j], this->m, this->a, this->l);

            m_j = this->outer_table[outer_index].size();
            // TODO: If m_j is not power of 2 (for use of multiply-shift hashing) - what should l_j be?
            l_j = std::log2(m_j);
            a_j = A[outer_index];

            inner_index = hash(keys[j], m_j, a_j, l_j);
            (this->outer_table[outer_index])[inner_index].push_back(keys[j]);
        }

        // Making sure that there are no collisions in inner tables
        unsigned int array_size;
        generate_hash_consts(seed);
        for(int j = 0; j < this->m; j++)
        {
            seed_shift = 0;
            do{
                outer_index = hash(keys[j], this->m, this->a, this->l);

                m_j = this->outer_table[outer_index].size();
                // TODO: If m_j is not power of 2 (for use of multiply-shift hashing) - what should l_j be?
                l_j = std::log2(m_j);
                a_j = A[outer_index];

                inner_index = hash(keys[j], m_j, a_j, l_j);
                this->A[outer_index] = get_random_odd_uint32(seed + seed_shift);

                if(has_collisions(this->outer_table[outer_index]))
                {
                    clear_lists(this->outer_table[outer_index]);
                    seed_shift++;
                }

            } while (has_collisions(this->outer_table[outer_index]));
        }

        // Final deposit of keys in inner tables
        for(int j = 0; j < this->m; j++)
        {
            outer_index = hash(keys[j], this->m, this->a, this->l);

            m_j = this->outer_table[outer_index].size();
            // TODO: If m_j is not power of 2 (for use of multiply-shift hashing) - what should l_j be?
            l_j = std::log2(m_j);
            a_j = A[outer_index];

            inner_index = hash(keys[j], m_j, a_j, l_j);
            (this->outer_table[outer_index])[inner_index].push_back(keys[j]);
        }
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