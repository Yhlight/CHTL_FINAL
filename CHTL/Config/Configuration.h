#ifndef CHTL_CONFIGURATION_H
#define CHTL_CONFIGURATION_H

#include <string>
#include <vector>
#include <map>
#include <variant>

namespace CHTL {

class Configuration {
public:
    Configuration(); // Constructor to set defaults

    // General Settings
    int index_initial_count;
    bool debug_mode;

    // Feature Toggles
    bool disable_name_group;
    bool disable_style_auto_add_class;
    bool disable_style_auto_add_id;
    bool disable_default_namespace;
    bool disable_custom_origin_type;

    // Keyword Mappings from [Name] block
    // Using a simple map for now. The value is a vector to support group options.
    std::map<std::string, std::vector<std::string>> keyword_map;

    // Custom Origin Types from [OriginType] block
    std::map<std::string, std::string> origin_type_map;

private:
    void setDefaultKeywords();
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_H
