#include "UnifiedScanner.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

UnifiedScanner::UnifiedScanner() {
    // 初始化 CHTL 关键词
    chtlKeywords_ = {
        "div", "span", "p", "h1", "h2", "h3", "h4", "h5", "h6",
        "a", "img", "ul", "ol", "li", "table", "tr", "td", "th",
        "form", "input", "button", "select", "option", "textarea",
        "style", "script", "head", "body", "html", "title", "meta",
        "link", "br", "hr", "strong", "em", "b", "i", "u", "s",
        "Template", "Custom", "Origin", "Import", "Configuration", "Namespace"
    };
    
    // 初始化 CHTL JS 关键词
    chtljsKeywords_ = {
        "fileloader", "script", "animate", "listen", "util",
        "function", "var", "let", "const", "if", "else", "for", "while",
        "return", "break", "continue", "switch", "case", "default",
        "try", "catch", "finally", "throw", "new", "this", "super",
        "class", "extends", "import", "export", "from", "as"
    };
    
    // 初始化 CSS 关键词
    cssKeywords_ = {
        "color", "background", "border", "margin", "padding", "width", "height",
        "font", "text", "display", "position", "float", "clear", "overflow",
        "z-index", "opacity", "visibility", "cursor", "pointer-events",
        "transition", "animation", "transform", "flex", "grid", "box-shadow"
    };
    
    // 初始化 JavaScript 关键词
    javascriptKeywords_ = {
        "function", "var", "let", "const", "if", "else", "for", "while",
        "return", "break", "continue", "switch", "case", "default",
        "try", "catch", "finally", "throw", "new", "this", "super",
        "class", "extends", "import", "export", "from", "as", "async", "await"
    };
    
    // 初始化块开始标记
    chtlBlockStarters_ = {
        "[Template]", "[Custom]", "[Origin]", "[Import]", "[Configuration]", "[Namespace]",
        "div {", "span {", "p {", "h1 {", "h2 {", "h3 {", "h4 {", "h5 {", "h6 {",
        "a {", "img {", "ul {", "ol {", "li {", "table {", "tr {", "td {", "th {",
        "form {", "input {", "button {", "select {", "option {", "textarea {",
        "style {", "script {", "head {", "body {", "html {", "title {", "meta {",
        "link {", "br {", "hr {", "strong {", "em {", "b {", "i {", "u {", "s {"
    };
    
    chtljsBlockStarters_ = {
        "fileloader {", "script {", "animate {", "listen {", "util {",
        "function ", "var ", "let ", "const ", "if ", "else ", "for ", "while ",
        "return ", "break ", "continue ", "switch ", "case ", "default ",
        "try {", "catch {", "finally {", "throw ", "new ", "this.", "super.",
        "class ", "extends ", "import ", "export ", "from ", "as "
    };
    
    cssBlockStarters_ = {
        ".", "#", "[", "*", "body", "html", "head", "div", "span", "p",
        "h1", "h2", "h3", "h4", "h5", "h6", "a", "img", "ul", "ol", "li",
        "table", "tr", "td", "th", "form", "input", "button", "select", "option", "textarea"
    };
    
    javascriptBlockStarters_ = {
        "function ", "var ", "let ", "const ", "if ", "else ", "for ", "while ",
        "return ", "break ", "continue ", "switch ", "case ", "default ",
        "try {", "catch {", "finally {", "throw ", "new ", "this.", "super.",
        "class ", "extends ", "import ", "export ", "from ", "as ", "async ", "await "
    };
    
    // 初始化块结束标记
    chtlBlockEnders_ = {"}", "];", ");"};
    chtljsBlockEnders_ = {"}", "];", ");", "break;", "continue;", "return;"};
    cssBlockEnders_ = {"}", "];", ");"};
    javascriptBlockEnders_ = {"}", "];", ");", "break;", "continue;", "return;"};
}

