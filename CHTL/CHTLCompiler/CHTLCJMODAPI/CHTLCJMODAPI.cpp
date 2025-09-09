#include "CHTLCJMODAPI.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>
#include <set>

namespace CHTL {

// CHTLSyntax implementation
CHTLSyntax::CHTLSyntax() {
}

CHTLSyntax::~CHTLSyntax() {
}

SyntaxType CHTLSyntax::analyze(const std::string& code) const {
    std::string trimmed = trimWhitespace(code);
    
    if (isObject(trimmed)) {
        return SyntaxType::Object;
    } else if (isFunction(trimmed)) {
        return SyntaxType::Function;
    } else if (isArray(trimmed)) {
        return SyntaxType::Array;
    } else if (isString(trimmed)) {
        return SyntaxType::String;
    } else if (isNumeric(trimmed)) {
        return SyntaxType::Number;
    } else if (isBoolean(trimmed)) {
        return SyntaxType::Boolean;
    } else if (isNull(trimmed)) {
        return SyntaxType::Null;
    } else if (isUndefined(trimmed)) {
        return SyntaxType::Undefined;
    } else if (isCHTLJSFunction(trimmed)) {
        return SyntaxType::CHTLJSFunction;
    }
    
    return SyntaxType::String; // Default
}

bool CHTLSyntax::isObject(const std::string& code) const {
    std::string trimmed = trimWhitespace(code);
    return trimmed.length() >= 2 && trimmed[0] == '{' && trimmed[trimmed.length() - 1] == '}' && isBalanced(trimmed, '{', '}');
}

bool CHTLSyntax::isFunction(const std::string& code) const {
    std::string trimmed = trimWhitespace(code);
    return trimmed.find("function") == 0 || trimmed.find("=>") != std::string::npos;
}

bool CHTLSyntax::isArray(const std::string& code) const {
    std::string trimmed = trimWhitespace(code);
    return trimmed.length() >= 2 && trimmed[0] == '[' && trimmed[trimmed.length() - 1] == ']' && isBalanced(trimmed, '[', ']');
}

bool CHTLSyntax::isCHTLJSFunction(const std::string& code) const {
    std::string trimmed = trimWhitespace(code);
    return trimmed.find("chtl::") != std::string::npos || trimmed.find("CHTLJS") != std::string::npos;
}

bool CHTLSyntax::isValidSyntax(const std::string& code) const {
    return getSyntaxErrors(code).empty();
}

std::vector<std::string> CHTLSyntax::getSyntaxErrors(const std::string& code) const {
    std::vector<std::string> errors;
    
    // Check for balanced brackets
    if (!isBalanced(code, '{', '}')) {
        errors.push_back("Unbalanced curly braces");
    }
    
    if (!isBalanced(code, '[', ']')) {
        errors.push_back("Unbalanced square brackets");
    }
    
    if (!isBalanced(code, '(', ')')) {
        errors.push_back("Unbalanced parentheses");
    }
    
    return errors;
}

std::map<std::string, std::string> CHTLSyntax::parseObject(const std::string& code) const {
    std::map<std::string, std::string> result;
    
    if (!isObject(code)) {
        return result;
    }
    
    std::string content = code.substr(1, code.length() - 2);
    std::stringstream ss(content);
    std::string line;
    
    while (std::getline(ss, line, ',')) {
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = trimWhitespace(line.substr(0, colon_pos));
            std::string value = trimWhitespace(line.substr(colon_pos + 1));
            
            // Remove quotes from key
            if (key.length() >= 2 && key[0] == '"' && key[key.length() - 1] == '"') {
                key = key.substr(1, key.length() - 2);
            }
            
            result[key] = value;
        }
    }
    
    return result;
}

std::vector<std::string> CHTLSyntax::parseArray(const std::string& code) const {
    std::vector<std::string> result;
    
    if (!isArray(code)) {
        return result;
    }
    
    std::string content = code.substr(1, code.length() - 2);
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        result.push_back(trimWhitespace(item));
    }
    
    return result;
}

std::string CHTLSyntax::parseFunction(const std::string& code) const {
    if (!isFunction(code)) {
        return "";
    }
    
    // Simple function parsing
    size_t start = code.find('{');
    size_t end = code.rfind('}');
    
    if (start != std::string::npos && end != std::string::npos && end > start) {
        return code.substr(start + 1, end - start - 1);
    }
    
    return "";
}

