#include "CHTLJSGlobalMap.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL_JS {

CHTLJSGlobalMap::CHTLJSGlobalMap() 
    : caseSensitive(true), strictMode(false), debugMode(false), languageVersion("1.0.0") {
    initialize();
}

void CHTLJSGlobalMap::initialize() {
    initializeKeywords();
    initializeOperators();
    initializePunctuation();
    initializeSyntaxBoundaries();
    initializeCHTLJSSyntax();
}

void CHTLJSGlobalMap::initializeKeywords() {
    // 基本关键字（仅CHTL JS需要的）
    addKeyword("true", CHTLJSTokenType::TRUE);
    addKeyword("false", CHTLJSTokenType::FALSE);
    addKeyword("null", CHTLJSTokenType::NULL_VALUE);
    addKeyword("undefined", CHTLJSTokenType::UNDEFINED);
    
    // CHTL JS特有关键字
    addKeyword("vir", CHTLJSTokenType::VIRTUAL_OBJECT);
    addKeyword("listen", CHTLJSTokenType::LISTEN);
    addKeyword("delegate", CHTLJSTokenType::DELEGATE);
    addKeyword("animate", CHTLJSTokenType::ANIMATE);
    addKeyword("router", CHTLJSTokenType::ROUTER);
    addKeyword("fileloader", CHTLJSTokenType::FILELOADER);
    addKeyword("iNeverAway", CHTLJSTokenType::INEVERAWAY);
    addKeyword("util", CHTLJSTokenType::UTIL);
    addKeyword("then", CHTLJSTokenType::THEN);
    addKeyword("change", CHTLJSTokenType::CHANGE);
}

void CHTLJSGlobalMap::initializeOperators() {
    // 基本运算符（仅CHTL JS需要的）
    addOperator("=", CHTLJSTokenType::ASSIGN);
    addOperator("->", CHTLJSTokenType::ARROW);
    addOperator(":", CHTLJSTokenType::COLON);
    addOperator(";", CHTLJSTokenType::SEMICOLON);
    addOperator(",", CHTLJSTokenType::COMMA);
}

void CHTLJSGlobalMap::initializePunctuation() {
    // 基本标点符号（仅CHTL JS需要的）
    addPunctuation('(', CHTLJSTokenType::LEFT_PAREN);
    addPunctuation(')', CHTLJSTokenType::RIGHT_PAREN);
    addPunctuation('[', CHTLJSTokenType::LEFT_BRACKET);
    addPunctuation(']', CHTLJSTokenType::RIGHT_BRACKET);
    addPunctuation('{', CHTLJSTokenType::LEFT_BRACE);
    addPunctuation('}', CHTLJSTokenType::RIGHT_BRACE);
    addPunctuation(';', CHTLJSTokenType::SEMICOLON);
    addPunctuation(',', CHTLJSTokenType::COMMA);
    addPunctuation(':', CHTLJSTokenType::COLON);
}

void CHTLJSGlobalMap::initializeSyntaxBoundaries() {
    // CHTL JS语法边界
    addSyntaxBoundary("{{", "}}");
    addSyntaxBoundary("vir ", " {");
    addSyntaxBoundary("listen ", " {");
    addSyntaxBoundary("delegate ", " {");
    addSyntaxBoundary("animate ", " {");
    addSyntaxBoundary("router ", " {");
    addSyntaxBoundary("fileloader ", " {");
    addSyntaxBoundary("util ", " ->");
    
    // JavaScript语法边界
    addSyntaxBoundary("function ", " {");
    addSyntaxBoundary("if ", " {");
    addSyntaxBoundary("for ", " {");
    addSyntaxBoundary("while ", " {");
    addSyntaxBoundary("switch ", " {");
    addSyntaxBoundary("try ", " {");
    addSyntaxBoundary("catch ", " {");
    addSyntaxBoundary("class ", " {");
    addSyntaxBoundary("interface ", " {");
    addSyntaxBoundary("enum ", " {");
    addSyntaxBoundary("namespace ", " {");
    addSyntaxBoundary("module ", " {");
}

