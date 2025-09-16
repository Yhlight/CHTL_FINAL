#ifndef CHTL_CJMOD_ATOM_ARG_H
#define CHTL_CJMOD_ATOM_ARG_H

#include <string>
#include <functional>
#include <stdexcept>

namespace CHTL {
namespace CJMOD {

// Defines the type of an atomic argument in a syntax rule.
enum class AtomType {
    Placeholder,      // A simple placeholder like '$'
    Literal,          // A literal keyword like '**' or '->'
    Optional,         // An optional placeholder '$?'
    Required,         // A required placeholder '$!'
    Unordered,        // An unordered placeholder '$_'
    Variadic          // A variadic placeholder '...'
};

class AtomArg {
public:
    AtomArg(const std::string& lexeme, AtomType type) : lexeme_(lexeme), type_(type) {}

    // Binds a function to be called when this argument is processed.
    void bind(std::function<std::string(const std::string&)> func) {
        transform_func_ = func;
    }

    // Fills the value of this argument.
    void fillValue(const std::string& value) {
        value_ = value;
    }

    const std::string& getLexeme() const { return lexeme_; }
    const std::string& getValue() const { return value_; }
    AtomType getType() const { return type_; }

private:
    std::string lexeme_; // The text from the syntax rule, e.g., "$", "**", or "$?"
    std::string value_;  // The actual value captured from the code, e.g., "3"
    AtomType type_;
    std::function<std::string(const std::string&)> transform_func_;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_ATOM_ARG_H
