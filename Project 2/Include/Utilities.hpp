//
// Created by Sebastian Yde Madsen on 02/03/2023.
//

#ifndef PROJECT_1_UTILITIES_HPP
#define PROJECT_1_UTILITIES_HPP

#include <iostream>
#include <climits>
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <random>
#include <chrono>
#include <filesystem>
#include <set>
#include <algorithm>    // std::random_shuffle

#include <Eigen/Dense>

#include <Xoshiro.hpp>

#define KEY_BIT_SIZE 32
#define NAN_TOKEN 0
#define MERSENNE_PRIME_EXPONENT 31

// Defining types.
using key_type = int32_t;
using value_type = int32_t;
using pair_type = std::pair<key_type,value_type>;

struct hashing_constants {
    // Storing as 64-bit because exercise 4 requires it to evaluate 'mod' operation efficiently.
    key_type a;
    key_type b;
    key_type c;
    key_type d;
};

/* N.B. the std::list in c++ is a doubly linked list,
 * i.e. nodes of (pointer to prev, pointer to next, data)
 * It has search O(n), and delete/insert O(1).
 * */
using linked_list_type = std::list<pair_type>;
using output_data_type = double_t;

key_type get_random_uint32(const key_type& seed, const key_type& upper_bound);

uint64_t get_random_uint64(const key_type& seed, const key_type& upper_bound);

key_type get_random_odd_uint32(const key_type& seed, const key_type& upper_bound);

key_type multiply_shift_hash(key_type key, key_type a, key_type l);

uint64_t mersenne_4_independent_hash(key_type key,  hashing_constants constants);

key_type slow_mersenne_4_independent_hash(key_type key,  hashing_constants constants);

void append_to_file(std::string filename, std::string path, std::vector<output_data_type> data);

void remove_file(std::string filename, std::string path);

void print_flag();

#endif //PROJECT_1_UTILITIES_HPP