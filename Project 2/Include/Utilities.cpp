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

uint32_t fast_uint32_pow_2(const uint32_t& power)
{
    /*
     * Calculates 2^q for some positive 32-bit integer in a fast manner.
     * */
    return UINT32_C(1) << power;
}

uint64_t fast_uint64_pow_2(const uint64_t& power)
{
    /*
     * Calculates 2^q for some positive 64-bit integer in a fast manner.
     * */
    return 1ULL << power;
}

int32_t fast_uint32_log_2(uint32_t x)
{
    /*
     * Calculates log2(r) assuming r=2^R is some positive power of 2
     * represented by an unsigned 32-bit integer.
     * (N.B.: returns -1 for x = 0.)
     *
     * The __builtin_clz function counts the number of leading zeros in
     * the binary representation of x, so subtracting this value from
     * the number of bits in x gives the position of the most significant bit, which is the base-2 logarithm of x.
     * */

    return sizeof(uint32_t) * BITS_PR_BYTE - __builtin_clz(x) - 1;
}

int64_t fast_uint64_log_2(uint64_t x)
{
    /*
     * Calculates log2(r) assuming r=2^R is some positive power of 2
     * represented by an unsigned 64-bit integer.
     * (N.B.: returns -1 for x = 0.)
     *
     * The __builtin_clz function counts the number of leading zeros in
     * the binary representation of x, so subtracting this value from
     * the number of bits in x gives the position of the most significant bit, which is the base-2 logarithm of x.
     * */

    return sizeof(uint64_t) * BITS_PR_BYTE - __builtin_clz(x) - 1;
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

std::pair<int64_t,int64_t> mersenne_4_independent_hash(key_type key, key_type array_size, hashing_constants constants)
{
    int64_t r = array_size;
    uint64_t x = key;
    uint64_t q = MERSENNE_PRIME_EXPONENT;
    uint64_t p = MERSENNE_PRIME;

    int64_t k, k1, k2;

    k = ((constants.a*x+constants.b)&p) + ((constants.a*x+constants.b)>>q);
    if (k >= p) k-=p;

    k1 = ((k*x+constants.c)&p) + ((k*x+constants.c)>>q);
    if (k1 >= p) k1-=p;

    k2 = ((k1*x+constants.d)&p) + ((k1*x+constants.d)>>q);
    if (k2 >= p) k2-=p;

    //TODO: Find out why three different k's are needed for correct result
    /*
     * (k2 & 1): returns the Least Significant Bit (LSB) of k2.
     * LSB is the rightmost bit of the bitstring - this is always {0,1}.
     * As such, 2 * (k2 & 1) - 1 always returns a number in {-1,+1}
     */
    int64_t g = 2*(k2 & 1)-1;


    /*
     * (k2 >> 1): will shift the bits of k2 one position to the right.
     * This is equivalent to floor(k2 / 2). (dividing w. 2, discarding remainder and rounding down).
     * Or equivalent to dropping the LSB.
     *
     * (r-1): This operation produces a mask of bits that has all the bits set to 1 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     *
     * (k2 >> 1) & (r-1): produces a new value that has all the bits of (k2 >> 1) set to 0 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     * Any higher-order bits of (k2 >> 1) that are above the bit corresponding to r are unaffected by the mask.
     *
     * Effectively (k2 >> 1) & (r-1) corresponds to k2 mod r, when r = 2^R (r is a power of 2).
     */
    int64_t h = (k2 >> 1) & (r-1);

    return std::make_pair(g,h);

}

std::pair<int64_t,int64_t> slow_mersenne_4_independent_hash(key_type key, key_type array_size, hashing_constants constants)
{
    int64_t r = array_size;
    uint64_t x = key;
    uint64_t q = MERSENNE_PRIME_EXPONENT;
    uint64_t p = MERSENNE_PRIME;

    int64_t k, k1, k2;

    k = (constants.a*x+constants.b) % p;
    if (k >= p) k-=p;

    k1 = (k*x+constants.c) % p;
    if (k1 >= p) k1-=p;

    k2 = (k1*x+constants.d) % p;
    if (k2 >= p) k2-=p;

    /*
     * (k2 & 1): returns the Least Significant Bit (LSB) of k2.
     * LSB is the rightmost bit of the bitstring - this is always {0,1}.
     * As such, 2 * (k2 & 1) - 1 always returns a number in {-1,+1}
     */
    int64_t g = 2*(k2 & 1)-1;


    /*
     * (k2 >> 1): will shift the bits of k2 one position to the right.
     * This is equivalent to floor(k2 / 2). (dividing w. 2, discarding remainder and rounding down).
     * Or equivalent to dropping the LSB.
     *
     * (r-1): This operation produces a mask of bits that has all the bits set to 1 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     *
     * (k2 >> 1) & (r-1): produces a new value that has all the bits of (k2 >> 1) set to 0 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     * Any higher-order bits of (k2 >> 1) that are above the bit corresponding to r are unaffected by the mask.
     *
     * Effectively (k2 >> 1) & (r-1) corresponds to first bit shifting k2 by 1, and then performing mod r, when r = 2^R (r is a power of 2).
     */
    int64_t h = k2 >> 1;
    int64_t h2 = h % r;
    //int64_t h = (k2 >> 1) & (r-1);

    return std::make_pair(g,h2);
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

template <typename T>
void print(const T &value) {
    // First checking for normal printable single-item types
    if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::string> || std::is_same_v<T, const char*>)
    {
        std::cout << value << std::endl;
    }
    else
    {
        // Checking if T is array type
        if constexpr (std::is_array_v<T>) {
            std::cout << '[';
            for (const auto& element : value) {
                std::cout << element << ", ";
            }
            std::cout << "\b\b]" << std::endl;
        }

        // Checking if T is vector type
        else if constexpr (std::is_same_v<T, std::vector<typename T::value_type, typename T::allocator_type>>) {
            std::cout << '[';
            for (const auto& element : value) {
                std::cout << element << ", ";
            }
            std::cout << "\b\b]" << std::endl;
        }

        // Checking if T is list type
        else if constexpr (std::is_same_v<T, std::list<typename T::value_type, typename T::allocator_type>>) {
            std::cout << '[';
            for (const auto &element: value) {
                std::cout << element << ", ";
            }
            std::cout << "\b\b]" << std::endl;
        }
            // Checking if T can be converted to std::string
        else if constexpr (std::is_convertible_v<T, std::string>) {
            std::cout << value << std::endl;
        }
    }
}

template void print<int>(const int&);
template void print<double>(const double&);
template void print<std::string>(const std::string&);
template void print<const char*>(const char* const&);
template void print<bool>(const bool&);

template void print<std::vector<int>>(const std::vector<int>&);
template void print<std::vector<double>>(const std::vector<double>&);
template void print<std::vector<std::string>>(const std::vector<std::string>&);
template void print<std::vector<const char*>>(const std::vector<const char*>&);
template void print<std::vector<bool>>(const std::vector<bool>&);

template void print<std::list<int>>(const std::list<int>&);
template void print<std::list<double>>(const std::list<double>&);
template void print<std::list<std::string>>(const std::list<std::string>&);
template void print<std::list<const char*>>(const std::list<const char*>&);
template void print<std::list<bool>>(const std::list<bool>&);