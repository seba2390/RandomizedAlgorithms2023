//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "lib/Utilities.hpp"

int main()
{

    auto TICKET_FRACTIONS = {2, 4, 10, 20};

    for (auto fraction : TICKET_FRACTIONS)
    {
        auto TICKET_FRACTION = static_cast<uint32_t>(fraction);
        const uint32_t NR_CUSTOMERS = 1000;
        const uint32_t NR_TRIALS = 1000;

        auto standard_result = fixed_pricing_revenue_float_32(TICKET_FRACTION,NR_CUSTOMERS,NR_TRIALS);
        auto optimal_result  = variable_pricing_revenue_float_32(TICKET_FRACTION,NR_CUSTOMERS,NR_TRIALS);


        // Saving errors to drive
        std::string filename = "Exercise_1_ticket_fraction_" + std::to_string(TICKET_FRACTION) + ".txt";
        std::string folder_path = "../../../../Data/Exercise_1";
        // Removing possibly already existing file with name 'filename' from drive.
        remove_file(filename, folder_path);
        // Writing out file
        for (unsigned int i = 0; i < std::get<0>(standard_result).size(); i++) {
            auto n_customers = static_cast<float_32_type>(std::get<2>(standard_result)[i]);
            float_32_type standard_avg_revenue   = std::get<0>(standard_result)[i];
            float_32_type standard_std_deviation = std::get<1>(standard_result)[i];
            float_32_type optimal_avg_revenue   = std::get<0>(optimal_result)[i];
            float_32_type optimal_std_deviation = std::get<1>(optimal_result)[i];

            append_to_file(filename, folder_path, {n_customers,
                                                   standard_avg_revenue,
                                                   standard_std_deviation,
                                                   optimal_avg_revenue,
                                                   optimal_std_deviation});
        }
    }





}