bool CHTLSyntax::isBalanced(const std::string& code, char open, char close) const {
    int count = 0;
    for (char c : code) {
        if (c == open) {
            count++;
        } else if (c == close) {
            count--;
            if (count < 0) {
                return false;
            }
        }
    }
    return count == 0;
}

std::string CHTLSyntax::trimWhitespace(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::vector<std::string> CHTLSyntax::tokenize(const std::string& code) const {
    std::vector<std::string> tokens;
    std::string current;
    
    for (char c : code) {
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')' || c == ',' || c == ':') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

bool CHTLSyntax::isNumeric(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[0] == '-' || str[0] == '+') pos++;
    if (pos >= str.length()) return false;
    
    bool has_digit = false;
    bool has_dot = false;
    
    for (size_t i = pos; i < str.length(); ++i) {
        if (std::isdigit(str[i])) {
            has_digit = true;
        } else if (str[i] == '.') {
            if (has_dot) return false;
            has_dot = true;
        } else {
            return false;
        }
    }
    
    return has_digit;
}

bool CHTLSyntax::isString(const std::string& str) const {
    return str.length() >= 2 && ((str[0] == '"' && str[str.length() - 1] == '"') ||
                                 (str[0] == '\'' && str[str.length() - 1] == '\''));
}

bool CHTLSyntax::isBoolean(const std::string& str) const {
    return str == "true" || str == "false";
}

bool CHTLSyntax::isNull(const std::string& str) const {
    return str == "null";
}

bool CHTLSyntax::isUndefined(const std::string& str) const {
    return str == "undefined";
}

// CHTLArgProcessor implementation
CHTLArgProcessor::CHTLArgProcessor() {
}

CHTLArgProcessor::~CHTLArgProcessor() {
}

CHTLArg CHTLArgProcessor::bind(const std::string& name, const CHTLArg& value) {
    bound_args[name] = value;
    return value;
}

CHTLArg CHTLArgProcessor::bind(const std::string& name, const std::string& value) {
    CHTLArg arg(ArgType::String, value);
    bound_args[name] = arg;
    return arg;
}

CHTLArg CHTLArgProcessor::bind(const std::string& name, int value) {
    CHTLArg arg(ArgType::Number, std::to_string(value));
    bound_args[name] = arg;
    return arg;
}

CHTLArg CHTLArgProcessor::bind(const std::string& name, bool value) {
    CHTLArg arg(ArgType::Boolean, value ? "true" : "false");
    bound_args[name] = arg;
    return arg;
}

CHTLArg CHTLArgProcessor::fillValue(const CHTLArg& arg, const std::map<std::string, CHTLArg>& context) {
    if (arg.is_placeholder) {
        return processPlaceholder(arg.placeholder_type, context);
    }
    
    CHTLArg result = arg;
    for (auto& child : result.children) {
        child = fillValue(child, context);
    }
    
    return result;
}

std::string CHTLArgProcessor::fillValue(const std::string& template_str, const std::map<std::string, CHTLArg>& context) {
    return replacePlaceholders(template_str, context);
}

CHTLArg CHTLArgProcessor::transform(const CHTLArg& arg, const std::function<CHTLArg(const CHTLArg&)>& transformer) {
    CHTLArg result = transformer(arg);
    
    for (auto& child : result.children) {
        child = transform(child, transformer);
    }
    
    return result;
}

std::vector<CHTLArg> CHTLArgProcessor::transformArray(const std::vector<CHTLArg>& args, const std::function<CHTLArg(const CHTLArg&)>& transformer) {
    std::vector<CHTLArg> result;
    
    for (const auto& arg : args) {
        result.push_back(transform(arg, transformer));
    }
    
    return result;
}

CHTLArg CHTLArgProcessor::createPlaceholder(const std::string& type) {
    CHTLArg arg;
    arg.is_placeholder = true;
    arg.placeholder_type = type;
    return arg;
}

bool CHTLArgProcessor::isPlaceholder(const CHTLArg& arg) const {
    return arg.is_placeholder;
}

std::string CHTLArgProcessor::getPlaceholderType(const CHTLArg& arg) const {
    return arg.placeholder_type;
}

bool CHTLArgProcessor::validateArg(const CHTLArg& arg) const {
    return !arg.value.empty() || !arg.children.empty() || arg.is_placeholder;
}

std::vector<std::string> CHTLArgProcessor::getArgErrors(const CHTLArg& arg) const {
    std::vector<std::string> errors;
    
    if (!validateArg(arg)) {
        errors.push_back("Invalid argument structure");
    }
    
    return errors;
}

std::string CHTLArgProcessor::replacePlaceholders(const std::string& str, const std::map<std::string, CHTLArg>& context) const {
    std::string result = str;
    
    for (const auto& pair : context) {
        std::string placeholder = "${" + pair.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second.value);
            pos += pair.second.value.length();
        }
    }
    
    return result;
}

