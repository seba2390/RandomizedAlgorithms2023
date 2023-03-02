//
// Created by Sebastian Yde Madsen on 02/03/2023.
//

#ifndef PROJECT_1_UTILITIES_HPP
#define PROJECT_1_UTILITIES_HPP

#include <iostream>
#include <climits>
#include <list>
#include <vector>
#include <random>

#define KEY_BIT_SIZE 32
#define SEED 12

// Defining types.
using key_type = uint32_t;
using array_type = std::vector<key_type>;

/* N.B. the std::list in c++ is a doubly linked list,
 * i.e. nodes of (pointer to prev, pointer to next, data)
 * It has search O(n), and delete/insert O(1).
 * */
using linked_list_type = std::list<key_type>;

key_type get_random_uint32(const key_type& seed);

key_type get_random_odd_uint32(const key_type& seed);

key_type hash(key_type key, key_type n);

array_type generate_keys(const unsigned int& n);

#endif //PROJECT_1_UTILITIES_HPP
