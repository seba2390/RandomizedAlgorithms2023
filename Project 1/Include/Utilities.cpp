//
// Created by Sebastian Yde Madsen on 02/03/2023.
//

#include "Utilities.hpp"

key_type get_random_uint32(const key_type& seed) {
    // create a random number generator and seed it.
    std::mt19937 generator(seed);

    // create a uniform distribution that generates values in the range [0, 2^bit-size(keytype) - 1]
    std::uniform_int_distribution<key_type> distribution(0, std::pow(2,KEY_BIT_SIZE) - 1);

    // generate and return a random bit-size(keytype)-bit integer
    return (key_type)distribution(generator);
}

key_type get_random_odd_uint32(const key_type& seed)
{
    unsigned int counter = 1;
    key_type a =  get_random_uint32(seed);
    while ( a % 2 == 0) {
        a = get_random_uint32(seed + counter);
        counter++;
    }
    return a;
}

key_type hash(key_type key, key_type n, key_type a, key_type l)
{
    /*
     * Multiply-shift hashing function. Only maps to a power of two: [2^w] -> [2^l]. As such, choose
     * hashtable with size that is a power of 2, i.e. m = 2^l.
     *
     * N.B. This hash function is 2-approx. universal.
     * */
    return (a * key) >> (KEY_BIT_SIZE - l);
}

array_type generate_ordered_keys(const unsigned int& n)
{
    std::vector<key_type> keys;
    keys.reserve(n);    // allocate memory for the array/vector
    keys.resize(n); // initialize the array/vector with the given size
    for(key_type i = 0; i < n; i++) keys[i] = 100 * i; // Setting keys in array/vector.
    return keys;
}

array_type generate_random_keys(const unsigned int& n, const unsigned int& seed)
{
    std::vector<key_type> keys;
    keys.reserve(n);    // allocate memory for the array/vector
    keys.resize(n); // initialize the array/vector with the given size
    for(key_type i = 0; i < n; i++) keys[i] = get_random_uint32(seed); // Setting keys in array/vector.
    return keys;
}

void append_to_file(std::string filename, std::string path, std::vector<output_data_type> data)
{
    std::ofstream output_stream(path+"/"+filename, std::ofstream::app); // Appending to end of file
    for(auto data_point : data) output_stream << data_point << "    ";
    output_stream << std::endl;
    output_stream.close();
}

void remove_file(std::string filename,  std::string path)
{
    std::filesystem::path folder_path = "../../Data";
    if(std::filesystem::exists(path / (std::filesystem::path)filename))
    {
        // Remove the file
        if (std::filesystem::remove((std::filesystem::path)path / filename)) {
            std::cout << "\nPre-existing file " << (std::filesystem::path)path / filename << " was successfully deleted." << std::endl;
        } else {
            std::cerr << "Error deleting file: " << (std::filesystem::path)path / filename << std::endl;
        }
    }
}