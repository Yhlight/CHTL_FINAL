#include "CHTLCodeMerger.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <regex>

namespace CHTL {

// CHTLCodeMerger implementation
CHTLCodeMerger::CHTLCodeMerger() : debug_mode(false), verbose(false) {
}

CHTLCodeMerger::~CHTLCodeMerger() {
}

MergeResult CHTLCodeMerger::merge(const std::vector<CodeFragment>& fragments, const MergeConfig& config) {
    MergeResult result;
    
    try {
        if (fragments.empty()) {
            result.success = false;
            result.error_message = "No fragments to merge";
            return result;
        }
        
        // Determine merge strategy
        FragmentType type = FragmentType::Mixed;
        if (fragments.size() == 1) {
            type = fragments[0].type;
        } else {
            // Check if all fragments are the same type
            bool all_same_type = true;
            FragmentType first_type = fragments[0].type;
            for (const auto& fragment : fragments) {
                if (fragment.type != first_type) {
                    all_same_type = false;
                    break;
                }
            }
            if (all_same_type) {
                type = first_type;
            }
        }
        
        // Merge based on type
        switch (type) {
            case FragmentType::HTML:
                result.merged_content = mergeHTMLFragments(fragments, config);
                break;
            case FragmentType::CSS:
                result.merged_content = mergeCSSFragments(fragments, config);
                break;
            case FragmentType::JavaScript:
                result.merged_content = mergeJavaScriptFragments(fragments, config);
                break;
            case FragmentType::CHTL:
                result.merged_content = mergeCHTLFragments(fragments, config);
                break;
            case FragmentType::CHTLJS:
                result.merged_content = mergeCHTLJSFragments(fragments, config);
                break;
            default:
                result.merged_content = mergeMixedFragments(fragments, config);
                break;
        }
        
        result.success = true;
        
    } catch (const std::exception& e) {
        result.success = false;
        result.error_message = "Merge error: " + std::string(e.what());
    }
    
    return result;
}

MergeResult CHTLCodeMerger::merge(const std::vector<CodeFragment>& fragments, MergeStrategy strategy) {
    MergeConfig config;
    config.strategy = strategy;
    return merge(fragments, config);
}

MergeResult CHTLCodeMerger::merge(const std::vector<std::string>& files, FragmentType type, const MergeConfig& config) {
    std::vector<CodeFragment> fragments;
    
    for (const auto& file : files) {
        std::string content = readFile(file);
        if (!content.empty()) {
            fragments.push_back(CodeFragment(type, content, file));
        }
    }
    
    return merge(fragments, config);
}

void CHTLCodeMerger::setDebugMode(bool enabled) {
    debug_mode = enabled;
}

void CHTLCodeMerger::setVerboseMode(bool enabled) {
    verbose = enabled;
}

FragmentType CHTLCodeMerger::detectFragmentType(const std::string& content) const {
    // Simple content-based type detection
    if (content.find("<html") != std::string::npos || content.find("<body") != std::string::npos) {
        return FragmentType::HTML;
    }
    if (content.find("color:") != std::string::npos || content.find("font-size:") != std::string::npos) {
        return FragmentType::CSS;
    }
    if (content.find("function") != std::string::npos || content.find("var ") != std::string::npos) {
        return FragmentType::JavaScript;
    }
    if (content.find("text {") != std::string::npos || content.find("div {") != std::string::npos) {
        return FragmentType::CHTL;
    }
    if (content.find("fileloader {") != std::string::npos || content.find("script {") != std::string::npos) {
        return FragmentType::CHTLJS;
    }
    
    return FragmentType::Mixed;
}

// Helper function to detect fragment type from file path
FragmentType detectFragmentTypeFromPath(const std::string& file_path) {
    size_t pos = file_path.find_last_of('.');
    if (pos == std::string::npos) {
        return FragmentType::Mixed;
    }
    
    std::string extension = file_path.substr(pos);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".html" || extension == ".htm") {
        return FragmentType::HTML;
    }
    if (extension == ".css") {
        return FragmentType::CSS;
    }
    if (extension == ".js") {
        return FragmentType::JavaScript;
    }
    if (extension == ".chtl") {
        return FragmentType::CHTL;
    }
    if (extension == ".cjjs") {
        return FragmentType::CHTLJS;
    }
    
    return FragmentType::Mixed;
}

CodeFragment CHTLCodeMerger::createFragment(const std::string& content, FragmentType type, const std::string& source_file) {
    return CodeFragment(type, content, source_file);
}

