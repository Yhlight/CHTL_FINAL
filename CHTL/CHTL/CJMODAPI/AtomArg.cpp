#include "AtomArg.h"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

AtomArg::AtomArg() : atomType(AtomType::Literal) {
}

AtomArg::AtomArg(const std::string& value, AtomType type) 
    : Arg(value), atomType(type) {
}

AtomArg::AtomArg(const Arg& arg, AtomType type) 
    : Arg(arg), atomType(type) {
}

// 占位符管理实现
void AtomArg::setPlaceholder(const std::string& placeholder) {
    atomType = AtomType::Placeholder;
    this->placeholder = placeholder;
}

void AtomArg::setOptionalPlaceholder(const std::string& placeholder) {
    atomType = AtomType::OptionalPlaceholder;
    this->placeholder = placeholder;
}

void AtomArg::setRequiredPlaceholder(const std::string& placeholder) {
    atomType = AtomType::RequiredPlaceholder;
    this->placeholder = placeholder;
}

void AtomArg::setUnorderedPlaceholder(const std::string& placeholder) {
    atomType = AtomType::UnorderedPlaceholder;
    this->placeholder = placeholder;
}

void AtomArg::setCombinedPlaceholder(const std::string& placeholder, const std::string& modifiers) {
    atomType = AtomType::CombinedPlaceholder;
    this->placeholder = placeholder;
    // 存储修饰符信息
    setValue("$" + modifiers + placeholder);
}

void AtomArg::setVariadic() {
    atomType = AtomType::Variadic;
    setValue("...");
}

std::string AtomArg::getPlaceholder() const {
    return placeholder;
}

std::string AtomArg::getPlaceholderModifiers() const {
    if (atomType == AtomType::CombinedPlaceholder) {
        std::string value = getValue();
        if (value.length() > 1 && value[0] == '$') {
            return value.substr(1, value.length() - placeholder.length() - 1);
        }
    }
    return "";
}

// 变量管理
void AtomArg::setVariable(const std::string& variable) {
    atomType = AtomType::Variable;
    this->variable = variable;
    setValue(variable);
}

std::string AtomArg::getVariable() const {
    return variable;
}

// 表达式管理
void AtomArg::setExpression(const std::string& expression) {
    atomType = AtomType::Expression;
    this->expression = expression;
    setValue(expression);
}

std::string AtomArg::getExpression() const {
    return expression;
}

// 函数调用管理
void AtomArg::setFunction(const std::string& functionName, const std::vector<AtomArg>& args) {
    atomType = AtomType::Function;
    this->functionName = functionName;
    this->functionArgs = args;
    
    std::ostringstream oss;
    oss << functionName << "(";
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << args[i].generate();
    }
    oss << ")";
    setValue(oss.str());
}

std::string AtomArg::getFunctionName() const {
    return functionName;
}

std::vector<AtomArg> AtomArg::getFunctionArgs() const {
    return functionArgs;
}

// 属性访问管理
void AtomArg::setProperty(const std::string& object, const std::string& property) {
    atomType = AtomType::Property;
    this->object = object;
    this->property = property;
    setValue(object + "." + property);
}

std::string AtomArg::getObject() const {
    return object;
}

std::string AtomArg::getProperty() const {
    return property;
}

// 索引访问管理
void AtomArg::setIndex(const std::string& object, const AtomArg& index) {
    atomType = AtomType::Index;
    this->object = object;
    this->indexValue = index;
    setValue(object + "[" + index.generate() + "]");
}

std::string AtomArg::getIndexObject() const {
    return object;
}

AtomArg AtomArg::getIndexValue() const {
    return indexValue;
}

