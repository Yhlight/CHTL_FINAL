#ifndef CHTL_COMPILER_H
#define CHTL_COMPILER_H

#include "CHTLLoader/CHTLLoader.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLContext.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>

namespace CHTL {

class CHTLCompiler {
public:
    CHTLCompiler();

    // Compiles the CHTL project starting from an entry file.
    // Returns the compilation context containing all parsed information.
    std::unique_ptr<CHTLContext> compile(const std::string& entry_path);

private:
    CHTLLoader loader;
    std::set<std::string> parsed_files;
};

} // namespace CHTL

#endif // CHTL_COMPILER_H
