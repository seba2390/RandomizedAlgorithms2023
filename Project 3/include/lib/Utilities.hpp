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

#include <boost/timer/progress_display.hpp>
#include <boost/timer/timer.hpp>

#include <lib/Xoshiro.hpp>
#include <Eigen/Core>

// Defining types.
using float_32_type = float_t;
using float_64_type = double_t;

using matrix_32_type = Eigen::Matrix<float_32_type, Eigen::Dynamic, Eigen::Dynamic>;
using matrix_64_type = Eigen::Matrix<float_64_type, Eigen::Dynamic, Eigen::Dynamic>;

using row_vector_32_type = Eigen::Matrix<float_32_type, 1, Eigen::Dynamic>;
using row_vector_64_type = Eigen::Matrix<float_64_type, 1, Eigen::Dynamic>;

using col_vector_32_type = Eigen::Matrix<float_32_type, Eigen::Dynamic, 1>;
using col_vector_64_type = Eigen::Matrix<float_64_type, Eigen::Dynamic, 1>;


// Functions
float_32_type get_random_float_32(const uint32_t& seed);
float_64_type get_random_float_64(const uint64_t& seed);

float_32_type std_deviation_float_32(const col_vector_32_type& x_arr);
float_64_type std_deviation_float_64(const col_vector_64_type& x_arr);

std::tuple<std::vector<float_32_type>, std::vector<float_32_type>, std::vector<uint32_t>> fixed_pricing_revenue_float_32(
        uint32_t ticket_fraction, uint32_t nr_customers, uint32_t nr_trials);

std::tuple<std::vector<float_32_type>, std::vector<float_32_type>, std::vector<uint32_t>> variable_pricing_revenue_float_32(
        uint32_t ticket_fraction, uint32_t nr_customers, uint32_t nr_trials);



void append_to_file(const std::string& filename, const std::string& path, const std::vector<float_32_type>& data);

void remove_file(const std::string& filename,  const std::string& path);



#endif //PROJECT_2_UTILITIES_HPP
