#ifndef CHTL_GENERATOR_CONTEXT_H
#define CHTL_GENERATOR_CONTEXT_H

#include <string>
#include <vector>
#include <sstream>

// A simple context object to pass around during generation.
// It's used to collect global CSS rules.
struct GeneratorContext {
    std::stringstream globalCss;
};

#endif // CHTL_GENERATOR_CONTEXT_H
