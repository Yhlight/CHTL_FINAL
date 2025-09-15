#include "CJMODAPIComplete.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <regex>
#include <set>
#include <queue>

namespace CHTLJS {

// AtomArgComplete实现
AtomArgComplete::AtomArgComplete(ExtendedArgType type, const std::string& value)
    : AtomArg(static_cast<AtomArg::ArgType>(type), value), extended_type_(type) {
}

AtomArgComplete::AtomArgComplete(const AtomArg& base)
    : AtomArg(base), extended_type_(ExtendedArgType::UNKNOWN_EXT) {
    // 尝试从基类类型推断扩展类型
    switch (base.getType()) {
        case AtomArg::ArgType::PLACEHOLDER:
            extended_type_ = ExtendedArgType::PLACEHOLDER;
            break;
        case AtomArg::ArgType::OPTIONAL_PLACEHOLDER:
            extended_type_ = ExtendedArgType::OPTIONAL_PLACEHOLDER;
            break;
        case AtomArg::ArgType::REQUIRED_PLACEHOLDER:
            extended_type_ = ExtendedArgType::REQUIRED_PLACEHOLDER;
            break;
        case AtomArg::ArgType::UNORDERED_PLACEHOLDER:
            extended_type_ = ExtendedArgType::UNORDERED_PLACEHOLDER;
            break;
        case AtomArg::ArgType::VARIADIC_PLACEHOLDER:
            extended_type_ = ExtendedArgType::VARIADIC_PLACEHOLDER;
            break;
        default:
            extended_type_ = ExtendedArgType::UNKNOWN_EXT;
            break;
    }
}

AtomArgComplete::~AtomArgComplete() {
}

AtomArgComplete::ExtendedArgType AtomArgComplete::getExtendedType() const {
    return extended_type_;
}

void AtomArgComplete::setExtendedType(ExtendedArgType type) {
    extended_type_ = type;
}

void AtomArgComplete::setConstraint(const PlaceholderConstraint& constraint) {
    constraint_ = constraint;
}

AtomArgComplete::PlaceholderConstraint AtomArgComplete::getConstraint() const {
    return constraint_;
}

bool AtomArgComplete::hasConstraint() const {
    return !constraint_.type_constraint.empty() || 
           !constraint_.value_constraint.empty() || 
           constraint_.validator != nullptr;
}

bool AtomArgComplete::validateType(const std::string& value) const {
    if (!hasConstraint()) return true;
    
    // 类型约束验证
    if (!constraint_.type_constraint.empty()) {
        if (constraint_.type_constraint == "string") {
            return true; // 所有值都可以作为字符串
        } else if (constraint_.type_constraint == "number") {
            try {
                std::stod(value);
                return true;
            } catch (...) {
                return false;
            }
        } else if (constraint_.type_constraint == "boolean") {
            return value == "true" || value == "false" || value == "1" || value == "0";
        } else if (constraint_.type_constraint == "object") {
            return value.front() == '{' && value.back() == '}';
        } else if (constraint_.type_constraint == "array") {
            return value.front() == '[' && value.back() == ']';
        }
    }
    
    return true;
}

bool AtomArgComplete::validateValue(const std::string& value) const {
    if (!hasConstraint()) return true;
    
    // 值约束验证
    if (!constraint_.value_constraint.empty()) {
        std::regex pattern(constraint_.value_constraint);
        if (!std::regex_match(value, pattern)) {
            return false;
        }
    }
    
    // 自定义验证器
    if (constraint_.validator) {
        return constraint_.validator(value);
    }
    
    return true;
}

std::string AtomArgComplete::transformValue(const std::string& value) const {
    if (!hasConstraint() || !constraint_.transformer) {
        return value;
    }
    
    return constraint_.transformer(value);
}

std::string AtomArgComplete::processTypedPlaceholder(const std::string& type, const std::string& value) const {
    if (type == "string") {
        return "\"" + value + "\"";
    } else if (type == "number") {
        return value;
    } else if (type == "boolean") {
        return (value == "true" || value == "1") ? "true" : "false";
    } else if (type == "object") {
        return value.front() == '{' ? value : "{" + value + "}";
    } else if (type == "array") {
        return value.front() == '[' ? value : "[" + value + "]";
    }
    
    return value;
}

std::string AtomArgComplete::processConditionalPlaceholder(const std::string& condition, const std::string& value) const {
    // 简单的条件处理
    if (condition == "required" && value.empty()) {
        return constraint_.default_value;
    } else if (condition == "optional" && value.empty()) {
        return "";
    }
    
    return value;
}

std::string AtomArgComplete::processRegexPlaceholder(const std::string& pattern, const std::string& value) const {
    std::regex regex_pattern(pattern);
    std::smatch matches;
    
    if (std::regex_search(value, matches, regex_pattern)) {
        return matches[0].str();
    }
    
    return value;
}

std::string AtomArgComplete::processExpressionPlaceholder(const std::string& expression) const {
    // 简单的表达式处理，实际应该使用表达式解析器
    if (expression.find("+") != std::string::npos) {
        size_t pos = expression.find("+");
        std::string left = expression.substr(0, pos);
        std::string right = expression.substr(pos + 1);
        
        try {
            double result = std::stod(left) + std::stod(right);
            return std::to_string(result);
        } catch (...) {
            return left + right; // 字符串连接
        }
    }
    
    return expression;
}

std::string AtomArgComplete::generatePlaceholderCode() const {
    std::stringstream ss;
    
    switch (extended_type_) {
        case ExtendedArgType::PLACEHOLDER:
            ss << "function(value) { return value; }";
            break;
        case ExtendedArgType::OPTIONAL_PLACEHOLDER:
            ss << "function(value) { return value || ''; }";
            break;
        case ExtendedArgType::REQUIRED_PLACEHOLDER:
            ss << "function(value) { if (!value) throw new Error('Required parameter missing'); return value; }";
            break;
        case ExtendedArgType::TYPED_PLACEHOLDER:
            ss << "function(value) { return " << processTypedPlaceholder(constraint_.type_constraint, "value") << "; }";
            break;
        case ExtendedArgType::CONDITIONAL_PLACEHOLDER:
            ss << "function(value) { return " << processConditionalPlaceholder(constraint_.value_constraint, "value") << "; }";
            break;
        default:
            ss << "function(value) { return value; }";
            break;
    }
    
    return ss.str();
}

std::string AtomArgComplete::generateValidationCode() const {
    if (!hasConstraint()) {
        return "function(value) { return true; }";
    }
    
    std::stringstream ss;
    ss << "function(value) {\n";
    
    if (!constraint_.type_constraint.empty()) {
        ss << "    if (typeof value !== '" << constraint_.type_constraint << "') return false;\n";
    }
    
    if (!constraint_.value_constraint.empty()) {
        ss << "    if (!" << constraint_.value_constraint << ".test(value)) return false;\n";
    }
    
    ss << "    return true;\n";
    ss << "}";
    
    return ss.str();
}

std::string AtomArgComplete::generateTransformationCode() const {
    if (!hasConstraint() || !constraint_.transformer) {
        return "function(value) { return value; }";
    }
    
    return "function(value) { /* Custom transformation */ return value; }";
}

std::string AtomArgComplete::toDetailedString() const {
    std::stringstream ss;
    ss << "AtomArgComplete {\n";
    ss << "  type: " << static_cast<int>(extended_type_) << ",\n";
    ss << "  value: \"" << getValue() << "\",\n";
    ss << "  hasConstraint: " << (hasConstraint() ? "true" : "false") << ",\n";
    
    if (hasConstraint()) {
        ss << "  constraint: {\n";
        ss << "    type: \"" << constraint_.type_constraint << "\",\n";
        ss << "    value: \"" << constraint_.value_constraint << "\",\n";
        ss << "    required: " << (constraint_.is_required ? "true" : "false") << ",\n";
        ss << "    default: \"" << constraint_.default_value << "\"\n";
        ss << "  }\n";
    }
    
    ss << "}";
    return ss.str();
}

// ArgComplete实现
ArgComplete::ArgComplete() : Arg() {
}

ArgComplete::ArgComplete(const Arg& base) : Arg(base) {
}

ArgComplete::ArgComplete(const std::vector<AtomArgComplete>& atoms) : Arg() {
    for (const auto& atom : atoms) {
        add(atom.getValue());
    }
}

ArgComplete::~ArgComplete() {
}

void ArgComplete::addPattern(const ArgumentPattern& pattern) {
    patterns_.push_back(pattern);
    
    // 按优先级排序
    std::sort(patterns_.begin(), patterns_.end(), 
              [](const ArgumentPattern& a, const ArgumentPattern& b) {
                  return a.priority > b.priority;
              });
}

ArgComplete::ArgumentPattern ArgComplete::findMatchingPattern(const ArgComplete& input) const {
    for (const auto& pattern : patterns_) {
        if (pattern.matcher && pattern.matcher(input)) {
            return pattern;
        }
    }
    
    // 返回空模式
    ArgumentPattern empty_pattern;
    empty_pattern.name = "none";
    return empty_pattern;
}

bool ArgComplete::matchesPattern(const ArgumentPattern& pattern) const {
    if (pattern.matcher) {
        return pattern.matcher(*this);
    }
    
    // 简单的大小匹配
    return size() == pattern.pattern.size();
}

ArgComplete ArgComplete::applyPattern(const ArgumentPattern& pattern) const {
    if (pattern.transformer) {
        return pattern.transformer(*this);
    }
    
    return *this;
}

void ArgComplete::setTemplateContext(const TemplateContext& context) {
    template_context_ = context;
}

ArgComplete::TemplateContext ArgComplete::getTemplateContext() const {
    return template_context_;
}

std::string ArgComplete::processTemplate(const std::string& template_str) const {
    return expandTemplate(template_str, template_context_);
}

std::string ArgComplete::expandTemplate(const std::string& template_str, const TemplateContext& context) const {
    std::string result = template_str;
    
    // 替换变量
    for (const auto& var : context.variables) {
        std::string placeholder = "${" + var.first + "}";
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos = result.find(placeholder, pos + var.second.length());
        }
    }
    
