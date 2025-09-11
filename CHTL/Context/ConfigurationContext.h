#pragma once

#include <string>
#include <map>
#include <vector>

class ConfigurationContext {
public:
    ConfigurationContext();

    void setString(const std::string& key, const std::string& value);
    std::string getString(const std::string& key, const std::string& defaultValue = "") const;

    void setBool(const std::string& key, bool value);
    bool getBool(const std::string& key, bool defaultValue = false) const;

    // For keyword re-definitions which can have multiple values
    void addKeyword(const std::string& key, const std::string& value);
    void clearKeyword(const std::string& key);
    std::vector<std::string> getKeyword(const std::string& key) const;

private:
    std::map<std::string, std::string> settings;
    std::map<std::string, std::vector<std::string>> keyword_settings;
};
