#ifndef CHTL_JS_PROCESSOR_H
#define CHTL_JS_PROCESSOR_H

#include <string>
#include <vector>

namespace CHTL {

class CHTLJSProcessor {
public:
    explicit CHTLJSProcessor(const std::string& source);
    std::string process();

private:
    // Main parsing loop
    void parseStatement();
    void parseIdentifier();

    // CHTL JS feature parsers
    void parseEnhancedSelector();
    void parseListenBlock(const std::string& element_var);
    void parseEventBindingOperator(const std::string& element_var);

    // Helpers
    std::string transformSelector(const std::string& selector);
    std::string extractBlockContent(char open, char close);
    void trim(std::string& s);
    std::string peek(int n = 0);
    bool match(const std::string& expected);
    void advance(int n = 1);
    bool isAtEnd();

    const std::string& source;
    std::string output_buffer;
    size_t current_pos = 0;
};

} // namespace CHTL

#endif // CHTL_JS_PROCESSOR_H