void CHTLJSGlobalMap::initializeCHTLJSSyntax() {
    // CHTL JS特有关键字
    chtlJSKeywords = {
        "vir", "listen", "delegate", "animate", 
        "router", "fileloader", "util"
    };
    
    // CHTL JS函数映射
    chtlJSFunctionMap["printMylove"] = "printMylove({url: string, mode: string, width: string, height: string, scale: number})";
    chtlJSFunctionMap["iNeverAway"] = "iNeverAway({key: string, value: any})";
    chtlJSFunctionMap["util"] = "util expression -> change { condition } -> then { action }";
    
    // CHTL JS语法映射
    chtlJSSyntaxMap["{{selector}}"] = "增强选择器 - 创建DOM对象";
    chtlJSSyntaxMap["vir name ="] = "虚对象声明";
    chtlJSSyntaxMap["listen {"] = "增强监听器";
    chtlJSSyntaxMap["delegate {"] = "事件委托";
    chtlJSSyntaxMap["animate {"] = "动画定义";
    chtlJSSyntaxMap["router {"] = "路由定义";
    chtlJSSyntaxMap["fileloader {"] = "文件加载器";
    chtlJSSyntaxMap["util expression ->"] = "工具表达式";
}

// 关键字管理实现
void CHTLJSGlobalMap::addKeyword(const std::string& keyword, CHTLJSTokenType type) {
    std::string normalized = normalizeText(keyword);
    keywordMap[normalized] = type;
    reverseKeywordMap[type] = normalized;
}

void CHTLJSGlobalMap::removeKeyword(const std::string& keyword) {
    std::string normalized = normalizeText(keyword);
    auto it = keywordMap.find(normalized);
    if (it != keywordMap.end()) {
        reverseKeywordMap.erase(it->second);
        keywordMap.erase(it);
    }
}

bool CHTLJSGlobalMap::hasKeyword(const std::string& keyword) const {
    std::string normalized = normalizeText(keyword);
    return keywordMap.find(normalized) != keywordMap.end();
}

CHTLJSTokenType CHTLJSGlobalMap::getKeywordType(const std::string& keyword) const {
    std::string normalized = normalizeText(keyword);
    auto it = keywordMap.find(normalized);
    return it != keywordMap.end() ? it->second : CHTLJSTokenType::UNKNOWN;
}

std::string CHTLJSGlobalMap::getKeywordName(CHTLJSTokenType type) const {
    auto it = reverseKeywordMap.find(type);
    return it != reverseKeywordMap.end() ? it->second : "";
}

std::vector<std::string> CHTLJSGlobalMap::getAllKeywords() const {
    std::vector<std::string> keywords;
    for (const auto& pair : keywordMap) {
        keywords.push_back(pair.first);
    }
    return keywords;
}

// 运算符管理实现
void CHTLJSGlobalMap::addOperator(const std::string& op, CHTLJSTokenType type) {
    operatorMap[op] = type;
    reverseOperatorMap[type] = op;
}

void CHTLJSGlobalMap::removeOperator(const std::string& op) {
    auto it = operatorMap.find(op);
    if (it != operatorMap.end()) {
        reverseOperatorMap.erase(it->second);
        operatorMap.erase(it);
    }
}

bool CHTLJSGlobalMap::hasOperator(const std::string& op) const {
    return operatorMap.find(op) != operatorMap.end();
}

CHTLJSTokenType CHTLJSGlobalMap::getOperatorType(const std::string& op) const {
    auto it = operatorMap.find(op);
    return it != operatorMap.end() ? it->second : CHTLJSTokenType::UNKNOWN;
}

std::string CHTLJSGlobalMap::getOperatorName(CHTLJSTokenType type) const {
    auto it = reverseOperatorMap.find(type);
    return it != reverseOperatorMap.end() ? it->second : "";
}

std::vector<std::string> CHTLJSGlobalMap::getAllOperators() const {
    std::vector<std::string> operators;
    for (const auto& pair : operatorMap) {
        operators.push_back(pair.first);
    }
    return operators;
}

