#ifndef CHTL_JS_PROCESSOR_H
#define CHTL_JS_PROCESSOR_H

#include <string>

namespace CHTL {

class CHTLJSProcessor {
public:
    explicit CHTLJSProcessor(const std::string& source);
    std::string process();

private:
    std::string transformSelector(const std::string& selector);

    const std::string& source;
    std::string output_buffer;
    size_t current_pos = 0;
};

} // namespace CHTL

#endif // CHTL_JS_PROCESSOR_H
