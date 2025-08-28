#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct SystemSettings {
    std::string currency;
};

SystemSettings getSystemSettings();
bool updateSystemSettings(const SystemSettings& settings);

#endif // SETTINGS_H
