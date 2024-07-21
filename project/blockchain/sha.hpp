#ifndef BLOCKCHAIN_SHA256
#define BLOCKCHAIN_SHA256

#include "../../global/security/SHA256/SHA256.hpp"
#include <vector>
#include <array>

typedef std::array<uint8_t, tin::SHA256::DIGEST_LENGTH> SHAHash;

std::string sha256(const std::string &data)
{
    tin::SHA256 sha;
    sha.update(data);
    auto digest = sha.digest();
    return tin::SHA256::toString(digest);
}

SHAHash sha256(std::vector<uint8_t> &data)
{
    tin::SHA256 sha;
    sha.update(data);
    return sha.digest();
}

SHAHash double_sha256(std::vector<uint8_t> &data)
{
    tin::SHA256 sha;
    sha.update(data);
    auto first_digest = sha.digest();
    sha.update(first_digest);
    return sha.digest();
}

#endif