std::vector<CodeBlock> UnifiedScanner::scan(const std::string& content) {
    std::vector<CodeBlock> blocks;
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    CodeBlock currentBlock;
    bool inBlock = false;
    CodeType currentType = CodeType::CHTL;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        // 检测代码类型
        CodeType detectedType = detectCodeTypeByContent(line);
        if (detectedType != CodeType::CHTL) {
            if (inBlock && currentBlock.type != detectedType) {
                // 结束当前块
                currentBlock.endLine = lineNumber - 1;
                blocks.push_back(currentBlock);
                inBlock = false;
            }
            
            if (!inBlock) {
                // 开始新块
                currentBlock = CodeBlock();
                currentBlock.type = detectedType;
                currentBlock.startLine = lineNumber;
                currentBlock.startColumn = 0;
                currentBlock.content = line + "\n";
                inBlock = true;
                currentType = detectedType;
            } else {
                // 继续当前块
                currentBlock.content += line + "\n";
            }
        } else {
            if (inBlock && currentType != CodeType::CHTL) {
                // 结束当前块
                currentBlock.endLine = lineNumber - 1;
                blocks.push_back(currentBlock);
                inBlock = false;
            }
            
            if (!inBlock) {
                // 开始 CHTL 块
                currentBlock = CodeBlock();
                currentBlock.type = CodeType::CHTL;
                currentBlock.startLine = lineNumber;
                currentBlock.startColumn = 0;
                currentBlock.content = line + "\n";
                inBlock = true;
                currentType = CodeType::CHTL;
            } else {
                // 继续 CHTL 块
                currentBlock.content += line + "\n";
            }
        }
    }
    
    // 结束最后一个块
    if (inBlock) {
        currentBlock.endLine = lineNumber;
        blocks.push_back(currentBlock);
    }
    
    return blocks;
}

std::vector<CodeBlock> UnifiedScanner::scanFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return {};
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    return scan(content);
}

CodeType UnifiedScanner::detectCodeType(const std::string& content) const {
    return detectCodeTypeByContent(content);
}

CodeType UnifiedScanner::detectCodeTypeByExtension(const std::string& filePath) const {
    std::string extension = filePath.substr(filePath.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "chtl") {
        return CodeType::CHTL;
    } else if (extension == "chtljs" || extension == "cjjs") {
        return CodeType::CHTL_JS;
    } else if (extension == "css") {
        return CodeType::CSS;
    } else if (extension == "js" || extension == "javascript") {
        return CodeType::JAVASCRIPT;
    }
    
    return CodeType::CHTL; // 默认
}

CodeType UnifiedScanner::detectCodeTypeByContent(const std::string& content) const {
    std::string trimmed = content;
    trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
    trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);
    
    // 检测 CHTL JS 代码
    if (isCHTLJSBlockStart(trimmed)) {
        return CodeType::CHTL_JS;
    }
    
    // 检测 CSS 代码
    if (isCSSBlockStart(trimmed)) {
        return CodeType::CSS;
    }
    
    // 检测 JavaScript 代码
    if (isJavaScriptBlockStart(trimmed)) {
        return CodeType::JAVASCRIPT;
    }
    
    // 检测 CHTL 代码
    if (isCHTLBlockStart(trimmed)) {
        return CodeType::CHTL;
    }
    
    return CodeType::CHTL; // 默认
}

std::vector<CodeBlock> UnifiedScanner::extractCHTLBlocks(const std::string& content) const {
    std::vector<CodeBlock> blocks;
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    CodeBlock currentBlock;
    bool inBlock = false;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        if (isCHTLBlockStart(line)) {
            if (inBlock) {
                // 结束当前块
                currentBlock.endLine = lineNumber - 1;
                blocks.push_back(currentBlock);
            }
            
            // 开始新块
            currentBlock = CodeBlock();
            currentBlock.type = CodeType::CHTL;
            currentBlock.startLine = lineNumber;
            currentBlock.startColumn = 0;
            currentBlock.content = line + "\n";
            inBlock = true;
        } else if (inBlock) {
            if (isCHTLBlockEnd(line)) {
                // 结束当前块
                currentBlock.content += line + "\n";
                currentBlock.endLine = lineNumber;
                blocks.push_back(currentBlock);
                inBlock = false;
            } else {
                // 继续当前块
                currentBlock.content += line + "\n";
            }
        }
    }
    
    return blocks;
}

std::vector<CodeBlock> UnifiedScanner::extractCHTLJSBlocks(const std::string& content) const {
    std::vector<CodeBlock> blocks;
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    CodeBlock currentBlock;
    bool inBlock = false;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        if (isCHTLJSBlockStart(line)) {
            if (inBlock) {
                // 结束当前块
                currentBlock.endLine = lineNumber - 1;
                blocks.push_back(currentBlock);
            }
            
            // 开始新块
            currentBlock = CodeBlock();
            currentBlock.type = CodeType::CHTL_JS;
            currentBlock.startLine = lineNumber;
            currentBlock.startColumn = 0;
            currentBlock.content = line + "\n";
            inBlock = true;
        } else if (inBlock) {
            if (isCHTLJSBlockEnd(line)) {
                // 结束当前块
                currentBlock.content += line + "\n";
                currentBlock.endLine = lineNumber;
                blocks.push_back(currentBlock);
                inBlock = false;
            } else {
                // 继续当前块
                currentBlock.content += line + "\n";
            }
        }
    }
    
    return blocks;
}

