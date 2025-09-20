#ifndef CHTL_SCRIPT_CONTENT_PROCESSOR_H
#define CHTL_SCRIPT_CONTENT_PROCESSOR_H

#include <string>
#include <memory>
#include "Config/Configuration.h"

namespace CHTL {

class ScriptContentProcessor {
public:
    ScriptContentProcessor(std::shared_ptr<Configuration> config);

    /**
     * @brief Processes a string containing mixed JS and CHTL-JS code.
     *
     * This method implements the placeholder mechanism. It scans the input string,
     * identifies CHTL-JS constructs, and replaces all surrounding pure JS code
     * with placeholders. The CHTL-JS with placeholders is then compiled, and
     * finally, the placeholders are replaced with the original JS code.
     *
     * @param script_content The raw string content from a <script> block.
     * @return std::string The final, compiled JavaScript code.
     */
    std::string process(const std::string& script_content);

private:
    std::shared_ptr<Configuration> m_config;
    int m_placeholder_counter = 0;
};

} // namespace CHTL

#endif // CHTL_SCRIPT_CONTENT_PROCESSOR_H
