#include "chtl/origin_embed.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace origin_embed {

// OriginEmbedParser implementation
OriginEmbedParser::OriginEmbedParser(const std::string& input) 
    : input_(input), position_(0) {}

std::vector<std::shared_ptr<OriginBlock>> OriginEmbedParser::parse() {
    std::vector<std::shared_ptr<OriginBlock>> blocks;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (currentChar() == '[') {
            auto block = parseOriginBlock();
            if (block) {
                blocks.push_back(block);
            }
        } else {
            advance();
        }
    }
    
    return blocks;
}

void OriginEmbedParser::skipWhitespace() {
    while (position_ < input_.length() && std::isspace(input_[position_])) {
        position_++;
    }
}

char OriginEmbedParser::currentChar() const {
    if (position_ >= input_.length()) return '\0';
    return input_[position_];
}

char OriginEmbedParser::peekChar() const {
    if (position_ + 1 >= input_.length()) return '\0';
    return input_[position_ + 1];
}

void OriginEmbedParser::advance() {
    if (position_ < input_.length()) {
        position_++;
    }
}

std::shared_ptr<OriginBlock> OriginEmbedParser::parseOriginBlock() {
    if (currentChar() != '[') return nullptr;
    advance(); // skip '['
    
    skipWhitespace();
    
    // Parse "Origin"
    std::string originKeyword = parseIdentifier();
    if (originKeyword != "Origin") return nullptr;
    
    skipWhitespace();
    if (currentChar() != ']') return nullptr;
    advance(); // skip ']'
    
    skipWhitespace();
    
    auto block = std::make_shared<OriginBlock>();
    block->type = parseOriginType();
    
    skipWhitespace();
    if (currentChar() != '{') {
        return nullptr;
    }
    advance(); // skip '{'
    
    skipWhitespace();
    if (currentChar() == '}') {
        advance(); // skip '}'
        return block;
    }
    
    // Parse content
    std::string content;
    while (position_ < input_.length() && currentChar() != '}') {
        content += currentChar();
        advance();
    }
    
    if (currentChar() == '}') {
        advance(); // skip '}'
    }
    
    block->content = content;
    return block;
}

OriginType OriginEmbedParser::parseOriginType() {
    skipWhitespace();
    if (currentChar() != '@') return OriginType::HTML;
    advance(); // skip '@'
    
    std::string type = parseIdentifier();
    if (type == "HTML") return OriginType::HTML;
    if (type == "CSS") return OriginType::CSS;
    if (type == "JS") return OriginType::JS;
    if (type == "Vue") return OriginType::VUE;
    if (type == "React") return OriginType::REACT;
    if (type == "Angular") return OriginType::ANGULAR;
    return OriginType::CUSTOM;
}

std::string OriginEmbedParser::parseString() {
    if (currentChar() != '"') return "";
    
    advance(); // skip opening quote
    std::string result;
    
    while (position_ < input_.length() && currentChar() != '"') {
        if (currentChar() == '\\' && peekChar() == '"') {
            advance(); // skip backslash
            result += '"';
        } else {
            result += currentChar();
        }
        advance();
    }
    
    if (currentChar() == '"') {
        advance(); // skip closing quote
    }
    
    return result;
}

std::string OriginEmbedParser::parseIdentifier() {
    std::string result;
    
    while (position_ < input_.length() && 
           (std::isalnum(input_[position_]) || input_[position_] == '_')) {
        result += input_[position_];
        position_++;
    }
    
    return result;
}

std::map<std::string, std::string> OriginEmbedParser::parseAttributes() {
    std::map<std::string, std::string> attributes;
    
    while (position_ < input_.length() && currentChar() != '}') {
        skipWhitespace();
        if (currentChar() == '}') break;
        
        std::string key = parseIdentifier();
        if (key.empty()) break;
        
        skipWhitespace();
        if (currentChar() != ':') break;
        advance(); // skip ':'
        
        skipWhitespace();
        std::string value = parseString();
        if (value.empty()) break;
        
        attributes[key] = value;
        
        skipWhitespace();
        if (currentChar() == ',') {
            advance(); // skip ','
        }
    }
    
    return attributes;
}

