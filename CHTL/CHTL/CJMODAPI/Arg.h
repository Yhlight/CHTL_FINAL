#pragma once
#include <string>
#include <vector>
#include <map>
#include <any>
#include <memory>

namespace CHTL {

// 参数类型枚举
enum class ArgType {
    String,         // 字符串
    Number,         // 数字
    Boolean,        // 布尔值
    Array,          // 数组
    Object,         // 对象
    Function,       // 函数
    Null,           // 空值
    Undefined       // 未定义
};

// 参数类
class Arg {
public:
    Arg();
    explicit Arg(const std::string& value);
    explicit Arg(int value);
    explicit Arg(double value);
    explicit Arg(bool value);
    explicit Arg(const std::vector<Arg>& value);
    explicit Arg(const std::map<std::string, Arg>& value);
    ~Arg() = default;
    
    // 类型获取
    ArgType getType() const { return type; }
    std::string getTypeName() const;
    
    // 值获取
    std::string getString() const;
    int getInt() const;
    double getDouble() const;
    bool getBool() const;
    std::vector<Arg> getArray() const;
    std::map<std::string, Arg> getObject() const;
    
    // 值设置
    void setString(const std::string& value);
    void setInt(int value);
    void setDouble(double value);
    void setBool(bool value);
    void setArray(const std::vector<Arg>& value);
    void setObject(const std::map<std::string, Arg>& value);
    
    // 类型转换
    std::string toString() const;
    int toInt() const;
    double toDouble() const;
    bool toBool() const;
    
    // 比较操作
    bool operator==(const Arg& other) const;
    bool operator!=(const Arg& other) const;
    bool operator<(const Arg& other) const;
    bool operator>(const Arg& other) const;
    bool operator<=(const Arg& other) const;
    bool operator>=(const Arg& other) const;
    
    // 序列化
    std::string toJSON() const;
    std::string toString() const;

private:
    ArgType type;
    std::any value;
    
    template<typename T>
    T getValue() const;
    
    template<typename T>
    void setValue(const T& val);
};

} // namespace CHTL