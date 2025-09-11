#include "Generator.h"

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
    for (const auto& handler : node->eventHandlers) {
        handler->accept(*this);
    }
    output << "}";
}

void CHTLJSGenerator::visit(AnimateNode* node) {
    output << "{\n";
    for (const auto& prop : node->properties) {
        prop->accept(*this);
    }
    output << "}";
}

void CHTLJSGenerator::visit(VirDeclNode* node) {
    output << "const " << node->name << " = ";
    node->value->accept(*this);
    output << ";\n";
}

void CHTLJSGenerator::visit(EnhancedSelectorNode* node) {
    // This needs to be translated to `document.querySelector` or similar.
    // For now, just output a placeholder.
    output << "`{{ " << node->selector << " }}`";
}

void CHTLJSGenerator::visit(CHTLJSPropertyNode* node) {
    output << "  " << node->key << ": " << node->value << ",\n";
}