CHTLArg CHTLArgProcessor::processPlaceholder(const std::string& placeholder, const std::map<std::string, CHTLArg>& context) const {
    auto it = context.find(placeholder);
    if (it != context.end()) {
        return it->second;
    }
    
    // Return empty arg if placeholder not found
    return CHTLArg();
}

bool CHTLArgProcessor::isValidPlaceholder(const std::string& placeholder) const {
    return !placeholder.empty() && placeholder[0] != ' ' && placeholder[placeholder.length() - 1] != ' ';
}

std::string CHTLArgProcessor::extractPlaceholderType(const std::string& placeholder) const {
    if (placeholder.length() >= 2 && placeholder[0] == '$' && placeholder[1] == '{') {
        return placeholder.substr(2, placeholder.length() - 3);
    }
    return placeholder;
}

// CHTLCJMODScanner implementation
CHTLCJMODScanner::CHTLCJMODScanner() {
}

CHTLCJMODScanner::~CHTLCJMODScanner() {
}

std::vector<std::string> CHTLCJMODScanner::scan(const std::string& code) const {
    return tokenize(code);
}

std::vector<std::string> CHTLCJMODScanner::scanFile(const std::string& path) const {
    std::string content = readFile(path);
    if (content.empty()) {
        return {};
    }
    return scan(content);
}

std::vector<std::string> CHTLCJMODScanner::scanDirectory(const std::string& directory) const {
    std::vector<std::string> results;
    auto files = listFiles(directory);
    
    for (const auto& file : files) {
        auto file_results = scanFile(file);
        results.insert(results.end(), file_results.begin(), file_results.end());
    }
    
    return results;
}

std::vector<std::string> CHTLCJMODScanner::findPatterns(const std::string& code, const std::string& pattern) const {
    std::vector<std::string> results;
    std::stringstream ss(code);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (matchesPattern(line, pattern)) {
            results.push_back(line);
        }
    }
    
    return results;
}

std::vector<std::string> CHTLCJMODScanner::findImports(const std::string& code) const {
    std::vector<std::string> imports;
    std::stringstream ss(code);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (matchesPattern(line, "import")) {
            std::string import_str = extractImport(line);
            if (!import_str.empty()) {
                imports.push_back(import_str);
            }
        }
    }
    
    return imports;
}

std::vector<std::string> CHTLCJMODScanner::findExports(const std::string& code) const {
    std::vector<std::string> exports;
    std::stringstream ss(code);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (matchesPattern(line, "export")) {
            std::string export_str = extractExport(line);
            if (!export_str.empty()) {
                exports.push_back(export_str);
            }
        }
    }
    
    return exports;
}

std::vector<std::string> CHTLCJMODScanner::findFunctions(const std::string& code) const {
    std::vector<std::string> functions;
    std::stringstream ss(code);
    std::string line;
    
    while (std::getline(ss, line)) {
        if (matchesPattern(line, "function")) {
            std::string func_str = extractFunction(line);
            if (!func_str.empty()) {
                functions.push_back(func_str);
            }
        }
    }
    
    return functions;
}

std::map<std::string, std::string> CHTLCJMODScanner::analyzeCode(const std::string& code) const {
    std::map<std::string, std::string> analysis;
    
    analysis["imports"] = std::to_string(findImports(code).size());
    analysis["exports"] = std::to_string(findExports(code).size());
    analysis["functions"] = std::to_string(findFunctions(code).size());
    analysis["lines"] = std::to_string(std::count(code.begin(), code.end(), '\n') + 1);
    
    return analysis;
}