// 标点符号管理实现
void CHTLJSGlobalMap::addPunctuation(char punct, CHTLJSTokenType type) {
    punctuationMap[punct] = type;
    reversePunctuationMap[type] = punct;
}

void CHTLJSGlobalMap::removePunctuation(char punct) {
    auto it = punctuationMap.find(punct);
    if (it != punctuationMap.end()) {
        reversePunctuationMap.erase(it->second);
        punctuationMap.erase(it);
    }
}

bool CHTLJSGlobalMap::hasPunctuation(char punct) const {
    return punctuationMap.find(punct) != punctuationMap.end();
}

CHTLJSTokenType CHTLJSGlobalMap::getPunctuationType(char punct) const {
    auto it = punctuationMap.find(punct);
    return it != punctuationMap.end() ? it->second : CHTLJSTokenType::UNKNOWN;
}

char CHTLJSGlobalMap::getPunctuationChar(CHTLJSTokenType type) const {
    auto it = reversePunctuationMap.find(type);
    return it != reversePunctuationMap.end() ? it->second : '\0';
}

std::vector<char> CHTLJSGlobalMap::getAllPunctuation() const {
    std::vector<char> punctuation;
    for (const auto& pair : punctuationMap) {
        punctuation.push_back(pair.first);
    }
    return punctuation;
}

// 语法边界管理实现
void CHTLJSGlobalMap::addSyntaxBoundary(const std::string& start, const std::string& end) {
    syntaxBoundaryMap[start] = end;
}

void CHTLJSGlobalMap::removeSyntaxBoundary(const std::string& start) {
    syntaxBoundaryMap.erase(start);
}

bool CHTLJSGlobalMap::hasSyntaxBoundary(const std::string& start) const {
    return syntaxBoundaryMap.find(start) != syntaxBoundaryMap.end();
}

std::string CHTLJSGlobalMap::getSyntaxBoundaryEnd(const std::string& start) const {
    auto it = syntaxBoundaryMap.find(start);
    return it != syntaxBoundaryMap.end() ? it->second : "";
}

std::vector<std::string> CHTLJSGlobalMap::getAllSyntaxBoundaries() const {
    std::vector<std::string> boundaries;
    for (const auto& pair : syntaxBoundaryMap) {
        boundaries.push_back(pair.first);
    }
    return boundaries;
}

// CHTL JS语法管理实现
void CHTLJSGlobalMap::addCHTLJSKeyword(const std::string& keyword) {
    std::string normalized = normalizeText(keyword);
    if (std::find(chtlJSKeywords.begin(), chtlJSKeywords.end(), normalized) == chtlJSKeywords.end()) {
        chtlJSKeywords.push_back(normalized);
    }
}

void CHTLJSGlobalMap::removeCHTLJSKeyword(const std::string& keyword) {
    std::string normalized = normalizeText(keyword);
    chtlJSKeywords.erase(std::remove(chtlJSKeywords.begin(), chtlJSKeywords.end(), normalized), chtlJSKeywords.end());
}

bool CHTLJSGlobalMap::hasCHTLJSKeyword(const std::string& keyword) const {
    std::string normalized = normalizeText(keyword);
    return std::find(chtlJSKeywords.begin(), chtlJSKeywords.end(), normalized) != chtlJSKeywords.end();
}

std::vector<std::string> CHTLJSGlobalMap::getAllCHTLJSKeywords() const {
    return chtlJSKeywords;
}

void CHTLJSGlobalMap::addCHTLJSFunction(const std::string& name, const std::string& signature) {
    chtlJSFunctionMap[name] = signature;
}

void CHTLJSGlobalMap::removeCHTLJSFunction(const std::string& name) {
    chtlJSFunctionMap.erase(name);
}

bool CHTLJSGlobalMap::hasCHTLJSFunction(const std::string& name) const {
    return chtlJSFunctionMap.find(name) != chtlJSFunctionMap.end();
}

std::string CHTLJSGlobalMap::getCHTLJSFunctionSignature(const std::string& name) const {
    auto it = chtlJSFunctionMap.find(name);
    return it != chtlJSFunctionMap.end() ? it->second : "";
}