std::vector<CodeBlock> UnifiedScanner::extractCSSBlocks(const std::string& content) const {
    std::vector<CodeBlock> blocks;
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    CodeBlock currentBlock;
    bool inBlock = false;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        if (isCSSBlockStart(line)) {
            if (inBlock) {
                // 结束当前块
                currentBlock.endLine = lineNumber - 1;
                blocks.push_back(currentBlock);
            }
            
            // 开始新块
            currentBlock = CodeBlock();
            currentBlock.type = CodeType::CSS;
            currentBlock.startLine = lineNumber;
            currentBlock.startColumn = 0;
            currentBlock.content = line + "\n";
            inBlock = true;
        } else if (inBlock) {
            if (isCSSBlockEnd(line)) {
                // 结束当前块
                currentBlock.content += line + "\n";
                currentBlock.endLine = lineNumber;
                blocks.push_back(currentBlock);
                inBlock = false;
            } else {
                // 继续当前块
                currentBlock.content += line + "\n";
            }
        }
    }
    
    return blocks;
}

std::vector<CodeBlock> UnifiedScanner::extractJavaScriptBlocks(const std::string& content) const {
    std::vector<CodeBlock> blocks;
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    
    CodeBlock currentBlock;
    bool inBlock = false;
    
    while (std::getline(stream, line)) {
        lineNumber++;
        
        if (isJavaScriptBlockStart(line)) {
            if (inBlock) {
                // 结束当前块
                currentBlock.endLine = lineNumber - 1;
                blocks.push_back(currentBlock);
            }
            
            // 开始新块
            currentBlock = CodeBlock();
            currentBlock.type = CodeType::JAVASCRIPT;
            currentBlock.startLine = lineNumber;
            currentBlock.startColumn = 0;
            currentBlock.content = line + "\n";
            inBlock = true;
        } else if (inBlock) {
            if (isJavaScriptBlockEnd(line)) {
                // 结束当前块
                currentBlock.content += line + "\n";
                currentBlock.endLine = lineNumber;
                blocks.push_back(currentBlock);
                inBlock = false;
            } else {
                // 继续当前块
                currentBlock.content += line + "\n";
            }
        }
    }
    
    return blocks;
}

bool UnifiedScanner::validateCodeBlock(const CodeBlock& block) const {
    switch (block.type) {
        case CodeType::CHTL:
            return validateCHTLBlock(block);
        case CodeType::CHTL_JS:
            return validateCHTLJSBlock(block);
        case CodeType::CSS:
            return validateCSSBlock(block);
        case CodeType::JAVASCRIPT:
            return validateJavaScriptBlock(block);
        default:
            return false;
    }
}

bool UnifiedScanner::validateCHTLBlock(const CodeBlock& block) const {
    // 简单的 CHTL 块验证
    return !block.content.empty() && block.content.find('{') != std::string::npos;
}

bool UnifiedScanner::validateCHTLJSBlock(const CodeBlock& block) const {
    // 简单的 CHTL JS 块验证
    return !block.content.empty() && 
           (block.content.find("fileloader") != std::string::npos ||
            block.content.find("script") != std::string::npos ||
            block.content.find("animate") != std::string::npos ||
            block.content.find("listen") != std::string::npos ||
            block.content.find("util") != std::string::npos);
}

bool UnifiedScanner::validateCSSBlock(const CodeBlock& block) const {
    // 简单的 CSS 块验证
    return !block.content.empty() && 
           (block.content.find('{') != std::string::npos &&
            block.content.find('}') != std::string::npos);
}

bool UnifiedScanner::validateJavaScriptBlock(const CodeBlock& block) const {
    // 简单的 JavaScript 块验证
    return !block.content.empty() && 
           (block.content.find("function") != std::string::npos ||
            block.content.find("var") != std::string::npos ||
            block.content.find("let") != std::string::npos ||
            block.content.find("const") != std::string::npos);
}

std::string UnifiedScanner::convertToCHTL(const CodeBlock& block) const {
    if (block.type == CodeType::CHTL) {
        return block.content;
    }
    
    // 其他类型转换为 CHTL 的简单实现
    std::ostringstream oss;
    oss << "// Converted from " << static_cast<int>(block.type) << "\n";
    oss << block.content;
    return oss.str();
}