std::vector<std::string> CHTLCJMODScanner::getDependencies(const std::string& code) const {
    return findImports(code);
}

std::vector<std::string> CHTLCJMODScanner::getExports(const std::string& code) const {
    return findExports(code);
}

bool CHTLCJMODScanner::isValidFile(const std::string& path) const {
    return isCJMODFile(path);
}

std::string CHTLCJMODScanner::readFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<std::string> CHTLCJMODScanner::listFiles(const std::string& /* directory */) const {
    std::vector<std::string> files;
    // Simplified implementation - would use filesystem in real implementation
    return files;
}

std::vector<std::string> CHTLCJMODScanner::tokenize(const std::string& code) const {
    std::vector<std::string> tokens;
    std::string current;
    
    for (char c : code) {
        if (std::isspace(c)) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (c == '{' || c == '}' || c == '[' || c == ']' || c == '(' || c == ')' || c == ',' || c == ':' || c == ';') {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
            tokens.push_back(std::string(1, c));
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

bool CHTLCJMODScanner::matchesPattern(const std::string& line, const std::string& pattern) const {
    return line.find(pattern) != std::string::npos;
}

std::string CHTLCJMODScanner::extractImport(const std::string& line) const {
    size_t start = line.find("import");
    if (start == std::string::npos) return "";
    
    size_t end = line.find(";", start);
    if (end == std::string::npos) end = line.length();
    
    return line.substr(start, end - start);
}

std::string CHTLCJMODScanner::extractExport(const std::string& line) const {
    size_t start = line.find("export");
    if (start == std::string::npos) return "";
    
    size_t end = line.find(";", start);
    if (end == std::string::npos) end = line.length();
    
    return line.substr(start, end - start);
}

std::string CHTLCJMODScanner::extractFunction(const std::string& line) const {
    size_t start = line.find("function");
    if (start == std::string::npos) return "";
    
    size_t end = line.find("{", start);
    if (end == std::string::npos) end = line.length();
    
    return line.substr(start, end - start);
}

bool CHTLCJMODScanner::isCJMODFile(const std::string& path) const {
    return path.length() >= 5 && path.substr(path.length() - 5) == ".cjjs";
}

// CHTLCJMODGenerator implementation
CHTLCJMODGenerator::CHTLCJMODGenerator() {
}

CHTLCJMODGenerator::~CHTLCJMODGenerator() {
}

std::string CHTLCJMODGenerator::generateModule(const std::string& name, const std::map<std::string, std::string>& exports) const {
    std::stringstream ss;
    ss << "// CHTL Module: " << name << "\n";
    ss << "module.exports = {\n";
    
    for (auto it = exports.begin(); it != exports.end(); ++it) {
        ss << "  " << it->first << ": " << it->second;
        if (std::next(it) != exports.end()) {
            ss << ",";
        }
        ss << "\n";
    }
    
    ss << "};\n";
    return ss.str();
}

std::string CHTLCJMODGenerator::generateImport(const std::string& module, const std::string& name) const {
    return "const " + name + " = require('" + module + "');";
}

std::string CHTLCJMODGenerator::generateExport(const std::string& name, const std::string& value) const {
    return "exports." + name + " = " + value + ";";
}

std::string CHTLCJMODGenerator::generateFunction(const std::string& name, const std::string& body) const {
    return "function " + name + "() {\n" + indentCode(body, 1) + "\n}";
}

std::string CHTLCJMODGenerator::generateTemplate(const std::string& template_name, const std::map<std::string, std::string>& params) const {
    std::stringstream ss;
    ss << "// Template: " << template_name << "\n";
    ss << "function " << template_name << "(";
    
    bool first = true;
    for (const auto& param : params) {
        if (!first) ss << ", ";
        ss << param.first;
        first = false;
    }
    
    ss << ") {\n";
    ss << "  // Template body\n";
    ss << "}\n";
    
    return ss.str();
}

std::string CHTLCJMODGenerator::generateCHTLJSFunction(const std::string& name, const std::string& body) const {
    return "chtl::" + name + " = function() {\n" + indentCode(body, 1) + "\n};";
}

std::string CHTLCJMODGenerator::formatCode(const std::string& code) const {
    return beautifyCode(code);
}

std::string CHTLCJMODGenerator::minifyCode(const std::string& code) const {
    std::string result = removeComments(code);
    result = removeWhitespace(result);
    return result;
}

std::string CHTLCJMODGenerator::beautifyCode(const std::string& code) const {
    std::string result = code;
    int indent_level = 0;
    std::stringstream ss;
    
    for (size_t i = 0; i < result.length(); ++i) {
        char c = result[i];
        
        if (c == '{' || c == '[') {
            ss << c << "\n";
            indent_level++;
            for (int j = 0; j < indent_level; ++j) {
                ss << "  ";
            }
        } else if (c == '}' || c == ']') {
            ss << "\n";
            indent_level--;
            for (int j = 0; j < indent_level; ++j) {
                ss << "  ";
            }
            ss << c;
        } else if (c == ';') {
            ss << c << "\n";
            for (int j = 0; j < indent_level; ++j) {
                ss << "  ";
            }
        } else {
            ss << c;
        }
    }
    
    return ss.str();
}

std::string CHTLCJMODGenerator::exportResult(const std::string& code, const std::string& format) const {
    if (format == "js") {
        return code;
    } else if (format == "min") {
        return minifyCode(code);
    } else if (format == "beautify") {
        return beautifyCode(code);
    }
    
    return code;
}

std::string CHTLCJMODGenerator::exportToFile(const std::string& code, const std::string& path) const {
    std::ofstream file(path);
    if (file.is_open()) {
        file << code;
        file.close();
        return "Successfully exported to " + path;
    }
    return "Failed to export to " + path;
}

std::string CHTLCJMODGenerator::exportToModule(const std::string& code, const std::string& module_name) const {
    return generateModule(module_name, {{"code", code}});
}

std::string CHTLCJMODGenerator::indentCode(const std::string& code, int level) const {
    std::string result = code;
    std::string indent(level * 2, ' ');
    
    std::stringstream ss;
    std::stringstream input(code);
    std::string line;
    
    while (std::getline(input, line)) {
        ss << indent << line << "\n";
    }
    
    return ss.str();
}

std::string CHTLCJMODGenerator::removeComments(const std::string& code) const {
    std::string result = code;
    
    // Remove single-line comments
    size_t pos = 0;
    while ((pos = result.find("//", pos)) != std::string::npos) {
        size_t end = result.find("\n", pos);
        if (end == std::string::npos) {
            result = result.substr(0, pos);
            break;
        }
        result.erase(pos, end - pos);
    }
    
    // Remove multi-line comments
    pos = 0;
    while ((pos = result.find("/*", pos)) != std::string::npos) {
        size_t end = result.find("*/", pos);
        if (end == std::string::npos) {
            result = result.substr(0, pos);
            break;
        }
        result.erase(pos, end - pos + 2);
    }
    
    return result;
}

std::string CHTLCJMODGenerator::removeWhitespace(const std::string& code) const {
    std::string result;
    bool in_string = false;
    char string_char = 0;
    
    for (size_t i = 0; i < code.length(); ++i) {
        char c = code[i];
        
        if (!in_string && (c == '"' || c == '\'')) {
            in_string = true;
            string_char = c;
        } else if (in_string && c == string_char) {
            in_string = false;
        }
        
        if (!in_string && std::isspace(c)) {
            continue;
        }
        
        result += c;
    }
    
    return result;
}

std::string CHTLCJMODGenerator::validateCode(const std::string& code) const {
    // Simple validation
    int brace_count = 0;
    int paren_count = 0;
    int bracket_count = 0;
    
    for (char c : code) {
        switch (c) {
            case '{': brace_count++; break;
            case '}': brace_count--; break;
            case '(': paren_count++; break;
            case ')': paren_count--; break;
            case '[': bracket_count++; break;
            case ']': bracket_count--; break;
        }
    }
    
    if (brace_count != 0 || paren_count != 0 || bracket_count != 0) {
        return "Unbalanced brackets";
    }
    
    return "Valid";
}

std::string CHTLCJMODGenerator::escapeString(const std::string& str) const {
    std::string result;
    
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

// CHTLAtomArg implementation
CHTLAtomArg::CHTLAtomArg() {
}

CHTLAtomArg::~CHTLAtomArg() {
}

CHTLArg CHTLAtomArg::createDollarArg() const {
    return createPlaceholderArg(AtomArgType::Dollar);
}

CHTLArg CHTLAtomArg::createDollarQuestionArg() const {
    return createPlaceholderArg(AtomArgType::DollarQuestion);
}

CHTLArg CHTLAtomArg::createDollarExclamationArg() const {
    return createPlaceholderArg(AtomArgType::DollarExclamation);
}

CHTLArg CHTLAtomArg::createDollarUnderscoreArg() const {
    return createPlaceholderArg(AtomArgType::DollarUnderscore);
}

CHTLArg CHTLAtomArg::createEllipsisArg() const {
    return createPlaceholderArg(AtomArgType::Ellipsis);
}

CHTLArg CHTLAtomArg::processAtomArg(const std::string& atom, const std::map<std::string, CHTLArg>& /* context */) const {
    AtomArgType type = getAtomType(atom);
    
    switch (type) {
        case AtomArgType::Dollar:
            return createDollarArg();
        case AtomArgType::DollarQuestion:
            return createDollarQuestionArg();
        case AtomArgType::DollarExclamation:
            return createDollarExclamationArg();
        case AtomArgType::DollarUnderscore:
            return createDollarUnderscoreArg();
        case AtomArgType::Ellipsis:
            return createEllipsisArg();
        default:
            return CHTLArg();
    }
}

std::vector<CHTLArg> CHTLAtomArg::processAtomArgs(const std::vector<std::string>& atoms, const std::map<std::string, CHTLArg>& context) const {
    std::vector<CHTLArg> result;
    
    for (const auto& atom : atoms) {
        result.push_back(processAtomArg(atom, context));
    }
    
    return result;
}

bool CHTLAtomArg::isValidAtom(const std::string& atom) const {
    return atom == "$" || atom == "$?" || atom == "$!" || atom == "$_" || atom == "...";
}

AtomArgType CHTLAtomArg::getAtomType(const std::string& atom) const {
    if (atom == "$") return AtomArgType::Dollar;
    if (atom == "$?") return AtomArgType::DollarQuestion;
    if (atom == "$!") return AtomArgType::DollarExclamation;
    if (atom == "$_") return AtomArgType::DollarUnderscore;
    if (atom == "...") return AtomArgType::Ellipsis;
    return AtomArgType::Dollar; // Default
}

std::string CHTLAtomArg::generatePlaceholder(const AtomArgType& type) const {
    return atomTypeToString(type);
}

std::string CHTLAtomArg::generatePlaceholder(const std::string& atom) const {
    return atom;
}

CHTLArg CHTLAtomArg::createPlaceholderArg(const AtomArgType& type) const {
    CHTLArg arg;
    arg.type = ArgType::Placeholder;
    arg.is_placeholder = true;
    arg.placeholder_type = atomTypeToString(type);
    return arg;
}

std::string CHTLAtomArg::atomTypeToString(const AtomArgType& type) const {
    switch (type) {
        case AtomArgType::Dollar: return "$";
        case AtomArgType::DollarQuestion: return "$?";
        case AtomArgType::DollarExclamation: return "$!";
        case AtomArgType::DollarUnderscore: return "$_";
        case AtomArgType::Ellipsis: return "...";
        default: return "$";
    }
}

AtomArgType CHTLAtomArg::stringToAtomType(const std::string& str) const {
    if (str == "$") return AtomArgType::Dollar;
    if (str == "$?") return AtomArgType::DollarQuestion;
    if (str == "$!") return AtomArgType::DollarExclamation;
    if (str == "$_") return AtomArgType::DollarUnderscore;
    if (str == "...") return AtomArgType::Ellipsis;
    return AtomArgType::Dollar; // Default
}

// CHTLCHTLJSFunction implementation
CHTLCHTLJSFunction::CHTLCHTLJSFunction() {
}

CHTLCHTLJSFunction::~CHTLCHTLJSFunction() {
}

CHTLCHTLJSFunction CHTLCHTLJSFunction::createCHTLJSFunction(const std::string& name, const std::string& body) {
    CHTLCHTLJSFunction func;
    func.name = name;
    func.body = body;
    return func;
}

CHTLCHTLJSFunction CHTLCHTLJSFunction::createCHTLJSFunction(const std::string& name, const std::string& body, const std::vector<std::string>& params) {
    CHTLCHTLJSFunction func;
    func.name = name;
    func.body = body;
    func.parameters = params;
    return func;
}

std::string CHTLCHTLJSFunction::getName() const {
    return name;
}

std::string CHTLCHTLJSFunction::getBody() const {
    return body;
}

std::vector<std::string> CHTLCHTLJSFunction::getParameters() const {
    return parameters;
}

std::string CHTLCHTLJSFunction::call(const std::vector<CHTLArg>& args) const {
    std::map<std::string, CHTLArg> context;
    
    for (size_t i = 0; i < args.size() && i < parameters.size(); ++i) {
        context[parameters[i]] = args[i];
    }
    
    return processBody(body, context);
}

std::string CHTLCHTLJSFunction::call(const std::map<std::string, CHTLArg>& args) const {
    return processBody(body, args);
}

CHTLCHTLJSFunction CHTLCHTLJSFunction::bind(const std::string& param, const CHTLArg& value) const {
    CHTLCHTLJSFunction result = *this;
    result.bindings[param] = value;
    return result;
}

CHTLCHTLJSFunction CHTLCHTLJSFunction::bind(const std::map<std::string, CHTLArg>& bindings) const {
    CHTLCHTLJSFunction result = *this;
    result.bindings = bindings;
    return result;
}

CHTLCHTLJSFunction CHTLCHTLJSFunction::bindVirtualObject(const std::string& object_name) const {
    CHTLCHTLJSFunction result = *this;
    CHTLArg virtual_obj;
    virtual_obj.type = ArgType::Object;
    virtual_obj.value = object_name;
    result.bindings["this"] = virtual_obj;
    return result;
}

bool CHTLCHTLJSFunction::isValid() const {
    return !name.empty() && !body.empty() && validateParameters(parameters);
}

std::vector<std::string> CHTLCHTLJSFunction::getErrors() const {
    std::vector<std::string> errors;
    
    if (name.empty()) {
        errors.push_back("Function name is empty");
    }
    
    if (body.empty()) {
        errors.push_back("Function body is empty");
    }
    
    if (!validateParameters(parameters)) {
        errors.push_back("Invalid parameters");
    }
    
    return errors;
}

bool CHTLCHTLJSFunction::hasParameter(const std::string& param) const {
    return std::find(parameters.begin(), parameters.end(), param) != parameters.end();
}

std::string CHTLCHTLJSFunction::generateCode() const {
    return generateDeclaration();
}

std::string CHTLCHTLJSFunction::generateCall(const std::vector<std::string>& args) const {
    std::stringstream ss;
    ss << "chtl::" << name << "(";
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << args[i];
    }
    
    ss << ")";
    return ss.str();
}

std::string CHTLCHTLJSFunction::generateDeclaration() const {
    std::stringstream ss;
    ss << "chtl::" << name << " = function(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << parameters[i];
    }
    
    ss << ") {\n";
    ss << "  " << body << "\n";
    ss << "};";
    
    return ss.str();
}

std::string CHTLCHTLJSFunction::processBody(const std::string& body, const std::map<std::string, CHTLArg>& context) const {
    return replaceParameters(body, context);
}

std::string CHTLCHTLJSFunction::replaceParameters(const std::string& body, const std::map<std::string, CHTLArg>& context) const {
    std::string result = body;
    
    // Replace bound parameters first
    for (const auto& binding : bindings) {
        std::string placeholder = "${" + binding.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), binding.second.value);
            pos += binding.second.value.length();
        }
    }
    
    // Replace context parameters
    for (const auto& param : context) {
        std::string placeholder = "${" + param.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), param.second.value);
            pos += param.second.value.length();
        }
    }
    
    return result;
}