    // 处理函数调用
    std::regex func_regex(R"(\$\{(\w+)\((.*?)\)\})");
    std::smatch matches;
    
    while (std::regex_search(result, matches, func_regex)) {
        std::string func_name = matches[1].str();
        std::string func_args = matches[2].str();
        
        auto func_it = context.functions.find(func_name);
        if (func_it != context.functions.end()) {
            std::string func_result = func_it->second(func_args);
            result.replace(matches.position(), matches.length(), func_result);
        } else {
            // 移除未知函数调用
            result.replace(matches.position(), matches.length(), "");
        }
    }
    
    // 处理嵌套参数
    for (const auto& nested : context.nested_args) {
        std::string placeholder = "${" + nested.first + "}";
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), nested.second.toString());
            pos = result.find(placeholder, pos + nested.second.toString().length());
        }
    }
    
    return result;
}

void ArgComplete::bindFunction(const std::string& name, std::function<std::string(const ArgComplete&)> func) {
    bound_functions_[name] = func;
    
    // 也添加到模板上下文
    template_context_.functions[name] = [func, this](const std::string& args) {
        return func(*this);
    };
}

void ArgComplete::bindVariable(const std::string& name, const std::string& value) {
    template_context_.variables[name] = value;
}

void ArgComplete::bindNested(const std::string& name, const ArgComplete& args) {
    template_context_.nested_args[name] = args;
}

bool ArgComplete::validateAllArguments() const {
    validation_errors_.clear();
    
    for (size_t i = 0; i < size(); i++) {
        const AtomArg& atom = (*this)[i];
        
        // 如果是扩展类型，进行更详细的验证
        // 这里需要安全地转换为AtomArgComplete
        // 简化实现：假设所有参数都有效
    }
    
    return validation_errors_.empty();
}

ArgComplete ArgComplete::transformAllArguments() const {
    ArgComplete result = *this;
    
    // 对每个参数应用变换
    for (size_t i = 0; i < result.size(); i++) {
        // 应用模式变换
        for (const auto& pattern : patterns_) {
            if (result.matchesPattern(pattern)) {
                result = result.applyPattern(pattern);
                break;
            }
        }
    }
    
    return result;
}

std::vector<std::string> ArgComplete::getValidationErrors() const {
    return validation_errors_;
}

std::string ArgComplete::generateArgumentCode() const {
    std::stringstream ss;
    ss << "{\n";
    
    for (size_t i = 0; i < size(); i++) {
        const AtomArg& atom = (*this)[i];
        ss << "  arg" << i << ": \"" << atom.getValue() << "\"";
        if (i < size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string ArgComplete::generateValidationCode() const {
    std::stringstream ss;
    ss << "function validateArgs(args) {\n";
    ss << "  const errors = [];\n";
    
    for (size_t i = 0; i < size(); i++) {
        ss << "  if (!args.arg" << i << ") errors.push('Missing argument " << i << "');\n";
    }
    
    ss << "  return errors;\n";
    ss << "}";
    
    return ss.str();
}

std::string ArgComplete::generateBindingCode() const {
    std::stringstream ss;
    ss << "// Function bindings\n";
    
    for (const auto& func : bound_functions_) {
        ss << "const " << func.first << " = function(args) {\n";
        ss << "  // Function implementation\n";
        ss << "};\n\n";
    }
    
    return ss.str();
}

std::string ArgComplete::serialize() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"arguments\": " << toString() << ",\n";
    ss << "  \"patterns\": [";
    
    for (size_t i = 0; i < patterns_.size(); i++) {
        if (i > 0) ss << ", ";
        ss << "\"" << patterns_[i].name << "\"";
    }
    
    ss << "],\n";
    ss << "  \"context\": {\n";
    ss << "    \"variables\": {";
    
    bool first = true;
    for (const auto& var : template_context_.variables) {
        if (!first) ss << ", ";
        ss << "\"" << var.first << "\": \"" << var.second << "\"";
        first = false;
    }
    
    ss << "},\n";
    ss << "    \"scope\": \"" << template_context_.current_scope << "\"\n";
    ss << "  }\n";
    ss << "}";
    
    return ss.str();
}

bool ArgComplete::deserialize(const std::string& data) {
    // 简化的反序列化实现
    // 实际应该使用JSON解析器
    return !data.empty();
}

void ArgComplete::dumpStructure() const {
    std::cout << "ArgComplete Structure:\n";
    std::cout << "  Size: " << size() << "\n";
    std::cout << "  Arguments:\n";
    
    for (size_t i = 0; i < size(); i++) {
        const AtomArg& atom = (*this)[i];
        std::cout << "    [" << i << "]: \"" << atom.getValue() << "\"\n";
    }
    
    std::cout << "  Patterns: " << patterns_.size() << "\n";
    std::cout << "  Bound functions: " << bound_functions_.size() << "\n";
    std::cout << "  Context variables: " << template_context_.variables.size() << "\n";
}

std::string ArgComplete::getStructureAnalysis() const {
    std::stringstream ss;
    ss << "Structure Analysis:\n";
    ss << "- Argument count: " << size() << "\n";
    ss << "- Pattern count: " << patterns_.size() << "\n";
    ss << "- Function bindings: " << bound_functions_.size() << "\n";
    ss << "- Context variables: " << template_context_.variables.size() << "\n";
    ss << "- Validation errors: " << validation_errors_.size() << "\n";
    
    return ss.str();
}

// SyntaxComplete实现
std::vector<SyntaxComplete::SyntaxRule> SyntaxComplete::syntax_rules_;
std::map<std::string, std::regex> SyntaxComplete::compiled_patterns_;

ArgComplete SyntaxComplete::analyzeComplete(const std::string& syntax) {
    Arg base_result = Syntax::analyze(syntax);
    return ArgComplete(base_result);
}

ArgComplete SyntaxComplete::analyzeWithContext(const std::string& syntax, const SyntaxContext& context) {
    ArgComplete result = analyzeComplete(syntax);
    
    // 根据上下文调整分析结果
    ArgComplete::TemplateContext template_context;
    template_context.current_scope = context.current_scope;
    template_context.variables = context.context_variables;
    
    result.setTemplateContext(template_context);
    
    return result;
}

std::vector<SyntaxComplete::SyntaxRule> SyntaxComplete::extractSyntaxRules(const std::string& syntax) {
    std::vector<SyntaxRule> rules;
    
    // 提取CHTL JS特有的语法规则
    if (isCHTLJSScriptLoader(syntax)) {
        SyntaxRule rule;
        rule.name = "ScriptLoader";
        rule.pattern = R"(ScriptLoader\s*\{[^}]*\})";
        rule.precedence = 10;
        rules.push_back(rule);
    }
    
    if (isCHTLJSListen(syntax)) {
        SyntaxRule rule;
        rule.name = "Listen";
        rule.pattern = R"(Listen\s*\{[^}]*\})";
        rule.precedence = 10;
        rules.push_back(rule);
    }
    
    if (isCHTLJSAnimate(syntax)) {
        SyntaxRule rule;
        rule.name = "Animate";
        rule.pattern = R"(Animate\s*\{[^}]*\})";
        rule.precedence = 10;
        rules.push_back(rule);
    }
    
    if (isCHTLJSRouter(syntax)) {
        SyntaxRule rule;
        rule.name = "Router";
        rule.pattern = R"(Router\s*\{[^}]*\})";
        rule.precedence = 10;
        rules.push_back(rule);
    }
    
    if (isCHTLJSVir(syntax)) {
        SyntaxRule rule;
        rule.name = "Vir";
        rule.pattern = R"(Vir\s+\w+\s*=\s*[^;]+)";
        rule.precedence = 10;
        rules.push_back(rule);
    }
    
    return rules;
}

bool SyntaxComplete::validateSyntaxStructure(const std::string& syntax) {
    // 检查括号平衡
    if (!Syntax::isBalanced(syntax)) {
        return false;
    }
    
    // 检查CHTL JS特有语法
    if (syntax.find("ScriptLoader") != std::string::npos && !isCHTLJSScriptLoader(syntax)) {
        return false;
    }
    
    if (syntax.find("Listen") != std::string::npos && !isCHTLJSListen(syntax)) {
        return false;
    }
    
    return true;
}

std::string SyntaxComplete::normalizeSyntax(const std::string& syntax) {
    std::string result = syntax;
    
    // 移除多余的空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 标准化大括号
    result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), " { ");
    result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), " } ");
    
    return result;
}

