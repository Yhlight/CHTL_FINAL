#pragma once

#include "Node.h"
#include <string>
#include <vector>

class ImportNode : public Node {
public:
    // A simple representation for now.
    // A more robust implementation would have a structured type.
    ImportNode(const std::string& import_type, const std::string& path, const std::string& alias)
        : import_type(import_type), path(path), alias(alias) {}

    void accept(Visitor& visitor) override { visitor.visit(this); }

    std::string import_type;
    std::string path;
    std::string alias;
};
