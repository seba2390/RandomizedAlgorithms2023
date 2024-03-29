//
// Created by Sebastian Yde Madsen on 02/03/2023.
//

#include "lib/Utilities.hpp"

uint32_t get_random_uint32(const uint32_t& seed, const uint32_t& upper_bound) {
    // create a random number generator and seed it.
    XoshiroCpp::Xoshiro128PlusPlus generator(seed);

    // create a uniform distribution that generates values in the range [0, 2^bit-size(keytype) - 1]
    std::uniform_int_distribution<uint32_t> distribution(0, upper_bound); // upper bound = std::pow(2,KEY_BIT_SIZE) - 1 for multiply shift

    // generate and return a random bit-size(keytype)-bit integer
    return static_cast<uint32_t>(distribution(generator));
}

uint64_t get_random_uint64(const uint64_t& seed, const uint64_t& upper_bound) {
    // create a random number generator and seed it.
    XoshiroCpp::Xoshiro256PlusPlus generator(seed);

    // create a uniform distribution that generates values in the range [0, 2^bit-size(keytype) - 1]
    std::uniform_int_distribution<uint64_t> distribution(0, upper_bound); // upper bound = std::pow(2,KEY_BIT_SIZE) - 1 for multiply shift

    // generate and return a random bit-size(keytype)-bit integer
    return distribution(generator);
}

uint32_t get_random_odd_uint32(const uint32_t& seed, const uint32_t& upper_bound)
{
    unsigned int counter = 1;

    uint32_t a =  get_random_uint32(seed, upper_bound);

    while ( a % 2 == 0) {

        a = get_random_uint32(seed + counter, upper_bound);
        counter++;
    }
    return a;
}

uint32_t fast_uint32_pow_2(const uint32_t& power)
{
    /*
     * Calculates 2^q for some positive 32-bit integer in a fast manner.
     * N.B. only works for power < 32.
     * */
    return UINT32_C(1) << power;
}

uint64_t fast_uint64_pow_2(const uint64_t& power)
{
    /*
     * Calculates 2^q for some positive 64-bit integer in a fast manner.
     * N.B. only works for power < 64.
     * */
    return 1ULL << power;
}

uint32_t fast_uint32_log_2(uint32_t x)
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

    return static_cast<uint32_t>(sizeof(uint32_t) * BITS_PR_BYTE - __builtin_clz(x) - 1);
}

/**
 * Computes the relative error between two unsigned 64-bit integers `a` and `b`
 * as |a-b|/b, using a fast implementation that avoids signed conversions.
 *
 * @param a The first unsigned 64-bit integer.
 * @param b The second unsigned 64-bit integer.
 * @throws std::runtime_error if b is equal to 0.
 * @return The relative error between `a` and `b`, as a double.
 */
double fast_relative_err(uint64_t a, uint64_t b)
{
    if(b==0) throw std::runtime_error("Trying to compute |a-b|/b for b=0 is undefined behaviour.");
    uint64_t diff = (a > b) ? (a - b) : (b - a);
    uint64_t mask = (diff >> ((sizeof(uint64_t)*BITS_PR_BYTE)-1)); // 0xFFFFFFFFFFFFFFFF if diff is negative, 0x0000000000000000 otherwise
    uint64_t abs_diff = (diff ^ mask) - mask; // abs(a - b)
    return static_cast<double>(abs_diff) / static_cast<double>(b);
}

double slow_relative_err(uint64_t a, uint64_t b)
{
    return static_cast<double>(std::abs(static_cast<int64_t>(a - b))) / static_cast<double>(b);
}

uint64_t fast_uint64_log_2(uint64_t x)
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

    return static_cast<uint64_t>(sizeof(uint64_t) * BITS_PR_BYTE - __builtin_clzll(x) - 1);
}

