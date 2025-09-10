#pragma once

#include "BaseNode.h"
#include "ExprNode.h"
#include <string>
#include <vector>
#include <utility>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    // Attributes can be simple key-value or key-expression
    std::vector<std::pair<std::string, std::string>> simple_attributes;
    std::vector<std::pair<std::string, std::unique_ptr<ExprNode>>> style_properties;
    NodeList children;

    ElementNode(const std::string& tagName) : tagName(tagName) {}
};
