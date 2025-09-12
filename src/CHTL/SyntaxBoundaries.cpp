#include "CHTL/SyntaxBoundaries.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// SyntaxBoundaryDetector 实现
SyntaxBoundaryDetector::SyntaxBoundaryDetector() {
    initializeDefaultRules();
}

SyntaxBoundaryDetector::~SyntaxBoundaryDetector() = default;

SyntaxBoundaryType SyntaxBoundaryDetector::detectBoundaryType(const std::string& content, size_t position) const {
    for (const auto& pair : rules_) {
        if (matchesPattern(content, position, pair.second.start_pattern)) {
            return pair.first;
        }
    }
    return SyntaxBoundaryType::HTML;
}

std::vector<std::pair<size_t, size_t>> SyntaxBoundaryDetector::findBoundaries(const std::string& content) const {
    std::vector<std::pair<size_t, size_t>> boundaries;
    
    for (const auto& pair : rules_) {
        auto typeBoundaries = findBoundariesByType(content, pair.first);
        boundaries.insert(boundaries.end(), typeBoundaries.begin(), typeBoundaries.end());
    }
    
    // 按起始位置排序
    std::sort(boundaries.begin(), boundaries.end(),
        [](const std::pair<size_t, size_t>& a, const std::pair<size_t, size_t>& b) {
            return a.first < b.first;
        });
    
    return boundaries;
}

std::vector<std::pair<size_t, size_t>> SyntaxBoundaryDetector::findBoundariesByType(const std::string& content, SyntaxBoundaryType type) const {
    auto it = rules_.find(type);
    if (it == rules_.end()) {
        return {};
    }
    
    const auto& rule = it->second;
    std::vector<std::pair<size_t, size_t>> boundaries;
    
    size_t pos = 0;
    while (pos < content.length()) {
        size_t start = content.find(rule.start_pattern, pos);
        if (start == std::string::npos) {
            break;
        }
        
        size_t end = findPatternEnd(content, start, rule.end_pattern);
        if (end != std::string::npos) {
            boundaries.push_back({start, end});
            pos = end + 1;
        } else {
            pos = start + 1;
        }
    }
    
    return boundaries;
}