std::vector<std::string> CHTLJSGlobalMap::getAllCHTLJSFunctions() const {
    std::vector<std::string> functions;
    for (const auto& pair : chtlJSFunctionMap) {
        functions.push_back(pair.first);
    }
    return functions;
}

void CHTLJSGlobalMap::addCHTLJSSyntax(const std::string& syntax, const std::string& description) {
    chtlJSSyntaxMap[syntax] = description;
}

void CHTLJSGlobalMap::removeCHTLJSSyntax(const std::string& syntax) {
    chtlJSSyntaxMap.erase(syntax);
}

bool CHTLJSGlobalMap::hasCHTLJSSyntax(const std::string& syntax) const {
    return chtlJSSyntaxMap.find(syntax) != chtlJSSyntaxMap.end();
}

std::string CHTLJSGlobalMap::getCHTLJSSyntaxDescription(const std::string& syntax) const {
    auto it = chtlJSSyntaxMap.find(syntax);
    return it != chtlJSSyntaxMap.end() ? it->second : "";
}

std::vector<std::string> CHTLJSGlobalMap::getAllCHTLJSSyntax() const {
    std::vector<std::string> syntax;
    for (const auto& pair : chtlJSSyntaxMap) {
        syntax.push_back(pair.first);
    }
    return syntax;
}

// 占位符管理实现
void CHTLJSGlobalMap::addPlaceholder(const std::string& placeholder, const std::string& content) {
    placeholderMap[placeholder] = content;
    reversePlaceholderMap[content] = placeholder;
    activePlaceholders.push_back(placeholder);
}

void CHTLJSGlobalMap::removePlaceholder(const std::string& placeholder) {
    auto it = placeholderMap.find(placeholder);
    if (it != placeholderMap.end()) {
        reversePlaceholderMap.erase(it->second);
        placeholderMap.erase(it);
        activePlaceholders.erase(std::remove(activePlaceholders.begin(), activePlaceholders.end(), placeholder), activePlaceholders.end());
    }
}

bool CHTLJSGlobalMap::hasPlaceholder(const std::string& placeholder) const {
    return placeholderMap.find(placeholder) != placeholderMap.end();
}

std::string CHTLJSGlobalMap::getPlaceholderContent(const std::string& placeholder) const {
    auto it = placeholderMap.find(placeholder);
    return it != placeholderMap.end() ? it->second : "";
}

std::string CHTLJSGlobalMap::getPlaceholderByContent(const std::string& content) const {
    auto it = reversePlaceholderMap.find(content);
    return it != reversePlaceholderMap.end() ? it->second : "";
}

void CHTLJSGlobalMap::clearPlaceholders() {
    placeholderMap.clear();
    reversePlaceholderMap.clear();
    activePlaceholders.clear();
}

std::vector<std::string> CHTLJSGlobalMap::getAllPlaceholders() const {
    return activePlaceholders;
}

// 统计管理实现
void CHTLJSGlobalMap::incrementUsage(const std::string& item) {
    usageCount[item]++;
}

void CHTLJSGlobalMap::incrementTokenTypeUsage(CHTLJSTokenType type) {
    tokenTypeCount[type]++;
}

int CHTLJSGlobalMap::getUsageCount(const std::string& item) const {
    auto it = usageCount.find(item);
    return it != usageCount.end() ? it->second : 0;
}

int CHTLJSGlobalMap::getTokenTypeUsageCount(CHTLJSTokenType type) const {
    auto it = tokenTypeCount.find(type);
    return it != tokenTypeCount.end() ? it->second : 0;
}

void CHTLJSGlobalMap::resetStatistics() {
    usageCount.clear();
    tokenTypeCount.clear();
}

std::map<std::string, int> CHTLJSGlobalMap::getUsageStatistics() const {
    return usageCount;
}

std::map<CHTLJSTokenType, int> CHTLJSGlobalMap::getTokenTypeStatistics() const {
    return tokenTypeCount;
}

