#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstdint>
#include <bitset>
#include <limits.h>

std::bitset<64> XOR(uint64_t value1, uint64_t value2)
{
    uint64_t mantissa1 = value1 & ((1ULL << 64) - 1);
    uint64_t mantissa2 = value2 & ((1ULL << 64) - 1);

    return std::bitset<64>(mantissa1 ^ mantissa2);
}

void printff(double value1, double value2)
{
    uint64_t br1 = reinterpret_cast<uint64_t>(&value1);
    uint64_t br2 = reinterpret_cast<uint64_t>(&value2);
    std::bitset<64> xor_res = XOR(br1, br2);
    std::cout << "Value 1 : " << std::bitset<64>(br1 & ((1ULL << 64) - 1)) << std::endl;
    std::cout << "Value 2 : " << std::bitset<64>(br2 & ((1ULL << 64) - 1)) << std::endl;
    std::cout << "XOR Res : " << xor_res << std::endl;


    std::cout << "Inf     : " << (std::bitset<64>(std::numeric_limits<double>::infinity())/* & std::bitset<64>(br1)*/) << std::endl;
}

int main() 
{
    double value1 = 3.12523812489728;
    double value2 = 3.1252381248972;

    printff(value1, value2);

    return 0;
}