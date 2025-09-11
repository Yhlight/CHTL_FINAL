#include "ModuleInfoParser.h"

ModuleInfo ModuleInfoParser::parse(NodeList& ast) {
    module_info = std::make_unique<ModuleInfo>();
    for (const auto& node : ast) {
        // The info file should only contain [Info] and [Export] blocks.
        // We can add validation for this later.
        node->accept(*this);
    }
    return std::move(*module_info);
}

void ModuleInfoParser::visit(ConfigurationNode* node) {
    // We are co-opting the ConfigurationNode for the [Info] block.
    // A more robust solution might create a dedicated [Info]Node.
    for (const auto& setting : node->settings) {
        std::string key = setting->name;
        // The value is a LiteralExprNode, get its string value.
        std::string value;
        if (auto* literal = dynamic_cast<LiteralExprNode*>(setting->value.get())) {
            value = literal->value.value;
        }

        if (key == "name") module_info->name = value;
        else if (key == "version") module_info->version = value;
        else if (key == "description") module_info->description = value;
        else if (key == "author") module_info->author = value;
        // ... and so on for other info fields.
    }
}

void ModuleInfoParser::visit(ExportNode* node) {
    // The parser has already populated the exports map.
    // We just need to move it into our ModuleInfo struct.
    if (!module_info->exports) {
        module_info->exports = std::make_unique<ExportNode>();
    }
    module_info->exports->exports = std::move(node->exports);
}
