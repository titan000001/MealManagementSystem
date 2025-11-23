#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "database.h"

// Simple test runner
void test_generateSalt() {
    std::string salt1 = generateSalt();
    std::string salt2 = generateSalt();

    assert(!salt1.empty());
    assert(salt1.length() == 64); // 32 bytes * 2 hex chars
    assert(salt1 != salt2);
    std::cout << "test_generateSalt passed" << std::endl;
}

void test_hashPassword() {
    std::string password = "mysecretpassword";
    std::string salt = generateSalt();

    std::string hash1 = hashPassword(password, salt);
    std::string hash2 = hashPassword(password, salt);

    assert(!hash1.empty());
    assert(hash1.length() == 64); // SHA256 produces 32 bytes * 2 hex chars
    assert(hash1 == hash2); // Deterministic

    std::string salt2 = generateSalt();
    std::string hash3 = hashPassword(password, salt2);
    assert(hash1 != hash3); // Different salt -> different hash

    std::cout << "test_hashPassword passed" << std::endl;
}

int main() {
    try {
        test_generateSalt();
        test_hashPassword();
        std::cout << "All crypto tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
