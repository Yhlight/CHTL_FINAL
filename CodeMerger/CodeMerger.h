#pragma once

#include "../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <string>
#include <vector>
#include <map>

namespace CHTL {

class CodeMerger {
public:
    CodeMerger();
    void setMainHtml(const std::string& html);
    void addCss(const std::string& css);
    void addJs(const std::string& js);
    void setCompiledBlocks(const std::map<std::string, std::string>& blocks);
    CompilationResult getResult();

private:
    std::string main_html;
    std::string final_css;
    std::string final_js;
    std::map<std::string, std::string> compiled_blocks;
};

} // namespace CHTL