uint32_t multiply_shift_hash(uint32_t key, uint32_t a, uint32_t l)
{
    /*
     * Multiply-shift hashing function. Only maps to a power of two: [2^w] -> [2^l]. As such, choose
     * hashtable with size that is a power of 2, i.e. m = 2^l.
     *
     * N.B. This hash function is 2-approx. universal.
     * */
    return (a * key) >> (KEY_BIT_SIZE - l);
}

std::pair<int64_t,int64_t> multiply_shift_2_independent(int64_t key, uint64_t array_size, hashing_constants constants)
{
    uint64_t k;

    k = (static_cast<uint64_t>(constants.a)*static_cast<uint64_t>(key)+static_cast<uint64_t>(constants.b)) >> static_cast<uint64_t>(33);
    //TODO: Find out why three different k's are needed for correct result
    /*
     * (k & 1): returns the Least Significant Bit (LSB) of k.
     * LSB is the rightmost bit of the bitstring - this is always {0,1}.
     * As such, 2 * (k & 1) - 1 always returns a number in {-1,+1}
     */
    int64_t g = 2*(static_cast<int64_t>(k) & 1)-1;


    /*
     * (k >> 1): will shift the bits of k2 one position to the right.
     * This is equivalent to floor(k / 2). (dividing w. 2, discarding remainder and rounding down).
     * Or equivalent to dropping the LSB.
     *
     * (r-1): This operation produces a mask of bits that has all the bits set to 1 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     *
     * (k >> 1) & (r-1): produces a new value that has all the bits of (k >> 1) set to 0 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     * Any higher-order bits of (k >> 1) that are above the bit corresponding to r are unaffected by the mask.
     *
     * Effectively (k >> 1) & (r-1) corresponds to first bit shifting k by 1, and then performing mod r, when r = 2^R (r is a power of 2).
     */
    uint64_t h = (k >> 1) & (array_size-1);
    if(h >= array_size) {
        std::cout<< "array size, h :" << array_size << "," << h << std::endl;
        throw std::runtime_error("Error in multiply_shift_2_independent - h too large.");
    }
    if(!((g==-1)||(g==1)))
    {
        std::cout<< "array size, g :" << array_size << "," << g << std::endl;
        throw std::runtime_error("Error in multiply_shift_2_independent");
    }
    //std::cout << "a:" << constants.a << " b:" << constants.b << std::endl;
    //std::cout << "h=" << h << ",key=" << key << std::endl << std::endl;
    return std::make_pair(g,h);

}


std::pair<int64_t,int64_t> multiply_shift_2_independent_2(int64_t key, uint64_t array_size, hashing_constants constants)
{
    uint64_t k;
    uint64_t p = 2147483647;
    uint64_t q = 31;

    //k = (static_cast<int64_t>(constants.a)*static_cast<int64_t>(key)+static_cast<int64_t>(constants.b)) >> static_cast<int64_t>(33);
    k = ((constants.a*key+constants.b)&p) + ((constants.a*key+constants.b)>>q);
    if (k >= p) k-=p;
    //TODO: Find out why three different k's are needed for correct result
    /*
     * (k & 1): returns the Least Significant Bit (LSB) of k.
     * LSB is the rightmost bit of the bitstring - this is always {0,1}.
     * As such, 2 * (k & 1) - 1 always returns a number in {-1,+1}
     */
    int64_t g = 2*(static_cast<int64_t>(k) & 1)-1;


    /*
     * (k >> 1): will shift the bits of k2 one position to the right.
     * This is equivalent to floor(k / 2). (dividing w. 2, discarding remainder and rounding down).
     * Or equivalent to dropping the LSB.
     *
     * (r-1): This operation produces a mask of bits that has all the bits set to 1 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     *
     * (k >> 1) & (r-1): produces a new value that has all the bits of (k >> 1) set to 0 from the LSB
     * up to the bit that corresponds to the highest power of 2 less than or equal to r.
     * Any higher-order bits of (k >> 1) that are above the bit corresponding to r are unaffected by the mask.
     *
     * Effectively (k >> 1) & (r-1) corresponds to first bit shifting k by 1, and then performing mod r, when r = 2^R (r is a power of 2).
     */
    uint64_t h = (k >> 1) & (array_size-1);
    if(h >= array_size) {
        std::cout<< "array size, h :" << array_size << "," << h << std::endl;
        throw std::runtime_error("Error in multiply_shift_2_independent - h too large.");
    }
    if(!((g==-1)||(g==1)))
    {
        std::cout<< "array size, g :" << array_size << "," << g << std::endl;
        throw std::runtime_error("Error in multiply_shift_2_independent");
    }
    //std::cout << "a:" << constants.a << " b:" << constants.b << std::endl;
    //std::cout << "h=" << h << ",key=" << key << std::endl << std::endl;
    return std::make_pair(g,h);

}

