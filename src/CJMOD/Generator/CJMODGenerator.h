#ifndef CHTL_CJMOD_GENERATOR_H
#define CHTL_CJMOD_GENERATOR_H

#include "../API/Arg.h"
#include <string>

namespace CHTL {
namespace CJMOD {

class CJMODGenerator {
public:
    /**
     * @brief Exports the final generated code from a filled Arg object.
     *
     * Uses the transform rule set on the Arg object to construct the output string.
     *
     * @param filled_arg The Arg object after its values have been filled by the scanner.
     * @return The final generated JavaScript string.
     */
    static std::string exportResult(const Arg& filled_arg);
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_GENERATOR_H
