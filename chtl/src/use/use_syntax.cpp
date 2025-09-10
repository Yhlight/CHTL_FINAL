#include "chtl/use_syntax.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace use_syntax {

// UseParser 实现
UseParser::UseParser(const std::string& input) : input_(input), position_(0) {}

std::vector<std::shared_ptr<UseStatement>> UseParser::parse() {
    std::vector<std::shared_ptr<UseStatement>> statements;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 3) == "use") {
            position_ += 3;
            skipWhitespace();
            
            auto statement = parseUseStatement();
            if (statement) {
                statements.push_back(statement);
            }
        } else {
            advance();
        }
    }
    
    return statements;
}

void UseParser::skipWhitespace() {
    while (position_ < input_.length() && std::isspace(input_[position_])) {
        position_++;
    }
}

char UseParser::currentChar() const {
    if (position_ >= input_.length()) return '\0';
    return input_[position_];
}

char UseParser::peekChar() const {
    if (position_ + 1 >= input_.length()) return '\0';
    return input_[position_ + 1];
}

void UseParser::advance() {
    if (position_ < input_.length()) {
        position_++;
    }
}

std::shared_ptr<UseStatement> UseParser::parseUseStatement() {
    auto statement = std::make_shared<UseStatement>();
    
    skipWhitespace();
    
    // 解析use类型
    if (input_.substr(position_, 5) == "html5") {
        position_ += 5;
        statement->type = UseType::HTML5_TYPE;
        statement->value = "html5";
    } else if (input_.substr(position_, 4) == "xhtml") {
        position_ += 4;
        statement->type = UseType::HTML5_TYPE;
        statement->value = "xhtml";
    } else if (input_.substr(position_, 6) == "config") {
        position_ += 6;
        statement->type = UseType::CONFIG_GROUP;
        statement->value = "config";
    } else {
        // 解析标识符
        statement->value = parseIdentifier();
        statement->type = UseType::HTML5_TYPE;
    }
    
    skipWhitespace();
    
    // 解析参数
    if (currentChar() == '(') {
        advance();
        statement->parameters = parseParameters();
        skipWhitespace();
        if (currentChar() == ')') advance();
    }
    
    return statement;
}

std::string UseParser::parseString() {
    std::string result;
    
    if (currentChar() == '"') {
        advance();
        while (position_ < input_.length() && currentChar() != '"') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '"') advance();
    } else if (currentChar() == '\'') {
        advance();
        while (position_ < input_.length() && currentChar() != '\'') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '\'') advance();
    } else {
        // 解析无引号字符串
        while (position_ < input_.length() && 
               !std::isspace(currentChar()) && 
               currentChar() != ',' && 
               currentChar() != ')' && 
               currentChar() != ';') {
            result += currentChar();
            advance();
        }
    }
    
    return result;
}