bool SyntaxBoundaryDetector::validateSyntax(const std::string& content, SyntaxBoundaryType type) const {
    auto it = rules_.find(type);
    if (it == rules_.end()) {
        return true;
    }
    
    const auto& rule = it->second;
    
    // 检查允许的语法
    for (size_t i = 0; i < content.length(); ++i) {
        if (!isAllowedSyntax(content, i, rule.allowed_syntax)) {
            if (rule.strict_mode) {
                return false;
            }
        }
        
        if (isForbiddenSyntax(content, i, rule.forbidden_syntax)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> SyntaxBoundaryDetector::getValidationErrors(const std::string& content, SyntaxBoundaryType type) const {
    std::vector<std::string> errors;
    
    auto it = rules_.find(type);
    if (it == rules_.end()) {
        return errors;
    }
    
    const auto& rule = it->second;
    
    // 检查允许的语法
    for (size_t i = 0; i < content.length(); ++i) {
        if (!isAllowedSyntax(content, i, rule.allowed_syntax)) {
            if (rule.strict_mode) {
                errors.push_back("Syntax not allowed at position " + std::to_string(i));
            }
        }
        
        if (isForbiddenSyntax(content, i, rule.forbidden_syntax)) {
            errors.push_back("Forbidden syntax at position " + std::to_string(i));
        }
    }
    
    return errors;
}

void SyntaxBoundaryDetector::addRule(const SyntaxBoundaryRule& rule) {
    rules_[rule.type] = rule;
}

void SyntaxBoundaryDetector::removeRule(SyntaxBoundaryType type) {
    rules_.erase(type);
}

const SyntaxBoundaryRule& SyntaxBoundaryDetector::getRule(SyntaxBoundaryType type) const {
    static SyntaxBoundaryRule defaultRule;
    auto it = rules_.find(type);
    if (it != rules_.end()) {
        return it->second;
    }
    return defaultRule;
}

const std::map<SyntaxBoundaryType, SyntaxBoundaryRule>& SyntaxBoundaryDetector::getRules() const {
    return rules_;
}

void SyntaxBoundaryDetector::clear() {
    rules_.clear();
}

bool SyntaxBoundaryDetector::matchesPattern(const std::string& content, size_t position, const std::string& pattern) const {
    if (position + pattern.length() > content.length()) {
        return false;
    }
    
    return content.substr(position, pattern.length()) == pattern;
}

size_t SyntaxBoundaryDetector::findPatternEnd(const std::string& content, size_t start, const std::string& pattern) const {
    size_t pos = content.find(pattern, start);
    if (pos != std::string::npos) {
        return pos + pattern.length() - 1;
    }
    return std::string::npos;
}

bool SyntaxBoundaryDetector::isAllowedSyntax(const std::string& content, size_t position, const std::set<std::string>& allowed) const {
    for (const auto& syntax : allowed) {
        if (matchesPattern(content, position, syntax)) {
            return true;
        }
    }
    return false;
}

bool SyntaxBoundaryDetector::isForbiddenSyntax(const std::string& content, size_t position, const std::set<std::string>& forbidden) const {
    for (const auto& syntax : forbidden) {
        if (matchesPattern(content, position, syntax)) {
            return true;
        }
    }
    return false;
}

void SyntaxBoundaryDetector::initializeDefaultRules() {
    // 全局样式块规则
    SyntaxBoundaryRule globalStyleRule;
    globalStyleRule.type = SyntaxBoundaryType::GLOBAL_STYLE;
    globalStyleRule.start_pattern = "<style>";
    globalStyleRule.end_pattern = "</style>";
    globalStyleRule.allowed_syntax = {
        "template", "custom", "import", "namespace", "from", "delete", "inherit",
        "{{", "}}", "/*", "*/", "//", "--", "@", "[", "]"
    };
    globalStyleRule.strict_mode = false;
    rules_[SyntaxBoundaryType::GLOBAL_STYLE] = globalStyleRule;
    
    // 局部样式块规则
    SyntaxBoundaryRule localStyleRule;
    localStyleRule.type = SyntaxBoundaryType::LOCAL_STYLE;
    localStyleRule.start_pattern = "style";
    localStyleRule.end_pattern = "}";
    localStyleRule.allowed_syntax = {
        "template", "custom", "import", "namespace", "from", "delete", "inherit",
        "{{", "}}", "/*", "*/", "//", "--", "@", "[", "]"
    };
    localStyleRule.strict_mode = false;
    rules_[SyntaxBoundaryType::LOCAL_STYLE] = localStyleRule;
    
    // 全局脚本块规则
    SyntaxBoundaryRule globalScriptRule;
    globalScriptRule.type = SyntaxBoundaryType::GLOBAL_SCRIPT;
    globalScriptRule.start_pattern = "<script>";
    globalScriptRule.end_pattern = "</script>";
    globalScriptRule.forbidden_syntax = {
        "template", "custom", "import", "namespace", "from", "delete", "inherit",
        "{{", "}}", "@", "[", "]"
    };
    globalScriptRule.strict_mode = true;
    rules_[SyntaxBoundaryType::GLOBAL_SCRIPT] = globalScriptRule;
    
    // 局部脚本块规则
    SyntaxBoundaryRule localScriptRule;
    localScriptRule.type = SyntaxBoundaryType::LOCAL_SCRIPT;
    localScriptRule.start_pattern = "script";
    localScriptRule.end_pattern = "}";
    localScriptRule.allowed_syntax = {
        "template", "custom", "import", "namespace", "from", "{{", "}}", "/*", "*/", "//", "--"
    };
    localScriptRule.strict_mode = false;
    rules_[SyntaxBoundaryType::LOCAL_SCRIPT] = localScriptRule;
    
    // CHTL JS规则
    SyntaxBoundaryRule chtlJSRule;
    chtlJSRule.type = SyntaxBoundaryType::CHTL_JS;
    chtlJSRule.start_pattern = "{{";
    chtlJSRule.end_pattern = "}}";
    chtlJSRule.allowed_syntax = {
        "fileloader", "animate", "route", "$", "{{", "}}", "->", "/*", "*/", "//"
    };
    chtlJSRule.strict_mode = true;
    rules_[SyntaxBoundaryType::CHTL_JS] = chtlJSRule;
}

// WideStrictProcessor 实现
WideStrictProcessor::WideStrictProcessor() 
    : wide_mode_(true), strict_mode_(false), mixed_mode_(false) {
}

WideStrictProcessor::~WideStrictProcessor() = default;

std::string WideStrictProcessor::processWide(const std::string& content) const {
    if (!wide_mode_) {
        return content;
    }
    
    std::string result = content;
    auto boundaries = detector_.findBoundaries(content);
    
    for (const auto& boundary : boundaries) {
        std::string block = content.substr(boundary.first, boundary.second - boundary.first + 1);
        std::string processedBlock = processWideBlock(block);
        result.replace(boundary.first, boundary.second - boundary.first + 1, processedBlock);
    }
    
    return result;
}

std::vector<std::string> WideStrictProcessor::extractWideBlocks(const std::string& content) const {
    std::vector<std::string> blocks;
    auto boundaries = detector_.findBoundaries(content);
    
    for (const auto& boundary : boundaries) {
        std::string block = content.substr(boundary.first, boundary.second - boundary.first + 1);
        blocks.push_back(block);
    }
    
    return blocks;
}

std::string WideStrictProcessor::processStrict(const std::string& content) const {
    if (!strict_mode_) {
        return content;
    }
    
    std::string result = content;
    auto boundaries = detector_.findBoundaries(content);
    
    for (const auto& boundary : boundaries) {
        std::string block = content.substr(boundary.first, boundary.second - boundary.first + 1);
        std::string processedBlock = processStrictBlock(block);
        result.replace(boundary.first, boundary.second - boundary.first + 1, processedBlock);
    }
    
    return result;
}

std::vector<std::string> WideStrictProcessor::extractStrictBlocks(const std::string& content) const {
    std::vector<std::string> blocks;
    auto boundaries = detector_.findBoundaries(content);
    
    for (const auto& boundary : boundaries) {
        std::string block = content.substr(boundary.first, boundary.second - boundary.first + 1);
        blocks.push_back(block);
    }
    
    return blocks;
}

std::string WideStrictProcessor::processMixed(const std::string& content) const {
    if (!mixed_mode_) {
        return content;
    }
    
    std::string result = content;
    auto mixedBlocks = extractMixedBlocks(content);
    
    for (const auto& pair : mixedBlocks) {
        for (const auto& block : pair.second) {
            std::string processedBlock = processMixedBlock(block, pair.first);
            // 替换处理后的块
            size_t pos = result.find(block);
            if (pos != std::string::npos) {
                result.replace(pos, block.length(), processedBlock);
            }
        }
    }
    
    return result;
}

std::map<SyntaxBoundaryType, std::vector<std::string>> WideStrictProcessor::extractMixedBlocks(const std::string& content) const {
    std::map<SyntaxBoundaryType, std::vector<std::string>> blocks;
    
    for (const auto& pair : detector_.getRules()) {
        auto typeBlocks = detector_.findBoundariesByType(content, pair.first);
        for (const auto& boundary : typeBlocks) {
            std::string block = content.substr(boundary.first, boundary.second - boundary.first + 1);
            blocks[pair.first].push_back(block);
        }
    }
    
    return blocks;
}

void WideStrictProcessor::setWideMode(bool enabled) {
    wide_mode_ = enabled;
    if (enabled) {
        strict_mode_ = false;
        mixed_mode_ = false;
    }
}

void WideStrictProcessor::setStrictMode(bool enabled) {
    strict_mode_ = enabled;
    if (enabled) {
        wide_mode_ = false;
        mixed_mode_ = false;
    }
}

void WideStrictProcessor::setMixedMode(bool enabled) {
    mixed_mode_ = enabled;
    if (enabled) {
        wide_mode_ = false;
        strict_mode_ = false;
    }
}

bool WideStrictProcessor::isWideMode() const {
    return wide_mode_;
}

bool WideStrictProcessor::isStrictMode() const {
    return strict_mode_;
}

bool WideStrictProcessor::isMixedMode() const {
    return mixed_mode_;
}

std::string WideStrictProcessor::processWideBlock(const std::string& block) const {
    // 宽判处理：允许更多语法
    return block;
}

std::string WideStrictProcessor::processStrictBlock(const std::string& block) const {
    // 严判处理：严格限制语法
    return block;
}

std::string WideStrictProcessor::processMixedBlock(const std::string& block, SyntaxBoundaryType type) const {
    // 混合处理：根据类型选择处理方式
    switch (type) {
        case SyntaxBoundaryType::GLOBAL_STYLE:
        case SyntaxBoundaryType::LOCAL_STYLE:
            return processWideBlock(block);
        case SyntaxBoundaryType::GLOBAL_SCRIPT:
        case SyntaxBoundaryType::LOCAL_SCRIPT:
            return processStrictBlock(block);
        default:
            return block;
    }
}

std::vector<std::string> WideStrictProcessor::extractBlocksByPattern(const std::string& content, const std::string& startPattern, const std::string& endPattern) const {
    std::vector<std::string> blocks;
    size_t pos = 0;
    
    while (pos < content.length()) {
        size_t start = content.find(startPattern, pos);
        if (start == std::string::npos) {
            break;
        }
        
        size_t end = content.find(endPattern, start);
        if (end != std::string::npos) {
            std::string block = content.substr(start, end - start + endPattern.length());
            blocks.push_back(block);
            pos = end + endPattern.length();
        } else {
            pos = start + 1;
        }
    }
    
    return blocks;
}

std::vector<std::string> WideStrictProcessor::extractBlocksByType(const std::string& content, SyntaxBoundaryType type) const {
    auto boundaries = detector_.findBoundariesByType(content, type);
    std::vector<std::string> blocks;
    
    for (const auto& boundary : boundaries) {
        std::string block = content.substr(boundary.first, boundary.second - boundary.first + 1);
        blocks.push_back(block);
    }
    
    return blocks;
}

// SyntaxBoundaryManager 实现
SyntaxBoundaryManager::SyntaxBoundaryManager() = default;

SyntaxBoundaryManager::~SyntaxBoundaryManager() = default;

void SyntaxBoundaryManager::addBoundary(SyntaxBoundaryType type, size_t start, size_t end) {
    boundaries_[type].push_back({start, end});
}

void SyntaxBoundaryManager::removeBoundary(SyntaxBoundaryType type, size_t start) {
    auto& typeBoundaries = boundaries_[type];
    typeBoundaries.erase(
        std::remove_if(typeBoundaries.begin(), typeBoundaries.end(),
            [start](const std::pair<size_t, size_t>& boundary) {
                return boundary.first == start;
            }),
        typeBoundaries.end()
    );
}

void SyntaxBoundaryManager::clearBoundaries() {
    boundaries_.clear();
}

void SyntaxBoundaryManager::clearBoundariesByType(SyntaxBoundaryType type) {
    boundaries_.erase(type);
}

std::vector<std::pair<size_t, size_t>> SyntaxBoundaryManager::getBoundaries(SyntaxBoundaryType type) const {
    auto it = boundaries_.find(type);
    if (it != boundaries_.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::pair<size_t, size_t>> SyntaxBoundaryManager::getAllBoundaries() const {
    std::vector<std::pair<size_t, size_t>> allBoundaries;
    
    for (const auto& pair : boundaries_) {
        allBoundaries.insert(allBoundaries.end(), pair.second.begin(), pair.second.end());
    }
    
    return allBoundaries;
}

bool SyntaxBoundaryManager::hasBoundary(SyntaxBoundaryType type, size_t start) const {
    auto it = boundaries_.find(type);
    if (it != boundaries_.end()) {
        return std::any_of(it->second.begin(), it->second.end(),
            [start](const std::pair<size_t, size_t>& boundary) {
                return boundary.first == start;
            });
    }
    return false;
}

std::string SyntaxBoundaryManager::extractBoundaryContent(const std::string& content, SyntaxBoundaryType type, size_t start) const {
    auto it = boundaries_.find(type);
    if (it != boundaries_.end()) {
        for (const auto& boundary : it->second) {
            if (boundary.first == start) {
                return extractContentRange(content, boundary.first, boundary.second);
            }
        }
    }
    return "";
}

std::string SyntaxBoundaryManager::replaceBoundaryContent(const std::string& content, SyntaxBoundaryType type, size_t start, const std::string& newContent) const {
    auto it = boundaries_.find(type);
    if (it != boundaries_.end()) {
        for (const auto& boundary : it->second) {
            if (boundary.first == start) {
                return replaceContentRange(content, boundary.first, boundary.second, newContent);
            }
        }
    }
    return content;
}

bool SyntaxBoundaryManager::validateBoundaries(const std::string& content) const {
    for (const auto& pair : boundaries_) {
        for (const auto& boundary : pair.second) {
            if (!isBoundaryValid(content, boundary.first, boundary.second)) {
                return false;
            }
        }
    }
    return true;
}

std::vector<std::string> SyntaxBoundaryManager::getBoundaryValidationErrors(const std::string& content) const {
    std::vector<std::string> errors;
    
    for (const auto& pair : boundaries_) {
        for (const auto& boundary : pair.second) {
            if (!isBoundaryValid(content, boundary.first, boundary.second)) {
                errors.push_back("Invalid boundary at position " + std::to_string(boundary.first));
            }
        }
    }
    
    return errors;
}

bool SyntaxBoundaryManager::isBoundaryValid(const std::string& content, size_t start, size_t end) const {
    if (start >= content.length() || end >= content.length() || start > end) {
        return false;
    }
    
    return true;
}

bool SyntaxBoundaryManager::doBoundariesOverlap(const std::pair<size_t, size_t>& boundary1, const std::pair<size_t, size_t>& boundary2) const {
    return !(boundary1.second < boundary2.first || boundary2.second < boundary1.first);
}

std::string SyntaxBoundaryManager::extractContentRange(const std::string& content, size_t start, size_t end) const {
    if (start >= content.length() || end >= content.length() || start > end) {
        return "";
    }
    
    return content.substr(start, end - start + 1);
}

std::string SyntaxBoundaryManager::replaceContentRange(const std::string& content, size_t start, size_t end, const std::string& newContent) const {
    if (start >= content.length() || end >= content.length() || start > end) {
        return content;
    }
    
    std::string result = content;
    result.replace(start, end - start + 1, newContent);
    return result;
}

// SyntaxBoundaryParser 实现
SyntaxBoundaryParser::SyntaxBoundaryParser() = default;

SyntaxBoundaryParser::~SyntaxBoundaryParser() = default;

std::vector<std::pair<SyntaxBoundaryType, std::pair<size_t, size_t>>> SyntaxBoundaryParser::parseBoundaries(const std::string& content) const {
    std::vector<std::pair<SyntaxBoundaryType, std::pair<size_t, size_t>>> boundaries;
    
    if (!detector_) {
        return boundaries;
    }
    
    for (const auto& pair : detector_->getRules()) {
        auto typeBoundaries = detector_->findBoundariesByType(content, pair.first);
        for (const auto& boundary : typeBoundaries) {
            boundaries.push_back({pair.first, boundary});
        }
    }
    
    return boundaries;
}

std::vector<std::pair<SyntaxBoundaryType, std::string>> SyntaxBoundaryParser::parseBoundaryContents(const std::string& content) const {
    std::vector<std::pair<SyntaxBoundaryType, std::string>> contents;
    
    if (!detector_) {
        return contents;
    }
    
    for (const auto& pair : detector_->getRules()) {
        auto typeBoundaries = detector_->findBoundariesByType(content, pair.first);
        for (const auto& boundary : typeBoundaries) {
            std::string boundaryContent = extractBoundaryContent(content, boundary.first, boundary.second);
            contents.push_back({pair.first, boundaryContent});
        }
    }
    
    return contents;
}

std::vector<std::pair<size_t, size_t>> SyntaxBoundaryParser::parseBoundariesByType(const std::string& content, SyntaxBoundaryType type) const {
    if (!detector_) {
        return {};
    }
    
    return detector_->findBoundariesByType(content, type);
}

std::vector<std::string> SyntaxBoundaryParser::parseBoundaryContentsByType(const std::string& content, SyntaxBoundaryType type) const {
    std::vector<std::string> contents;
    
    if (!detector_) {
        return contents;
    }
    
    auto boundaries = detector_->findBoundariesByType(content, type);
    for (const auto& boundary : boundaries) {
        std::string boundaryContent = extractBoundaryContent(content, boundary.first, boundary.second);
        contents.push_back(boundaryContent);
    }
    
    return contents;
}

void SyntaxBoundaryParser::setDetector(std::shared_ptr<SyntaxBoundaryDetector> detector) {
    detector_ = detector;
}

std::shared_ptr<SyntaxBoundaryDetector> SyntaxBoundaryParser::getDetector() const {
    return detector_;
}

std::vector<std::pair<size_t, size_t>> SyntaxBoundaryParser::findPatternBoundaries(const std::string& content, const std::string& startPattern, const std::string& endPattern) const {
    std::vector<std::pair<size_t, size_t>> boundaries;
    size_t pos = 0;
    
    while (pos < content.length()) {
        size_t start = content.find(startPattern, pos);
        if (start == std::string::npos) {
            break;
        }
        
        size_t end = content.find(endPattern, start);
        if (end != std::string::npos) {
            boundaries.push_back({start, end + endPattern.length() - 1});
            pos = end + endPattern.length();
        } else {
            pos = start + 1;
        }
    }
    
    return boundaries;
}

std::string SyntaxBoundaryParser::extractBoundaryContent(const std::string& content, size_t start, size_t end) const {
    if (start >= content.length() || end >= content.length() || start > end) {
        return "";
    }
    
    return content.substr(start, end - start + 1);
}

bool SyntaxBoundaryParser::isBoundaryComplete(const std::string& content, size_t start, size_t end) const {
    return start < content.length() && end < content.length() && start <= end;
}

// SyntaxBoundaryValidator 实现
SyntaxBoundaryValidator::SyntaxBoundaryValidator() = default;

SyntaxBoundaryValidator::~SyntaxBoundaryValidator() = default;

bool SyntaxBoundaryValidator::validateBoundary(const std::string& content, SyntaxBoundaryType type, size_t start, size_t end) const {
    if (!detector_) {
        return true;
    }
    
    return detector_->validateSyntax(content, type);
}

bool SyntaxBoundaryValidator::validateAllBoundaries(const std::string& content) const {
    if (!detector_) {
        return true;
    }
    
    for (const auto& pair : detector_->getRules()) {
        if (!detector_->validateSyntax(content, pair.first)) {
            return false;
        }
    }
    
    return true;
}

bool SyntaxBoundaryValidator::validateSyntaxInBoundary(const std::string& content, SyntaxBoundaryType type, size_t start, size_t end) const {
    if (!detector_) {
        return true;
    }
    
    std::string boundaryContent = content.substr(start, end - start + 1);
    return detector_->validateSyntax(boundaryContent, type);
}

bool SyntaxBoundaryValidator::validateSyntaxInAllBoundaries(const std::string& content) const {
    if (!detector_) {
        return true;
    }
    
    auto boundaries = detector_->findBoundaries(content);
    
    for (const auto& boundary : boundaries) {
        std::string boundaryContent = content.substr(boundary.first, boundary.second - boundary.first + 1);
        SyntaxBoundaryType type = detector_->detectBoundaryType(content, boundary.first);
        
        if (!detector_->validateSyntax(boundaryContent, type)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> SyntaxBoundaryValidator::getBoundaryValidationErrors(const std::string& content, SyntaxBoundaryType type, size_t start, size_t end) const {
    if (!detector_) {
        return {};
    }
    
    std::string boundaryContent = content.substr(start, end - start + 1);
    return detector_->getValidationErrors(boundaryContent, type);
}

std::vector<std::string> SyntaxBoundaryValidator::getAllBoundaryValidationErrors(const std::string& content) const {
    std::vector<std::string> errors;
    
    if (!detector_) {
        return errors;
    }
    
    auto boundaries = detector_->findBoundaries(content);
    
    for (const auto& boundary : boundaries) {
        std::string boundaryContent = content.substr(boundary.first, boundary.second - boundary.first + 1);
        SyntaxBoundaryType type = detector_->detectBoundaryType(content, boundary.first);
        
        auto boundaryErrors = detector_->getValidationErrors(boundaryContent, type);
        errors.insert(errors.end(), boundaryErrors.begin(), boundaryErrors.end());
    }
    
    return errors;
}

void SyntaxBoundaryValidator::setDetector(std::shared_ptr<SyntaxBoundaryDetector> detector) {
    detector_ = detector;
}

std::shared_ptr<SyntaxBoundaryDetector> SyntaxBoundaryValidator::getDetector() const {
    return detector_;
}

bool SyntaxBoundaryValidator::isSyntaxAllowed(const std::string& content, size_t position, const std::set<std::string>& allowed) const {
    for (const auto& syntax : allowed) {
        if (position + syntax.length() <= content.length() && 
            content.substr(position, syntax.length()) == syntax) {
            return true;
        }
    }
    return false;
}

bool SyntaxBoundaryValidator::isSyntaxForbidden(const std::string& content, size_t position, const std::set<std::string>& forbidden) const {
    for (const auto& syntax : forbidden) {
        if (position + syntax.length() <= content.length() && 
            content.substr(position, syntax.length()) == syntax) {
            return true;
        }
    }
    return false;
}

bool SyntaxBoundaryValidator::isPositionInBoundary(size_t position, size_t start, size_t end) const {
    return position >= start && position <= end;
}

std::string SyntaxBoundaryValidator::getSyntaxAtPosition(const std::string& content, size_t position) const {
    if (position >= content.length()) {
        return "";
    }
    
    // 简单的语法提取，实际实现可能需要更复杂的逻辑
    size_t start = position;
    while (start > 0 && std::isalnum(content[start - 1])) {
        start--;
    }
    
    size_t end = position;
    while (end < content.length() && std::isalnum(content[end])) {
        end++;
    }
    
    return content.substr(start, end - start);
}

} // namespace CHTL