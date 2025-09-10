#pragma once

#include "BaseNode.h"
#include <string>

// This is a simplified representation for now.
// A full implementation would need to handle the different import types more robustly.
struct ImportInfo {
    std::string import_type; // e.g., "Chtl", "Element", "Style"
    std::string item_name;   // e.g., "MyModule", "Box"
    std::string path;
    std::string alias;
};

class ImportNode : public BaseNode {
public:
    ImportInfo info;

    ImportNode(const ImportInfo& info) : info(info) {}
};