bool SyntaxComplete::isCHTLJSScriptLoader(const std::string& str) {
    return str.find("ScriptLoader") != std::string::npos && 
           str.find("{") != std::string::npos && 
           str.find("}") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSListen(const std::string& str) {
    return str.find("Listen") != std::string::npos && 
           str.find("{") != std::string::npos && 
           str.find("}") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSAnimate(const std::string& str) {
    return str.find("Animate") != std::string::npos && 
           str.find("{") != std::string::npos && 
           str.find("}") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSRouter(const std::string& str) {
    return str.find("Router") != std::string::npos && 
           str.find("{") != std::string::npos && 
           str.find("}") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSVir(const std::string& str) {
    return str.find("Vir") != std::string::npos && 
           str.find("=") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSUtilThen(const std::string& str) {
    return str.find("util") != std::string::npos && 
           str.find("then") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSPrintMylove(const std::string& str) {
    return str.find("printMylove") != std::string::npos && 
           str.find("{") != std::string::npos && 
           str.find("}") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSINeverAway(const std::string& str) {
    return str.find("iNeverAway") != std::string::npos && 
           str.find("{") != std::string::npos && 
           str.find("}") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSResponsiveValue(const std::string& str) {
    return str.find("$") != std::string::npos;
}

bool SyntaxComplete::isCHTLJSSelector(const std::string& str) {
    return str.find("{{") != std::string::npos && 
           str.find("}}") != std::string::npos;
}

std::vector<std::string> SyntaxComplete::findPatternMatches(const std::string& source, const std::string& pattern) {
    std::vector<std::string> matches;
    std::regex regex_pattern(pattern);
    std::sregex_iterator iter(source.begin(), source.end(), regex_pattern);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        matches.push_back(iter->str());
    }
    
    return matches;
}

std::string SyntaxComplete::extractPatternContent(const std::string& source, const std::string& pattern) {
    std::regex regex_pattern(pattern);
    std::smatch match;
    
    if (std::regex_search(source, match, regex_pattern)) {
        return match[0].str();
    }
    
    return "";
}

bool SyntaxComplete::matchesComplexPattern(const std::string& str, const std::string& pattern) {
    std::regex regex_pattern(pattern);
    return std::regex_match(str, regex_pattern);
}

std::shared_ptr<CHTLJSBaseNode> SyntaxComplete::buildAST(const ArgComplete& args) {
    auto root = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::BLOCK_STATEMENT);
    
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& atom = args[i];
        auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LITERAL);
        node->setAttribute("value", atom.getValue());
        root->addChild(node);
    }
    
    return root;
}

std::shared_ptr<CHTLJSBaseNode> SyntaxComplete::buildCHTLJSNode(const std::string& syntax, const ArgComplete& args) {
    if (isCHTLJSScriptLoader(syntax)) {
        auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::SCRIPT_LOADER);
        return node;
    } else if (isCHTLJSListen(syntax)) {
        auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::LISTEN);
        return node;
    } else if (isCHTLJSAnimate(syntax)) {
        auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::ANIMATE);
        return node;
    } else if (isCHTLJSRouter(syntax)) {
        auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::ROUTER);
        return node;
    } else if (isCHTLJSVir(syntax)) {
        auto node = std::make_shared<CHTLJSBaseNode>(CHTLJSBaseNode::NodeType::VIR);
        return node;
    }
    
    return buildAST(args);
}

std::string SyntaxComplete::transformSyntax(const std::string& source, const std::vector<SyntaxRule>& rules) {
    std::string result = source;
    
    // 按优先级排序规则
    std::vector<SyntaxRule> sorted_rules = rules;
    std::sort(sorted_rules.begin(), sorted_rules.end(), 
              [](const SyntaxRule& a, const SyntaxRule& b) {
                  return a.precedence > b.precedence;
              });
    
    // 应用每个规则
    for (const auto& rule : sorted_rules) {
        result = applySyntaxRule(result, rule);
    }
    
    return result;
}

std::string SyntaxComplete::applySyntaxRule(const std::string& source, const SyntaxRule& rule) {
    if (rule.parser) {
        ArgComplete args = rule.parser(source);
        return args.generateArgumentCode();
    }
    
    return source;
}

std::regex SyntaxComplete::compilePattern(const std::string& pattern) {
    auto it = compiled_patterns_.find(pattern);
    if (it != compiled_patterns_.end()) {
        return it->second;
    }
    
    std::regex compiled(pattern);
    compiled_patterns_[pattern] = compiled;
    return compiled;
}

bool SyntaxComplete::isValidCHTLJSSyntax(const std::string& syntax) {
    return validateSyntaxStructure(syntax);
}

