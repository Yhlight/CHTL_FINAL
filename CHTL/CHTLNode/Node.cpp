#include "Node.h"

// Returns a string representation of the entire program, for debugging.
std::string Program::toString() const {
    std::string result;
    for (const auto& stmt : statements) {
        if (stmt) {
            result += stmt->toString();
        }
    }
    return result;
}