std::string UseParser::parseIdentifier() {
    std::string result;
    
    while (position_ < input_.length() && 
           (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::map<std::string, std::string> UseParser::parseParameters() {
    std::map<std::string, std::string> parameters;
    
    while (position_ < input_.length() && currentChar() != ')') {
        skipWhitespace();
        if (currentChar() == ')') break;
        
        std::string key = parseIdentifier();
        skipWhitespace();
        
        if (currentChar() == ':') {
            advance();
            skipWhitespace();
            std::string value = parseString();
            parameters[key] = value;
        }
        
        skipWhitespace();
        
        if (currentChar() == ',') {
            advance();
            skipWhitespace();
        }
    }
    
    return parameters;
}

bool UseParser::isKeyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "use", "html5", "xhtml", "config", "strict", "transitional"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool UseParser::isUseKeyword(const std::string& word) const {
    static const std::vector<std::string> useKeywords = {
        "use", "html5", "xhtml", "config", "strict", "transitional"
    };
    
    return std::find(useKeywords.begin(), useKeywords.end(), word) != useKeywords.end();
}

// UseCompiler 实现
UseCompiler::UseCompiler() {}

std::string UseCompiler::compile(const std::vector<std::shared_ptr<UseStatement>>& statements) {
    std::string result = "// Generated Use Statements\n\n";
    
    for (const auto& statement : statements) {
        result += compileUseStatement(*statement) + "\n";
    }
    
    return result;
}

std::string UseCompiler::compileUseStatement(const UseStatement& statement) {
    std::string result = "// Use Statement: " + statement.value + "\n";
    result += "// Type: " + std::to_string(static_cast<int>(statement.type)) + "\n";
    result += "// Active: " + std::string(statement.isActive ? "true" : "false") + "\n";
    
    if (!statement.parameters.empty()) {
        result += "// Parameters:\n";
        for (const auto& param : statement.parameters) {
            result += "//   " + param.first + ": " + param.second + "\n";
        }
    }
    
    result += "\n";
    return result;
}

std::string UseCompiler::compileHTML5Type(const std::string& type) {
    std::string result = "// HTML5 Type: " + type + "\n";
    
    if (type == "html5") {
        result += HTML5TypeSupport::generateHTML5Doctype("html5");
    } else if (type == "xhtml") {
        result += HTML5TypeSupport::generateXHTMLDoctype("xhtml");
    }
    
    return result;
}

std::string UseCompiler::compileConfigGroupUse(const ConfigGroupUse& config) {
    std::string result = "// Config Group Use: " + config.groupName + "\n";
    result += "// Active: " + std::string(config.isActive ? "true" : "false") + "\n";
    
    if (!config.parameters.empty()) {
        result += "// Parameters:\n";
        for (const auto& param : config.parameters) {
            result += "//   " + param.first + ": " + param.second + "\n";
        }
    }
    
    result += "\n";
    return result;
}

void UseCompiler::addUseStatement(std::shared_ptr<UseStatement> statement) {
    statements_[statement->value] = statement;
}

std::shared_ptr<UseStatement> UseCompiler::getUseStatement(const std::string& value) {
    auto it = statements_.find(value);
    return (it != statements_.end()) ? it->second : nullptr;
}

bool UseCompiler::validateUseStatement(const UseStatement& statement) {
    switch (statement.type) {
        case UseType::HTML5_TYPE:
            return validateHTML5Type(statement.value);
        case UseType::CONFIG_GROUP:
            return validateConfigGroupUse(ConfigGroupUse(statement.value));
        default:
            return false;
    }
}

bool UseCompiler::validateHTML5Type(const std::string& type) {
    return HTML5TypeSupport::isSupportedType(type);
}

bool UseCompiler::validateConfigGroupUse(const ConfigGroupUse& config) {
    return ConfigGroupSupport::isAvailableConfigGroup(config.groupName);
}

std::string UseCompiler::generateUseCode(const UseStatement& statement) {
    return "// Use code for " + statement.value;
}

std::string UseCompiler::generateHTML5Code(const std::string& type) {
    return "// HTML5 code for " + type;
}

std::string UseCompiler::generateConfigGroupCode(const ConfigGroupUse& config) {
    return "// Config group code for " + config.groupName;
}

// UseManager 实现
UseManager::UseManager() : html5Type_(HTML5Type::HTML5) {}

void UseManager::addUseStatement(std::shared_ptr<UseStatement> statement) {
    compiler_.addUseStatement(statement);
}

std::shared_ptr<UseStatement> UseManager::getUseStatement(const std::string& value) {
    return compiler_.getUseStatement(value);
}

std::string UseManager::generateCode(const std::vector<std::shared_ptr<UseStatement>>& statements) {
    return compiler_.compile(statements);
}

std::string UseManager::generateUseCode() {
    std::string result = "// Use Code\n\n";
    
    for (const auto& statement : compiler_.statements_) {
        result += compiler_.generateUseCode(*statement.second) + "\n";
    }
    
    return result;
}

bool UseManager::validateUseStatement(const UseStatement& statement) {
    return compiler_.validateUseStatement(statement);
}

bool UseManager::validateHTML5Type(const std::string& type) {
    return compiler_.validateHTML5Type(type);
}

bool UseManager::validateConfigGroupUse(const ConfigGroupUse& config) {
    return compiler_.validateConfigGroupUse(config);
}

void UseManager::setHTML5Type(HTML5Type type) {
    html5Type_ = type;
}

HTML5Type UseManager::getHTML5Type() const {
    return html5Type_;
}

void UseManager::addConfigGroupUse(const ConfigGroupUse& config) {
    configGroupUses_.push_back(config);
}

std::vector<ConfigGroupUse> UseManager::getConfigGroupUses() const {
    return configGroupUses_;
}

void UseManager::clear() {
    compiler_.statements_.clear();
    configGroupUses_.clear();
}

size_t UseManager::getUseStatementCount() const {
    return compiler_.statements_.size();
}

std::string UseManager::processUseDependencies(const std::string& content) {
    return content;
}

std::string UseManager::resolveUseReferences(const std::string& content) {
    return content;
}

std::string UseManager::validateUseParameters(const std::string& useValue, const std::map<std::string, std::string>& parameters) {
    return "";
}

// UseValidator 实现
UseValidator::UseValidator() {}

void UseValidator::setUseStatements(const std::vector<std::shared_ptr<UseStatement>>& statements) {
    statements_ = statements;
}

bool UseValidator::validate(const UseStatement& statement) {
    clearErrors();
    clearWarnings();
    
    return checkUseStatement(statement);
}

std::vector<std::string> UseValidator::getValidationErrors() {
    return errors_;
}

std::vector<std::string> UseValidator::getValidationWarnings() {
    return warnings_;
}

void UseValidator::clearErrors() {
    errors_.clear();
}

void UseValidator::clearWarnings() {
    warnings_.clear();
}

void UseValidator::addError(const std::string& error) {
    errors_.push_back(error);
}

void UseValidator::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

bool UseValidator::checkUseStatement(const UseStatement& statement) {
    switch (statement.type) {
        case UseType::HTML5_TYPE:
            return checkHTML5Type(statement.value);
        case UseType::CONFIG_GROUP:
            return checkConfigGroupUse(ConfigGroupUse(statement.value));
        default:
            addError("Unknown use statement type");
            return false;
    }
}

bool UseValidator::checkHTML5Type(const std::string& type) {
    if (!HTML5TypeSupport::isSupportedType(type)) {
        addError("Unsupported HTML5 type: " + type);
        return false;
    }
    
    return true;
}

bool UseValidator::checkConfigGroupUse(const ConfigGroupUse& config) {
    if (!ConfigGroupSupport::isAvailableConfigGroup(config.groupName)) {
        addError("Unavailable config group: " + config.groupName);
        return false;
    }
    
    return true;
}

// HTML5TypeSupport 实现
std::vector<std::string> HTML5TypeSupport::getSupportedTypes() {
    return {"html5", "html5-strict", "html5-transitional", "xhtml", "xhtml-strict", "xhtml-transitional"};
}

bool HTML5TypeSupport::isSupportedType(const std::string& type) {
    auto supportedTypes = getSupportedTypes();
    return std::find(supportedTypes.begin(), supportedTypes.end(), type) != supportedTypes.end();
}

std::string HTML5TypeSupport::getTypeDescription(const std::string& type) {
    if (type == "html5") return "Standard HTML5";
    if (type == "html5-strict") return "Strict HTML5";
    if (type == "html5-transitional") return "Transitional HTML5";
    if (type == "xhtml") return "Standard XHTML";
    if (type == "xhtml-strict") return "Strict XHTML";
    if (type == "xhtml-transitional") return "Transitional XHTML";
    return "Unknown type";
}

std::vector<std::string> HTML5TypeSupport::getTypeFeatures(const std::string& type) {
    if (type == "html5") return {"HTML5 elements", "CSS3 support", "JavaScript ES6"};
    if (type == "html5-strict") return {"HTML5 elements", "CSS3 support", "JavaScript ES6", "Strict validation"};
    if (type == "html5-transitional") return {"HTML5 elements", "CSS3 support", "JavaScript ES6", "Legacy support"};
    if (type == "xhtml") return {"XHTML elements", "XML compliance", "Strict validation"};
    if (type == "xhtml-strict") return {"XHTML elements", "XML compliance", "Strict validation", "No deprecated elements"};
    if (type == "xhtml-transitional") return {"XHTML elements", "XML compliance", "Strict validation", "Legacy support"};
    return {};
}

std::string HTML5TypeSupport::generateHTML5Doctype(const std::string& type) {
    if (type == "html5") return "<!DOCTYPE html>";
    if (type == "html5-strict") return "<!DOCTYPE html>";
    if (type == "html5-transitional") return "<!DOCTYPE html>";
    return "<!DOCTYPE html>";
}

std::string HTML5TypeSupport::generateXHTMLDoctype(const std::string& type) {
    if (type == "xhtml") return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
    if (type == "xhtml-strict") return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">";
    if (type == "xhtml-transitional") return "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">";
    return "<!DOCTYPE html>";
}

std::string HTML5TypeSupport::generateMetaTags(const std::string& type) {
    std::string result = "<meta charset=\"UTF-8\">\n";
    
    if (type.find("xhtml") != std::string::npos) {
        result += "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n";
    }
    
    return result;
}

// ConfigGroupSupport 实现
std::vector<std::string> ConfigGroupSupport::getAvailableConfigGroups() {
    return {"debug-mode", "production", "development", "testing", "staging"};
}

bool ConfigGroupSupport::isAvailableConfigGroup(const std::string& groupName) {
    auto availableGroups = getAvailableConfigGroups();
    return std::find(availableGroups.begin(), availableGroups.end(), groupName) != availableGroups.end();
}

std::string ConfigGroupSupport::getConfigGroupDescription(const std::string& groupName) {
    if (groupName == "debug-mode") return "Debug mode configuration";
    if (groupName == "production") return "Production environment configuration";
    if (groupName == "development") return "Development environment configuration";
    if (groupName == "testing") return "Testing environment configuration";
    if (groupName == "staging") return "Staging environment configuration";
    return "Unknown config group";
}

std::vector<std::string> ConfigGroupSupport::getConfigGroupParameters(const std::string& groupName) {
    if (groupName == "debug-mode") return {"DEBUG_MODE", "LOG_LEVEL", "VERBOSE"};
    if (groupName == "production") return {"DEBUG_MODE", "LOG_LEVEL", "VERBOSE", "OPTIMIZE"};
    if (groupName == "development") return {"DEBUG_MODE", "LOG_LEVEL", "VERBOSE", "HOT_RELOAD"};
    if (groupName == "testing") return {"DEBUG_MODE", "LOG_LEVEL", "VERBOSE", "TEST_MODE"};
    if (groupName == "staging") return {"DEBUG_MODE", "LOG_LEVEL", "VERBOSE", "STAGING_MODE"};
    return {};
}

std::string ConfigGroupSupport::generateConfigGroupCode(const std::string& groupName, const std::map<std::string, std::string>& parameters) {
    std::string result = "// Config Group: " + groupName + "\n";
    
    for (const auto& param : parameters) {
        result += param.first + ": " + param.second + "\n";
    }
    
    return result;
}

std::string ConfigGroupSupport::generateConfigGroupImport(const std::string& groupName) {
    return "import " + groupName + " from './configs/" + groupName + ".chtl'";
}

std::string ConfigGroupSupport::generateConfigGroupUsage(const std::string& groupName, const std::map<std::string, std::string>& parameters) {
    std::string result = "use " + groupName;
    
    if (!parameters.empty()) {
        result += "(";
        bool first = true;
        for (const auto& param : parameters) {
            if (!first) result += ", ";
            result += param.first + ": " + param.second;
            first = false;
        }
        result += ")";
    }
    
    return result;
}

} // namespace use_syntax
} // namespace chtl