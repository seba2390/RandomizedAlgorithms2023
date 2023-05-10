//
// Created by Sebastian Yde Madsen on 01/03/2023.
//
#include "lib/Utilities.hpp"

int main()
{

    auto TICKET_FRACTIONS = {5, 10, 20};

    auto counter = 1;
    for (auto fraction : TICKET_FRACTIONS)
    {
        std::cout << "Part a, nr.: " << counter << std::endl;
        auto TICKET_FRACTION = static_cast<uint32_t>(fraction);
        const uint32_t NR_CUSTOMERS = 1000;
        const uint32_t NR_TRIALS = 1000;
        const uint32_t OUTER_SEED = std::chrono::system_clock::now().time_since_epoch().count();

        auto standard_result = fixed_pricing_revenue_float_32(TICKET_FRACTION,NR_CUSTOMERS,NR_TRIALS, OUTER_SEED);
        auto optimal_result  = variable_pricing_revenue_float_32(TICKET_FRACTION,NR_CUSTOMERS,NR_TRIALS,OUTER_SEED);


        // Saving errors to drive
        std::string filename = "Exercise_1_ticket_fraction_" + std::to_string(TICKET_FRACTION) + ".txt";
        std::string folder_path = "../../../../Data/Exercise_1_a";
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
        counter++;
    }

    std::cout << " =================== " << std::endl;

    counter = 1;
    std::vector<float_32_type> avg_abs_rel_distances{};
    std::vector<float_32_type> avg_abs_rel_distances_std_devs{};
    std::vector<float_32_type> fractions{};
    for(uint32_t TICKET_FRACTION = 2; TICKET_FRACTION <= 50; TICKET_FRACTION++)
    {
        std::cout << "Part b, nr.: " << counter << std::endl;

        const uint32_t RERUNS = 100;

        col_vector_32_type temp_avg_abs_rel_distances(RERUNS);

        for(int RUN = 0; RUN < RERUNS; RUN++)
        {
            const uint32_t NR_CUSTOMERS = 200;
            const uint32_t NR_TRIALS = 100;
            const uint32_t OUTER_SEED = std::chrono::system_clock::now().time_since_epoch().count();

            auto standard_result = fixed_pricing_revenue_float_32(TICKET_FRACTION,NR_CUSTOMERS,NR_TRIALS,OUTER_SEED);
            auto optimal_result  = variable_pricing_revenue_float_32(TICKET_FRACTION,NR_CUSTOMERS,NR_TRIALS,OUTER_SEED);
            temp_avg_abs_rel_distances[RUN] = avg_abs_rel_dist(std::get<0>(optimal_result),std::get<0>(standard_result));

        }
        avg_abs_rel_distances.push_back(temp_avg_abs_rel_distances.mean());
        avg_abs_rel_distances_std_devs.push_back(std_deviation_float_32(temp_avg_abs_rel_distances));
        fractions.push_back(static_cast<float_32_type>(TICKET_FRACTION));
        counter++;
    }
    // Saving errors to drive
    std::string filename = "Exercise_1_abs_rel.txt";
    std::string folder_path = "../../../../Data/Exercise_1_b";
    // Removing possibly already existing file with name 'filename' from drive.
    remove_file(filename, folder_path);
    // Writing out file
    for (unsigned int i = 0; i < avg_abs_rel_distances.size(); i++) {

        append_to_file(filename, folder_path, {fractions.at(i),
                                               avg_abs_rel_distances.at(i),
                                               avg_abs_rel_distances_std_devs.at(i)});
    }


}