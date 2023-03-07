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
    inner_hash_table_type outer_collisions; // j'th entry = linked list of keys hashed to j'th entry in outer table.



    // Methods
    void initialize_outer_table()
    {
        outer_table.reserve(this->m);       // allocate memory for the array/vector
        outer_table.resize(this->m);           // initialize the array/vector with the given size
        for(key_type i = 0; i < this->m; i++) this->outer_table[i] = inner_hash_table_type{};
    }

    void initialize_inner_table(const unsigned int& m, inner_hash_table_type& inner_table)
    {
        /*
         * Initializes inner_table array to size 'm' and fills with empty lists.
         * */
        inner_table.reserve(m);    // allocate memory for the array/vector
        inner_table.resize(m);        // initialize the array/vector with the given size
        for(key_type i = 0; i < m; i++) inner_table[i] = std::list<key_type>{}; // Setting lists in array/vector.
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

    void clear_lists(inner_hash_table_type& inner_table)
    {
        for(int i = 0; i < inner_table.size(); i++)
        {
            if(!inner_table[i].empty())
            {
                inner_table[i].clear();
            }
        }
    }


    bool has_collisions(const inner_hash_table_type& inner_table)
    {
        for(list_type linked_list: inner_table)
        {
            if (linked_list.size()>1)
            {
                return true;
            }
        }
        return false;
    }

    unsigned int sum_of_squares(const inner_hash_table_type& collision_lists)
    {
        /*
         * Calculates sum of squared size of each bucket in outer table, where each squared
         * size is rounded to the nearest power of 2 (due to using multiply-shift hash function).
         * */
        unsigned int sum_of_squares = 0;
        for(list_type list: collision_lists)
        {
            if(!list.empty())
            {
                sum_of_squares +=  std::pow(nearest_power_of_2((key_type)std::pow(list.size(),2)),2);
            }
        }
        return sum_of_squares;
    }

    unsigned int nearest_power_of_2(const key_type& m)
    {
        /*
         * Given a number 'm', calculates the number 'l', l >= 1,  such
         * that the expression |(2^l)-m| is minimized.
         * */

        if(m == 1) return 1; // rounds up to 2^1.
        else
        {
            return (unsigned int)round((double)std::log2(m));
        }
    }
public:

    // Attributes
    std::vector<inner_hash_table_type> outer_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit PerfectHashing(const unsigned int& n, const unsigned int& seed)
    {
        // TODO: should one still use m=n for Perfect hashing ?
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
            clear_lists(this->outer_collisions);
            this->a = get_random_odd_uint32(seed + seed_shift);
            for(int j = 0; j < this->m; j++){
                this->outer_collisions[hash(keys[j], this->m, this->a, this->l)].push_back(keys[j]);
            }
            seed_shift++;
            seed_shift *= 3; // Multiply seed by odd int to avoid getting same a_j even though different seed.
        }
        // TODO: Maybe refine this conditions (do we know 'c' for given hash function) ?
        while(sum_of_squares(this->outer_collisions) >= std::pow(this->m,2));

        // Setting sizes of inner tables and fills them w. empty lists
        unsigned int m_j;
        for(int j = 0; j < this->m; j++)
        {
            if(!this->outer_collisions[j].empty()){
                // TODO: How do I make sure this is a power of 2 (for use of multiply-shift hashing)? - currently rounding to nearest power of 2 above 1.
                m_j = std::pow(2, nearest_power_of_2(std::pow(this->outer_collisions[j].size(),2)));
                initialize_inner_table(m_j, this->outer_table[j]);
            }
        }
        // Initial deposit of keys in inner tables
        unsigned int outer_index, inner_index, l_j, a_j;
        for(int j = 0; j < this->m; j++)
        {
            if(!this->outer_collisions[j].empty())
            {
                outer_index = j;
                l_j = nearest_power_of_2(std::pow(this->outer_collisions[j].size(),2)); // TODO: If m_j is not power of 2 (for use of multiply-shift hashing) - what should l_j be? - currently rounding to nearest power of 2 above 1
                m_j = std::pow(2, l_j);
                a_j = this->A[outer_index];
                for(key_type key: this->outer_collisions[j])
                {
                    inner_index = hash(key, m_j, a_j, l_j);
                    (this->outer_table[outer_index])[inner_index].push_back(key);
                }
            }
        }

        // Making sure that there are no collisions in inner tables
        generate_hash_consts(seed);
        for(int j = 0; j < this->m; j++)
        {
            // Only check inner hash tables that holds any keys
            if(!this->outer_collisions[j].empty())
            {
                seed_shift = 1;
                outer_index = j;
                m_j = std::pow(this->outer_collisions[j].size(),2);
                l_j = std::log2(m_j); // TODO: If m_j is not power of 2 (for use of multiply-shift hashing) - what should l_j be?
                a_j = this->A[outer_index];

                int counter = 0;
                while (has_collisions(this->outer_table[outer_index]))
                {
                    // Remove keys from lists in inner hash table.
                    clear_lists(this->outer_table[outer_index]);

                    // Re-calculate hash function const 'a' for given inner hash table.
                    this->A[outer_index] = get_random_odd_uint32(seed + seed_shift);
                    a_j = this->A[outer_index];


                    // Re-fill keys in given inner hash table.
                    for(key_type key : this->outer_collisions[outer_index])
                    {
                        inner_index = hash(key,m_j,a_j,l_j);
                        (this->outer_table[outer_index])[inner_index].push_back(key);
                    }

                    // Increment seed shift for new hash func seed.
                    seed_shift += 1;
                    seed_shift *= 3; // Multiply seed by odd int to avoid getting same a_j even though different seed.
                    counter++;
                }
            }
        }

    }

    bool holds(const key_type& key)
    {
        /*
         * Checks whether the provided key is stored in the hash table.
         */
        key_type outer_index = hash(key, this->m, this->a, this->l);

        key_type m_j, l_j, a_j;
        m_j = (this->outer_table[outer_index]).size();
        l_j = std::log2(m_j);
        a_j = (this->A)[outer_index];
        key_type inner_index = hash(key,m_j,a_j,l_j);
        // Only start iterating through linked list if bucket is not empty
        if(!(this->outer_table[outer_index])[inner_index].empty())
        {
            auto iterator = std::find((this->outer_table[outer_index])[inner_index].begin(), (this->outer_table[outer_index])[inner_index].end(), key);
            if(iterator != (this->outer_table[outer_index])[inner_index].end())
            {
                return true;
            }
        }
        return false;
    }

};