/**
 * Computes the Mersenne 4-Independent Hash function on the given key, array size and hashing constants.
 * @param key the key to be hashed, a 64-bit signed integer
 * @param array_size the size of the hash table array, a 64-bit unsigned integer
 * @param constants a struct containing the four hashing constants a, b, c, and d, used for the hash function
 * @return a pair of two 64-bit signed integers, representing the resulting hash values g and h, respectively
 * */
std::pair<int64_t,int64_t> mersenne_4_independent_hash(int64_t key, uint64_t array_size, hashing_constants constants)
{
    uint64_t q = MERSENNE_PRIME_EXPONENT;
    uint64_t p = MERSENNE_PRIME;

    uint64_t k, k1, k2;

    k = ((constants.a*key+constants.b)&p) + ((constants.a*key+constants.b)>>q);
    if (k >= p) k-=p;

    k1 = ((k*key+constants.c)&p) + ((k*key+constants.c)>>q);
    if (k1 >= p) k1-=p;

    k2 = ((k1*key+constants.d)&p) + ((k1*key+constants.d)>>q);
    if (k2 >= p) k2-=p;

    //TODO: Find out why three different k's are needed for correct result
    /*
     * (k2 & 1): returns the Least Significant Bit (LSB) of k2.
     * LSB is the rightmost bit of the bitstring - this is always {0,1}.
     * As such, 2 * (k2 & 1) - 1 always returns a number in {-1,+1}
     */
    uint64_t g = 2*(k2 & 1)-1;


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
    uint64_t h = (k2 >> 1) & (array_size-1);

    return std::make_pair(g,h);

}

std::pair<int64_t,int64_t> slow_mersenne_4_independent_hash(int64_t key, uint64_t array_size, hashing_constants constants)
{
    uint64_t r = array_size;
    uint64_t x = key;
    uint64_t p = MERSENNE_PRIME;

    uint64_t k, k1, k2;

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
    uint64_t g = 2*(k2 & 1)-1;


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
    uint64_t h = k2 >> 1;
    uint64_t h2 = h % r;
    //int64_t h = (k2 >> 1) & (r-1);

    return std::make_pair(g,h2);
    }

/**
 * Calculates the variance of a vector of integer values.
 *
 * This function calculates the sample variance of the given vector of integer values.
 * The sample variance is an unbiased estimator of the population variance, which measures
 * the amount of variation or spread in the data.
 *
 * @param values A vector of integer values.
 * @throws std::runtime_error If the given vector is empty.
 * @return The sample variance of the vector.
 */
double variance(const std::vector<int64_t>& values)
{
    // Check if the vector is empty
    if (values.empty())
    {
        throw std::runtime_error("Given vector of values is empty.");
    }

    // Calculate the average of the values
    double avg = static_cast<double>(std::accumulate(values.begin(), values.end(), static_cast<int64_t>(0))) / static_cast<double>(values.size());

    // Calculate the sum of squares of deviations from the mean
    double result = 0;
    for (const int64_t& value : values)
    {
        result += (static_cast<double>(value) - avg) * (static_cast<double>(value) - avg);
    }

    // Calculate the sample variance and return it
    return result / static_cast<double>(values.size() - 1);
}


void append_to_file(const std::string& filename, const std::string& path, const std::vector<output_data_type>& data)
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

