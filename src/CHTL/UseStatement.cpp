#include "CHTL/UseStatement.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// UseStatement 实现
UseStatement::UseStatement(UseStatementType type, const std::string& value) 
    : CHTLNode(NodeType::CONFIG), type_(type), value_(value) {
}

UseStatement::~UseStatement() = default;

std::shared_ptr<CHTLNode> UseStatement::clone() const {
    return std::make_shared<UseStatement>(type_, value_);
}

UseStatementType UseStatement::getType() const {
    return type_;
}

void UseStatement::setType(UseStatementType type) {
    type_ = type;
}

const std::string& UseStatement::getValue() const {
    return value_;
}

void UseStatement::setValue(const std::string& value) {
    value_ = value;
}

std::string UseStatement::generateHTML() const {
    return generateUseStatement();
}

std::string UseStatement::generateUseStatement() const {
    switch (type_) {
        case UseStatementType::HTML5:
            return generateHTML5Statement();
        case UseStatementType::CONFIG:
            return generateConfigStatement();
        default:
            return "";
    }
}

bool UseStatement::validate() const {
    switch (type_) {
        case UseStatementType::HTML5:
            return value_.empty() || value_ == "html5";
        case UseStatementType::CONFIG:
            return !value_.empty();
        default:
            return false;
    }
}

std::vector<std::string> UseStatement::getValidationErrors() const {
    std::vector<std::string> errors;
    
    switch (type_) {
        case UseStatementType::HTML5:
            if (!value_.empty() && value_ != "html5") {
                errors.push_back("Invalid HTML5 use statement value");
            }
            break;
        case UseStatementType::CONFIG:
            if (value_.empty()) {
                errors.push_back("Config use statement requires a value");
            }
            break;
        default:
            errors.push_back("Unknown use statement type");
            break;
    }
    
    return errors;
}

std::string UseStatement::generateHTML5Statement() const {
    return "use html5;";
}

std::string UseStatement::generateConfigStatement() const {
    return "use @Config " + value_ + ";";
}

// UseStatementManager 实现
UseStatementManager::UseStatementManager() = default;

UseStatementManager::~UseStatementManager() = default;

void UseStatementManager::addUseStatement(std::shared_ptr<UseStatement> useStatement) {
    if (useStatement) {
        use_statements_.push_back(useStatement);
    }
}

void UseStatementManager::removeUseStatement(const std::string& value) {
    use_statements_.erase(
        std::remove_if(use_statements_.begin(), use_statements_.end(),
            [&value](const std::shared_ptr<UseStatement>& stmt) {
                return stmt->getValue() == value;
            }),
        use_statements_.end()
    );
}

std::vector<std::shared_ptr<UseStatement>> UseStatementManager::getUseStatements() const {
    return use_statements_;
}

std::vector<std::shared_ptr<UseStatement>> UseStatementManager::getUseStatementsByType(UseStatementType type) const {
    std::vector<std::shared_ptr<UseStatement>> filtered;
    for (const auto& stmt : use_statements_) {
        if (stmt->getType() == type) {
            filtered.push_back(stmt);
        }
    }
    return filtered;
}

void UseStatementManager::setConfigurationManager(std::shared_ptr<ConfigurationManager> configManager) {
    config_manager_ = configManager;
}

std::shared_ptr<ConfigurationManager> UseStatementManager::getConfigurationManager() const {
    return config_manager_;
}

void UseStatementManager::applyUseStatements() {
    for (const auto& stmt : use_statements_) {
        switch (stmt->getType()) {
            case UseStatementType::HTML5:
                applyHTML5UseStatement();
                break;
            case UseStatementType::CONFIG:
                applyConfigUseStatement(stmt->getValue());
                break;
        }
    }
}

void UseStatementManager::applyHTML5UseStatement() {
    applyHTML5Configuration();
}

void UseStatementManager::applyConfigUseStatement(const std::string& configName) {
    applyConfigConfiguration(configName);
}

bool UseStatementManager::hasUseStatement(const std::string& value) const {
    return std::any_of(use_statements_.begin(), use_statements_.end(),
        [&value](const std::shared_ptr<UseStatement>& stmt) {
            return stmt->getValue() == value;
        });
}

