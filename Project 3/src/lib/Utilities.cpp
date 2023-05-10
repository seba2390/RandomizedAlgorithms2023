//
// Created by Sebastian Yde Madsen on 02/03/2023.
//

#include "lib/Utilities.hpp"


float_32_type get_random_float_32(const uint32_t& seed) {
    // create a random number generator and seed it.
    XoshiroCpp::Xoshiro128PlusPlus generator(seed);

    // Generate random uniform float in [0,1)
    return XoshiroCpp::FloatFromBits(generator());
}

float_64_type get_random_float_64(const uint64_t& seed) {
    // create a random number generator and seed it.
    XoshiroCpp::Xoshiro256PlusPlus generator(seed);

    // Generate random uniform float in [0,1)
    return XoshiroCpp::DoubleFromBits(generator());
}


float_64_type std_deviation_float_64(const col_vector_64_type& x_arr) {
    auto n = static_cast<int64_t>(x_arr.size());

    float_64_type mean = x_arr.mean();

    // Compute the deviation from the mean and store it in a temporary vector
    col_vector_64_type deviation = x_arr.array() - mean;

    // Compute the sum of squares of the deviations using vectorized multiplication and reduction
    float_64_type sum_of_squares = deviation.squaredNorm();

    // Compute the standard deviation using Eigen's optimized square root function
    return static_cast<float_64_type>(std::sqrt(1.0 / static_cast<float_64_type>((n - 1)) * sum_of_squares));
}

float_32_type std_deviation_float_32(const col_vector_32_type& x_arr) {
    auto n = static_cast<int64_t>(x_arr.size());

    float_32_type mean = x_arr.mean();

    // Compute the deviation from the mean and store it in a temporary vector
    col_vector_32_type deviation = x_arr.array() - mean;

    // Compute the sum of squares of the deviations using vectorized multiplication and reduction
    float_32_type sum_of_squares = deviation.squaredNorm();

    // Compute the standard deviation using Eigen's optimized square root function
    return static_cast<float_32_type>(std::sqrt(1.0 / static_cast<float_32_type>((n - 1)) * sum_of_squares));
}




std::tuple<std::vector<float_32_type>, std::vector<float_32_type>, std::vector<uint32_t>> fixed_pricing_revenue_float_32(
        uint32_t ticket_fraction, uint32_t nr_customers, uint32_t nr_trials) {

    if (nr_customers <= 10) {
        throw std::invalid_argument("Should have n > 10");
    }

    std::vector<float_32_type> avg_revenues;
    std::vector<float_32_type> std_deviations;
    std::vector<uint32_t> n_customers;

    for (uint32_t n = 10; n < nr_customers+1; n++) {
        uint32_t nr_tickets = n / ticket_fraction; // Integer round down
        float_32_type fixed_price = static_cast<float_32_type>(1.0) - static_cast<float_32_type>(nr_tickets) / static_cast<float_32_type>(n);
        col_vector_32_type revenues(nr_trials);

        // Repeating selling to "n" customers multiple times
        for (uint32_t trial = 0; trial < nr_trials; trial++) {
            uint32_t ticket_nr = 1;
            float_32_type revenue = 0.0;

            // Selling "nr_tickets" to "n" customers
            for (uint32_t customer_nr = 1; customer_nr < n+1; customer_nr++) {
                // If any tickets left - try selling
                if (ticket_nr <= nr_tickets) {
                    // uint32_t SEED = std::chrono::system_clock::now().time_since_epoch().count();
                    float_32_type v_i = get_random_float_32(customer_nr + 3711 * trial);
                    if (v_i >= fixed_price) {
                        revenue   += fixed_price;
                        ticket_nr += 1;
                    }
                }
                    // If no tickets left - move to next simulation
                else {
                    break;
                }
            }
            revenues[trial] = revenue;
        }

        n_customers.push_back(n);
        avg_revenues.push_back(revenues.mean());
        std_deviations.push_back(std_deviation_float_32(revenues));
    }

    return std::make_tuple(avg_revenues, std_deviations, n_customers);
}