// 查找和匹配实现
CHTLJSTokenType CHTLJSGlobalMap::findTokenType(const std::string& text) const {
    // 首先检查关键字
    if (hasKeyword(text)) {
        return getKeywordType(text);
    }
    
    // 然后检查运算符
    if (hasOperator(text)) {
        return getOperatorType(text);
    }
    
    // 最后检查标点符号
    if (text.length() == 1 && hasPunctuation(text[0])) {
        return getPunctuationType(text[0]);
    }
    
    return CHTLJSTokenType::UNKNOWN;
}

std::string CHTLJSGlobalMap::findTokenName(CHTLJSTokenType type) const {
    // 首先检查关键字
    std::string keywordName = getKeywordName(type);
    if (!keywordName.empty()) {
        return keywordName;
    }
    
    // 然后检查运算符
    std::string operatorName = getOperatorName(type);
    if (!operatorName.empty()) {
        return operatorName;
    }
    
    // 最后检查标点符号
    char punctChar = getPunctuationChar(type);
    if (punctChar != '\0') {
        return std::string(1, punctChar);
    }
    
    return "";
}

bool CHTLJSGlobalMap::isReservedWord(const std::string& word) const {
    return hasKeyword(word) || hasCHTLJSKeyword(word);
}

bool CHTLJSGlobalMap::isBuiltinFunction(const std::string& function) const {
    // 检查JavaScript内置函数
    std::vector<std::string> builtins = {
        "console", "alert", "confirm", "prompt", "parseInt", "parseFloat",
        "isNaN", "isFinite", "eval", "encodeURI", "decodeURI", "encodeURIComponent",
        "decodeURIComponent", "setTimeout", "setInterval", "clearTimeout", "clearInterval"
    };
    
    return std::find(builtins.begin(), builtins.end(), function) != builtins.end();
}

bool CHTLJSGlobalMap::isCHTLJSFunction(const std::string& function) const {
    return hasCHTLJSFunction(function);
}

// 验证实现
bool CHTLJSGlobalMap::validateKeyword(const std::string& keyword) const {
    return isValidKeyword(keyword);
}

bool CHTLJSGlobalMap::validateOperator(const std::string& op) const {
    return isValidOperator(op);
}

bool CHTLJSGlobalMap::validatePunctuation(char punct) const {
    return isValidPunctuation(punct);
}

bool CHTLJSGlobalMap::validateSyntax(const std::string& syntax) const {
    return isValidSyntax(syntax);
}

bool CHTLJSGlobalMap::validateIdentifier(const std::string& identifier) const {
    if (identifier.empty()) return false;
    
    // 标识符必须以字母、下划线或美元符号开头
    if (!std::isalpha(identifier[0]) && identifier[0] != '_' && identifier[0] != '$') {
        return false;
    }
    
    // 其余字符必须是字母、数字、下划线或美元符号
    for (size_t i = 1; i < identifier.length(); i++) {
        if (!std::isalnum(identifier[i]) && identifier[i] != '_' && identifier[i] != '$') {
            return false;
        }
    }
    
    // 不能是保留字
    if (isReservedWord(identifier)) {
        return false;
    }
    
    return true;
}

// 转换实现
std::string CHTLJSGlobalMap::toJavaScript(const std::string& chtljs) const {
    return convertToJavaScript(chtljs);
}

std::string CHTLJSGlobalMap::toCHTLJS(const std::string& javascript) const {
    return convertToCHTLJS(javascript);
}

std::string CHTLJSGlobalMap::toHTML(const std::string& chtljs) const {
    return convertToHTML(chtljs);
}

std::string CHTLJSGlobalMap::toCSS(const std::string& chtljs) const {
    return convertToCSS(chtljs);
}

// 调试和诊断实现
std::string CHTLJSGlobalMap::getDebugInfo() const {
    return formatDebugInfo();
}

std::string CHTLJSGlobalMap::getStatisticsInfo() const {
    return formatStatistics();
}

void CHTLJSGlobalMap::printDebugInfo() const {
    std::cout << getDebugInfo() << std::endl;
}

void CHTLJSGlobalMap::printStatistics() const {
    std::cout << getStatisticsInfo() << std::endl;
}