bool UseStatementManager::hasHTML5UseStatement() const {
    return std::any_of(use_statements_.begin(), use_statements_.end(),
        [](const std::shared_ptr<UseStatement>& stmt) {
            return stmt->getType() == UseStatementType::HTML5;
        });
}

bool UseStatementManager::hasConfigUseStatement(const std::string& configName) const {
    return std::any_of(use_statements_.begin(), use_statements_.end(),
        [&configName](const std::shared_ptr<UseStatement>& stmt) {
            return stmt->getType() == UseStatementType::CONFIG && stmt->getValue() == configName;
        });
}

void UseStatementManager::clear() {
    clearUseStatements();
    config_manager_.reset();
}

void UseStatementManager::clearUseStatements() {
    use_statements_.clear();
}

void UseStatementManager::applyHTML5Configuration() {
    if (!config_manager_) {
        return;
    }
    
    // 应用HTML5默认配置
    auto defaultGroup = config_manager_->getDefaultConfigurationGroup();
    if (defaultGroup) {
        config_manager_->applyConfiguration(defaultGroup->getName());
    }
}

void UseStatementManager::applyConfigConfiguration(const std::string& configName) {
    if (!config_manager_) {
        return;
    }
    
    // 应用指定配置
    config_manager_->applyConfiguration(configName);
}

// UseStatementParser 实现
UseStatementParser::UseStatementParser() = default;

UseStatementParser::~UseStatementParser() = default;

std::shared_ptr<UseStatement> UseStatementParser::parseUseStatement(const std::string& content) {
    std::string trimmed = trim(content);
    
    if (isHTML5Statement(trimmed)) {
        auto useStatement = std::make_shared<UseStatement>(UseStatementType::HTML5);
        parseHTML5Statement(trimmed, *useStatement);
        return useStatement;
    } else if (isConfigStatement(trimmed)) {
        auto useStatement = std::make_shared<UseStatement>(UseStatementType::CONFIG);
        parseConfigStatement(trimmed, *useStatement);
        return useStatement;
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<UseStatement>> UseStatementParser::parseUseStatements(const std::string& content) {
    std::vector<std::shared_ptr<UseStatement>> statements;
    
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '/' || trimmed[0] == '-') {
            continue;
        }
        
        auto statement = parseUseStatement(trimmed);
        if (statement) {
            statements.push_back(statement);
        }
    }
    
    return statements;
}

bool UseStatementParser::parseHTML5Statement(const std::string& content, UseStatement& useStatement) {
    std::regex html5Regex(R"(use\s+html5\s*;?)");
    std::smatch match;
    
    if (std::regex_match(content, match, html5Regex)) {
        useStatement.setType(UseStatementType::HTML5);
        useStatement.setValue("html5");
        return true;
    }
    
    return false;
}

bool UseStatementParser::parseConfigStatement(const std::string& content, UseStatement& useStatement) {
    std::regex configRegex(R"(use\s+@Config\s+(\w+)\s*;?)");
    std::smatch match;
    
    if (std::regex_match(content, match, configRegex)) {
        useStatement.setType(UseStatementType::CONFIG);
        useStatement.setValue(match[1].str());
        return true;
    }
    
    return false;
}

bool UseStatementParser::validateUseStatement(const UseStatement& useStatement) const {
    return useStatement.validate();
}

std::vector<std::string> UseStatementParser::getValidationErrors(const UseStatement& useStatement) const {
    return useStatement.getValidationErrors();
}

std::string UseStatementParser::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> UseStatementParser::split(const std::string& str, char delimiter) const {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    
    return tokens;
}

bool UseStatementParser::isHTML5Statement(const std::string& content) const {
    std::regex html5Regex(R"(use\s+html5\s*;?)");
    return std::regex_match(content, html5Regex);
}

bool UseStatementParser::isConfigStatement(const std::string& content) const {
    std::regex configRegex(R"(use\s+@Config\s+\w+\s*;?)");
    return std::regex_match(content, configRegex);
}

std::string UseStatementParser::extractConfigName(const std::string& content) const {
    std::regex configRegex(R"(use\s+@Config\s+(\w+)\s*;?)");
    std::smatch match;
    
    if (std::regex_match(content, match, configRegex)) {
        return match[1].str();
    }
    
    return "";
}

} // namespace CHTL