std::vector<CodeFragment> CHTLCodeMerger::createFragmentsFromFiles(const std::vector<std::string>& files) {
    std::vector<CodeFragment> fragments;
    
    for (const auto& file : files) {
        std::string content = readFile(file);
        if (!content.empty()) {
            FragmentType type = detectFragmentTypeFromPath(file);
            fragments.push_back(CodeFragment(type, content, file));
        }
    }
    
    return fragments;
}

bool CHTLCodeMerger::validateFragment(const CodeFragment& fragment) const {
    return !fragment.content.empty();
}

bool CHTLCodeMerger::validateFragments(const std::vector<CodeFragment>& fragments) const {
    for (const auto& fragment : fragments) {
        if (!validateFragment(fragment)) {
            return false;
        }
    }
    return true;
}

// Private methods
std::string CHTLCodeMerger::mergeHTMLFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& /* config */) {
    std::string merged_html = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::HTML) {
            merged_html += fragment.content + "\n";
        }
    }
    
    return merged_html;
}

std::string CHTLCodeMerger::mergeCSSFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config) {
    std::string merged_css = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CSS) {
            std::string css = fragment.content;
            
            if (config.remove_comments) {
                css = removeComments(css);
            }
            
            if (config.remove_whitespace) {
                css = removeWhitespace(css);
            }
            
            merged_css += css + "\n";
        }
    }
    
    if (config.minify) {
        merged_css = minifyCSS(merged_css);
    }
    
    return merged_css;
}

std::string CHTLCodeMerger::mergeJavaScriptFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& config) {
    std::string merged_js = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::JavaScript) {
            std::string js = fragment.content;
            
            if (config.remove_comments) {
                js = removeComments(js);
            }
            
            if (config.remove_whitespace) {
                js = removeWhitespace(js);
            }
            
            merged_js += js + ";\n";
        }
    }
    
    if (config.minify) {
        merged_js = minifyJavaScript(merged_js);
    }
    
    return merged_js;
}

std::string CHTLCodeMerger::mergeCHTLFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& /* config */) {
    std::string merged_chtl = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTL) {
            merged_chtl += fragment.content + "\n";
        }
    }
    
    return merged_chtl;
}

std::string CHTLCodeMerger::mergeCHTLJSFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& /* config */) {
    std::string merged_chtljs = "";
    
    for (const auto& fragment : fragments) {
        if (fragment.type == FragmentType::CHTLJS) {
            merged_chtljs += fragment.content + "\n";
        }
    }
    
    return merged_chtljs;
}

std::string CHTLCodeMerger::mergeMixedFragments(const std::vector<CodeFragment>& fragments, const MergeConfig& /* config */) {
    std::string merged_content = "";
    
    // Group by type
    std::map<FragmentType, std::string> type_contents;
    
    for (const auto& fragment : fragments) {
        type_contents[fragment.type] += fragment.content + "\n";
    }
    
    // Merge in order: HTML, CSS, JavaScript, CHTL, CHTLJS
    for (const auto& pair : type_contents) {
        merged_content += pair.second + "\n";
    }
    
    return merged_content;
}

std::string CHTLCodeMerger::minifyCSS(const std::string& css) {
    std::string result = css;
    
    // Remove comments
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // Remove extra whitespace
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // Remove whitespace around certain characters
    result = std::regex_replace(result, std::regex(R"(\s*{\s*)"), "{");
    result = std::regex_replace(result, std::regex(R"(\s*}\s*)"), "}");
    result = std::regex_replace(result, std::regex(R"(\s*;\s*)"), ";");
    result = std::regex_replace(result, std::regex(R"(\s*:\s*)"), ":");
    result = std::regex_replace(result, std::regex(R"(\s*,\s*)"), ",");
    
    return result;
}

std::string CHTLCodeMerger::minifyJavaScript(const std::string& js) {
    std::string result = js;
    
    // Remove single-line comments
    result = std::regex_replace(result, std::regex(R"(//.*$)"), "");
    
    // Remove multi-line comments
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // Remove extra whitespace
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    return result;
}

std::string CHTLCodeMerger::removeComments(const std::string& content) {
    std::string result = content;
    
    // Remove single-line comments
    result = std::regex_replace(result, std::regex(R"(//.*$)"), "");
    
    // Remove multi-line comments
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    return result;
}

std::string CHTLCodeMerger::removeWhitespace(const std::string& content) {
    return std::regex_replace(content, std::regex(R"(\s+)"), " ");
}

std::string CHTLCodeMerger::readFile(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLCodeMerger::writeFile(const std::string& file_path, const std::string& content) const {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::string CHTLCodeMerger::getFileExtension(const std::string& file_path) const {
    size_t pos = file_path.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return file_path.substr(pos);
}

bool CHTLCodeMerger::fileExists(const std::string& file_path) const {
    std::ifstream file(file_path);
    return file.good();
}

} // namespace CHTL