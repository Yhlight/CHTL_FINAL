#include "CJMODGenerator.h"
#include <iostream>

namespace CHTL {
namespace CJMOD {

std::string CJMODGenerator::exportResult(const Arg& filled_arg) {
    std::string result = filled_arg.getTransformRule();
    const auto& atoms = filled_arg.getAtoms();

    // This is a simple substitution that assumes placeholders are just '$'
    // and that they appear in the same order as the atoms.
    for (const auto& atom : atoms) {
        if (atom.getType() == AtomType::Placeholder ||
            atom.getType() == AtomType::Optional ||
            atom.getType() == AtomType::Required)
        {
            size_t pos = result.find("$");
            if (pos != std::string::npos) {
                result.replace(pos, 1, atom.getValue());
            }
        }
    }

    return result;
}

} // namespace CJMOD
} // namespace CHTL
