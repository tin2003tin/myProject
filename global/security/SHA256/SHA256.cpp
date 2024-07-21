#include "SHA256.hpp"
#include <cstring>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace tin
{

    constexpr std::array<uint32_t, 64> SHA256::K;

    SHA256::SHA256() : blockLen(0), bitLen(0)
    {
        state[0] = 0x6a09e667;
        state[1] = 0xbb67ae85;
        state[2] = 0x3c6ef372;
        state[3] = 0xa54ff53a;
        state[4] = 0x510e527f;
        state[5] = 0x9b05688c;
        state[6] = 0x1f83d9ab;
        state[7] = 0x5be0cd19;
    }

    SHA256 &SHA256::update(const uint8_t *input, size_t length)
    {
        for (size_t i = 0; i < length; i++)
        {
            data[blockLen++] = input[i];
            if (blockLen == 64)
            {
                transform();

                bitLen += 512;
                blockLen = 0;
            }
        }
        return *this;
    }

    SHA256 &SHA256::update(const std::string &data)
    {
        update(reinterpret_cast<const uint8_t *>(data.c_str()), data.size());
        return *this;
    }

    SHA256 &SHA256::update(std::vector<uint8_t> &data)
    {
        return update(data.data(), data.size());
    }

    SHA256 &SHA256::update(std::array<uint8_t, tin::SHA256::DIGEST_LENGTH> &data)
    {
        return update(data.data(), data.size());
    }
    
    std::array<uint8_t, 32> SHA256::digest()
    {
        std::array<uint8_t, 32> hash;

        pad();
        revert(hash);

        return hash;
    }

    uint32_t SHA256::rotate(uint32_t x, uint32_t n)
    {
        return (x >> n) | (x << (32 - n));
    }

    uint32_t SHA256::choose(uint32_t e, uint32_t f, uint32_t g)
    {
        return (e & f) ^ (~e & g);
    }

    uint32_t SHA256::majority(uint32_t a, uint32_t b, uint32_t c)
    {
        return (a & (b | c)) | (b & c);
    }

    uint32_t SHA256::sig0(uint32_t x)
    {
        return SHA256::rotate(x, 7) ^ SHA256::rotate(x, 18) ^ (x >> 3);
    }

    uint32_t SHA256::sig1(uint32_t x)
    {
        return SHA256::rotate(x, 17) ^ SHA256::rotate(x, 19) ^ (x >> 10);
    }

    void SHA256::transform()
    {
        uint32_t maj, xorA, ch, xorE, sum, newA, newE, m[64];
        uint32_t state[8];

        for (uint8_t i = 0, j = 0; i < 16; i++, j += 4)
        {
            m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
        }

        for (uint8_t k = 16; k < 64; k++)
        {
            m[k] = SHA256::sig1(m[k - 2]) + m[k - 7] + SHA256::sig0(m[k - 15]) + m[k - 16];
        }

        for (uint8_t i = 0; i < 8; i++)
        {
            state[i] = this->state[i];
        }

        for (uint8_t i = 0; i < 64; i++)
        {
            maj = SHA256::majority(state[0], state[1], state[2]);
            xorA = SHA256::rotate(state[0], 2) ^ SHA256::rotate(state[0], 13) ^ SHA256::rotate(state[0], 22);

            ch = choose(state[4], state[5], state[6]);

            xorE = SHA256::rotate(state[4], 6) ^ SHA256::rotate(state[4], 11) ^ SHA256::rotate(state[4], 25);

            sum = m[i] + K[i] + state[7] + ch + xorE;
            newA = xorA + maj + sum;
            newE = state[3] + sum;

            state[7] = state[6];
            state[6] = state[5];
            state[5] = state[4];
            state[4] = newE;
            state[3] = state[2];
            state[2] = state[1];
            state[1] = state[0];
            state[0] = newA;
        }

        for (uint8_t i = 0; i < 8; i++)
        {
            this->state[i] += state[i];
        }
    }

    void SHA256::pad()
    {

        uint64_t i = blockLen;
        uint8_t end = blockLen < 56 ? 56 : 64;

        data[i++] = 0x80;
        while (i < end)
        {
            data[i++] = 0x00;
        }

        if (blockLen >= 56)
        {
            transform();
            memset(data, 0, 56);
        }

        bitLen += blockLen * 8;
        data[63] = bitLen;
        data[62] = bitLen >> 8;
        data[61] = bitLen >> 16;
        data[60] = bitLen >> 24;
        data[59] = bitLen >> 32;
        data[58] = bitLen >> 40;
        data[57] = bitLen >> 48;
        data[56] = bitLen >> 56;
        transform();
    }

    void SHA256::revert(std::array<uint8_t, 32> &hash)
    {
        for (uint8_t i = 0; i < 4; i++)
        {
            for (uint8_t j = 0; j < 8; j++)
            {
                hash[i + (j * 4)] = (state[j] >> (24 - i * 8)) & 0x000000ff;
            }
        }
    }

    std::string SHA256::toString(const std::array<uint8_t, 32> &digest)
    {
        std::stringstream s;
        s << std::setfill('0') << std::hex;

        for (uint8_t i = 0; i < 32; i++)
        {
            s << std::setw(2) << (unsigned int)digest[i];
        }

        return s.str();
    }

}