bool CHTLCHTLJSFunction::validateParameters(const std::vector<std::string>& params) const {
    // Simple validation - check for duplicates
    std::set<std::string> unique_params;
    for (const auto& param : params) {
        if (unique_params.find(param) != unique_params.end()) {
            return false;
        }
        unique_params.insert(param);
    }
    return true;
}

std::string CHTLCHTLJSFunction::sanitizeName(const std::string& name) const {
    std::string result = name;
    
    // Remove invalid characters
    result.erase(std::remove_if(result.begin(), result.end(), 
        [](char c) { return !std::isalnum(c) && c != '_'; }), result.end());
    
    // Ensure it starts with a letter or underscore
    if (!result.empty() && !std::isalpha(result[0]) && result[0] != '_') {
        result = "_" + result;
    }
    
    return result;
}

// CHTLCJMODAPIManager implementation
CHTLCJMODAPIManager::CHTLCJMODAPIManager() : initialized(false) {
}

CHTLCJMODAPIManager::~CHTLCJMODAPIManager() {
    cleanup();
}

bool CHTLCJMODAPIManager::initialize() {
    if (initialized) {
        return true;
    }
    
    if (!initializeComponents()) {
        return false;
    }
    
    initialized = true;
    return true;
}

void CHTLCJMODAPIManager::cleanup() {
    if (initialized) {
        cleanupComponents();
        initialized = false;
    }
}

