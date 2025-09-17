#ifndef CHTL_CJMOD_SCANNER_H
#define CHTL_CJMOD_SCANNER_H

#include "../API/Arg.h"
#include <string>

namespace CHTL {
namespace CJMOD {

class CJMODScanner {
public:
    /**
     * @brief Scans a source string for a pattern defined by a syntax rule.
     *
     * @param syntax_arg The parsed syntax rule from Syntax::analyze.
     * @param keyword The main literal keyword to look for (e.g., "**").
     * @param source The source code string to scan.
     * @return An Arg object with its values filled from the source code.
     *         Returns an empty Arg if no match is found.
     */
    static Arg scan(const Arg& syntax_arg, const std::string& keyword, const std::string& source);
};

} // namespace CJMOD
} // namespace CHTL

#endif // CHTL_CJMOD_SCANNER_H