std::string UnifiedScanner::convertToCHTLJS(const CodeBlock& block) const {
    if (block.type == CodeType::CHTL_JS) {
        return block.content;
    }
    
    // 其他类型转换为 CHTL JS 的简单实现
    std::ostringstream oss;
    oss << "// Converted from " << static_cast<int>(block.type) << "\n";
    oss << block.content;
    return oss.str();
}

std::string UnifiedScanner::convertToCSS(const CodeBlock& block) const {
    if (block.type == CodeType::CSS) {
        return block.content;
    }
    
    // 其他类型转换为 CSS 的简单实现
    std::ostringstream oss;
    oss << "/* Converted from " << static_cast<int>(block.type) << " */\n";
    oss << block.content;
    return oss.str();
}

std::string UnifiedScanner::convertToJavaScript(const CodeBlock& block) const {
    if (block.type == CodeType::JAVASCRIPT) {
        return block.content;
    }
    
    // 其他类型转换为 JavaScript 的简单实现
    std::ostringstream oss;
    oss << "// Converted from " << static_cast<int>(block.type) << "\n";
    oss << block.content;
    return oss.str();
}

std::string UnifiedScanner::mergeBlocks(const std::vector<CodeBlock>& blocks, CodeType targetType) const {
    switch (targetType) {
        case CodeType::CHTL:
            return mergeToCHTL(blocks);
        case CodeType::CHTL_JS:
            return mergeToCHTLJS(blocks);
        case CodeType::CSS:
            return mergeToCSS(blocks);
        case CodeType::JAVASCRIPT:
            return mergeToJavaScript(blocks);
        default:
            return "";
    }
}

std::string UnifiedScanner::mergeToCHTL(const std::vector<CodeBlock>& blocks) const {
    std::ostringstream oss;
    
    for (const auto& block : blocks) {
        oss << convertToCHTL(block) << "\n";
    }
    
    return oss.str();
}

std::string UnifiedScanner::mergeToCHTLJS(const std::vector<CodeBlock>& blocks) const {
    std::ostringstream oss;
    
    for (const auto& block : blocks) {
        oss << convertToCHTLJS(block) << "\n";
    }
    
    return oss.str();
}

std::string UnifiedScanner::mergeToCSS(const std::vector<CodeBlock>& blocks) const {
    std::ostringstream oss;
    
    for (const auto& block : blocks) {
        oss << convertToCSS(block) << "\n";
    }
    
    return oss.str();
}

std::string UnifiedScanner::mergeToJavaScript(const std::vector<CodeBlock>& blocks) const {
    std::ostringstream oss;
    
    for (const auto& block : blocks) {
        oss << convertToJavaScript(block) << "\n";
    }
    
    return oss.str();
}

std::vector<CodeBlock> UnifiedScanner::filterByType(const std::vector<CodeBlock>& blocks, CodeType type) const {
    std::vector<CodeBlock> filtered;
    
    for (const auto& block : blocks) {
        if (block.type == type) {
            filtered.push_back(block);
        }
    }
    
    return filtered;
}

std::vector<CodeBlock> UnifiedScanner::filterByLanguage(const std::vector<CodeBlock>& blocks, const std::string& language) const {
    std::vector<CodeBlock> filtered;
    
    for (const auto& block : blocks) {
        if (block.language == language) {
            filtered.push_back(block);
        }
    }
    
    return filtered;
}

size_t UnifiedScanner::countBlocks(const std::vector<CodeBlock>& blocks, CodeType type) const {
    size_t count = 0;
    
    for (const auto& block : blocks) {
        if (block.type == type) {
            count++;
        }
    }
    
    return count;
}

size_t UnifiedScanner::countLines(const std::vector<CodeBlock>& blocks, CodeType type) const {
    size_t count = 0;
    
    for (const auto& block : blocks) {
        if (block.type == type) {
            count += block.endLine - block.startLine + 1;
        }
    }
    
    return count;
}

size_t UnifiedScanner::countCharacters(const std::vector<CodeBlock>& blocks, CodeType type) const {
    size_t count = 0;
    
    for (const auto& block : blocks) {
        if (block.type == type) {
            count += block.content.length();
        }
    }
    
    return count;
}

std::string UnifiedScanner::formatCodeBlock(const CodeBlock& block) const {
    std::ostringstream oss;
    
    oss << "Type: " << static_cast<int>(block.type) << "\n";
    oss << "Language: " << block.language << "\n";
    oss << "Start: " << block.startLine << ":" << block.startColumn << "\n";
    oss << "End: " << block.endLine << ":" << block.endColumn << "\n";
    oss << "Content:\n" << block.content << "\n";
    
    return oss.str();
}