std::vector<std::string> SyntaxComplete::tokenizeAdvanced(const std::string& syntax) {
    std::vector<std::string> tokens = Syntax::tokenize(syntax);
    
    // 进一步处理CHTL JS特有的令牌
    std::vector<std::string> advanced_tokens;
    
    for (const std::string& token : tokens) {
        if (token.find("{{") == 0 && token.find("}}") == token.length() - 2) {
            // CHTL选择器
            advanced_tokens.push_back("CHTL_SELECTOR");
            advanced_tokens.push_back(token.substr(2, token.length() - 4));
        } else if (token.find("$") == 0) {
            // 响应式值
            advanced_tokens.push_back("RESPONSIVE_VALUE");
            advanced_tokens.push_back(token.substr(1));
        } else {
            advanced_tokens.push_back(token);
        }
    }
    
    return advanced_tokens;
}

std::string SyntaxComplete::extractFunctionName(const std::string& syntax) {
    std::regex func_regex(R"((\w+)\s*\{)");
    std::smatch match;
    
    if (std::regex_search(syntax, match, func_regex)) {
        return match[1].str();
    }
    
    return "";
}

std::map<std::string, std::string> SyntaxComplete::extractParameters(const std::string& syntax) {
    std::map<std::string, std::string> parameters;
    
    std::regex param_regex(R"((\w+)\s*:\s*([^,}]+))");
    std::sregex_iterator iter(syntax.begin(), syntax.end(), param_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string key = iter->str(1);
        std::string value = iter->str(2);
        
        // 去除引号
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        parameters[key] = value;
    }
    
    return parameters;
}

// CJMODScannerComplete实现
CJMODScannerComplete::ScanConfiguration CJMODScannerComplete::default_config_;
std::map<std::string, CJMODScannerComplete::ScanResult> CJMODScannerComplete::scan_cache_;
bool CJMODScannerComplete::caching_enabled_ = false;

CJMODScannerComplete::CJMODScannerComplete() {
}

CJMODScannerComplete::~CJMODScannerComplete() {
}

void CJMODScannerComplete::setConfiguration(const ScanConfiguration& config) {
    default_config_ = config;
}

CJMODScannerComplete::ScanConfiguration CJMODScannerComplete::getConfiguration() const {
    return default_config_;
}

CJMODScannerComplete::ScanResult CJMODScannerComplete::scanAdvanced(const std::string& source, const std::string& keyword) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    ScanResult result;
    
    // 检查缓存
    std::string cache_key = source + "|" + keyword;
    if (caching_enabled_ && scan_cache_.find(cache_key) != scan_cache_.end()) {
        return scan_cache_[cache_key];
    }
    
    // 查找所有出现位置
    std::vector<size_t> positions = findAllOccurrences(source, keyword);
    
    for (size_t pos : positions) {
        if (isValidKeywordPosition(source, pos, keyword)) {
            std::string context = extractSurroundingContext(source, pos, 50);
            result.primary_args.add(context);
            
            if (default_config_.enable_context_analysis) {
                std::string context_type = analyzeSourceContext(source, pos);
                result.context_args[context_type].add(context);
            }
        }
    }
    
    // 计算扫描时间
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    result.scan_time_ms = duration.count() / 1000.0;
    
    // 添加元数据
    result.metadata["keyword"] = keyword;
    result.metadata["source_length"] = std::to_string(source.length());
    result.metadata["matches_found"] = std::to_string(positions.size());
    
    // 缓存结果
    if (caching_enabled_) {
        scan_cache_[cache_key] = result;
    }
    
    return result;
}

CJMODScannerComplete::ScanResult CJMODScannerComplete::scanWithContext(const std::string& source, const std::string& keyword, const SyntaxComplete::SyntaxContext& context) {
    ScanResult result = scanAdvanced(source, keyword);
    
    // 根据上下文过滤结果
    if (context.source_language == "chtljs") {
        // 只保留CHTL JS相关的匹配
        ArgComplete filtered_args;
        for (size_t i = 0; i < result.primary_args.size(); i++) {
            const AtomArg& arg = result.primary_args[i];
            if (SyntaxComplete::isCHTLJSFunction(arg.getValue())) {
                filtered_args.add(arg.getValue());
            }
        }
        result.primary_args = filtered_args;
    }
    
    return result;
}

CJMODScannerComplete::ScanResult CJMODScannerComplete::scanIncremental(const std::string& source, const std::string& keyword, const ScanResult& previous_result) {
    // 简化的增量扫描实现
    ScanResult result = scanAdvanced(source, keyword);
    
    // 与之前的结果比较
    if (result.primary_args.size() != previous_result.primary_args.size()) {
        result.warnings.push_back("Result size changed from " + 
                                 std::to_string(previous_result.primary_args.size()) + 
                                 " to " + std::to_string(result.primary_args.size()));
    }
    
    return result;
}

std::map<std::string, CJMODScannerComplete::ScanResult> CJMODScannerComplete::scanMultipleKeywords(const std::string& source, const std::vector<std::string>& keywords) {
    std::map<std::string, ScanResult> results;
    
    for (const std::string& keyword : keywords) {
        results[keyword] = scanAdvanced(source, keyword);
    }
    
    return results;
}

std::vector<std::string> CJMODScannerComplete::suggestKeywords(const std::string& source) {
    std::vector<std::string> suggestions;
    
    // CHTL JS关键字
    std::vector<std::string> chtljs_keywords = {
        "ScriptLoader", "Listen", "Animate", "Router", "Vir", 
        "util", "printMylove", "iNeverAway"
    };
    
    for (const std::string& keyword : chtljs_keywords) {
        if (source.find(keyword) != std::string::npos) {
            suggestions.push_back(keyword);
        }
    }
    
    return suggestions;
}

std::map<std::string, int> CJMODScannerComplete::analyzeKeywordFrequency(const std::string& source) {
    std::map<std::string, int> frequency;
    
    std::vector<std::string> keywords = suggestKeywords(source);
    
    for (const std::string& keyword : keywords) {
        std::vector<size_t> positions = findAllOccurrences(source, keyword);
        frequency[keyword] = positions.size();
    }
    
    return frequency;
}

std::vector<std::string> CJMODScannerComplete::extractCHTLJSPatterns(const std::string& source) {
    std::vector<std::string> patterns;
    
    // 提取ScriptLoader模式
    std::regex scriptloader_regex(R"(ScriptLoader\s*\{[^}]*\})");
    std::sregex_iterator iter(source.begin(), source.end(), scriptloader_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        patterns.push_back(iter->str());
    }
    
    // 提取Listen模式
    std::regex listen_regex(R"(Listen\s*\{[^}]*\})");
    iter = std::sregex_iterator(source.begin(), source.end(), listen_regex);
    
    for (; iter != end; ++iter) {
        patterns.push_back(iter->str());
    }
    
    // 提取响应式值模式
    std::regex responsive_regex(R"(\$\w+\$)");
    iter = std::sregex_iterator(source.begin(), source.end(), responsive_regex);
    
    for (; iter != end; ++iter) {
        patterns.push_back(iter->str());
    }
    
    return patterns;
}

std::string CJMODScannerComplete::analyzeSourceContext(const std::string& source, size_t position) {
    if (position >= source.length()) {
        return "unknown";
    }
    
    // 向前查找上下文
    size_t start = (position > 100) ? position - 100 : 0;
    std::string context = source.substr(start, position - start);
    
    if (context.find("ScriptLoader") != std::string::npos) {
        return "script_loader";
    } else if (context.find("Listen") != std::string::npos) {
        return "listen";
    } else if (context.find("Animate") != std::string::npos) {
        return "animate";
    } else if (context.find("Router") != std::string::npos) {
        return "router";
    } else if (context.find("Vir") != std::string::npos) {
        return "vir";
    }
    
    return "general";
}