bool OriginEmbedParser::isKeyword(const std::string& word) const {
    static const std::set<std::string> keywords = {
        "Origin", "HTML", "CSS", "JS", "Vue", "React", "Angular"
    };
    return keywords.find(word) != keywords.end();
}

bool OriginEmbedParser::isOriginKeyword(const std::string& word) const {
    return word == "Origin";
}

// OriginEmbedCompiler implementation
OriginEmbedCompiler::OriginEmbedCompiler() {}

std::string OriginEmbedCompiler::compile(const std::vector<std::shared_ptr<OriginBlock>>& blocks) {
    std::string result;
    
    for (const auto& block : blocks) {
        if (block && block->isActive) {
            result += compileOriginBlock(*block);
            result += "\n\n";
        }
    }
    
    return result;
}

std::string OriginEmbedCompiler::compileOriginBlock(const OriginBlock& block) {
    std::string result;
    
    result += "// Origin Block: " + block.name + "\n";
    result += "// Type: " + std::to_string(static_cast<int>(block.type)) + "\n";
    
    switch (block.type) {
        case OriginType::HTML:
            result += compileHTML(block.content);
            break;
        case OriginType::CSS:
            result += compileCSS(block.content);
            break;
        case OriginType::JS:
            result += compileJS(block.content);
            break;
        case OriginType::VUE:
            result += compileVue(block.content);
            break;
        case OriginType::REACT:
            result += compileReact(block.content);
            break;
        case OriginType::ANGULAR:
            result += compileAngular(block.content);
            break;
        case OriginType::CUSTOM:
            result += compileCustom(block.content, "custom");
            break;
    }
    
    return result;
}

std::string OriginEmbedCompiler::compileHTML(const std::string& content) {
    return "<!DOCTYPE html>\n<html>\n<head>\n</head>\n<body>\n" + content + "\n</body>\n</html>";
}

std::string OriginEmbedCompiler::compileCSS(const std::string& content) {
    return "<style>\n" + content + "\n</style>";
}

std::string OriginEmbedCompiler::compileJS(const std::string& content) {
    return "<script>\n" + content + "\n</script>";
}

std::string OriginEmbedCompiler::compileVue(const std::string& content) {
    return "// Vue.js component\n" + content;
}

std::string OriginEmbedCompiler::compileReact(const std::string& content) {
    return "// React component\n" + content;
}

std::string OriginEmbedCompiler::compileAngular(const std::string& content) {
    return "// Angular component\n" + content;
}

std::string OriginEmbedCompiler::compileCustom(const std::string& content, const std::string& type) {
    return "// Custom " + type + " content\n" + content;
}

void OriginEmbedCompiler::addOriginBlock(std::shared_ptr<OriginBlock> block) {
    if (block && !block->name.empty()) {
        blocks_[block->name] = block;
    }
}

std::shared_ptr<OriginBlock> OriginEmbedCompiler::getOriginBlock(const std::string& name) {
    auto it = blocks_.find(name);
    if (it != blocks_.end()) {
        return it->second;
    }
    return nullptr;
}

bool OriginEmbedCompiler::validateOriginBlock(const OriginBlock& block) {
    if (block.name.empty()) return false;
    if (block.content.empty()) return false;
    
    switch (block.type) {
        case OriginType::HTML:
            return validateHTML(block.content);
        case OriginType::CSS:
            return validateCSS(block.content);
        case OriginType::JS:
            return validateJS(block.content);
        case OriginType::VUE:
            return validateVue(block.content);
        case OriginType::REACT:
            return validateReact(block.content);
        case OriginType::ANGULAR:
            return validateAngular(block.content);
        case OriginType::CUSTOM:
            return validateCustom(block.content, "custom");
    }
    
    return false;
}