// 解析和生成
AtomArg AtomArg::parse(const std::string& input) {
    AtomArg result;
    
    // 解析占位符
    if (input == "$") {
        result.setPlaceholder("");
        return result;
    } else if (input == "$?") {
        result.setOptionalPlaceholder("");
        return result;
    } else if (input == "$!") {
        result.setRequiredPlaceholder("");
        return result;
    } else if (input == "$_") {
        result.setUnorderedPlaceholder("");
        return result;
    } else if (input == "...") {
        result.setVariadic();
        return result;
    } else if (input.length() > 1 && input[0] == '$') {
        // 解析组合占位符
        std::string modifiers = "";
        std::string placeholder = "";
        
        size_t i = 1;
        while (i < input.length() && (input[i] == '?' || input[i] == '!' || input[i] == '_')) {
            modifiers += input[i];
            i++;
        }
        
        if (i < input.length()) {
            placeholder = input.substr(i);
        }
        
        if (!modifiers.empty()) {
            result.setCombinedPlaceholder(placeholder, modifiers);
        } else {
            result.setPlaceholder(placeholder);
        }
        return result;
    }
    
    // 解析函数调用
    size_t parenPos = input.find('(');
    if (parenPos != std::string::npos && input.back() == ')') {
        std::string funcName = input.substr(0, parenPos);
        std::string argsStr = input.substr(parenPos + 1, input.length() - parenPos - 2);
        
        std::vector<AtomArg> args;
        if (!argsStr.empty()) {
            // 简单的参数解析（实际实现需要更复杂的解析）
            std::istringstream iss(argsStr);
            std::string arg;
            while (std::getline(iss, arg, ',')) {
                args.push_back(parse(arg));
            }
        }
        
        result.setFunction(funcName, args);
        return result;
    }
    
    // 解析属性访问
    size_t dotPos = input.find('.');
    if (dotPos != std::string::npos) {
        std::string obj = input.substr(0, dotPos);
        std::string prop = input.substr(dotPos + 1);
        result.setProperty(obj, prop);
        return result;
    }
    
    // 解析索引访问
    size_t bracketPos = input.find('[');
    if (bracketPos != std::string::npos && input.back() == ']') {
        std::string obj = input.substr(0, bracketPos);
        std::string indexStr = input.substr(bracketPos + 1, input.length() - bracketPos - 2);
        AtomArg index = parse(indexStr);
        result.setIndex(obj, index);
        return result;
    }
    
    // 默认为字面量
    result = AtomArg(input, AtomType::Literal);
    return result;
}

std::string AtomArg::generate() const {
    switch (atomType) {
        case AtomType::Placeholder:
            return "$" + placeholder;
        case AtomType::OptionalPlaceholder:
            return "$?" + placeholder;
        case AtomType::RequiredPlaceholder:
            return "$!" + placeholder;
        case AtomType::UnorderedPlaceholder:
            return "$_" + placeholder;
        case AtomType::CombinedPlaceholder:
            return "$" + getPlaceholderModifiers() + placeholder;
        case AtomType::Variadic:
            return "...";
        case AtomType::Function: {
            std::ostringstream oss;
            oss << functionName << "(";
            for (size_t i = 0; i < functionArgs.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << functionArgs[i].generate();
            }
            oss << ")";
            return oss.str();
        }
        case AtomType::Property:
            return object + "." + property;
        case AtomType::Index:
            return object + "[" + indexValue.generate() + "]";
        default:
            return getValue();
    }
}

bool AtomArg::isValid() const {
    validateAtom();
    return validationErrors.empty();
}

std::vector<std::string> AtomArg::getValidationErrors() const {
    validateAtom();
    return validationErrors;
}

void AtomArg::validateAtom() {
    validationErrors.clear();
    
    switch (atomType) {
        case AtomType::Placeholder:
        case AtomType::OptionalPlaceholder:
        case AtomType::RequiredPlaceholder:
        case AtomType::UnorderedPlaceholder:
        case AtomType::CombinedPlaceholder:
            if (placeholder.empty() && atomType != AtomType::Placeholder) {
                validationErrors.push_back("Placeholder cannot be empty for this type");
            }
            break;
        case AtomType::Function:
            if (functionName.empty()) {
                validationErrors.push_back("Function name cannot be empty");
            }
            break;
        case AtomType::Property:
            if (object.empty() || property.empty()) {
                validationErrors.push_back("Object and property cannot be empty");
            }
            break;
        case AtomType::Index:
            if (object.empty()) {
                validationErrors.push_back("Object cannot be empty for index access");
            }
            break;
        default:
            break;
    }
}

std::string AtomArg::toJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"type\": \"" << static_cast<int>(atomType) << "\",\n";
    oss << "  \"value\": \"" << getValue() << "\",\n";
    
    switch (atomType) {
        case AtomType::Placeholder:
        case AtomType::OptionalPlaceholder:
        case AtomType::RequiredPlaceholder:
        case AtomType::UnorderedPlaceholder:
        case AtomType::CombinedPlaceholder:
            oss << "  \"placeholder\": \"" << placeholder << "\",\n";
            if (atomType == AtomType::CombinedPlaceholder) {
                oss << "  \"modifiers\": \"" << getPlaceholderModifiers() << "\",\n";
            }
            break;
        case AtomType::Function:
            oss << "  \"functionName\": \"" << functionName << "\",\n";
            oss << "  \"functionArgs\": [\n";
            for (size_t i = 0; i < functionArgs.size(); ++i) {
                if (i > 0) oss << ",\n";
                oss << "    " << functionArgs[i].toJSON();
            }
            oss << "\n  ],\n";
            break;
        case AtomType::Property:
            oss << "  \"object\": \"" << object << "\",\n";
            oss << "  \"property\": \"" << property << "\",\n";
            break;
        case AtomType::Index:
            oss << "  \"object\": \"" << object << "\",\n";
            oss << "  \"index\": " << indexValue.toJSON() << ",\n";
            break;
        default:
            break;
    }
    
    oss << "}";
    return oss.str();
}

std::string AtomArg::toString() const {
    return generate();
}

} // namespace CHTL