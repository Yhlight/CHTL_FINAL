#ifndef CJMOD_ATOM_ARG_H
#define CJMOD_ATOM_ARG_H

#include <string>
#include <memory>
#include <functional>

namespace CHTL {
namespace CJMOD {

enum class PlaceholderType {
    NORMAL,         // $ 普通占位符
    OPTIONAL,       // $? 可选占位符
    REQUIRED,       // $! 必须占位符
    UNORDERED,      // $_ 无序占位符
    OPTIONAL_UNORDERED,  // $?_ 可选无序占位符
    REQUIRED_UNORDERED,  // $!_ 必须无序占位符
    VARIADIC,       // ... 不定参数占位符
    NON_PLACEHOLDER // 非占位符
};

class AtomArg {
private:
    std::string token;
    std::string value;
    PlaceholderType type;
    bool isPlaceholder;
    std::function<std::string(const std::string&)> bindingFunction;
    int matchCount;
    
public:
    AtomArg();
    AtomArg(const std::string& token);
    AtomArg(const std::string& token, const std::string& value);
    AtomArg(const std::string& token, PlaceholderType type);
    ~AtomArg() = default;
    
    // 基本信息
    std::string getToken() const;
    void setToken(const std::string& token);
    std::string getValue() const;
    void setValue(const std::string& value);
    PlaceholderType getType() const;
    void setType(PlaceholderType type);
    
    // 占位符判断
    bool isPlaceholder() const;
    bool isNormalPlaceholder() const;
    bool isOptional() const;
    bool isRequired() const;
    bool isUnordered() const;
    bool isVariadic() const;
    
    // 占位符类型转换
    static PlaceholderType parsePlaceholderType(const std::string& token);
    static std::string placeholderTypeToString(PlaceholderType type);
    static std::string getPlaceholderPrefix(PlaceholderType type);
    
    // 绑定函数
    void bind(std::function<std::string(const std::string&)> func);
    void bind(const std::string& value);
    void unbind();
    bool hasBinding() const;
    std::string applyBinding(const std::string& input) const;
    
    // 值填充
    void fillValue(const std::string& value);
    void fillValue(int value);
    void fillValue(double value);
    void fillValue(bool value);
    void clearValue();
    
    // 匹配计数
    int getMatchCount() const;
    void setMatchCount(int count);
    void incrementMatchCount();
    void resetMatchCount();
    
    // 验证
    bool validate() const;
    bool hasValue() const;
    bool isEmpty() const;
    
    // 转换
    std::string toString() const;
    std::string toFormattedString() const;
    std::string toDebugString() const;
    
    // 比较
    bool operator==(const AtomArg& other) const;
    bool operator!=(const AtomArg& other) const;
    bool operator<(const AtomArg& other) const;
    
    // 复制和移动
    AtomArg(const AtomArg& other);
    AtomArg& operator=(const AtomArg& other);
    AtomArg(AtomArg&& other) noexcept;
    AtomArg& operator=(AtomArg&& other) noexcept;
    
private:
    void parseToken(const std::string& token);
    bool isValidPlaceholderToken(const std::string& token) const;
    bool isVariadicToken(const std::string& token) const;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CJMOD_ATOM_ARG_H