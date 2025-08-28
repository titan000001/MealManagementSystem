#include "database.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h> // Use modern EVP API for hashing
#include <openssl/rand.h> // For salt generation
#include <memory> // For std::unique_ptr
#include <mysql_driver.h> // Include for sql::mysql::get_driver_instance()

#include "database.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdexcept>      // For std::runtime_error
#include <openssl/evp.h> // Use modern EVP API for hashing
#include <openssl/rand.h> // For salt generation
#include <memory> // For std::unique_ptr
#include <mysql_driver.h> // Include for sql::mysql::get_driver_instance()
#include <QSettings>      // For reading config file
#include <QFileInfo>      // For checking if config file exists

std::unique_ptr<sql::Connection> getConnection() {
    const QString configFileName = "config.ini";
    QFileInfo configFile(configFileName);

    if (!configFile.exists()) {
        throw std::runtime_error("FATAL: Configuration file 'config.ini' not found. "
                                 "Please copy 'config.ini.example' to 'config.ini' and fill in your database details.");
    }

    QSettings settings(configFileName, QSettings::IniFormat);

    std::string host = settings.value("Database/host").toString().toStdString();
    std::string user = settings.value("Database/user").toString().toStdString();
    std::string password = settings.value("Database/password").toString().toStdString();
    std::string schema = settings.value("Database/database").toString().toStdString();

    if (host.empty() || user.empty() || schema.empty()) {
        throw std::runtime_error("FATAL: One or more required database settings (host, user, database) are missing from 'config.ini'.");
    }
     if (password == "your_password") {
        std::cerr << "WARNING: You are using the default password from 'config.ini.example'. Please change it." << std::endl;
    }


    try {
        sql::Driver* driver = sql::mysql::get_driver_instance();
        std::unique_ptr<sql::Connection> con(driver->connect(host, user, password));
        con->setSchema(schema);
        return con;
    } catch (sql::SQLException &e) {
        std::cerr << "Could not connect to the database. Error: " << e.what() << std::endl;
        throw; // Re-throw the exception to be handled by the caller
    }
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

// NOTE: This is a basic hashing implementation. For production systems, you should
// use a stronger, more resource-intensive key derivation function like Argon2 or scrypt.
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