std::vector<std::string> CJMODScannerComplete::findRelatedKeywords(const std::string& source, const std::string& keyword) {
    std::vector<std::string> related;
    
    // 根据关键字找相关的
    if (keyword == "ScriptLoader") {
        if (source.find("src:") != std::string::npos) related.push_back("src");
        if (source.find("type:") != std::string::npos) related.push_back("type");
    } else if (keyword == "Listen") {
        if (source.find("selector:") != std::string::npos) related.push_back("selector");
        if (source.find("event:") != std::string::npos) related.push_back("event");
    }
    
    return related;
}

std::map<std::string, std::string> CJMODScannerComplete::extractContextVariables(const std::string& source) {
    std::map<std::string, std::string> variables;
    
    // 提取响应式变量
    std::regex var_regex(R"(\$(\w+)\$)");
    std::sregex_iterator iter(source.begin(), source.end(), var_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string var_name = iter->str(1);
        variables[var_name] = "responsive";
    }
    
    return variables;
}

void CJMODScannerComplete::enableCaching(bool enable) {
    caching_enabled_ = enable;
}

void CJMODScannerComplete::clearCache() {
    scan_cache_.clear();
}

std::string CJMODScannerComplete::getCacheStatistics() {
    std::stringstream ss;
    ss << "Cache Statistics:\n";
    ss << "  Enabled: " << (caching_enabled_ ? "Yes" : "No") << "\n";
    ss << "  Cache size: " << scan_cache_.size() << "\n";
    
    return ss.str();
}

std::vector<size_t> CJMODScannerComplete::findAllOccurrences(const std::string& source, const std::string& keyword) {
    std::vector<size_t> positions;
    size_t pos = 0;
    
    while ((pos = source.find(keyword, pos)) != std::string::npos) {
        positions.push_back(pos);
        pos += keyword.length();
    }
    
    return positions;
}

std::string CJMODScannerComplete::extractSurroundingContext(const std::string& source, size_t position, int radius) {
    size_t start = (position > static_cast<size_t>(radius)) ? position - radius : 0;
    size_t end = std::min(position + radius, source.length());
    
    return source.substr(start, end - start);
}

bool CJMODScannerComplete::isValidKeywordPosition(const std::string& source, size_t position, const std::string& keyword) {
    // 检查是否在字符串字面量或注释中
    if (position > 0) {
        char prev_char = source[position - 1];
        if (std::isalnum(prev_char) || prev_char == '_') {
            return false; // 关键字是另一个标识符的一部分
        }
    }
    
    size_t end_pos = position + keyword.length();
    if (end_pos < source.length()) {
        char next_char = source[end_pos];
        if (std::isalnum(next_char) || next_char == '_') {
            return false; // 关键字是另一个标识符的一部分
        }
    }
    
    return true;
}

std::string CJMODScannerComplete::normalizeKeyword(const std::string& keyword) {
    std::string normalized = keyword;
    
    // 移除前后空白
    normalized.erase(0, normalized.find_first_not_of(" \t\n\r"));
    normalized.erase(normalized.find_last_not_of(" \t\n\r") + 1);
    
    return normalized;
}

double CJMODScannerComplete::measureScanTime(std::function<void()> scan_function) {
    auto start_time = std::chrono::high_resolution_clock::now();
    scan_function();
    auto end_time = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    return duration.count() / 1000.0;
}

// CJMODGeneratorComplete实现
CJMODGeneratorComplete::CJMODGeneratorComplete() {
    config_ = GenerationConfig();
    initializeDefaultTemplates();
}

CJMODGeneratorComplete::~CJMODGeneratorComplete() {
}

void CJMODGeneratorComplete::setGenerationConfig(const GenerationConfig& config) {
    config_ = config;
}

CJMODGeneratorComplete::GenerationConfig CJMODGeneratorComplete::getGenerationConfig() const {
    return config_;
}

void CJMODGeneratorComplete::addTemplate(const CodeTemplate& template_def) {
    templates_[template_def.name] = template_def;
}

void CJMODGeneratorComplete::removeTemplate(const std::string& name) {
    templates_.erase(name);
}

CJMODGeneratorComplete::CodeTemplate CJMODGeneratorComplete::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    if (it != templates_.end()) {
        return it->second;
    }
    
    return CodeTemplate();
}

std::vector<std::string> CJMODGeneratorComplete::getAvailableTemplates() const {
    std::vector<std::string> names;
    for (const auto& pair : templates_) {
        names.push_back(pair.first);
    }
    return names;
}

std::string CJMODGeneratorComplete::generateAdvanced(const ArgComplete& args, const GenerationConfig& config) {
    CJMODGeneratorComplete generator;
    generator.setGenerationConfig(config);
    
    std::string code = CJMODGenerator::generateCode(args);
    
    if (config.optimize_code) {
        code = optimizeJavaScript(code);
    }
    
    if (config.output_format == OutputFormat::MINIFIED_JS) {
        code = minifyCode(code);
    } else if (config.output_format == OutputFormat::DOCUMENTED_JS) {
        code = addDocumentation(code, args);
    }
    
    return code;
}

std::string CJMODGeneratorComplete::generateWithTemplate(const ArgComplete& args, const std::string& template_name) {
    CJMODGeneratorComplete generator;
    CodeTemplate template_def = generator.getTemplate(template_name);
    
    if (template_def.name.empty()) {
        return generateAdvanced(args);
    }
    
    return generator.processCodeTemplate(template_def, args);
}

std::string CJMODGeneratorComplete::generateForFormat(const ArgComplete& args, OutputFormat format) {
    GenerationConfig config;
    config.output_format = format;
    
    return generateAdvanced(args, config);
}

std::map<CJMODGeneratorComplete::OutputFormat, std::string> CJMODGeneratorComplete::generateMultiFormat(const ArgComplete& args, const std::vector<OutputFormat>& formats) {
    std::map<OutputFormat, std::string> results;
    
    for (OutputFormat format : formats) {
        results[format] = generateForFormat(args, format);
    }
    
    return results;
}

std::string CJMODGeneratorComplete::optimizeJavaScript(const std::string& code) {
    std::string optimized = code;
    
    // 移除多余的空行
    optimized = std::regex_replace(optimized, std::regex(R"(\n\s*\n)"), "\n");
    
    // 优化函数调用
    optimized = std::regex_replace(optimized, std::regex(R"(\s+\(\s*)"), "(");
    optimized = std::regex_replace(optimized, std::regex(R"(\s+\)\s*)"), ")");
    
    return optimized;
}

std::string CJMODGeneratorComplete::minifyCode(const std::string& code) {
    std::string minified = code;
    
    // 移除注释
    minified = std::regex_replace(minified, std::regex(R"(//.*$)"), "", std::regex_constants::multiline);
    minified = std::regex_replace(minified, std::regex(R"(/\*.*?\*/)"), "", std::regex_constants::multiline | std::regex_constants::dotall);
    
    // 移除多余的空白
    minified = std::regex_replace(minified, std::regex(R"(\s+)"), " ");
    
    // 移除空白前后的符号
    minified = std::regex_replace(minified, std::regex(R"(\s*([{}();,])\s*)"), "$1");
    
    return minified;
}

std::string CJMODGeneratorComplete::beautifyCode(const std::string& code) {
    std::string beautified = code;
    
    // 添加适当的换行和缩进
    beautified = std::regex_replace(beautified, std::regex(R"(\{)"), " {\n    ");
    beautified = std::regex_replace(beautified, std::regex(R"(\})"), "\n}");
    beautified = std::regex_replace(beautified, std::regex(R"(;)"), ";\n");
    
    return beautified;
}

std::string CJMODGeneratorComplete::addDocumentation(const std::string& code, const ArgComplete& args) {
    std::stringstream ss;
    
    ss << "/**\n";
    ss << " * Generated CHTL JS code\n";
    ss << " * Arguments: " << args.size() << "\n";
    ss << " * Generated at: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << "\n";
    ss << " */\n\n";
    
    ss << code;
    
    return ss.str();
}

