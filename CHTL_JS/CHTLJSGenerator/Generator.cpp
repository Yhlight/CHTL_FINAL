#include "Generator.h"

// Helper to trim leading/trailing whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string CHTLJSGenerator::generate(const CHTLJSNodeList& ast) {
    for (const auto& node : ast) {
        node->accept(*this);
    }
    return output.str();
}

void CHTLJSGenerator::visit(FileLoaderNode* node) {
    output << "// CHTL JS fileloader statement\n";
    for (const auto& file : node->files) {
        output << "// load: " << file << "\n";
    }
    output << "\n";
}

void CHTLJSGenerator::visit(ListenNode* node) {
    output << "{\n";
    for (size_t i = 0; i < node->eventHandlers.size(); ++i) {
        node->eventHandlers[i]->accept(*this);
        if (i < node->eventHandlers.size() - 1) {
            output << ",";
        }
        output << "\n";
    }
    output << "}";
}

void CHTLJSGenerator::visit(AnimateNode* node) {
    std::string target;
    std::vector<std::string> options_list;
    std::vector<std::string> keyframes_list;

    for (const auto& prop : node->properties) {
        std::string value = trim(prop->value);
        if (prop->key == "target") {
            target = value;
        } else if (prop->key == "duration" || prop->key == "delay" || prop->key == "loop") {
            options_list.push_back(prop->key + ": " + value);
        } else if (prop->key == "easing" || prop->key == "direction") {
            options_list.push_back(prop->key + ": '" + value + "'");
        } else if (prop->key == "begin") {
            value.erase(0, value.find_first_not_of(" \t\n\r{"));
            value.erase(value.find_last_not_of(" \t\n\r}") + 1);
            keyframes_list.push_back("{ \"offset\": 0, " + value + " }");
        } else if (prop->key == "end") {
            value.erase(0, value.find_first_not_of(" \t\n\r{"));
            value.erase(value.find_last_not_of(" \t\n\r}") + 1);
            keyframes_list.push_back("{ \"offset\": 1, " + value + " }");
        }
    }

    // Build options string
    std::string options_str = "{ ";
    for(size_t i = 0; i < options_list.size(); ++i) {
        options_str += options_list[i] + (i < options_list.size() - 1 ? ", " : "");
    }
    options_str += " }";

    // Build keyframes string
    std::string keyframes_str = "[ ";
    for(size_t i = 0; i < keyframes_list.size(); ++i) {
        keyframes_str += keyframes_list[i] + (i < keyframes_list.size() - 1 ? ", " : "");
    }
    keyframes_str += " ]";


    // Replace the {{...}} syntax with a document.querySelector call
    if (target.find("{{") != std::string::npos) {
        size_t start = target.find("{{");
        size_t end = target.find("}}");
        std::string selector = target.substr(start + 2, end - start - 2);
        selector = trim(selector);
        target = "document.querySelector('" + selector + "')";
    }

    output << target << ".animate(" << keyframes_str << ", " << options_str << ")";
}

void CHTLJSGenerator::visit(VirDeclNode* node) {
    output << "const " << node->name << " = ";
    node->value->accept(*this);
    output << ";\n";
}

void CHTLJSGenerator::visit(EnhancedSelectorNode* node) {
    output << "`{{ " << node->selector << " }}`";
}

void CHTLJSGenerator::visit(CHTLJSPropertyNode* node) {
    output << "  " << node->key << ": " << trim(node->value);
}