std::tuple<std::vector<float_32_type>, std::vector<float_32_type>, std::vector<uint32_t>> variable_pricing_revenue_float_32(
        uint32_t ticket_fraction, uint32_t nr_customers, uint32_t nr_trials) {

    if (nr_customers <= 10) {
        throw std::invalid_argument("Should have n > 10");
    }

    auto p_kn = [](uint32_t n, uint32_t k, const matrix_32_type &exps) -> float_32_type {
        return (1 - exps(n - 1, k - 1) + exps(n - 1, k)) / 2;
    };

    auto e_kn = [&p_kn](uint32_t n, uint32_t k, const matrix_32_type &exps) -> float_32_type {
        float_32_type p = p_kn(n, k, exps);
        return (1 - p) * (p + exps(n - 1, k - 1)) + p * exps(n - 1, k);
    };

    matrix_32_type P_init = matrix_32_type::Zero(nr_customers+1, nr_customers / ticket_fraction+1);
    matrix_32_type E = matrix_32_type::Zero(nr_customers+1, nr_customers / ticket_fraction+1);

    for (uint32_t n = 0; n < nr_customers + 1 ; ++n) {
        for (uint32_t k = 0; k < nr_customers / ticket_fraction + 1; ++k) {
            if (k == 0 || n == 0) {
                E(n, k) = 0.0;
            } else {
                P_init(n, k) = p_kn(n, k, E);
                E(n, k) = e_kn(n, k, E);
            }
        }
    }
    // Slice matrix to remove first row and first column and flip matrix across both diagonals
    matrix_32_type P = P_init.block(1, 1, P_init.rows() - 1, P_init.cols() - 1).reverse();

    std::vector<float_32_type> avg_revenues;
    std::vector<float_32_type> std_deviations;
    std::vector<uint32_t> n_customers;

    for (uint32_t n = 10; n < nr_customers + 1; ++n) {
        uint32_t nr_tickets = n / ticket_fraction;
        col_vector_32_type revenues(nr_trials);

        // Repeating selling to "n" customers multiple times
        for (uint32_t trial = 0; trial < nr_trials; trial++) {
            uint32_t ticket_nr = 1;
            float_32_type revenue = 0.0;

            // Selling "nr_tickets" to "n" customers
            for (uint32_t customer_nr = 1; customer_nr < n + 1; customer_nr++) {
                // If any tickets left - try selling
                if (ticket_nr <= nr_tickets) {
                    //uint32_t SEED = std::chrono::system_clock::now().time_since_epoch().count();
                    float_32_type v_i = get_random_float_32(customer_nr + 131 * trial);
                    if (v_i >= P(customer_nr-1, ticket_nr-1)) {
                        revenue   += P(customer_nr-1, ticket_nr-1);
                        ticket_nr += 1;
                    }
                }
                    // If no tickets left - move to next simulation
                else {
                    break;
                }
            }
            revenues[trial] = revenue;
        }

        n_customers.push_back(n);
        avg_revenues.push_back(revenues.mean());
        std_deviations.push_back(std_deviation_float_32(revenues));

    }

    return std::make_tuple(avg_revenues, std_deviations, n_customers);
}





void append_to_file(const std::string& filename, const std::string& path, const std::vector<float_32_type>& data)
{
    // Check if the path exists
    if (!std::filesystem::exists(path)) {
        std::cerr << "Error: path " << path << " does not exist" << std::endl;
        return;
    }

    std::ofstream output_stream(path+"/"+filename, std::ofstream::app); // Appending to end of file
    for(auto data_point : data) output_stream << data_point << "    ";
    output_stream << std::endl;
    output_stream.close();
}


void remove_file(const std::string& filename,  const std::string& path)
{
    if(std::filesystem::exists(path / (std::filesystem::path)filename))
    {
        // Remove the file
        if (std::filesystem::remove((std::filesystem::path)path / filename)) {
            //std::cout << "\nPre-existing file " << (std::filesystem::path)path / filename << " was successfully deleted." << std::endl;
        } else {
            std::cerr << "Error deleting file: " << (std::filesystem::path)path / filename << std::endl;
        }
    }
}