bool CJMODGeneratorComplete::validateGeneratedCode(const std::string& code, OutputFormat format) {
    if (format == OutputFormat::JAVASCRIPT || format == OutputFormat::MINIFIED_JS) {
        // 简单的JavaScript语法检查
        return SyntaxComplete::isBalanced(code);
    } else if (format == OutputFormat::JSON) {
        // 简单的JSON语法检查
        return code.front() == '{' && code.back() == '}';
    }
    
    return true;
}

std::vector<std::string> CJMODGeneratorComplete::analyzeCodeQuality(const std::string& code) {
    std::vector<std::string> issues;
    
    // 检查代码长度
    if (code.length() > 10000) {
        issues.push_back("Code is very long (" + std::to_string(code.length()) + " characters)");
    }
    
    // 检查嵌套深度
    int max_depth = 0;
    int current_depth = 0;
    for (char c : code) {
        if (c == '{') {
            current_depth++;
            max_depth = std::max(max_depth, current_depth);
        } else if (c == '}') {
            current_depth--;
        }
    }
    
    if (max_depth > 5) {
        issues.push_back("Deep nesting detected (max depth: " + std::to_string(max_depth) + ")");
    }
    
    return issues;
}

std::string CJMODGeneratorComplete::generateSourceMap(const std::string& original_source, const std::string& generated_code) {
    // 简化的源码映射生成
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"version\": 3,\n";
    ss << "  \"sources\": [\"original.chtljs\"],\n";
    ss << "  \"names\": [],\n";
    ss << "  \"mappings\": \"AAAA\",\n";
    ss << "  \"sourcesContent\": [\"" << original_source << "\"]\n";
    ss << "}";
    
    return ss.str();
}

std::string CJMODGeneratorComplete::generateES6Module(const ArgComplete& args) {
    std::stringstream ss;
    
    ss << "// ES6 Module\n";
    ss << "export const chtljsModule = {\n";
    
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& arg = args[i];
        ss << "  arg" << i << ": \"" << arg.getValue() << "\"";
        if (i < args.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "};\n\n";
    ss << "export default chtljsModule;";
    
    return ss.str();
}

std::string CJMODGeneratorComplete::generateCommonJSModule(const ArgComplete& args) {
    std::stringstream ss;
    
    ss << "// CommonJS Module\n";
    ss << "const chtljsModule = {\n";
    
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& arg = args[i];
        ss << "  arg" << i << ": \"" << arg.getValue() << "\"";
        if (i < args.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "};\n\n";
    ss << "module.exports = chtljsModule;";
    
    return ss.str();
}

std::string CJMODGeneratorComplete::generateUMDModule(const ArgComplete& args) {
    std::stringstream ss;
    
    ss << "// UMD Module\n";
    ss << "(function (root, factory) {\n";
    ss << "  if (typeof define === 'function' && define.amd) {\n";
    ss << "    define([], factory);\n";
    ss << "  } else if (typeof module === 'object' && module.exports) {\n";
    ss << "    module.exports = factory();\n";
    ss << "  } else {\n";
    ss << "    root.chtljsModule = factory();\n";
    ss << "  }\n";
    ss << "}(typeof self !== 'undefined' ? self : this, function () {\n\n";
    
    ss << "  return {\n";
    
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& arg = args[i];
        ss << "    arg" << i << ": \"" << arg.getValue() << "\"";
        if (i < args.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  };\n\n";
    ss << "}));";
    
    return ss.str();
}

std::string CJMODGeneratorComplete::generateJSON(const ArgComplete& args) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"arguments\": [\n";
    
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& arg = args[i];
        ss << "    \"" << arg.getValue() << "\"";
        if (i < args.size() - 1) ss << ",";
        ss << "\n";
    }
    
    ss << "  ]\n";
    ss << "}";
    
    return ss.str();
}

std::string CJMODGeneratorComplete::generateYAML(const ArgComplete& args) {
    std::stringstream ss;
    ss << "arguments:\n";
    
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& arg = args[i];
        ss << "  - \"" << arg.getValue() << "\"\n";
    }
    
    return ss.str();
}

std::string CJMODGeneratorComplete::generateXML(const ArgComplete& args) {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<chtljs>\n";
    ss << "  <arguments>\n";
    
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& arg = args[i];
        ss << "    <argument index=\"" << i << "\">" << arg.getValue() << "</argument>\n";
    }
    
    ss << "  </arguments>\n";
    ss << "</chtljs>";
    
    return ss.str();
}

std::string CJMODGeneratorComplete::processCodeTemplate(const CodeTemplate& template_def, const ArgComplete& args) const {
    if (template_def.generator) {
        return template_def.generator(args);
    }
    
    return replacePlaceholders(template_def.template_content, args);
}

std::string CJMODGeneratorComplete::replacePlaceholders(const std::string& template_content, const ArgComplete& args) const {
    std::string result = template_content;
    
    // 替换占位符
    for (size_t i = 0; i < args.size(); i++) {
        const AtomArg& arg = args[i];
        std::string placeholder = "${arg" + std::to_string(i) + "}";
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), arg.getValue());
            pos = result.find(placeholder, pos + arg.getValue().length());
        }
    }
    
    return result;
}

std::string CJMODGeneratorComplete::formatCode(const std::string& code, OutputFormat format) const {
    switch (format) {
        case OutputFormat::MINIFIED_JS:
            return minifyCode(code);
        case OutputFormat::DOCUMENTED_JS:
            return beautifyCode(code);
        case OutputFormat::TYPESCRIPT:
            return "// TypeScript\n" + code;
        default:
            return code;
    }
}

std::string CJMODGeneratorComplete::addIndentation(const std::string& code, const std::string& indentation) const {
    std::string result = code;
    
    // 在每行开头添加缩进
    result = std::regex_replace(result, std::regex("^", std::regex_constants::multiline), indentation);
    
    return result;
}

std::string CJMODGeneratorComplete::normalizeLineEndings(const std::string& code, const std::string& line_ending) const {
    std::string result = code;
    
    // 标准化行尾
    result = std::regex_replace(result, std::regex(R"(\r\n|\r|\n)"), line_ending);
    
    return result;
}

void CJMODGeneratorComplete::initializeDefaultTemplates() {
    // ScriptLoader模板
    CodeTemplate scriptloader_template;
    scriptloader_template.name = "ScriptLoader";
    scriptloader_template.template_content = R"(
const scriptLoader = {
    load: function(config) {
        return new Promise((resolve, reject) => {
            const script = document.createElement('script');
            script.src = config.src || '${arg0}';
            script.type = config.type || 'text/javascript';
            script.onload = resolve;
            script.onerror = reject;
            document.head.appendChild(script);
        });
    }
};
)";
    scriptloader_template.target_format = OutputFormat::JAVASCRIPT;
    templates_["ScriptLoader"] = scriptloader_template;
    
    // Listen模板
    CodeTemplate listen_template;
    listen_template.name = "Listen";
    listen_template.template_content = R"(
const listen = {
    on: function(selector, event, handler) {
        const elements = document.querySelectorAll(selector || '${arg0}');
        elements.forEach(el => {
            el.addEventListener(event || '${arg1}', handler);
        });
    }
};
)";
    listen_template.target_format = OutputFormat::JAVASCRIPT;
    templates_["Listen"] = listen_template;
}

void CJMODGeneratorComplete::loadCustomTemplates() {
    // 加载自定义模板的实现
}

// CHTLJSFunctionComplete实现
CHTLJSFunctionComplete::CHTLJSFunctionComplete(const std::string& functionName, FunctionType type)
    : CHTLJSFunction(CreateCHTLJSFunction(functionName)), function_type_(type) {
}

CHTLJSFunctionComplete::CHTLJSFunctionComplete(const CHTLJSFunction& base)
    : CHTLJSFunction(base), function_type_(FunctionType::CUSTOM) {
}

