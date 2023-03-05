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

#define KEY_BIT_SIZE 32

// Defining types.
using key_type = uint32_t;
using array_type = std::vector<key_type>;
using column_vector = Eigen::Matrix<key_type,Eigen::Dynamic, 1>;

/* N.B. the std::list in c++ is a doubly linked list,
 * i.e. nodes of (pointer to prev, pointer to next, data)
 * It has search O(n), and delete/insert O(1).
 * */
using linked_list_type = std::list<key_type>;
using output_data_type = double_t;

key_type get_random_uint32(const key_type& seed);

key_type get_random_odd_uint32(const key_type& seed);

key_type hash(key_type key, key_type n, key_type a, key_type l);

array_type generate_ordered_keys(const unsigned int& n);

array_type generate_random_keys(const unsigned int& n, const unsigned int& seed);

void append_to_file(std::string filename, std::string path, std::vector<output_data_type> data);

void remove_file(std::string filename, std::string path);

void print_flag();

#endif //PROJECT_1_UTILITIES_HPP
