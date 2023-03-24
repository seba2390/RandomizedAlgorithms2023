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
#include <functional> // for std::forward of args
#include <utility>
#include <typeinfo> //
#include <any>

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
using pair_type = std::pair<key_type,value_type>;
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

uint32_t get_random_uint32(const uint32_t& seed, const uint32_t& upper_bound);

uint64_t get_random_uint64(const uint64_t& seed, const uint64_t& upper_bound);

uint32_t get_random_odd_uint32(const uint32_t& seed, const uint32_t& upper_bound);

uint32_t multiply_shift_hash(uint32_t key, uint32_t a, uint32_t l);

std::pair<int64_t,int64_t> multiply_shift_2_independent(int64_t key, uint64_t array_size, hashing_constants constants);

std::pair<int64_t,int64_t> mersenne_4_independent_hash(int64_t key, uint64_t array_size,  hashing_constants constants);

std::pair<int64_t,int64_t> slow_mersenne_4_independent_hash(int64_t key, uint64_t array_size, hashing_constants constants);

uint32_t fast_uint32_pow_2(const uint32_t& power);

uint64_t fast_uint64_pow_2(const uint64_t& power);

uint32_t fast_uint32_log_2(uint32_t x);

uint64_t fast_uint64_log_2(uint64_t x);

double fast_relative_err(uint64_t a, uint64_t b);

double slow_relative_err(uint64_t a, uint64_t b);

void append_to_file(const std::string& filename, const std::string& path, const std::vector<output_data_type>& data);

void remove_file(const std::string& filename,  const std::string& path);

template<typename T>
struct is_pair : std::false_type {};

template<typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

#endif //PROJECT_2_UTILITIES_HPP
