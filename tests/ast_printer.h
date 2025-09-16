#ifndef CHTL_JS_AST_PRINTER_H
#define CHTL_JS_AST_PRINTER_H

#include "../src/CHTLJS/CHTLJSNode/CHTLJSNode.h"
#include "../src/CHTLJS/CHTLJSNode/SequenceNode.h"
#include "../src/CHTLJS/CHTLJSNode/EnhancedSelectorNode.h"
#include "../src/CHTLJS/CHTLJSNode/ListenNode.h"
#include "../src/CHTLJS/CHTLJSNode/DelegateNode.h"
#include "../src/CHTLJS/CHTLJSNode/AnimateNode.h"
#include "../src/CHTLJS/CHTLJSNode/PlaceholderNode.h"
#include "../src/CHTLJS/CHTLJSNode/ValueNode.h"
#include "../src/CHTLJS/CHTLJSNode/VirNode.h"
#include <string>
#include <iostream>

namespace CHTLJS {

class AstPrinter {
public:
    void print(const CHTLJSNode* node) {
        if (!node) return;
        visit(node);
    }

private:
    void visit(const CHTLJSNode* node) {
        switch (node->getType()) {
            case CHTLJSNodeType::Sequence: visitSequence(static_cast<const SequenceNode*>(node)); break;
            case CHTLJSNodeType::EnhancedSelector: visitEnhancedSelector(static_cast<const EnhancedSelectorNode*>(node)); break;
            case CHTLJSNodeType::Listen: visitListen(static_cast<const ListenNode*>(node)); break;
            case CHTLJSNodeType::Animate: visitAnimate(static_cast<const AnimateNode*>(node)); break;
            case CHTLJSNodeType::Placeholder: visitPlaceholder(static_cast<const PlaceholderNode*>(node)); break;
            case CHTLJSNodeType::Value: visitValue(static_cast<const ValueNode*>(node)); break;
            case CHTLJSNodeType::Vir: visitVir(static_cast<const VirNode*>(node)); break;
            default: std::cout << "Unknown Node Type" << std::endl; break;
        }
    }

    void visitSequence(const SequenceNode* node) {
        indent();
        std::cout << "Sequence:" << std::endl;
        level_++;
        for (const auto& stmt : node->statements_) {
            visit(stmt.get());
        }
        level_--;
    }

    void visitEnhancedSelector(const EnhancedSelectorNode* node) {
        indent();
        std::cout << "EnhancedSelector(" << node->getSelector() << ")" << std::endl;
    }

    void visitListen(const ListenNode* node) {
        indent();
        std::cout << "Listen:" << std::endl;
        level_++;
        indent();
        std::cout << "Object:" << std::endl;
        level_++;
        visit(node->getObject());
        level_--;
        indent();
        std::cout << "Events:" << std::endl;
        level_++;
        for (const auto& pair : node->getEvents()) {
            indent();
            std::cout << pair.first << ": " << pair.second << std::endl;
        }
        level_--;
        level_--;
    }

    void visitAnimate(const AnimateNode* node) {
        indent();
        std::cout << "Animate:" << std::endl;
        level_++;
        // Print other properties...
        level_--;
    }

    void visitPlaceholder(const PlaceholderNode* node) {
        indent();
        std::cout << "Placeholder(" << node->getPlaceholderText() << ")" << std::endl;
    }

    void visitValue(const ValueNode* node) {
        indent();
        std::cout << "Value(" << node->getValue() << ")" << std::endl;
    }

    void visitVir(const VirNode* node) {
        indent();
        std::cout << "Vir(" << node->getName() << "):" << std::endl;
        level_++;
        visit(node->getValue());
        level_--;
    }


    void indent() {
        for (int i = 0; i < level_; ++i) {
            std::cout << "  ";
        }
    }

    int level_ = 0;
};

} // namespace CHTLJS

#endif // CHTL_JS_AST_PRINTER_H