CHTLSyntax& CHTLCJMODAPIManager::getSyntax() {
    return *syntax;
}

CHTLArgProcessor& CHTLCJMODAPIManager::getArgProcessor() {
    return *arg_processor;
}

CHTLCJMODScanner& CHTLCJMODAPIManager::getScanner() {
    return *scanner;
}

CHTLCJMODGenerator& CHTLCJMODAPIManager::getGenerator() {
    return *generator;
}

CHTLAtomArg& CHTLCJMODAPIManager::getAtomArg() {
    return *atom_arg;
}

std::string CHTLCJMODAPIManager::processCode(const std::string& code) const {
    if (!initialized) {
        return "";
    }
    
    // Simple code processing
    auto tokens = scanner->scan(code);
    auto analysis = scanner->analyzeCode(code);
    
    std::stringstream result;
    result << "// Processed code\n";
    result << "// Tokens: " << tokens.size() << "\n";
    result << "// Analysis: " << analysis.size() << " items\n";
    result << code;
    
    return result.str();
}

std::string CHTLCJMODAPIManager::processFile(const std::string& path) const {
    if (!initialized) {
        return "";
    }
    
    std::string content = scanner->readFile(path);
    if (content.empty()) {
        return "";
    }
    
    return processCode(content);
}

std::string CHTLCJMODAPIManager::processModule(const std::string& module_name) const {
    if (!initialized) {
        return "";
    }
    
    // Simple module processing
    std::stringstream result;
    result << "// Module: " << module_name << "\n";
    result << "// Generated by CHTL CJMOD API\n";
    
    return result.str();
}

