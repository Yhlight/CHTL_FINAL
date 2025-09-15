#include "UnifiedScannerComplete.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace CHTL {

UnifiedScannerComplete::UnifiedScannerComplete() 
    : error_recovery_mode_(true), verbose_mode_(false), next_placeholder_id_(1) {
    initializeDefaultConfig();
    initializeRegexCache();
}

UnifiedScannerComplete::~UnifiedScannerComplete() {
}

std::vector<UnifiedScanner::CodeFragment> UnifiedScannerComplete::scan(const std::string& source) {
    auto extended_fragments = scanExtended(source);
    std::vector<CodeFragment> fragments;
    
    // 转换扩展片段为基础片段
    for (const auto& ext_frag : extended_fragments) {
        CodeFragment frag;
        frag.type = ext_frag.type;
        frag.content = ext_frag.content;
        frag.start_pos = ext_frag.start_pos;
        frag.end_pos = ext_frag.end_pos;
        frag.line = ext_frag.line;
        frag.column = ext_frag.column;
        fragments.push_back(frag);
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanExtended(const std::string& source) {
    if (scan_config_.enable_placeholder_mode) {
        return scanWithPlaceholders(source);
    } else {
        return scanWithContext(source);
    }
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanWithPlaceholders(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    std::string working_source = source;
    
    logVerbose("Starting scan with placeholders, source length: " + std::to_string(source.length()));
    
    // 第一阶段：提取并替换特殊语法为占位符
    std::vector<ExtendedCodeFragment> special_fragments;
    
    // 提取CHTL原始嵌入 {{& ... &}}
    auto origin_fragments = scanSpecialSyntax(working_source);
    for (const auto& frag : origin_fragments) {
        if (frag.extended_type == ExtendedCodeType::CHTL_ORIGIN) {
            std::string placeholder = createPlaceholder(frag);
            working_source.replace(frag.start_pos, frag.end_pos - frag.start_pos, placeholder);
            special_fragments.push_back(frag);
        }
    }
    
    // 提取CHTL JS特殊语法 {{&}}
    auto chtljs_fragments = scanCHTLJSSyntax(working_source);
    for (const auto& frag : chtljs_fragments) {
        if (frag.extended_type == ExtendedCodeType::CHTL_JS_SPECIAL) {
            std::string placeholder = createPlaceholder(frag);
            working_source.replace(frag.start_pos, frag.end_pos - frag.start_pos, placeholder);
            special_fragments.push_back(frag);
        }
    }
    
    // 提取模板变量、自定义变量、样式组引用 {{ ... }}
    auto template_fragments = scanTemplateSyntax(working_source);
    for (const auto& frag : template_fragments) {
        std::string placeholder = createPlaceholder(frag);
        working_source.replace(frag.start_pos, frag.end_pos - frag.start_pos, placeholder);
        special_fragments.push_back(frag);
    }
    
    // 第二阶段：扫描主要代码结构
    size_t pos = 0;
    while (pos < working_source.length()) {
        pos = skipWhitespaceAndComments(working_source, pos);
        if (pos >= working_source.length()) break;
        
        ExtendedCodeFragment fragment;
        bool found = false;
        
        // 检测CHTL语法
        if (isCHTLElementBoundary(working_source, pos)) {
            fragment = extractCHTLElement(working_source, pos);
            found = true;
        } else if (isCHTLTemplateBoundary(working_source, pos)) {
            fragment = extractCHTLTemplate(working_source, pos);
            found = true;
        } else if (isCHTLCustomBoundary(working_source, pos)) {
            fragment = extractCHTLCustom(working_source, pos);
            found = true;
        } else if (isCHTLNamespaceBoundary(working_source, pos)) {
            fragment = extractCHTLNamespace(working_source, pos);
            found = true;
        } else if (isCHTLConfigBoundary(working_source, pos)) {
            fragment = extractCHTLConfig(working_source, pos);
            found = true;
        } else if (isCHTLImportBoundary(working_source, pos)) {
            fragment = extractCHTLImport(working_source, pos);
            found = true;
        } else if (isCHTLStyleBlockBoundary(working_source, pos)) {
            fragment = extractCHTLStyleBlock(working_source, pos);
            found = true;
        } else if (isCHTLScriptBlockBoundary(working_source, pos)) {
            fragment = extractCHTLScriptBlock(working_source, pos);
            found = true;
        }
        
        if (found && fragment.extended_type != ExtendedCodeType::UNKNOWN_EXT) {
            // 检查嵌套
            if (scan_config_.enable_nesting_detection) {
                fragment.children = extractNestedFragments(fragment);
                fragment.depth_level = calculateNestingDepth(working_source, pos);
            }
            
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else {
            if (error_recovery_mode_) {
                if (!recoverFromSyntaxError(working_source, pos)) {
                    pos++;
                }
            } else {
                pos++;
            }
        }
    }
    
    // 第三阶段：合并特殊片段
    fragments.insert(fragments.end(), special_fragments.begin(), special_fragments.end());
    
    // 按位置排序
    std::sort(fragments.begin(), fragments.end(), 
              [](const ExtendedCodeFragment& a, const ExtendedCodeFragment& b) {
                  return a.start_pos < b.start_pos;
              });
    
    // 更新统计信息
    fragment_statistics_.clear();
    for (const auto& frag : fragments) {
        fragment_statistics_[frag.extended_type]++;
    }
    
    logVerbose("Scan completed, found " + std::to_string(fragments.size()) + " fragments");
    
    if (verbose_mode_) {
        dumpFragments(fragments);
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanWithContext(const std::string& source, const std::string& context) {
    std::vector<ExtendedCodeFragment> fragments;
    
    logVerbose("Starting context-aware scan, context: " + context);
    
    size_t pos = 0;
    std::string current_context = context;
    
    while (pos < source.length()) {
        pos = skipWhitespaceAndComments(source, pos);
        if (pos >= source.length()) break;
        
        // 分析当前上下文
        if (scan_config_.enable_context_awareness) {
            current_context = analyzeContext(source, pos);
        }
        
        ExtendedCodeFragment fragment;
        fragment.context = current_context;
        
        // 根据上下文选择适当的扫描策略
        if (current_context == "chtl_element") {
            fragment = extractCHTLElement(source, pos);
        } else if (current_context == "chtl_template") {
            fragment = extractCHTLTemplate(source, pos);
        } else if (current_context == "chtl_custom") {
            fragment = extractCHTLCustom(source, pos);
        } else if (current_context == "css_block") {
            fragment = extractCHTLStyleBlock(source, pos);
        } else if (current_context == "js_block") {
            fragment = extractCHTLScriptBlock(source, pos);
        } else {
            // 默认检测
            if (isCHTLElementBoundary(source, pos)) {
                fragment = extractCHTLElement(source, pos);
            } else if (isCSSBoundary(source, pos)) {
                fragment = extractCHTLStyleBlock(source, pos);
            } else if (isJSBoundary(source, pos)) {
                fragment = extractCHTLScriptBlock(source, pos);
            }
        }
        
        if (fragment.extended_type != ExtendedCodeType::UNKNOWN_EXT) {
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanCHTLSyntax(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isCHTLElementBoundary(source, pos)) {
            auto fragment = extractCHTLElement(source, pos);
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else if (isCHTLTemplateBoundary(source, pos)) {
            auto fragment = extractCHTLTemplate(source, pos);
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else if (isCHTLCustomBoundary(source, pos)) {
            auto fragment = extractCHTLCustom(source, pos);
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanCHTLJSSyntax(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    
    // 扫描 {{&}} 模式
    std::regex chtljs_regex(R"(\{\{&\}\})");
    std::sregex_iterator iter(source.begin(), source.end(), chtljs_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        ExtendedCodeFragment fragment;
        fragment.extended_type = ExtendedCodeType::CHTL_JS_SPECIAL;
        fragment.type = CodeType::CHTL_JS;
        fragment.start_pos = iter->position();
        fragment.end_pos = iter->position() + iter->length();
        fragment.content = iter->str();
        fragment.line = getLineNumber(source, fragment.start_pos);
        fragment.column = getColumnNumber(source, fragment.start_pos);
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanLocalStyleBlocks(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isCHTLStyleBlockBoundary(source, pos)) {
            auto fragment = extractCHTLStyleBlock(source, pos);
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanLocalScriptBlocks(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isCHTLScriptBlockBoundary(source, pos)) {
            auto fragment = extractCHTLScriptBlock(source, pos);
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanTemplateSyntax(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    
    // 扫描 {{ ... }} 模式
    std::regex template_regex(R"(\{\{[^&][^}]*\}\})");
    std::sregex_iterator iter(source.begin(), source.end(), template_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        ExtendedCodeFragment fragment;
        fragment.extended_type = ExtendedCodeType::CHTL_TEMPLATE;
        fragment.type = CodeType::CHTL;
        fragment.start_pos = iter->position();
        fragment.end_pos = iter->position() + iter->length();
        fragment.content = iter->str();
        fragment.line = getLineNumber(source, fragment.start_pos);
        fragment.column = getColumnNumber(source, fragment.start_pos);
        
        // 分析模板变量类型
        std::string inner = fragment.content.substr(2, fragment.content.length() - 4);
        if (inner.find("->") != std::string::npos) {
            fragment.attributes["type"] = "style_group_reference";
        } else if (inner.find("<") != std::string::npos) {
            fragment.attributes["type"] = "template_specialization";
        } else {
            fragment.attributes["type"] = "template_variable";
        }
        
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanCustomSyntax(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    size_t pos = 0;
    
    while (pos < source.length()) {
        if (isCHTLCustomBoundary(source, pos)) {
            auto fragment = extractCHTLCustom(source, pos);
            fragments.push_back(fragment);
            pos = fragment.end_pos;
        } else {
            pos++;
        }
    }
    
    return fragments;
}

std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::scanSpecialSyntax(const std::string& source) {
    std::vector<ExtendedCodeFragment> fragments;
    
    // 扫描原始嵌入 {{& ... &}}
    std::regex origin_regex(R"(\{\{&.*?&\}\})");
    std::sregex_iterator iter(source.begin(), source.end(), origin_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        ExtendedCodeFragment fragment;
        fragment.extended_type = ExtendedCodeType::CHTL_ORIGIN;
        fragment.type = CodeType::CHTL;
        fragment.start_pos = iter->position();
        fragment.end_pos = iter->position() + iter->length();
        fragment.content = iter->str();
        fragment.line = getLineNumber(source, fragment.start_pos);
        fragment.column = getColumnNumber(source, fragment.start_pos);
        
        // 提取内部内容
        std::string inner = fragment.content.substr(3, fragment.content.length() - 6);
        fragment.attributes["raw_content"] = inner;
        
        fragments.push_back(fragment);
    }
    
    return fragments;
}

std::string UnifiedScannerComplete::createPlaceholder(const ExtendedCodeFragment& fragment) {
    std::string id = generateUniqueId();
    std::string placeholder = placeholder_config_.prefix + id + placeholder_config_.suffix;
    
    // 存储片段
    placeholder_map_[placeholder] = fragment;
    
    return placeholder;
}

std::string UnifiedScannerComplete::createPlaceholder(const std::string& content, ExtendedCodeType type) {
    ExtendedCodeFragment fragment;
    fragment.content = content;
    fragment.extended_type = type;
    fragment.is_placeholder = true;
    
    return createPlaceholder(fragment);
}

bool UnifiedScannerComplete::isPlaceholder(const std::string& text) const {
    return text.find(placeholder_config_.prefix) == 0 && 
           text.find(placeholder_config_.suffix) == text.length() - placeholder_config_.suffix.length();
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::resolvePlaceholder(const std::string& placeholder) const {
    auto it = placeholder_map_.find(placeholder);
    if (it != placeholder_map_.end()) {
        return it->second;
    }
    
    ExtendedCodeFragment empty;
    empty.extended_type = ExtendedCodeType::UNKNOWN_EXT;
    return empty;
}

std::string UnifiedScannerComplete::replacePlaceholders(const std::string& text) {
    std::string result = text;
    
    for (const auto& pair : placeholder_map_) {
        const std::string& placeholder = pair.first;
        const ExtendedCodeFragment& fragment = pair.second;
        
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), fragment.content);
            pos = result.find(placeholder, pos + fragment.content.length());
        }
    }
    
    return result;
}

std::string UnifiedScannerComplete::restorePlaceholders(const std::string& text) {
    return replacePlaceholders(text);
}

// 边界检测方法实现
bool UnifiedScannerComplete::isCHTLElementBoundary(const std::string& source, size_t pos) const {
    if (pos >= source.length()) return false;
    
    // 检测HTML元素开始
    if (std::isalpha(source[pos])) {
        size_t end = pos;
        while (end < source.length() && (std::isalnum(source[end]) || source[end] == '-')) {
            end++;
        }
        
        // 跳过空白
        while (end < source.length() && std::isspace(source[end])) {
            end++;
        }
        
        return end < source.length() && source[end] == '{';
    }
    
    return false;
}

bool UnifiedScannerComplete::isCHTLTemplateBoundary(const std::string& source, size_t pos) const {
    return matchKeyword(source, pos, "template");
}

bool UnifiedScannerComplete::isCHTLCustomBoundary(const std::string& source, size_t pos) const {
    return matchKeyword(source, pos, "custom");
}

bool UnifiedScannerComplete::isCHTLNamespaceBoundary(const std::string& source, size_t pos) const {
    return matchKeyword(source, pos, "namespace");
}

bool UnifiedScannerComplete::isCHTLConfigBoundary(const std::string& source, size_t pos) const {
    return matchKeyword(source, pos, "config");
}

bool UnifiedScannerComplete::isCHTLImportBoundary(const std::string& source, size_t pos) const {
    return matchKeyword(source, pos, "@Html") || 
           matchKeyword(source, pos, "@Style") ||
           matchKeyword(source, pos, "@JavaScript") ||
           matchKeyword(source, pos, "@Chtl") ||
           matchKeyword(source, pos, "@CJmod");
}

bool UnifiedScannerComplete::isCHTLOriginBoundary(const std::string& source, size_t pos) const {
    return pos + 2 < source.length() && source.substr(pos, 3) == "{{&";
}

bool UnifiedScannerComplete::isCHTLStyleBlockBoundary(const std::string& source, size_t pos) const {
    return matchKeyword(source, pos, "style") || 
           (pos + 6 < source.length() && source.substr(pos, 7) == "<style>");
}

bool UnifiedScannerComplete::isCHTLScriptBlockBoundary(const std::string& source, size_t pos) const {
    return matchKeyword(source, pos, "script") || 
           (pos + 7 < source.length() && source.substr(pos, 8) == "<script>");
}

bool UnifiedScannerComplete::isCHTLJSSpecialBoundary(const std::string& source, size_t pos) const {
    return pos + 3 < source.length() && source.substr(pos, 4) == "{{&}";
}

// 内容提取方法实现
UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLElement(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_ELEMENT;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos;
    
    // 提取元素名
    size_t name_end = pos;
    while (name_end < source.length() && (std::isalnum(source[name_end]) || source[name_end] == '-')) {
        name_end++;
    }
    
    std::string element_name = source.substr(pos, name_end - pos);
    fragment.attributes["element_name"] = element_name;
    
    // 跳过空白
    pos = name_end;
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 找到匹配的右括号
    if (pos < source.length() && source[pos] == '{') {
        size_t end_pos = findMatchingDelimiter(source, pos, '{', '}');
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
            
            // 解析属性
            std::string attr_content = source.substr(name_end, pos - name_end);
            fragment.attributes = parseAttributes(attr_content);
        } else {
            addSyntaxError("Unmatched brace in CHTL element", fragment.line, fragment.column);
            fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
        }
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLTemplate(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_TEMPLATE;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos + 8; // skip "template"
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 提取模板名
    size_t name_start = pos;
    while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
        pos++;
    }
    
    std::string template_name = source.substr(name_start, pos - name_start);
    fragment.attributes["template_name"] = template_name;
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 找到匹配的右括号
    if (pos < source.length() && source[pos] == '{') {
        size_t end_pos = findMatchingDelimiter(source, pos, '{', '}');
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            addSyntaxError("Unmatched brace in CHTL template", fragment.line, fragment.column);
            fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
        }
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLCustom(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_CUSTOM;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos + 6; // skip "custom"
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 提取自定义名
    size_t name_start = pos;
    while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
        pos++;
    }
    
    std::string custom_name = source.substr(name_start, pos - name_start);
    fragment.attributes["custom_name"] = custom_name;
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 找到匹配的右括号
    if (pos < source.length() && source[pos] == '{') {
        size_t end_pos = findMatchingDelimiter(source, pos, '{', '}');
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            addSyntaxError("Unmatched brace in CHTL custom", fragment.line, fragment.column);
            fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
        }
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLNamespace(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_NAMESPACE;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos + 9; // skip "namespace"
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 提取命名空间名
    size_t name_start = pos;
    while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
        pos++;
    }
    
    std::string namespace_name = source.substr(name_start, pos - name_start);
    fragment.attributes["namespace_name"] = namespace_name;
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 找到匹配的右括号
    if (pos < source.length() && source[pos] == '{') {
        size_t end_pos = findMatchingDelimiter(source, pos, '{', '}');
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            addSyntaxError("Unmatched brace in CHTL namespace", fragment.line, fragment.column);
            fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
        }
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLConfig(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_CONFIG;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos + 6; // skip "config"
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 提取配置名
    size_t name_start = pos;
    while (pos < source.length() && (std::isalnum(source[pos]) || source[pos] == '_')) {
        pos++;
    }
    
    std::string config_name = source.substr(name_start, pos - name_start);
    fragment.attributes["config_name"] = config_name;
    
    // 跳过空白
    while (pos < source.length() && std::isspace(source[pos])) {
        pos++;
    }
    
    // 找到匹配的右括号
    if (pos < source.length() && source[pos] == '{') {
        size_t end_pos = findMatchingDelimiter(source, pos, '{', '}');
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 1;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
        } else {
            addSyntaxError("Unmatched brace in CHTL config", fragment.line, fragment.column);
            fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
        }
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLImport(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_IMPORT;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    // 找到行尾
    size_t pos = start_pos;
    while (pos < source.length() && source[pos] != '\n' && source[pos] != '\r') {
        pos++;
    }
    
    fragment.end_pos = pos;
    fragment.content = source.substr(start_pos, pos - start_pos);
    
    // 解析导入类型和路径
    if (fragment.content.find("@Html") == 0) {
        fragment.attributes["import_type"] = "html";
    } else if (fragment.content.find("@Style") == 0) {
        fragment.attributes["import_type"] = "css";
    } else if (fragment.content.find("@JavaScript") == 0) {
        fragment.attributes["import_type"] = "js";
    } else if (fragment.content.find("@Chtl") == 0) {
        fragment.attributes["import_type"] = "chtl";
    } else if (fragment.content.find("@CJmod") == 0) {
        fragment.attributes["import_type"] = "cjmod";
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLOrigin(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_ORIGIN;
    fragment.type = CodeType::CHTL;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    // 找到 &}}
    size_t end_pos = source.find("&}}", start_pos + 3);
    if (end_pos != std::string::npos) {
        fragment.end_pos = end_pos + 3;
        fragment.content = source.substr(start_pos, end_pos - start_pos + 3);
        
        // 提取内部内容
        std::string inner = fragment.content.substr(3, fragment.content.length() - 6);
        fragment.attributes["raw_content"] = inner;
    } else {
        addSyntaxError("Unclosed origin block", fragment.line, fragment.column);
        fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLStyleBlock(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_STYLE_LOCAL;
    fragment.type = CodeType::CSS;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos;
    
    if (source.substr(pos, 7) == "<style>") {
        // 全局样式块
        pos += 7;
        size_t end_pos = source.find("</style>", pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 8;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 8);
            fragment.extended_type = ExtendedCodeType::CSS_GLOBAL;
        } else {
            addSyntaxError("Unclosed global style block", fragment.line, fragment.column);
            fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
        }
    } else if (source.substr(pos, 5) == "style") {
        // 局部样式块
        pos += 5;
        while (pos < source.length() && std::isspace(source[pos])) {
            pos++;
        }
        
        if (pos < source.length() && source[pos] == '{') {
            size_t end_pos = findMatchingDelimiter(source, pos, '{', '}');
            if (end_pos != std::string::npos) {
                fragment.end_pos = end_pos + 1;
                fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
            } else {
                addSyntaxError("Unmatched brace in local style block", fragment.line, fragment.column);
                fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
            }
        }
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLScriptBlock(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_SCRIPT_LOCAL;
    fragment.type = CodeType::JS;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    
    size_t pos = start_pos;
    
    if (source.substr(pos, 8) == "<script>") {
        // 全局脚本块
        pos += 8;
        size_t end_pos = source.find("</script>", pos);
        if (end_pos != std::string::npos) {
            fragment.end_pos = end_pos + 9;
            fragment.content = source.substr(start_pos, end_pos - start_pos + 9);
            fragment.extended_type = ExtendedCodeType::JS_GLOBAL;
        } else {
            addSyntaxError("Unclosed global script block", fragment.line, fragment.column);
            fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
        }
    } else if (source.substr(pos, 6) == "script") {
        // 局部脚本块
        pos += 6;
        while (pos < source.length() && std::isspace(source[pos])) {
            pos++;
        }
        
        if (pos < source.length() && source[pos] == '{') {
            size_t end_pos = findMatchingDelimiter(source, pos, '{', '}');
            if (end_pos != std::string::npos) {
                fragment.end_pos = end_pos + 1;
                fragment.content = source.substr(start_pos, end_pos - start_pos + 1);
            } else {
                addSyntaxError("Unmatched brace in local script block", fragment.line, fragment.column);
                fragment.extended_type = ExtendedCodeType::UNKNOWN_EXT;
            }
        }
    }
    
    return fragment;
}

UnifiedScannerComplete::ExtendedCodeFragment UnifiedScannerComplete::extractCHTLJSSpecial(const std::string& source, size_t start_pos) {
    ExtendedCodeFragment fragment;
    fragment.extended_type = ExtendedCodeType::CHTL_JS_SPECIAL;
    fragment.type = CodeType::CHTL_JS;
    fragment.start_pos = start_pos;
    fragment.line = getLineNumber(source, start_pos);
    fragment.column = getColumnNumber(source, start_pos);
    fragment.end_pos = start_pos + 4; // {{&}}
    fragment.content = source.substr(start_pos, 4);
    
    return fragment;
}

// 属性解析方法实现
std::map<std::string, std::string> UnifiedScannerComplete::parseAttributes(const std::string& attr_string) {
    std::map<std::string, std::string> attributes;
    
    // 简单的属性解析
    std::regex attr_regex(R"((\w+):\s*([^;]+);?)");
    std::sregex_iterator iter(attr_string.begin(), attr_string.end(), attr_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = iter->str(1);
        std::string value = iter->str(2);
        
        // 去除引号
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        } else if (value.front() == '\'' && value.back() == '\'') {
            value = value.substr(1, value.length() - 2);
        }
        
        attributes[key] = value;
    }
    
    return attributes;
}

std::vector<std::string> UnifiedScannerComplete::parseStyleGroups(const std::string& style_string) {
    std::vector<std::string> groups;
    
    std::regex group_regex(R"(\{\{([^}]+)\}\})");
    std::sregex_iterator iter(style_string.begin(), style_string.end(), group_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        groups.push_back(iter->str(1));
    }
    
    return groups;
}

std::map<std::string, std::string> UnifiedScannerComplete::parseStyleProperties(const std::string& style_string) {
    std::map<std::string, std::string> properties;
    
    std::regex prop_regex(R"(([^:]+):\s*([^;]+);?)");
    std::sregex_iterator iter(style_string.begin(), style_string.end(), prop_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string property = iter->str(1);
        std::string value = iter->str(2);
        
        // 去除前后空白
        property.erase(0, property.find_first_not_of(" \t"));
        property.erase(property.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        properties[property] = value;
    }
    
    return properties;
}

std::vector<std::string> UnifiedScannerComplete::parseInheritanceList(const std::string& inherit_string) {
    std::vector<std::string> inheritance;
    
    std::regex inherit_regex(R"(inherit\(([^)]+)\))");
    std::sregex_iterator iter(inherit_string.begin(), inherit_string.end(), inherit_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string inherited = iter->str(1);
        
        // 分割多个继承
        std::regex comma_regex(R"(\s*,\s*)");
        std::sregex_token_iterator token_iter(inherited.begin(), inherited.end(), comma_regex, -1);
        std::sregex_token_iterator token_end;
        
        for (; token_iter != token_end; ++token_iter) {
            inheritance.push_back(token_iter->str());
        }
    }
    
    return inheritance;
}

// 嵌套处理方法实现
std::vector<UnifiedScannerComplete::ExtendedCodeFragment> UnifiedScannerComplete::extractNestedFragments(const ExtendedCodeFragment& parent) {
    std::vector<ExtendedCodeFragment> children;
    
    if (parent.content.empty()) return children;
    
    // 找到内容开始位置（跳过开头的标记）
    size_t content_start = parent.content.find('{');
    if (content_start == std::string::npos) return children;
    
    content_start++; // 跳过 '{'
    
    // 找到内容结束位置
    size_t content_end = parent.content.rfind('}');
    if (content_end == std::string::npos || content_end <= content_start) return children;
    
    std::string inner_content = parent.content.substr(content_start, content_end - content_start);
    
    // 递归扫描内部内容
    auto nested_fragments = scanExtended(inner_content);
    
    // 调整位置偏移
    for (auto& fragment : nested_fragments) {
        fragment.start_pos += parent.start_pos + content_start;
        fragment.end_pos += parent.start_pos + content_start;
        fragment.depth_level = parent.depth_level + 1;
    }
    
    return nested_fragments;
}

int UnifiedScannerComplete::calculateNestingDepth(const std::string& source, size_t pos) const {
    int depth = 0;
    
    for (size_t i = 0; i < pos && i < source.length(); i++) {
        if (source[i] == '{') {
            depth++;
        } else if (source[i] == '}') {
            depth--;
        }
    }
    
    return std::max(0, depth);
}

bool UnifiedScannerComplete::validateNestingRules(const ExtendedCodeFragment& fragment) const {
    // 验证嵌套规则
    if (fragment.depth_level > scan_config_.max_nesting_depth) {
        return false;
    }
    
    // 检查嵌套兼容性
    for (const auto& child : fragment.children) {
        if (!validateNestingRules(*child)) {
            return false;
        }
    }
    
    return true;
}

// 配置管理方法实现
void UnifiedScannerComplete::setPlaceholderConfig(const PlaceholderConfig& config) {
    placeholder_config_ = config;
}

UnifiedScannerComplete::PlaceholderConfig UnifiedScannerComplete::getPlaceholderConfig() const {
    return placeholder_config_;
}

void UnifiedScannerComplete::setScanConfig(const ScanConfig& config) {
    scan_config_ = config;
}

UnifiedScannerComplete::ScanConfig UnifiedScannerComplete::getScanConfig() const {
    return scan_config_;
}

// 错误处理方法实现
void UnifiedScannerComplete::setErrorRecoveryMode(bool enable) {
    error_recovery_mode_ = enable;
}

bool UnifiedScannerComplete::getErrorRecoveryMode() const {
    return error_recovery_mode_;
}

std::vector<std::string> UnifiedScannerComplete::getSyntaxErrors() const {
    return syntax_errors_;
}

void UnifiedScannerComplete::clearSyntaxErrors() {
    syntax_errors_.clear();
}

bool UnifiedScannerComplete::recoverFromSyntaxError(const std::string& source, size_t& pos) {
    // 简单的错误恢复：寻找下一个可能的语法边界
    size_t next_boundary = findNextSyntaxBoundary(source, pos);
    if (next_boundary != std::string::npos) {
        pos = next_boundary;
        return true;
    }
    
    return false;
}

// 统计信息方法实现
size_t UnifiedScannerComplete::getFragmentCount() const {
    size_t total = 0;
    for (const auto& pair : fragment_statistics_) {
        total += pair.second;
    }
    return total;
}

size_t UnifiedScannerComplete::getPlaceholderCount() const {
    return placeholder_map_.size();
}

std::map<UnifiedScannerComplete::ExtendedCodeType, size_t> UnifiedScannerComplete::getFragmentStatistics() const {
    return fragment_statistics_;
}

std::string UnifiedScannerComplete::getScannerStatistics() const {
    std::stringstream ss;
    ss << "Scanner Statistics:\n";
    ss << "Total Fragments: " << getFragmentCount() << "\n";
    ss << "Placeholders: " << getPlaceholderCount() << "\n";
    ss << "Syntax Errors: " << syntax_errors_.size() << "\n";
    
    ss << "\nFragment Types:\n";
    for (const auto& pair : fragment_statistics_) {
        ss << "  " << static_cast<int>(pair.first) << ": " << pair.second << "\n";
    }
    
    return ss.str();
}

// 工具方法实现
std::string UnifiedScannerComplete::generateUniqueId() const {
    return std::to_string(next_placeholder_id_++);
}

std::string UnifiedScannerComplete::normalizeWhitespace(const std::string& text) const {
    if (!placeholder_config_.preserve_whitespace) {
        return std::regex_replace(text, std::regex(R"(\s+)"), " ");
    }
    return text;
}

std::string UnifiedScannerComplete::preserveIndentation(const std::string& text, const std::string& source, size_t pos) const {
    if (!placeholder_config_.preserve_indentation) {
        return text;
    }
    
    // 找到当前行的缩进
    size_t line_start = pos;
    while (line_start > 0 && source[line_start - 1] != '\n') {
        line_start--;
    }
    
    std::string indentation;
    for (size_t i = line_start; i < source.length() && (source[i] == ' ' || source[i] == '\t'); i++) {
        indentation += source[i];
    }
    
    // 添加缩进到每一行
    std::string result = text;
    size_t pos_nl = result.find('\n');
    while (pos_nl != std::string::npos) {
        result.insert(pos_nl + 1, indentation);
        pos_nl = result.find('\n', pos_nl + 1 + indentation.length());
    }
    
    return result;
}

// 调试和验证方法实现
void UnifiedScannerComplete::enableVerboseMode(bool enable) {
    verbose_mode_ = enable;
}

bool UnifiedScannerComplete::isVerboseMode() const {
    return verbose_mode_;
}

void UnifiedScannerComplete::dumpFragments(const std::vector<ExtendedCodeFragment>& fragments) const {
    if (!verbose_mode_) return;
    
    std::cout << "=== Fragment Dump ===" << std::endl;
    for (size_t i = 0; i < fragments.size(); i++) {
        const auto& frag = fragments[i];
        std::cout << "Fragment " << i << ":" << std::endl;
        std::cout << "  Type: " << static_cast<int>(frag.extended_type) << std::endl;
        std::cout << "  Position: " << frag.start_pos << "-" << frag.end_pos << std::endl;
        std::cout << "  Line: " << frag.line << ", Column: " << frag.column << std::endl;
        std::cout << "  Depth: " << frag.depth_level << std::endl;
        std::cout << "  Content: " << frag.content.substr(0, 50);
        if (frag.content.length() > 50) std::cout << "...";
        std::cout << std::endl;
        std::cout << "  Children: " << frag.children.size() << std::endl;
        std::cout << std::endl;
    }
}

bool UnifiedScannerComplete::validateFragmentIntegrity(const std::vector<ExtendedCodeFragment>& fragments) const {
    // 验证片段完整性
    for (const auto& fragment : fragments) {
        if (fragment.start_pos >= fragment.end_pos) {
            return false;
        }
        
        if (!validateNestingRules(fragment)) {
            return false;
        }
    }
    
    return true;
}

// 私有辅助方法实现
std::regex UnifiedScannerComplete::getRegex(const std::string& pattern) const {
    auto it = regex_cache_.find(pattern);
    if (it != regex_cache_.end()) {
        return it->second;
    }
    
    std::regex regex_obj(pattern);
    regex_cache_[pattern] = regex_obj;
    return regex_obj;
}

size_t UnifiedScannerComplete::findMatchingDelimiter(const std::string& source, size_t start_pos, char open_char, char close_char) const {
    int count = 0;
    size_t pos = start_pos;
    
    while (pos < source.length()) {
        if (source[pos] == open_char) {
            count++;
        } else if (source[pos] == close_char) {
            count--;
            if (count == 0) {
                return pos;
            }
        } else if (source[pos] == '"' || source[pos] == '\'') {
            // 跳过字符串字面量
            pos = skipString(source, pos, source[pos]);
            continue;
        }
        pos++;
    }
    
    return std::string::npos;
}

size_t UnifiedScannerComplete::findNextSyntaxBoundary(const std::string& source, size_t start_pos) const {
    for (size_t pos = start_pos; pos < source.length(); pos++) {
        if (isCHTLElementBoundary(source, pos) ||
            isCHTLTemplateBoundary(source, pos) ||
            isCHTLCustomBoundary(source, pos) ||
            isCHTLStyleBlockBoundary(source, pos) ||
            isCHTLScriptBlockBoundary(source, pos)) {
            return pos;
        }
    }
    
    return std::string::npos;
}

size_t UnifiedScannerComplete::skipWhitespaceAndComments(const std::string& source, size_t pos) const {
    while (pos < source.length()) {
        if (std::isspace(source[pos])) {
            pos++;
        } else if (pos + 1 < source.length() && source.substr(pos, 2) == "//") {
            // 跳过单行注释
            while (pos < source.length() && source[pos] != '\n') {
                pos++;
            }
        } else if (pos + 1 < source.length() && source.substr(pos, 2) == "/*") {
            // 跳过多行注释
            pos = source.find("*/", pos + 2);
            if (pos == std::string::npos) {
                break;
            }
            pos += 2;
        } else {
            break;
        }
    }
    
    return pos;
}

size_t UnifiedScannerComplete::skipString(const std::string& source, size_t pos, char quote_char) const {
    if (pos >= source.length() || source[pos] != quote_char) {
        return pos;
    }
    
    pos++; // 跳过开始引号
    
    while (pos < source.length()) {
        if (source[pos] == quote_char) {
            return pos + 1; // 跳过结束引号
        } else if (source[pos] == '\\') {
            pos += 2; // 跳过转义字符
        } else {
            pos++;
        }
    }
    
    return pos;
}

bool UnifiedScannerComplete::matchKeyword(const std::string& source, size_t pos, const std::string& keyword) const {
    if (pos + keyword.length() > source.length()) {
        return false;
    }
    
    if (source.substr(pos, keyword.length()) != keyword) {
        return false;
    }
    
    // 检查边界
    size_t end_pos = pos + keyword.length();
    if (end_pos < source.length()) {
        char next_char = source[end_pos];
        if (std::isalnum(next_char) || next_char == '_') {
            return false;
        }
    }
    
    return true;
}

bool UnifiedScannerComplete::matchPattern(const std::string& source, size_t pos, const std::string& pattern) const {
    auto regex_obj = getRegex(pattern);
    std::smatch match;
    std::string substr = source.substr(pos);
    
    if (std::regex_search(substr, match, regex_obj)) {
        return match.position() == 0;
    }
    
    return false;
}

std::string UnifiedScannerComplete::extractUntilDelimiter(const std::string& source, size_t start_pos, const std::string& delimiter) const {
    size_t end_pos = source.find(delimiter, start_pos);
    if (end_pos == std::string::npos) {
        return source.substr(start_pos);
    }
    
    return source.substr(start_pos, end_pos - start_pos);
}

std::string UnifiedScannerComplete::extractBetweenDelimiters(const std::string& source, size_t start_pos, const std::string& start_delim, const std::string& end_delim) const {
    size_t content_start = source.find(start_delim, start_pos);
    if (content_start == std::string::npos) {
        return "";
    }
    
    content_start += start_delim.length();
    
    size_t content_end = source.find(end_delim, content_start);
    if (content_end == std::string::npos) {
        return source.substr(content_start);
    }
    
    return source.substr(content_start, content_end - content_start);
}

std::string UnifiedScannerComplete::analyzeContext(const std::string& source, size_t pos) const {
    if (isInCHTLBlock(source, pos)) {
        return "chtl_block";
    } else if (isInCHTLJSBlock(source, pos)) {
        return "chtljs_block";
    } else if (isInStringLiteral(source, pos)) {
        return "string_literal";
    } else if (isInComment(source, pos)) {
        return "comment";
    }
    
    return "unknown";
}

bool UnifiedScannerComplete::isInStringLiteral(const std::string& source, size_t pos) const {
    // 简单的字符串字面量检测
    size_t quote_pos = pos;
    while (quote_pos > 0) {
        quote_pos--;
        if (source[quote_pos] == '"' || source[quote_pos] == '\'') {
            char quote_char = source[quote_pos];
            size_t end_quote = source.find(quote_char, quote_pos + 1);
            return end_quote != std::string::npos && pos <= end_quote;
        }
    }
    
    return false;
}

bool UnifiedScannerComplete::isInComment(const std::string& source, size_t pos) const {
    // 检查是否在注释中
    size_t line_start = pos;
    while (line_start > 0 && source[line_start - 1] != '\n') {
        line_start--;
    }
    
    std::string line = source.substr(line_start, pos - line_start);
    return line.find("//") != std::string::npos;
}

bool UnifiedScannerComplete::isInCHTLBlock(const std::string& source, size_t pos) const {
    return calculateNestingDepth(source, pos) > 0;
}

bool UnifiedScannerComplete::isInCHTLJSBlock(const std::string& source, size_t pos) const {
    // 检查是否在CHTL JS块中
    // 这里可以添加更复杂的逻辑
    return false;
}

void UnifiedScannerComplete::addSyntaxError(const std::string& error, int line, int column) {
    std::stringstream ss;
    ss << error;
    if (line > 0) {
        ss << " at line " << line;
    }
    if (column > 0) {
        ss << ", column " << column;
    }
    
    syntax_errors_.push_back(ss.str());
    
    if (verbose_mode_) {
        std::cerr << "[SYNTAX ERROR] " << ss.str() << std::endl;
    }
}

void UnifiedScannerComplete::logVerbose(const std::string& message) const {
    if (verbose_mode_) {
        std::cout << "[VERBOSE] " << message << std::endl;
    }
}

void UnifiedScannerComplete::initializeRegexCache() {
    // 初始化常用的正则表达式
    regex_cache_["template_var"] = std::regex(R"(\{\{[^&][^}]*\}\})");
    regex_cache_["origin_embed"] = std::regex(R"(\{\{&.*?&\}\})");
    regex_cache_["chtljs_special"] = std::regex(R"(\{\{&\}\})");
    regex_cache_["responsive_var"] = std::regex(R"(\$\w+\$)");
    regex_cache_["style_property"] = std::regex(R"(([^:]+):\s*([^;]+);?)");
    regex_cache_["attribute"] = std::regex(R"((\w+):\s*([^;]+);?)");
}

void UnifiedScannerComplete::initializeDefaultConfig() {
    // 初始化默认配置
    placeholder_config_ = PlaceholderConfig();
    scan_config_ = ScanConfig();
    
    error_recovery_mode_ = true;
    verbose_mode_ = false;
}

} // namespace CHTL