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
    unsigned int m, n;

    key_type l;
    key_type a;   // Rng. const for functions hashing to entries in outer table.
    array_type A; // Rng. consts for the m hash functions hashing from outer table -> inner tables.
    inner_hash_table_type outer_collisions; // j'th entry = linked list of keys hashed to j'th entry in outer table.
    column_vector outer_collisions_vector;



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
            if(this->outer_collisions_vector[table_entry] != 0) this->A[table_entry] = get_random_odd_uint32(seed);
        }
    }

    void initialize_consts(const unsigned int& seed)
    {
        A.reserve(this->m);           // allocate memory for the array/vector
        A.resize(this->m);            // initialize the array/vector with the given size
        this->outer_collisions.resize(this->m);

        this->l = std::log2(this->m); // if m = 2^l then l = log2(m)
        this->a = get_random_odd_uint32(seed);

        this->outer_collisions_vector.setZero(this->m);
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




public:

    // Attributes
    std::vector<inner_hash_table_type> outer_table;

    // Parameterized C-tor
    [[maybe_unused]] explicit PerfectHashing(const unsigned int& n, const unsigned int& seed)
    {
        // TODO: should one still use m=n for Perfect hashing ?
        const key_type c = 2; // Multiply-shift is 2-approximately universal
        this->m = 4*c*n;
        this->n = n;
        initialize_outer_table();
        initialize_consts(seed);
    }

    // Methods
    void insert_keys(const array_type& keys, const unsigned int& seed)
    {

        //auto start_1= std::chrono::high_resolution_clock::now();

        /////// ----- Sum of squares should be O(n) (prob 1/2 to be less than 4*c*n). ----- ///////
        // Counting collisions
        for(int j = 0; j < this->n; j++){
            this->outer_collisions_vector[hash(keys[j], this->a, this->l)] += 1;
        }
        // Rounding up to the square of each entry to the nearest power of two to enable use of multiply-shift hashing.
        this->outer_collisions_vector.array().square().matrix();
        this->outer_collisions_vector = this->outer_collisions_vector.unaryExpr([](key_type x) {
            if (x == 0) return (key_type)x;
            else return (key_type)std::pow(2, std::ceil(std::log2(x)));
        });
        // Grouping keys.
        for(key_type key : keys)
        {
            this->outer_collisions[hash(key, this->a, this->l)].push_back(key);
        }
        // TODO: Maybe smarter to check that there are no more than n/2 collisions?
        unsigned int seed_shift = 1;
        while(this->outer_collisions_vector.sum() > 4 * this->n){
            // Clearing current values
            this->outer_collisions_vector.setZero();
            clear_lists(this->outer_collisions);

            // Re-setting rng. const for hash func.
            this->a = get_random_odd_uint32(seed + seed_shift * 11);

            // Counting collisions
            for(int j = 0; j < this->n; j++){
                this->outer_collisions_vector[hash(keys[j], this->a, this->l)] += 1;
            }

            // Rounding up to the square of each entry to the nearest power of two to enable use of multiply-shift hashing.
            this->outer_collisions_vector.array().square().matrix();
            this->outer_collisions_vector = this->outer_collisions_vector.unaryExpr([](key_type x) {
                if (x == 0) return (key_type)x;
                else return (key_type)std::pow(2, std::ceil(std::log2(x)));
            });

            // Grouping keys.
            for(key_type key : keys)
            {
                this->outer_collisions[hash(key, this->a, this->l)].push_back(key);
            }

            seed_shift++;
        }

        //auto stop_1= std::chrono::high_resolution_clock::now();
        //std::cout << "Process 1: " << duration_cast<std::chrono::nanoseconds>(stop_1 - start_1).count() << std::endl << std::endl;

        //auto start_2= std::chrono::high_resolution_clock::now();
        // Setting sizes of inner tables and fills them w. empty lists
        unsigned int m_j;
        for(int j = 0; j < this->m; j++)
        {
            if(!this->outer_collisions[j].empty()){
                // Each inner table is initialized to the square of the number of collisions (rounded up to the nearest power of 2)
                m_j = this->outer_collisions_vector[j];
                initialize_inner_table(m_j, this->outer_table[j]);
            }
        }
        //auto stop_2= std::chrono::high_resolution_clock::now();
        //std::cout << "Process 2: " << duration_cast<std::chrono::nanoseconds>(stop_2 - start_2).count() << std::endl << std::endl;


        //auto start_3 = std::chrono::high_resolution_clock::now();
        // Initial deposit of keys in inner tables
        unsigned int  l_j, a_j;
        for(int j = 0; j < this->m; j++)
        {
            if(this->outer_collisions_vector[j] != 0)
            {
                l_j = std::log2(this->outer_collisions_vector[j]);
                a_j = this->A[j];
                for(key_type key: this->outer_collisions[j])
                {
                    (this->outer_table[j])[hash(key, a_j, l_j)].push_back(key);
                }
            }
        }
        //auto stop_3= std::chrono::high_resolution_clock::now();
        //std::cout << "Process 3: " << duration_cast<std::chrono::nanoseconds>(stop_3 - start_3).count() << std::endl << std::endl;

        //auto start_4 = std::chrono::high_resolution_clock::now();
        /////// ----- Making sure that there are no collisions in inner tables. ----- ///////
        generate_hash_consts(seed);
        for(int j = 0; j < this->m; j++)
        {
            // Only check inner hash tables that holds any keys
            if(!this->outer_collisions[j].empty())
            {
                seed_shift = 1;

                //start = std::chrono::high_resolution_clock::now();
                while (has_collisions(this->outer_table[j]))
                {
                    // Remove keys from lists in inner hash table.
                    clear_lists(this->outer_table[j]);

                    // Re-calculate hash function const 'a' for given inner hash table.
                    this->A[j] = get_random_odd_uint32(seed + seed_shift * 11);
                    m_j = this->outer_collisions_vector[j];
                    l_j = std::log2(m_j);
                    a_j = this->A[j];

                    // Re-fill keys in given inner hash table.
                    for(key_type key : this->outer_collisions[j])
                    {
                        (this->outer_table[j])[hash(key,a_j,l_j)].push_back(key);
                    }

                    // Increment seed shift for new hash func seed.
                    seed_shift += 1;
                    seed_shift *= 3; // Multiply seed by odd int to avoid getting same a_j even though different seed.
                }
            }
        }
        //auto stop_4 = std::chrono::high_resolution_clock::now();
        //std::cout << "Process 4: " << duration_cast<std::chrono::nanoseconds>(stop_4 - start_4).count() << std::endl << std::endl;

    }

    bool holds(const key_type& key)
    {
        /*
         * Checks whether the provided key is stored in the hash table.
         */
        key_type outer_index = hash(key, this->a, this->l);
        key_type m_j, l_j, a_j;
        m_j = (this->outer_table[outer_index]).size();
        // Only start iterating through linked list if bucket is not empty
        if (m_j > 1) // If table is length zero hashfunction will not make sense and faulty index will occur in
        {
            l_j = std::log2(m_j);
            a_j = (this->A)[outer_index];
            key_type inner_index = hash(key,a_j,l_j);
            auto iterator = std::find((this->outer_table[outer_index])[inner_index].begin(), (this->outer_table[outer_index])[inner_index].end(), key);
            if(iterator != (this->outer_table[outer_index])[inner_index].end())
            {
                return true;
            }

        }
        if(m_j == 1) return true;
        return false;
    }

};