bool OriginEmbedCompiler::validateHTML(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedCompiler::validateCSS(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedCompiler::validateJS(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedCompiler::validateVue(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedCompiler::validateReact(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedCompiler::validateAngular(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedCompiler::validateCustom(const std::string& content, const std::string& type) {
    return !content.empty() && !type.empty();
}

// OriginEmbedManager implementation
OriginEmbedManager::OriginEmbedManager() {}

void OriginEmbedManager::addOriginBlock(std::shared_ptr<OriginBlock> block) {
    if (block && !block->name.empty()) {
        compiler_.addOriginBlock(block);
    }
}

std::shared_ptr<OriginBlock> OriginEmbedManager::getOriginBlock(const std::string& name) {
    return compiler_.getOriginBlock(name);
}

std::string OriginEmbedManager::generateCode(const std::vector<std::shared_ptr<OriginBlock>>& blocks) {
    return compiler_.compile(blocks);
}

std::string OriginEmbedManager::generateOriginCode() {
    return "// Origin embedding system generated code\n";
}

bool OriginEmbedManager::validateOriginBlock(const OriginBlock& block) {
    return compiler_.validateOriginBlock(block);
}

bool OriginEmbedManager::validateHTML(const std::string& content) {
    return compiler_.validateHTML(content);
}

bool OriginEmbedManager::validateCSS(const std::string& content) {
    return compiler_.validateCSS(content);
}

bool OriginEmbedManager::validateJS(const std::string& content) {
    return compiler_.validateJS(content);
}

bool OriginEmbedManager::validateVue(const std::string& content) {
    return compiler_.validateVue(content);
}

bool OriginEmbedManager::validateReact(const std::string& content) {
    return compiler_.validateReact(content);
}

bool OriginEmbedManager::validateAngular(const std::string& content) {
    return compiler_.validateAngular(content);
}

bool OriginEmbedManager::validateCustom(const std::string& content, const std::string& type) {
    return compiler_.validateCustom(content, type);
}

void OriginEmbedManager::registerCustomType(const std::string& type, const std::string& processor) {
    customTypeProcessors_[type] = processor;
}

std::string OriginEmbedManager::getCustomTypeProcessor(const std::string& type) {
    auto it = customTypeProcessors_.find(type);
    if (it != customTypeProcessors_.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> OriginEmbedManager::getRegisteredCustomTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : customTypeProcessors_) {
        types.push_back(pair.first);
    }
    return types;
}

void OriginEmbedManager::clear() {
    customTypeProcessors_.clear();
}

size_t OriginEmbedManager::getOriginBlockCount() const {
    return customTypeProcessors_.size();
}

// OriginEmbedValidator implementation
OriginEmbedValidator::OriginEmbedValidator() {}

void OriginEmbedValidator::setOriginBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks) {
    blocks_ = blocks;
}

bool OriginEmbedValidator::validate(const OriginBlock& block) {
    clearErrors();
    clearWarnings();
    
    return checkOriginBlock(block);
}

std::vector<std::string> OriginEmbedValidator::getValidationErrors() {
    return errors_;
}

std::vector<std::string> OriginEmbedValidator::getValidationWarnings() {
    return warnings_;
}

void OriginEmbedValidator::clearErrors() {
    errors_.clear();
}

void OriginEmbedValidator::clearWarnings() {
    warnings_.clear();
}

void OriginEmbedValidator::addError(const std::string& error) {
    errors_.push_back(error);
}

void OriginEmbedValidator::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

bool OriginEmbedValidator::checkOriginBlock(const OriginBlock& block) {
    if (block.name.empty()) {
        addError("Origin block name cannot be empty");
        return false;
    }
    
    if (block.content.empty()) {
        addWarning("Origin block content is empty");
    }
    
    switch (block.type) {
        case OriginType::HTML:
            return checkHTML(block.content);
        case OriginType::CSS:
            return checkCSS(block.content);
        case OriginType::JS:
            return checkJS(block.content);
        case OriginType::VUE:
            return checkVue(block.content);
        case OriginType::REACT:
            return checkReact(block.content);
        case OriginType::ANGULAR:
            return checkAngular(block.content);
        case OriginType::CUSTOM:
            return checkCustom(block.content, "custom");
    }
    
    return true;
}

bool OriginEmbedValidator::checkHTML(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedValidator::checkCSS(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedValidator::checkJS(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedValidator::checkVue(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedValidator::checkReact(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedValidator::checkAngular(const std::string& content) {
    return !content.empty();
}

bool OriginEmbedValidator::checkCustom(const std::string& content, const std::string& type) {
    return !content.empty() && !type.empty();
}

// OriginEmbedProcessor implementation
std::string OriginEmbedProcessor::processHTML(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::processCSS(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::processJS(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::processVue(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::processReact(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::processAngular(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::processCustom(const std::string& content, const std::string& type) {
    return content;
}

std::string OriginEmbedProcessor::extractHTML(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::extractCSS(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::extractJS(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::extractVue(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::extractReact(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::extractAngular(const std::string& content) {
    return content;
}

std::string OriginEmbedProcessor::extractCustom(const std::string& content, const std::string& type) {
    return content;
}

std::string OriginEmbedProcessor::mergeOriginBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks) {
    std::string result;
    
    for (const auto& block : blocks) {
        if (block && block->isActive) {
            result += block->content + "\n";
        }
    }
    
    return result;
}

std::string OriginEmbedProcessor::mergeHTMLBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks) {
    std::string result;
    
    for (const auto& block : blocks) {
        if (block && block->isActive && block->type == OriginType::HTML) {
            result += block->content + "\n";
        }
    }
    
    return result;
}

std::string OriginEmbedProcessor::mergeCSSBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks) {
    std::string result;
    
    for (const auto& block : blocks) {
        if (block && block->isActive && block->type == OriginType::CSS) {
            result += block->content + "\n";
        }
    }
    
    return result;
}

std::string OriginEmbedProcessor::mergeJSBlocks(const std::vector<std::shared_ptr<OriginBlock>>& blocks) {
    std::string result;
    
    for (const auto& block : blocks) {
        if (block && block->isActive && block->type == OriginType::JS) {
            result += block->content + "\n";
        }
    }
    
    return result;
}

// OriginEmbedSupport implementation
std::vector<std::string> OriginEmbedSupport::getSupportedTypes() {
    return {"HTML", "CSS", "JS", "Vue", "React", "Angular", "Custom"};
}

bool OriginEmbedSupport::isSupportedType(const std::string& type) {
    auto types = getSupportedTypes();
    return std::find(types.begin(), types.end(), type) != types.end();
}

std::string OriginEmbedSupport::getTypeDescription(const std::string& type) {
    if (type == "HTML") return "HTML markup language";
    if (type == "CSS") return "CSS stylesheet language";
    if (type == "JS") return "JavaScript programming language";
    if (type == "Vue") return "Vue.js framework";
    if (type == "React") return "React framework";
    if (type == "Angular") return "Angular framework";
    if (type == "Custom") return "Custom type";
    return "Unknown type";
}

std::vector<std::string> OriginEmbedSupport::getTypeFeatures(const std::string& type) {
    if (type == "HTML") return {"Markup", "Elements", "Attributes", "Structure"};
    if (type == "CSS") return {"Styling", "Selectors", "Properties", "Values"};
    if (type == "JS") return {"Programming", "Functions", "Variables", "Objects"};
    if (type == "Vue") return {"Components", "Templates", "Reactivity", "Directives"};
    if (type == "React") return {"Components", "JSX", "Hooks", "State"};
    if (type == "Angular") return {"Components", "Services", "Dependency Injection", "Templates"};
    if (type == "Custom") return {"Flexible", "Extensible", "Configurable"};
    return {};
}

std::string OriginEmbedSupport::generateOriginBlock(const std::string& type, const std::string& content) {
    return "[Origin] @{ " + type + " " + content + " }";
}

std::string OriginEmbedSupport::generateOriginImport(const std::string& type) {
    return "import { " + type + " } from 'origin-embed';";
}

std::string OriginEmbedSupport::generateOriginUsage(const std::string& type, const std::string& content) {
    return "// Using " + type + " origin embedding\n" + content;
}

} // namespace origin_embed
} // namespace chtl