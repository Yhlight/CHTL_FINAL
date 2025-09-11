#include "ConfigurationContext.h"
#include <algorithm>

ConfigurationContext::ConfigurationContext() {
    // Initialize with default values from CHTL.md
    setBool("DEBUG_MODE", false);
    setBool("DISABLE_NAME_GROUP", false);
    setBool("DISABLE_STYLE_AUTO_ADD_CLASS", false);
    setBool("DISABLE_STYLE_AUTO_ADD_ID", false);
    setBool("DISABLE_DEFAULT_NAMESPACE", false);
    setBool("DISABLE_CUSTOM_ORIGIN_TYPE", false);
    setString("INDEX_INITIAL_COUNT", "0");
    setString("OPTION_COUNT", "3");

    // Default keywords
    addKeyword("KEYWORD_TEXT", "text");
    addKeyword("KEYWORD_STYLE", "style");
    addKeyword("KEYWORD_SCRIPT", "script");
    addKeyword("KEYWORD_TEMPLATE", "[Template]");
    addKeyword("KEYWORD_CUSTOM", "[Custom]");
    addKeyword("KEYWORD_ORIGIN", "[Origin]");
    addKeyword("KEYWORD_IMPORT", "[Import]");
    addKeyword("KEYWORD_NAMESPACE", "[Namespace]");
    addKeyword("KEYWORD_CONFIGURATION", "[Configuration]");
    addKeyword("KEYWORD_DELETE", "delete");
    addKeyword("KEYWORD_INSERT", "insert");
    addKeyword("KEYWORD_FROM", "from");
    addKeyword("KEYWORD_USE", "use");
    addKeyword("KEYWORD_EXCEPT", "except");
}

void ConfigurationContext::setString(const std::string& key, const std::string& value) {
    settings[key] = value;
}

std::string ConfigurationContext::getString(const std::string& key, const std::string& defaultValue) const {
    auto it = settings.find(key);
    if (it != settings.end()) {
        return it->second;
    }
    return defaultValue;
}

void ConfigurationContext::setBool(const std::string& key, bool value) {
    settings[key] = value ? "true" : "false";
}

bool ConfigurationContext::getBool(const std::string& key, bool defaultValue) const {
    auto it = settings.find(key);
    if (it != settings.end()) {
        std::string val = it->second;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return val == "true";
    }
    return defaultValue;
}

void ConfigurationContext::addKeyword(const std::string& key, const std::string& value) {
    keyword_settings[key].push_back(value);
}

void ConfigurationContext::clearKeyword(const std::string& key) {
    keyword_settings.erase(key);
}

std::vector<std::string> ConfigurationContext::getKeyword(const std::string& key) const {
    auto it = keyword_settings.find(key);
    if (it != keyword_settings.end()) {
        return it->second;
    }
    return {};
}
