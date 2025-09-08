#pragma once

#include <string>
#include <sstream> // For std::stringstream
#include <iostream>

namespace CHTL {

// A variant-like struct to hold the result of an expression evaluation.
struct ChtlValue {
    enum class Type {
        Null,
        Bool,
        Number,
        String
    };

    Type type = Type::Null;

    bool boolValue = false;
    double numberValue = 0.0;
    std::string stringValue;
    std::string unit; // e.g., "px", "%"

    // Helper to convert the value back to a string for CSS output
    std::string toString() const {
        switch (type) {
            case Type::Bool:
                return boolValue ? "true" : "false";
            case Type::Number: {
                // Use a stringstream to format the double without trailing zeros
                std::stringstream ss;
                ss << numberValue;
                return ss.str() + unit;
            }
            case Type::String:
                return stringValue;
            case Type::Null:
            default:
                return "";
        }
    }
};

} // namespace CHTL
