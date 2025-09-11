#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <string>
#include <vector>
#include <map>

namespace CHTL {

// 代码片段结构
struct CodeFragment {
    std::string type;        // 片段类型：CHTL, CHTL_JS, CSS, JS
    std::string content;     // 片段内容
    size_t startLine;        // 起始行号
    size_t endLine;          // 结束行号
    size_t startColumn;      // 起始列号
    size_t endColumn;        // 结束列号
    std::map<std::string, std::string> metadata; // 元数据
};

// 扫描结果
struct ScanResult {
    std::vector<CodeFragment> fragments;
    std::string originalSource;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// 编译结果
struct CompileResult {
    std::map<std::string, std::string> outputs; // 类型 -> 输出内容
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata; // 元数据
};

// 最终结果
struct FinalResult {
    std::string html;
    std::string css;
    std::string javascript;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::map<std::string, std::string> metadata;
};

} // namespace CHTL

#endif // COMMON_TYPES_H