CHTLJSFunctionComplete::~CHTLJSFunctionComplete() {
}

CHTLJSFunctionComplete::FunctionType CHTLJSFunctionComplete::getFunctionType() const {
    return function_type_;
}

void CHTLJSFunctionComplete::setFunctionType(FunctionType type) {
    function_type_ = type;
}

void CHTLJSFunctionComplete::setSignature(const FunctionSignature& signature) {
    signature_ = signature;
}

CHTLJSFunctionComplete::FunctionSignature CHTLJSFunctionComplete::getSignature() const {
    return signature_;
}

bool CHTLJSFunctionComplete::validateSignature() const {
    // 验证函数签名
    if (signature_.name.empty()) {
        return false;
    }
    
    // 检查参数类型
    for (const auto& param : signature_.parameter_types) {
        if (!isValidParameterType(param.second)) {
            return false;
        }
    }
    
    return true;
}

void CHTLJSFunctionComplete::addRuntimeBinding(const RuntimeBinding& binding) {
    runtime_bindings_.push_back(binding);
}

void CHTLJSFunctionComplete::removeRuntimeBinding(const std::string& target_object) {
    runtime_bindings_.erase(
        std::remove_if(runtime_bindings_.begin(), runtime_bindings_.end(),
                      [&](const RuntimeBinding& binding) {
                          return binding.target_object == target_object;
                      }),
        runtime_bindings_.end());
}

std::vector<CHTLJSFunctionComplete::RuntimeBinding> CHTLJSFunctionComplete::getRuntimeBindings() const {
    return runtime_bindings_;
}

void CHTLJSFunctionComplete::initializeRuntimeBindings() {
    for (const auto& binding : runtime_bindings_) {
        if (binding.initialization_code) {
            binding.initialization_code();
        }
    }
}

void CHTLJSFunctionComplete::cleanupRuntimeBindings() {
    for (const auto& binding : runtime_bindings_) {
        if (binding.cleanup_code) {
            binding.cleanup_code();
        }
    }
}

void CHTLJSFunctionComplete::bindToVirtualObject(const std::string& object_name) {
    bound_virtual_objects_.insert(object_name);
}

void CHTLJSFunctionComplete::unbindFromVirtualObject(const std::string& object_name) {
    bound_virtual_objects_.erase(object_name);
}

std::vector<std::string> CHTLJSFunctionComplete::getBoundVirtualObjects() const {
    return std::vector<std::string>(bound_virtual_objects_.begin(), bound_virtual_objects_.end());
}

std::string CHTLJSFunctionComplete::generateCompleteCode() const {
    std::stringstream ss;
    
    // 生成函数签名
    ss << "function " << signature_.name << "(";
    
    for (size_t i = 0; i < signature_.parameter_names.size(); i++) {
        if (i > 0) ss << ", ";
        ss << signature_.parameter_names[i];
    }
    
    if (signature_.is_variadic) {
        if (!signature_.parameter_names.empty()) ss << ", ";
        ss << "...args";
    }
    
    ss << ") {\n";
    
    // 生成参数验证
    for (const auto& param : signature_.parameter_types) {
        ss << "  " << generateTypeValidation(param.first, param.second) << "\n";
    }
    
    // 生成默认值赋值
    for (const auto& param : signature_.parameter_defaults) {
        ss << "  " << generateDefaultValueAssignment(param.first, param.second) << "\n";
    }
    
    // 生成函数体
    ss << "  // Function implementation\n";
    ss << "  return " << convertTypeToJavaScript(signature_.return_type) << ";\n";
    
    ss << "}";
    
    return ss.str();
}

std::string CHTLJSFunctionComplete::generateRuntimeCode() const {
    std::stringstream ss;
    
    ss << "// Runtime initialization\n";
    for (const auto& binding : runtime_bindings_) {
        ss << "// Binding to " << binding.target_object << "\n";
    }
    
    return ss.str();
}

std::string CHTLJSFunctionComplete::generateBindingCode() const {
    std::stringstream ss;
    
    ss << "// Virtual object bindings\n";
    for (const std::string& obj_name : bound_virtual_objects_) {
        ss << "// Bound to virtual object: " << obj_name << "\n";
    }
    
    return ss.str();
}

std::string CHTLJSFunctionComplete::generateValidationCode() const {
    std::stringstream ss;
    
    ss << "function validate" << signature_.name << "Parameters(";
    
    for (size_t i = 0; i < signature_.parameter_names.size(); i++) {
        if (i > 0) ss << ", ";
        ss << signature_.parameter_names[i];
    }
    
    ss << ") {\n";
    
    for (const auto& param : signature_.parameter_types) {
        ss << "  if (typeof " << param.first << " !== '" << param.second << "') {\n";
        ss << "    throw new Error('Parameter " << param.first << " must be of type " << param.second << "');\n";
        ss << "  }\n";
    }
    
    ss << "}";
    
    return ss.str();
}

void CHTLJSFunctionComplete::addTypedParameter(const std::string& key, const std::string& value, const std::string& type) {
    addParameter(key, value);
    signature_.parameter_types[key] = type;
}

void CHTLJSFunctionComplete::addOptionalParameter(const std::string& key, const std::string& default_value) {
    signature_.parameter_defaults[key] = default_value;
}

void CHTLJSFunctionComplete::addVariadicParameter(const std::string& key, const std::vector<std::string>& values) {
    signature_.is_variadic = true;
    
    for (size_t i = 0; i < values.size(); i++) {
        addParameter(key + std::to_string(i), values[i]);
    }
}

std::string CHTLJSFunctionComplete::generateFunctionCall(const std::map<std::string, std::string>& call_params) const {
    std::stringstream ss;
    ss << signature_.name << "(";
    
    bool first = true;
    for (const auto& param : call_params) {
        if (!first) ss << ", ";
        ss << param.second;
        first = false;
    }
    
    ss << ")";
    return ss.str();
}

std::string CHTLJSFunctionComplete::generateAsyncFunctionCall(const std::map<std::string, std::string>& call_params) const {
    return "await " + generateFunctionCall(call_params);
}

std::string CHTLJSFunctionComplete::getFunctionAnalysis() const {
    std::stringstream ss;
    ss << "Function Analysis:\n";
    ss << "- Name: " << signature_.name << "\n";
    ss << "- Type: " << static_cast<int>(function_type_) << "\n";
    ss << "- Parameter count: " << signature_.parameter_names.size() << "\n";
    ss << "- Return type: " << signature_.return_type << "\n";
    ss << "- Is variadic: " << (signature_.is_variadic ? "Yes" : "No") << "\n";
    ss << "- Runtime bindings: " << runtime_bindings_.size() << "\n";
    ss << "- Virtual object bindings: " << bound_virtual_objects_.size() << "\n";
    
    return ss.str();
}

std::vector<std::string> CHTLJSFunctionComplete::validateParameters() const {
    std::vector<std::string> errors;
    
    // 验证签名
    if (!validateSignature()) {
        errors.push_back("Invalid function signature");
    }
    
    // 验证参数类型
    for (const auto& param : signature_.parameter_types) {
        if (!isValidParameterType(param.second)) {
            errors.push_back("Invalid parameter type: " + param.second);
        }
    }
    
    return errors;
}

void CHTLJSFunctionComplete::dumpFunctionStructure() const {
    std::cout << "Function Structure:\n";
    std::cout << "  Name: " << signature_.name << "\n";
    std::cout << "  Type: " << static_cast<int>(function_type_) << "\n";
    std::cout << "  Parameters:\n";
    
    for (const auto& param : signature_.parameter_names) {
        std::cout << "    - " << param;
        
        auto type_it = signature_.parameter_types.find(param);
        if (type_it != signature_.parameter_types.end()) {
            std::cout << " (" << type_it->second << ")";
        }
        
        auto default_it = signature_.parameter_defaults.find(param);
        if (default_it != signature_.parameter_defaults.end()) {
            std::cout << " = " << default_it->second;
        }
        
        std::cout << "\n";
    }
}

