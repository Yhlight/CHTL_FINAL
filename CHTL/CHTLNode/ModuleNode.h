#pragma once

#include "Node.h"
#include <string>
#include <vector>
#include <map>

// Represents an [Export] block in a module info file.
class ExportNode : public Node {
public:
    void accept(Visitor& visitor) override;

    // The structure can be complex, e.g., `[Custom] @Style ChthollyStyle, ChthollyCard;`
    // For now, we'll store the raw tokens for the info parser to handle.
    // A map from type (e.g., "[Custom] @Style") to a list of names.
    std::map<std::string, std::vector<std::string>> exports;
};

// Represents the parsed content of a module's [Info] block.
struct ModuleInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    // ... other fields from the spec
    std::unique_ptr<ExportNode> exports;
};