// 辅助方法实现
std::string CHTLJSGlobalMap::normalizeText(const std::string& text) const {
    if (caseSensitive) {
        return text;
    }
    
    std::string normalized = text;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(), ::tolower);
    return normalized;
}

bool CHTLJSGlobalMap::matchesPattern(const std::string& text, const std::string& pattern) const {
    try {
        std::regex regexPattern(pattern);
        return std::regex_match(text, regexPattern);
    } catch (const std::regex_error&) {
        return false;
    }
}

std::vector<std::string> CHTLJSGlobalMap::splitPattern(const std::string& pattern) const {
    std::vector<std::string> parts;
    std::stringstream ss(pattern);
    std::string part;
    
    while (std::getline(ss, part, '|')) {
        parts.push_back(part);
    }
    
    return parts;
}

bool CHTLJSGlobalMap::isPatternMatch(const std::string& text, const std::vector<std::string>& patterns) const {
    for (const auto& pattern : patterns) {
        if (matchesPattern(text, pattern)) {
            return true;
        }
    }
    return false;
}

// 内部验证实现
bool CHTLJSGlobalMap::isValidKeyword(const std::string& keyword) const {
    return !keyword.empty() && std::all_of(keyword.begin(), keyword.end(), ::isalpha);
}

bool CHTLJSGlobalMap::isValidOperator(const std::string& op) const {
    return !op.empty() && std::all_of(op.begin(), op.end(), [](char c) {
        return std::isalnum(c) || c == '+' || c == '-' || c == '*' || c == '/' || 
               c == '%' || c == '=' || c == '!' || c == '<' || c == '>' || 
               c == '&' || c == '|' || c == '^' || c == '~' || c == '?' || c == '.';
    });
}

bool CHTLJSGlobalMap::isValidPunctuation(char punct) const {
    return std::ispunct(punct) && punct != '_' && punct != '$';
}

bool CHTLJSGlobalMap::isValidSyntax(const std::string& syntax) const {
    return !syntax.empty() && syntax.length() > 1;
}

// 转换辅助实现
std::string CHTLJSGlobalMap::convertToJavaScript(const std::string& chtljs) const {
    std::string result = chtljs;
    
    // 转换增强选择器
    std::regex enhancedSelectorRegex(R"(\{\{([^}]+)\}\})");
    result = std::regex_replace(result, enhancedSelectorRegex, "document.querySelector('$1')");
    
    // 转换箭头运算符
    std::regex arrowRegex(R"(\s*->\s*)");
    result = std::regex_replace(result, arrowRegex, " => ");
    
    // 转换虚对象
    std::regex virtualObjectRegex(R"(\bvir\s+(\w+)\s*=)");
    result = std::regex_replace(result, virtualObjectRegex, "const $1 =");
    
    return result;
}

std::string CHTLJSGlobalMap::convertToCHTLJS(const std::string& javascript) const {
    std::string result = javascript;
    
    // 转换箭头运算符
    std::regex arrowRegex(R"(\s*=>\s*)");
    result = std::regex_replace(result, arrowRegex, " -> ");
    
    // 转换选择器
    std::regex selectorRegex(R"(document\.querySelector\('([^']+)'\))");
    result = std::regex_replace(result, selectorRegex, "{{$1}}");
    
    return result;
}

std::string CHTLJSGlobalMap::convertToHTML(const std::string& chtljs) const {
    // HTML转换逻辑
    return chtljs;
}

std::string CHTLJSGlobalMap::convertToCSS(const std::string& chtljs) const {
    // CSS转换逻辑
    return chtljs;
}

// 统计辅助实现
void CHTLJSGlobalMap::updateUsageCount(const std::string& item) {
    usageCount[item]++;
}

void CHTLJSGlobalMap::updateTokenTypeCount(CHTLJSTokenType type) {
    tokenTypeCount[type]++;
}