bool CHTLCJMODAPIManager::isAPIReady() const {
    return initialized;
}

std::vector<std::string> CHTLCJMODAPIManager::getAPIVersion() const {
    return {"1.0.0", "CHTL CJMOD API"};
}

std::map<std::string, std::string> CHTLCJMODAPIManager::getAPIInfo() const {
    std::map<std::string, std::string> info;
    info["name"] = "CHTL CJMOD API";
    info["version"] = "1.0.0";
    info["status"] = initialized ? "ready" : "not initialized";
    return info;
}

bool CHTLCJMODAPIManager::initializeComponents() {
    try {
        syntax = std::make_unique<CHTLSyntax>();
        arg_processor = std::make_unique<CHTLArgProcessor>();
        scanner = std::make_unique<CHTLCJMODScanner>();
        generator = std::make_unique<CHTLCJMODGenerator>();
        atom_arg = std::make_unique<CHTLAtomArg>();
        
        return validateAPI();
    } catch (...) {
        return false;
    }
}

void CHTLCJMODAPIManager::cleanupComponents() {
    syntax.reset();
    arg_processor.reset();
    scanner.reset();
    generator.reset();
    atom_arg.reset();
}

bool CHTLCJMODAPIManager::validateAPI() const {
    return syntax != nullptr && 
           arg_processor != nullptr && 
           scanner != nullptr && 
           generator != nullptr && 
           atom_arg != nullptr;
}

} // namespace CHTL