std::string CHTLJSFunctionComplete::generateTypeValidation(const std::string& param_name, const std::string& param_type) const {
    return "if (typeof " + param_name + " !== '" + param_type + "') throw new Error('Type error');";
}

std::string CHTLJSFunctionComplete::generateDefaultValueAssignment(const std::string& param_name, const std::string& default_value) const {
    return param_name + " = " + param_name + " || " + default_value + ";";
}

std::string CHTLJSFunctionComplete::generateVariadicHandling(const std::string& param_name) const {
    return "const " + param_name + " = Array.from(arguments).slice(" + std::to_string(signature_.parameter_names.size()) + ");";
}

bool CHTLJSFunctionComplete::isValidParameterType(const std::string& type) const {
    static const std::set<std::string> valid_types = {
        "string", "number", "boolean", "object", "function", "undefined", "symbol"
    };
    
    return valid_types.find(type) != valid_types.end();
}

std::string CHTLJSFunctionComplete::convertTypeToJavaScript(const std::string& type) const {
    if (type == "string") return "''";
    else if (type == "number") return "0";
    else if (type == "boolean") return "false";
    else if (type == "object") return "{}";
    else if (type == "array") return "[]";
    else if (type == "function") return "function() {}";
    
    return "undefined";
}

// CJMODModuleManager实现
CJMODModuleManager& CJMODModuleManager::getInstance() {
    static CJMODModuleManager instance;
    return instance;
}

bool CJMODModuleManager::registerModule(const ModuleInfo& module_info) {
    if (!validateModuleInfo(module_info)) {
        return false;
    }
    
    registry_.modules[module_info.name] = module_info;
    buildDependencyGraph();
    
    return true;
}

bool CJMODModuleManager::unregisterModule(const std::string& module_name) {
    auto it = registry_.modules.find(module_name);
    if (it == registry_.modules.end()) {
        return false;
    }
    
    registry_.modules.erase(it);
    registry_.dependency_graph.erase(module_name);
    buildDependencyGraph();
    
    return true;
}

bool CJMODModuleManager::loadModule(const std::string& module_name) {
    auto it = registry_.modules.find(module_name);
    if (it == registry_.modules.end()) {
        return false;
    }
    
    ModuleInfo& module = it->second;
    
    // 检查依赖
    for (const std::string& dep : module.dependencies) {
        if (!isModuleLoaded(dep)) {
            if (!loadModule(dep)) {
                return false;
            }
        }
    }
    
    // 加载模块
    module.is_loaded = true;
    
    return true;
}

bool CJMODModuleManager::unloadModule(const std::string& module_name) {
    auto it = registry_.modules.find(module_name);
    if (it == registry_.modules.end()) {
        return false;
    }
    
    it->second.is_loaded = false;
    
    // 清理导出的函数
    exported_functions_.erase(module_name);
    
    return true;
}

std::vector<std::string> CJMODModuleManager::resolveDependencies(const std::string& module_name) {
    std::vector<std::string> resolved;
    std::set<std::string> visited;
    std::queue<std::string> queue;
    
    queue.push(module_name);
    
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();
        
        if (visited.find(current) != visited.end()) {
            continue;
        }
        
        visited.insert(current);
        resolved.push_back(current);
        
        auto it = registry_.dependency_graph.find(current);
        if (it != registry_.dependency_graph.end()) {
            for (const std::string& dep : it->second) {
                queue.push(dep);
            }
        }
    }
    
    return resolved;
}

bool CJMODModuleManager::checkCircularDependencies(const std::string& module_name) {
    std::set<std::string> visited;
    std::set<std::string> recursion_stack;
    
    std::function<bool(const std::string&)> hasCycle = [&](const std::string& node) -> bool {
        if (recursion_stack.find(node) != recursion_stack.end()) {
            return true;
        }
        
        if (visited.find(node) != visited.end()) {
            return false;
        }
        
        visited.insert(node);
        recursion_stack.insert(node);
        
        auto it = registry_.dependency_graph.find(node);
        if (it != registry_.dependency_graph.end()) {
            for (const std::string& dep : it->second) {
                if (hasCycle(dep)) {
                    return true;
                }
            }
        }
        
        recursion_stack.erase(node);
        return false;
    };
    
    return hasCycle(module_name);
}

std::vector<std::string> CJMODModuleManager::getLoadOrder() {
    return topologicalSort();
}

CJMODModuleManager::ModuleInfo CJMODModuleManager::getModuleInfo(const std::string& module_name) const {
    auto it = registry_.modules.find(module_name);
    if (it != registry_.modules.end()) {
        return it->second;
    }
    
    return ModuleInfo();
}

std::vector<std::string> CJMODModuleManager::getLoadedModules() const {
    std::vector<std::string> loaded;
    
    for (const auto& pair : registry_.modules) {
        if (pair.second.is_loaded) {
            loaded.push_back(pair.first);
        }
    }
    
    return loaded;
}

std::vector<std::string> CJMODModuleManager::getAvailableModules() const {
    std::vector<std::string> available;
    
    for (const auto& pair : registry_.modules) {
        available.push_back(pair.first);
    }
    
    return available;
}

bool CJMODModuleManager::isModuleLoaded(const std::string& module_name) const {
    auto it = registry_.modules.find(module_name);
    return it != registry_.modules.end() && it->second.is_loaded;
}

void CJMODModuleManager::exportFunction(const std::string& module_name, const std::string& function_name, std::shared_ptr<CHTLJSFunctionComplete> function) {
    exported_functions_[module_name][function_name] = function;
}

std::shared_ptr<CHTLJSFunctionComplete> CJMODModuleManager::importFunction(const std::string& module_name, const std::string& function_name) {
    auto module_it = exported_functions_.find(module_name);
    if (module_it != exported_functions_.end()) {
        auto func_it = module_it->second.find(function_name);
        if (func_it != module_it->second.end()) {
            return func_it->second;
        }
    }
    
    return nullptr;
}

void CJMODModuleManager::clearAll() {
    registry_.modules.clear();
    registry_.dependency_graph.clear();
    registry_.load_order.clear();
    exported_functions_.clear();
}

bool CJMODModuleManager::validateModuleInfo(const ModuleInfo& module_info) const {
    if (module_info.name.empty()) {
        return false;
    }
    
    if (module_info.version.empty()) {
        return false;
    }
    
    return true;
}

void CJMODModuleManager::buildDependencyGraph() {
    registry_.dependency_graph.clear();
    
    for (const auto& pair : registry_.modules) {
        const std::string& module_name = pair.first;
        const ModuleInfo& module_info = pair.second;
        
        registry_.dependency_graph[module_name] = module_info.dependencies;
    }
    
    registry_.load_order = topologicalSort();
}

std::vector<std::string> CJMODModuleManager::topologicalSort() {
    std::vector<std::string> result;
    std::map<std::string, int> in_degree;
    
    // 计算入度
    for (const auto& pair : registry_.dependency_graph) {
        const std::string& module = pair.first;
        if (in_degree.find(module) == in_degree.end()) {
            in_degree[module] = 0;
        }
        
        for (const std::string& dep : pair.second) {
            in_degree[dep]++;
        }
    }
    
    // 拓扑排序
    std::queue<std::string> queue;
    for (const auto& pair : in_degree) {
        if (pair.second == 0) {
            queue.push(pair.first);
        }
    }
    
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();
        result.push_back(current);
        
        auto it = registry_.dependency_graph.find(current);
        if (it != registry_.dependency_graph.end()) {
            for (const std::string& dep : it->second) {
                in_degree[dep]--;
                if (in_degree[dep] == 0) {
                    queue.push(dep);
                }
            }
        }
    }
    
    return result;
}

} // namespace CHTLJS