// 调试辅助实现
std::string CHTLJSGlobalMap::formatDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLJSGlobalMap Debug Info:\n";
    oss << "Keywords: " << keywordMap.size() << "\n";
    oss << "Operators: " << operatorMap.size() << "\n";
    oss << "Punctuation: " << punctuationMap.size() << "\n";
    oss << "Syntax Boundaries: " << syntaxBoundaryMap.size() << "\n";
    oss << "CHTL JS Keywords: " << chtlJSKeywords.size() << "\n";
    oss << "CHTL JS Functions: " << chtlJSFunctionMap.size() << "\n";
    oss << "CHTL JS Syntax: " << chtlJSSyntaxMap.size() << "\n";
    oss << "Placeholders: " << placeholderMap.size() << "\n";
    oss << "Case Sensitive: " << (caseSensitive ? "Yes" : "No") << "\n";
    oss << "Strict Mode: " << (strictMode ? "Yes" : "No") << "\n";
    oss << "Debug Mode: " << (debugMode ? "Yes" : "No") << "\n";
    oss << "Language Version: " << languageVersion << "\n";
    return oss.str();
}

std::string CHTLJSGlobalMap::formatStatistics() const {
    std::ostringstream oss;
    oss << "CHTLJSGlobalMap Statistics:\n";
    oss << "Total Usage Count: " << usageCount.size() << "\n";
    oss << "Total Token Type Count: " << tokenTypeCount.size() << "\n";
    
    if (!usageCount.empty()) {
        oss << "\nTop 10 Most Used Items:\n";
        std::vector<std::pair<std::string, int>> sortedUsage(usageCount.begin(), usageCount.end());
        std::sort(sortedUsage.begin(), sortedUsage.end(), 
                 [](const auto& a, const auto& b) { return a.second > b.second; });
        
        for (size_t i = 0; i < std::min(size_t(10), sortedUsage.size()); i++) {
            oss << "  " << sortedUsage[i].first << ": " << sortedUsage[i].second << "\n";
        }
    }
    
    return oss.str();
}

// 其他方法的简化实现
bool CHTLJSGlobalMap::loadFromFile(const std::string& filename) { return false; }
bool CHTLJSGlobalMap::saveToFile(const std::string& filename) const { return false; }
std::string CHTLJSGlobalMap::exportToJSON() const { return ""; }
bool CHTLJSGlobalMap::importFromJSON(const std::string& json) { return false; }
void CHTLJSGlobalMap::reset() { clear(); }
void CHTLJSGlobalMap::clear() { 
    clearKeywords(); 
    clearOperators(); 
    clearPunctuation(); 
    clearSyntaxBoundaries(); 
    clearCHTLJSSyntax(); 
    clearPlaceholders(); 
}
void CHTLJSGlobalMap::clearKeywords() { keywordMap.clear(); reverseKeywordMap.clear(); }
void CHTLJSGlobalMap::clearOperators() { operatorMap.clear(); reverseOperatorMap.clear(); }
void CHTLJSGlobalMap::clearPunctuation() { punctuationMap.clear(); reversePunctuationMap.clear(); }
void CHTLJSGlobalMap::clearSyntaxBoundaries() { syntaxBoundaryMap.clear(); }
void CHTLJSGlobalMap::clearCHTLJSSyntax() { 
    chtlJSKeywords.clear(); 
    chtlJSFunctionMap.clear(); 
    chtlJSSyntaxMap.clear(); 
}
void CHTLJSGlobalMap::optimize() {}
void CHTLJSGlobalMap::compress() {}
void CHTLJSGlobalMap::decompress() {}
void CHTLJSGlobalMap::addCustomKeyword(const std::string& keyword, CHTLJSTokenType type) { addKeyword(keyword, type); }
void CHTLJSGlobalMap::addCustomOperator(const std::string& op, CHTLJSTokenType type) { addOperator(op, type); }
void CHTLJSGlobalMap::addCustomSyntax(const std::string& syntax, const std::string& description) { addCHTLJSSyntax(syntax, description); }
bool CHTLJSGlobalMap::isCompatible(const std::string& version) const { return true; }
std::vector<std::string> CHTLJSGlobalMap::getCompatibilityIssues() const { return {}; }
void CHTLJSGlobalMap::fixCompatibilityIssues() {}

} // namespace CHTL_JS