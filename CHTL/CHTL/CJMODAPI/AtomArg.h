#pragma once
#include "Arg.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

// 原子参数类型
enum class AtomType {
    Placeholder,    // 占位符 $
    OptionalPlaceholder,  // 可选占位符 $?
    RequiredPlaceholder,  // 必须占位符 $!
    UnorderedPlaceholder, // 无序占位符 $_
    CombinedPlaceholder,  // 组合占位符 $!_, $?_, etc.
    Variable,       // 变量
    Constant,       // 常量
    Expression,     // 表达式
    Function,       // 函数调用
    Property,       // 属性访问
    Index,          // 索引访问
    Literal,        // 字面量
    Variadic        // 不定参数 ...
};

// 原子参数类
class AtomArg : public Arg {
public:
    AtomArg();
    explicit AtomArg(const std::string& value, AtomType type = AtomType::Literal);
    explicit AtomArg(const Arg& arg, AtomType type = AtomType::Literal);
    ~AtomArg() = default;
    
    // 原子类型管理
    AtomType getAtomType() const { return atomType; }
    void setAtomType(AtomType type) { atomType = type; }
    
    // 占位符管理
    bool isPlaceholder() const { 
        return atomType == AtomType::Placeholder || 
               atomType == AtomType::OptionalPlaceholder ||
               atomType == AtomType::RequiredPlaceholder ||
               atomType == AtomType::UnorderedPlaceholder ||
               atomType == AtomType::CombinedPlaceholder;
    }
    
    bool isOptionalPlaceholder() const { return atomType == AtomType::OptionalPlaceholder; }
    bool isRequiredPlaceholder() const { return atomType == AtomType::RequiredPlaceholder; }
    bool isUnorderedPlaceholder() const { return atomType == AtomType::UnorderedPlaceholder; }
    bool isCombinedPlaceholder() const { return atomType == AtomType::CombinedPlaceholder; }
    bool isVariadic() const { return atomType == AtomType::Variadic; }
    
    void setPlaceholder(const std::string& placeholder);
    void setOptionalPlaceholder(const std::string& placeholder);
    void setRequiredPlaceholder(const std::string& placeholder);
    void setUnorderedPlaceholder(const std::string& placeholder);
    void setCombinedPlaceholder(const std::string& placeholder, const std::string& modifiers);
    void setVariadic();
    
    std::string getPlaceholder() const;
    std::string getPlaceholderModifiers() const;
    
    // 变量管理
    bool isVariable() const { return atomType == AtomType::Variable; }
    void setVariable(const std::string& variable);
    std::string getVariable() const;
    
    // 表达式管理
    bool isExpression() const { return atomType == AtomType::Expression; }
    void setExpression(const std::string& expression);
    std::string getExpression() const;
    
    // 函数调用管理
    bool isFunction() const { return atomType == AtomType::Function; }
    void setFunction(const std::string& functionName, const std::vector<AtomArg>& args = {});
    std::string getFunctionName() const;
    std::vector<AtomArg> getFunctionArgs() const;
    
    // 属性访问管理
    bool isProperty() const { return atomType == AtomType::Property; }
    void setProperty(const std::string& object, const std::string& property);
    std::string getObject() const;
    std::string getProperty() const;
    
    // 索引访问管理
    bool isIndex() const { return atomType == AtomType::Index; }
    void setIndex(const std::string& object, const AtomArg& index);
    std::string getIndexObject() const;
    AtomArg getIndexValue() const;
    
    // 解析和生成
    static AtomArg parse(const std::string& input);
    std::string generate() const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 序列化
    std::string toJSON() const override;
    std::string toString() const override;

private:
    AtomType atomType;
    std::string placeholder;
    std::string variable;
    std::string expression;
    std::string functionName;
    std::vector<AtomArg> functionArgs;
    std::string object;
    std::string property;
    AtomArg indexValue;
    
    void validateAtom();
    std::vector<std::string> validationErrors;
};

} // namespace CHTL