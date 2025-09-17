#ifndef CHTL_CJMOD_ARG_H
#define CHTL_CJMOD_ARG_H

#include "AtomArg.h"
#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace CHTL {
namespace CJMOD {

class Arg {
public:
    // Adds a new atomic argument to the list.
    void addAtom(const std::string& lexeme, AtomType type) {
        atoms_.emplace_back(lexeme, type);
    }

    // Binds a function to a specific placeholder lexeme.
    void bind(const std::string& lexeme, std::function<std::string(const std::string&)> func) {
        for (auto& atom : atoms_) {
            if (atom.getLexeme() == lexeme) {
                atom.bind(func);
                return;
            }
        }
    }

    // Fills the values of this Arg object from another Arg object (presumably from the scanner).
    void fillValue(const Arg& source_arg) {
        if (atoms_.size() != source_arg.atoms_.size()) {
            // Handle error: mismatched number of arguments
            return;
        }
        for (size_t i = 0; i < atoms_.size(); ++i) {
            atoms_[i].fillValue(source_arg.atoms_[i].getValue());
        }
    }

    // Sets the transform string for the generator.
    void transform(const std::string& transform_rule) {
        transform_rule_ = transform_rule;
    }

    const std::string& getTransformRule() const { return transform_rule_; }

    // Provides access to the underlying AtomArg objects.
    const std::vector<AtomArg>& getAtoms() const { return atoms_; }
    AtomArg& operator[](size_t index) { return atoms_[index]; }
    const AtomArg& operator[](size_t index) const { return atoms_[index]; }
    size_t size() const { return atoms_.size(); }

private:
    std::vector<AtomArg> atoms_;
    std::string transform_rule_;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_ARG_H
