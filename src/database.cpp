#include "database.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h> // Use modern EVP API for hashing
#include <openssl/rand.h> // For salt generation
#include <memory> // For std::unique_ptr
#include <mysql_driver.h> // Include for sql::mysql::get_driver_instance()

sql::Connection* getConnection() {
    // Use a static unique_ptr to manage the connection's lifetime.
    // This avoids global variables and ensures the connection is cleaned up on exit.
    static std::unique_ptr<sql::Connection> con = nullptr;
    try {
        if (!con || con->isClosed()) {
            sql::Driver* driver = sql::mysql::get_driver_instance();
            // IMPORTANT: Replace these hardcoded values with your actual credentials.
            // For better security, use a configuration file or environment variables instead of hardcoding.
            con.reset(driver->connect("tcp://127.0.0.1:3306", "meal_user", "newpassword"));
            con->setSchema("meal_management");
            std::cout << "Successfully connected/reconnected to the database." << std::endl;
        }
    } catch (sql::SQLException &e) {
        std::cerr << "Could not connect to the database. Error: " << e.what() << std::endl;
        // For this console app, exiting is a simple way to handle a fatal error.
        exit(1);
    }
    return con.get();
}

std::string generateSalt() {
    unsigned char salt_bytes[32]; // 256 bits
    if (RAND_bytes(salt_bytes, sizeof(salt_bytes)) != 1) {
        // This is a critical error, OpenSSL's random number generator failed.
        // In a real-world app, this should be logged and handled more gracefully.
        throw std::runtime_error("Failed to generate secure random salt.");
    }
    std::stringstream ss;
    for (size_t i = 0; i < sizeof(salt_bytes); ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)salt_bytes[i];
    }
    return ss.str();
}

std::string hashPassword(const std::string& password, const std::string& salt) {
    // Modern OpenSSL 3.0+ approach using the EVP API
    EVP_MD_CTX* mdctx;
    const EVP_MD* md;
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    md = EVP_get_digestbyname("SHA256");
    if (md == NULL) {
        std::cerr << "Error: SHA256 digest not found." << std::endl;
        return "";
    }

    mdctx = EVP_MD_CTX_new();
    std::string to_hash = password + salt;
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, to_hash.c_str(), to_hash.size());
    EVP_DigestFinal_ex(mdctx, hash, &md_len);
    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < md_len; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}