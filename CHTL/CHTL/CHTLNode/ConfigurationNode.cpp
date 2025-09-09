#include "ConfigurationNode.h"
#include <sstream>

namespace CHTL {

ConfigurationNode::ConfigurationNode(ConfigurationType type, const std::string& name)
    : BaseNode(NodeType::Configuration), configType(type), name(name) {}

std::string ConfigurationNode::toString(int indent) const {
    std::string indentStr(indent * 2, ' ');
    std::stringstream ss;
    ss << indentStr << "Configuration (Type: ";
    switch (configType) {
        case ConfigurationType::Keyword: ss << "Keyword"; break;
        case ConfigurationType::OriginType: ss << "OriginType"; break;
        case ConfigurationType::Named: ss << "Named"; break;
        case ConfigurationType::Import: ss << "Import"; break;
    }
    ss << ", Name: \"" << name << "\"";
    
    if (!options.empty()) {
        ss << "\n" << indentStr << "  Options:";
        for (const auto& option : options) {
            ss << "\n" << indentStr << "    " << option.first << ": " << option.second;
        }
    }
    
    if (!keywords.empty()) {
        ss << "\n" << indentStr << "  Keywords: [";
        for (size_t i = 0; i < keywords.size(); ++i) {
            ss << keywords[i] << (i == keywords.size() - 1 ? "" : ", ");
        }
        ss << "]";
    }
    
    if (!originTypes.empty()) {
        ss << "\n" << indentStr << "  OriginTypes: [";
        for (size_t i = 0; i < originTypes.size(); ++i) {
            ss << originTypes[i] << (i == originTypes.size() - 1 ? "" : ", ");
        }
        ss << "]";
    }
    
    if (!importPath.empty()) {
        ss << "\n" << indentStr << "  ImportPath: \"" << importPath << "\"";
    }
    
    ss << ")\n";
    for (const auto& child : children) {
        ss << child->toString(indent + 1);
    }
    return ss.str();
}

} // namespace CHTL