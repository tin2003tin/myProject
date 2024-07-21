#ifndef SHA256_HPP
#define SHA256_HPP

#include <string>
#include <array>
#include <cstdint>

namespace tin
{
    class SHA256
    {
    public:
        static constexpr size_t DIGEST_LENGTH = 32;
        static constexpr std::array<uint32_t, 64> K = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
            0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
            0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
            0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
            0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
            0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
            0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
            0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
            0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

    private:
        uint8_t data[64];
        uint32_t blockLen;
        uint64_t bitLen;
        uint32_t state[8];
        static uint32_t rotate(uint32_t x, uint32_t n);
        static uint32_t choose(uint32_t e, uint32_t f, uint32_t g);
        static uint32_t majority(uint32_t a, uint32_t b, uint32_t c);
        static uint32_t sig0(uint32_t x);
        static uint32_t sig1(uint32_t x);
        void transform();
        void pad();
        void revert(std::array<uint8_t, DIGEST_LENGTH> &hash);

    public:
        SHA256();
        SHA256 &update(const uint8_t *input, size_t length);
        SHA256 &update(const std::string &data);
        SHA256 &update(std::vector<uint8_t> &data);
        SHA256 &update(std::array<uint8_t, tin::SHA256::DIGEST_LENGTH> &data);
        std::array<uint8_t, DIGEST_LENGTH> digest();
        static std::string toString(const std::array<uint8_t, DIGEST_LENGTH> &digest);
    };

}

#include "SHA256.cpp"

#endif // SHA256_HPP
