//
// Created by Sebastian Yde Madsen on 02/03/2023.
//

#include "Utilities.hpp"

key_type get_random_uint32(const key_type& seed, const key_type& upper_bound) {
    // create a random number generator and seed it.
    XoshiroCpp::Xoshiro128PlusPlus generator(seed);

    // create a uniform distribution that generates values in the range [0, 2^bit-size(keytype) - 1]
    std::uniform_int_distribution<key_type> distribution(0, upper_bound); // upper bound = std::pow(2,KEY_BIT_SIZE) - 1 for multiply shift

    // generate and return a random bit-size(keytype)-bit integer
    return (key_type)distribution(generator);
}

uint64_t get_random_uint64(const key_type& seed, const key_type& upper_bound) {
    // create a random number generator and seed it.
    XoshiroCpp::Xoshiro256PlusPlus generator((uint64_t)seed);

    // create a uniform distribution that generates values in the range [0, 2^bit-size(keytype) - 1]
    std::uniform_int_distribution<uint64_t> distribution(0, (uint64_t)upper_bound); // upper bound = std::pow(2,KEY_BIT_SIZE) - 1 for multiply shift

    // generate and return a random bit-size(keytype)-bit integer
    return (uint64_t)distribution(generator);
}

key_type get_random_odd_uint32(const key_type& seed, const key_type& upper_bound)
{
    unsigned int counter = 1;
    key_type a =  get_random_uint32(seed, upper_bound);

    while ( a % 2 == 0) {
        a = get_random_uint32(seed + counter, upper_bound);
        counter++;
    }

    //if(a % 2 == 0) return a+1;
    return a;
}


key_type multiply_shift_hash(key_type key, key_type a, key_type l)
{
    /*
     * Multiply-shift hashing function. Only maps to a power of two: [2^w] -> [2^l]. As such, choose
     * hashtable with size that is a power of 2, i.e. m = 2^l.
     *
     * N.B. This hash function is 2-approx. universal.
     * */
    return (a * key) >> (KEY_BIT_SIZE - l);
}

key_type slow_mersenne_4_independent_hash(key_type key,  hashing_constants constants)
{
    key_type p = std::pow(2,MERSENNE_PRIME_EXPONENT) - 1;
    return (constants.a*(key_type)std::pow(key,3)+constants.b*(key_type)std::pow(constants.b,2)+constants.c*key+constants.d)%p;
}

uint64_t mersenne_4_independent_hash(key_type key,  hashing_constants constants)
{
    key_type q = MERSENNE_PRIME_EXPONENT;
    key_type p = std::pow(2,q) - 1;
    uint64_t k;

    k = ((constants.a*key+constants.b)&p) + ((constants.a*key+constants.b)>>q);
    if(k>=p) k-=p;

    k = ((k*key+constants.c)&p) + ((constants.a*key+constants.b)>>q);
    if(k>=p) k-=p;

    k = ((k*key+constants.d)&p) + ((constants.a*key+constants.b)>>q);
    if(k>=p) k-=p;
    return k;
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

void print_flag()
{
    std::cout << "PRINTING HERE!!!" << std::endl;
}