std::string UnifiedScanner::formatBlocks(const std::vector<CodeBlock>& blocks) const {
    std::ostringstream oss;
    
    for (size_t i = 0; i < blocks.size(); ++i) {
        oss << "Block " << i + 1 << ":\n";
        oss << formatCodeBlock(blocks[i]) << "\n";
    }
    
    return oss.str();
}

// 私有方法实现
bool UnifiedScanner::isCHTLKeyword(const std::string& word) const {
    return std::find(chtlKeywords_.begin(), chtlKeywords_.end(), word) != chtlKeywords_.end();
}

bool UnifiedScanner::isCHTLJSKeyword(const std::string& word) const {
    return std::find(chtljsKeywords_.begin(), chtljsKeywords_.end(), word) != chtljsKeywords_.end();
}

bool UnifiedScanner::isCSSKeyword(const std::string& word) const {
    return std::find(cssKeywords_.begin(), cssKeywords_.end(), word) != cssKeywords_.end();
}

bool UnifiedScanner::isJavaScriptKeyword(const std::string& word) const {
    return std::find(javascriptKeywords_.begin(), javascriptKeywords_.end(), word) != javascriptKeywords_.end();
}

bool UnifiedScanner::isCHTLComment(const std::string& line) const {
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    return trimmed.substr(0, 2) == "//" || 
           (trimmed.substr(0, 2) == "/*" && trimmed.substr(trimmed.length() - 2) == "*/");
}

bool UnifiedScanner::isCHTLJSComment(const std::string& line) const {
    return isCHTLComment(line); // CHTL JS 使用相同的注释语法
}

bool UnifiedScanner::isCSSComment(const std::string& line) const {
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    return trimmed.substr(0, 2) == "/*" && trimmed.substr(trimmed.length() - 2) == "*/";
}

bool UnifiedScanner::isJavaScriptComment(const std::string& line) const {
    std::string trimmed = line;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    return trimmed.substr(0, 2) == "//" || 
           (trimmed.substr(0, 2) == "/*" && trimmed.substr(trimmed.length() - 2) == "*/");
}

bool UnifiedScanner::isCHTLBlockStart(const std::string& line) const {
    for (const auto& starter : chtlBlockStarters_) {
        if (line.find(starter) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isCHTLJSBlockStart(const std::string& line) const {
    for (const auto& starter : chtljsBlockStarters_) {
        if (line.find(starter) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isCSSBlockStart(const std::string& line) const {
    for (const auto& starter : cssBlockStarters_) {
        if (line.find(starter) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isJavaScriptBlockStart(const std::string& line) const {
    for (const auto& starter : javascriptBlockStarters_) {
        if (line.find(starter) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isCHTLBlockEnd(const std::string& line) const {
    for (const auto& ender : chtlBlockEnders_) {
        if (line.find(ender) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isCHTLJSBlockEnd(const std::string& line) const {
    for (const auto& ender : chtljsBlockEnders_) {
        if (line.find(ender) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isCSSBlockEnd(const std::string& line) const {
    for (const auto& ender : cssBlockEnders_) {
        if (line.find(ender) != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool UnifiedScanner::isJavaScriptBlockEnd(const std::string& line) const {
    for (const auto& ender : javascriptBlockEnders_) {
        if (line.find(ender) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::string UnifiedScanner::extractLanguageFromBlock(const std::string& content) const {
    // 简单的语言检测
    if (content.find("fileloader") != std::string::npos ||
        content.find("script") != std::string::npos ||
        content.find("animate") != std::string::npos ||
        content.find("listen") != std::string::npos ||
        content.find("util") != std::string::npos) {
        return "chtljs";
    }
    
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return "css";
    }
    
    if (content.find("function") != std::string::npos ||
        content.find("var") != std::string::npos ||
        content.find("let") != std::string::npos ||
        content.find("const") != std::string::npos) {
        return "javascript";
    }
    
    return "chtl";
}

std::string UnifiedScanner::normalizeCodeBlock(const CodeBlock& block) const {
    std::string normalized = block.content;
    
    // 移除多余的空行
    std::regex multipleNewlines("\\n\\s*\\n\\s*\\n");
    normalized = std::regex_replace(normalized, multipleNewlines, "\n\n");
    
    // 移除行尾空格
    std::regex trailingSpaces("\\s+\\n");
    normalized = std::regex_replace(normalized, trailingSpaces, "\n");
    
    return normalized;
}

} // namespace CHTL