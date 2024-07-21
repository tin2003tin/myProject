#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <random>

// Elliptic Curve Parameters (secp256k1)
const int a = 0;
const int b = 7;
const int p = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F;
const int Gx = 55066263022277343669578718895168534326250603453777594175500187360389116729240;
const int Gy = 32670510020758816978083085130507043184471273380659243275938904335757337482424;
const int n = 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141;
const int h = 1;

struct Point
{
    int x;
    int y;

    Point operator+(const Point &other) const
    {
        // Elliptic curve point addition
        if (x == other.x && y == other.y)
        {
            // Point doubling
            int m = (3 * x * x + a) * mod_inverse(2 * y, p) % p;
            int xr = (m * m - 2 * x) % p;
            int yr = (m * (x - xr) - y) % p;
            return {xr, yr};
        }
        else
        {
            // Point addition
            int m = (other.y - y) * mod_inverse(other.x - x, p) % p;
            int xr = (m * m - x - other.x) % p;
            int yr = (m * (x - xr) - y) % p;
            return {xr, yr};
        }
    }

    Point operator*(int scalar) const
    {
        Point result = {0, 0};
        Point base = *this;

        while (scalar > 0)
        {
            if (scalar % 2 == 1)
            {
                result = result + base;
            }
            base = base + base;
            scalar /= 2;
        }

        return result;
    }
};

// Modular inverse
int mod_inverse(int a, int m)
{
    int m0 = m, t, q;
    int x0 = 0, x1 = 1;

    if (m == 1)
        return 0;

    while (a > 1)
    {
        q = a / m;
        t = m;
        m = a % m, a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
    }

    if (x1 < 0)
        x1 += m0;

    return x1;
}

// Key generation
void generate_keys(int &private_key, Point &public_key)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, n - 1);

    private_key = dis(gen);
    public_key = {Gx, Gy} * private_key;
}

// Encryption
Point encrypt(const Point &public_key, const std::string &plaintext, int &k)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, n - 1);

    k = dis(gen);
    Point C1 = {Gx, Gy} * k;
    Point C2 = public_key * k + Point{plaintext[0], plaintext[1]}; // Simplified

    return {C1, C2};
}

// Decryption
std::string decrypt(const Point &private_key, const Point &ciphertext, int k)
{
    Point C1 = ciphertext.x;
    Point C2 = ciphertext.y;

    Point P = C2 - C1 * k;
    return std::string{char(P.x), char(P.y)};
}

int main()
{
    int private_key;
    Point public_key;
    generate_keys(private_key, public_key);

    std::string plaintext = "Hi";
    int k;
    Point ciphertext = encrypt(public_key, plaintext, k);

    std::string decrypted_text = decrypt(private_key, ciphertext, k);
    std::cout << "Decrypted text: " << decrypted_text << std::endl;

    return 0;
}
