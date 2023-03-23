//
// Created by Sebastian Yde Madsen on 02/03/2023.
//

#ifndef PROJECT_2_UTILITIES_HPP
#define PROJECT_2_UTILITIES_HPP

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
#include <array>
#include <type_traits>
#include <numeric> // std::inner_product for std::vector

#include <Eigen/Dense>

#include <Xoshiro.hpp>

#define KEY_BIT_SIZE 32
#define NAN_TOKEN 0
#define MERSENNE_PRIME_EXPONENT 31
#define MERSENNE_PRIME (1ULL << MERSENNE_PRIME_EXPONENT) - 1
#define BITS_PR_BYTE CHAR_BIT

// Defining types.
using key_type = int32_t;
using value_type = int64_t;
using pair_type = std::pair<value_type,value_type>;
using array_type = std::vector<value_type>;

struct hashing_constants {
    // Storing as 64-bit because exercise 4 requires it to evaluate 'mod' operation efficiently.
    uint64_t a;
    uint64_t b;
    uint64_t c;
    uint64_t d;
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

std::pair<int64_t,int64_t> mersenne_4_independent_hash(key_type key, key_type array_size,  hashing_constants constants);

std::pair<int64_t,int64_t> slow_mersenne_4_independent_hash(key_type key, key_type array_size, hashing_constants constants);

uint32_t fast_uint32_pow_2(const uint32_t& power);

uint64_t fast_uint64_pow_2(const uint64_t& power);

int32_t fast_uint32_log_2(uint32_t x);

int64_t fast_uint64_log_2(uint64_t x);

void append_to_file(std::string filename, std::string path, std::vector<output_data_type> data);

void remove_file(std::string filename, std::string path);

template <typename T>
void print(const T &value);

#endif //PROJECT_2_UTILITIES_HPP
