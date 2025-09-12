#include "Scanner/UnifiedScanner.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

// 静态成员初始化
const std::vector<std::string> UnifiedScanner::CHTL_BOUNDARIES = {
    "[Template]", "[Custom]", "[Origin]", "[Import]", "[Namespace]", 
    "[Configuration]", "[Info]", "[Export]", "use", "text", "style", "script"
};

const std::vector<std::string> UnifiedScanner::CHTL_JS_BOUNDARIES = {
    "listen", "animate", "delegate", "router", "fileloader", "vir", "iNeverAway", "util"
};

const std::vector<std::string> UnifiedScanner::CSS_BOUNDARIES = {
    "@media", "@keyframes", "@import", "@charset", "@page", "@font-face"
};

const std::vector<std::string> UnifiedScanner::JS_BOUNDARIES = {
    "function", "const", "let", "var", "class", "import", "export", "if", "for", "while"
};

UnifiedScanner::UnifiedScanner() 
    : placeholder_prefix_("_PLACEHOLDER_"), placeholder_counter_(0) {
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::scan(const std::string& source_code) {
    placeholder_map_.clear();
    placeholder_counter_ = 0;
    
    std::vector<CodeFragment> fragments;
    
    // 首先进行宽判处理，识别大块的CHTL代码
    auto wide_fragments = wideJudgment(source_code);
    fragments.insert(fragments.end(), wide_fragments.begin(), wide_fragments.end());
    
    // 对剩余的代码进行严判处理
    std::string remaining_code = source_code;
    for (const auto& fragment : wide_fragments) {
        // 移除已处理的代码片段
        remaining_code.erase(fragment.start_pos, fragment.end_pos - fragment.start_pos);
    }
    
    if (!remaining_code.empty()) {
        auto strict_fragments = strictJudgment(remaining_code);
        fragments.insert(fragments.end(), strict_fragments.begin(), strict_fragments.end());
    }
    
    return fragments;
}

void UnifiedScanner::setPlaceholderPrefix(const std::string& prefix) {
    placeholder_prefix_ = prefix;
}

const std::unordered_map<std::string, std::string>& UnifiedScanner::getPlaceholderMap() const {
    return placeholder_map_;
}

void UnifiedScanner::clearPlaceholderMap() {
    placeholder_map_.clear();
    placeholder_counter_ = 0;
}

UnifiedScanner::CodeType UnifiedScanner::identifyCodeType(const std::string& content) {
    // 检查CHTL边界
    for (const auto& boundary : CHTL_BOUNDARIES) {
        if (content.find(boundary) != std::string::npos) {
            return CodeType::CHTL;
        }
    }
    
    // 检查CHTL JS边界
    for (const auto& boundary : CHTL_JS_BOUNDARIES) {
        if (content.find(boundary) != std::string::npos) {
            return CodeType::CHTL_JS;
        }
    }
    
    // 检查CSS边界
    for (const auto& boundary : CSS_BOUNDARIES) {
        if (content.find(boundary) != std::string::npos) {
            return CodeType::CSS;
        }
    }
    
    // 检查JS边界
    for (const auto& boundary : JS_BOUNDARIES) {
        if (content.find(boundary) != std::string::npos) {
            return CodeType::JS;
        }
    }
    
    return CodeType::UNKNOWN;
}

UnifiedScanner::CodeFragment UnifiedScanner::processCHTLFragment(const std::string& content, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::CHTL;
    fragment.content = content;
    fragment.start_pos = start_pos;
    fragment.end_pos = start_pos + content.length();
    return fragment;
}

UnifiedScanner::CodeFragment UnifiedScanner::processCHTLJSFragment(const std::string& content, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::CHTL_JS;
    fragment.content = content;
    fragment.start_pos = start_pos;
    fragment.end_pos = start_pos + content.length();
    return fragment;
}

UnifiedScanner::CodeFragment UnifiedScanner::processCSSFragment(const std::string& content, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::CSS;
    fragment.content = content;
    fragment.start_pos = start_pos;
    fragment.end_pos = start_pos + content.length();
    return fragment;
}

UnifiedScanner::CodeFragment UnifiedScanner::processJSFragment(const std::string& content, size_t start_pos) {
    CodeFragment fragment;
    fragment.type = CodeType::JS;
    fragment.content = content;
    fragment.start_pos = start_pos;
    fragment.end_pos = start_pos + content.length();
    return fragment;
}

std::string UnifiedScanner::createPlaceholder(const std::string& content) {
    std::string placeholder = placeholder_prefix_ + std::to_string(placeholder_counter_++);
    placeholder_map_[placeholder] = content;
    return placeholder;
}

std::vector<std::pair<size_t, size_t>> UnifiedScanner::doublePointerScan(
    const std::string& content, 
    const std::string& keyword) {
    
    std::vector<std::pair<size_t, size_t>> results;
    size_t front = 0;
    size_t back = 0;
    
    while (front < content.length()) {
        // 预先扫描是否存在关键字
        size_t keyword_pos = content.find(keyword, front);
        if (keyword_pos == std::string::npos) {
            break;
        }
        
        // 移动前指针到关键字位置
        front = keyword_pos;
        
        // 通知后指针准备收集
        back = front;
        
        // 收集完整的语法片段
        size_t start = back;
        size_t end = front + keyword.length();
        
        // 扩展边界以包含完整的语法块
        while (end < content.length() && !isSyntaxBoundary(content, end)) {
            end++;
        }
        
        results.push_back({start, end});
        front = end;
    }
    
    return results;
}

std::string UnifiedScanner::preemptiveCapture(const std::string& content, const std::string& keyword) {
    size_t keyword_pos = content.find(keyword);
    if (keyword_pos == std::string::npos) {
        return content;
    }
    
    // 截取关键字之前的内容
    return content.substr(0, keyword_pos);
}

bool UnifiedScanner::isSyntaxBoundary(const std::string& content, size_t pos) {
    if (pos >= content.length()) return true;
    
    char ch = content[pos];
    return (ch == '{' || ch == '}' || ch == ';' || ch == '\n' || ch == '\r');
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::wideJudgment(const std::string& content) {
    std::vector<CodeFragment> fragments;
    
    // 使用双指针扫描识别大块的CHTL代码
    for (const auto& boundary : CHTL_BOUNDARIES) {
        auto positions = doublePointerScan(content, boundary);
        for (const auto& pos : positions) {
            std::string fragment_content = content.substr(pos.first, pos.second - pos.first);
            CodeType type = identifyCodeType(fragment_content);
            
            if (type == CodeType::CHTL) {
                fragments.push_back(processCHTLFragment(fragment_content, pos.first));
            }
        }
    }
    
    return fragments;
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScanner::strictJudgment(const std::string& content) {
    std::vector<CodeFragment> fragments;
    
    // 使用占位符机制分离CHTL JS和JS代码
    std::string processed_content = content;
    
    // 识别并替换JS代码为占位符
    std::regex js_pattern(R"((function\s+\w+\s*\([^)]*\)\s*\{[^}]*\})|(const\s+\w+\s*=.*?;)|(let\s+\w+\s*=.*?;)|(var\s+\w+\s*=.*?;))");
    std::smatch matches;
    
    while (std::regex_search(processed_content, matches, js_pattern)) {
        std::string js_code = matches[0].str();
        std::string placeholder = createPlaceholder(js_code);
        processed_content = std::regex_replace(processed_content, js_pattern, placeholder, std::regex_constants::format_first_only);
    }
    
    // 识别CHTL JS代码
    for (const auto& boundary : CHTL_JS_BOUNDARIES) {
        size_t pos = processed_content.find(boundary);
        if (pos != std::string::npos) {
            // 找到CHTL JS代码的完整块
            size_t start = pos;
            size_t end = pos + boundary.length();
            
            // 扩展以包含完整的语法块
            while (end < processed_content.length() && !isSyntaxBoundary(processed_content, end)) {
                end++;
            }
            
            std::string fragment_content = processed_content.substr(start, end - start);
            fragments.push_back(processCHTLJSFragment(fragment_content, start));
        }
    }
    
    // 处理剩余的JS代码
    std::string remaining = processed_content;
    for (const auto& fragment : fragments) {
        remaining.erase(fragment.start_pos, fragment.end_pos - fragment.start_pos);
    }
    
    if (!remaining.empty()) {
        // 移除占位符，恢复原始JS代码
        for (const auto& pair : placeholder_map_) {
            size_t pos = remaining.find(pair.first);
            if (pos != std::string::npos) {
                remaining.replace(pos, pair.first.length(), pair.second);
            }
        }
        
        if (!remaining.empty()) {
            fragments.push_back(processJSFragment(remaining, 0));
        }
    }
    
    return fragments;
}

} // namespace CHTL