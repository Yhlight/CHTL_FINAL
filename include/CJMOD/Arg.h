#ifndef CJMOD_ARG_H
#define CJMOD_ARG_H

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace CJMOD {

/**
 * 原子参数类
 */
class AtomArg {
public:
    AtomArg(const std::string& value = "");
    ~AtomArg() = default;
    
    /**
     * 获取参数值
     * @return 参数值
     */
    const std::string& getValue() const { return m_value; }
    
    /**
     * 设置参数值
     * @param value 参数值
     */
    void setValue(const std::string& value) { m_value = value; }
    
    /**
     * 绑定获取值的函数
     * @param func 函数对象
     */
    void bind(std::function<std::string(const std::string&)> func);
    
    /**
     * 填充参数值
     * @param value 参数值
     */
    void fillValue(const std::string& value);
    
    /**
     * 填充参数值（整数）
     * @param value 参数值
     */
    void fillValue(int value);
    
    /**
     * 检查是否为占位符
     * @return 是否为占位符
     */
    bool isPlaceholder() const;
    
    /**
     * 检查是否为可选占位符
     * @return 是否为可选占位符
     */
    bool isOptional() const;
    
    /**
     * 检查是否为必须占位符
     * @return 是否为必须占位符
     */
    bool isRequired() const;
    
    /**
     * 检查是否为无序占位符
     * @return 是否为无序占位符
     */
    bool isUnordered() const;
    
    /**
     * 检查是否为不定参数占位符
     * @return 是否为不定参数占位符
     */
    bool isVariadic() const;
    
    /**
     * 执行绑定的函数
     * @return 执行结果
     */
    std::string execute() const;

private:
    std::string m_value;
    std::function<std::string(const std::string&)> m_bindFunction;
    bool m_isPlaceholder;
    bool m_isOptional;
    bool m_isRequired;
    bool m_isUnordered;
    bool m_isVariadic;
    
    /**
     * 解析占位符类型
     */
    void parsePlaceholderType();
};

/**
 * 参数列表类
 * 包含解析出的参数列表，并提供参数的绑定、填充、转换等操作
 */
class Arg {
public:
    Arg();
    explicit Arg(const std::vector<std::string>& values);
    ~Arg() = default;
    
    /**
     * 添加参数
     * @param value 参数值
     */
    void add(const std::string& value);
    
    /**
     * 获取参数数量
     * @return 参数数量
     */
    size_t size() const { return m_args.size(); }
    
    /**
     * 检查是否为空
     * @return 是否为空
     */
    bool empty() const { return m_args.empty(); }
    
    /**
     * 获取指定位置的参数
     * @param index 索引
     * @return 参数引用
     */
    AtomArg& operator[](size_t index);
    const AtomArg& operator[](size_t index) const;
    
    /**
     * 让一个原子参数绑定获取值的函数
     * @param placeholder 占位符
     * @param func 函数对象
     */
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    
    /**
     * 填充参数列表的值
     * @param values 值列表
     */
    void fillValue(const std::vector<std::string>& values);
    
    /**
     * 填充参数列表的值（从另一个 Arg 对象）
     * @param other 另一个 Arg 对象
     */
    void fillValue(const Arg& other);
    
    /**
     * 参数最终输出什么代码
     * @param templateStr 模板字符串
     */
    void transform(const std::string& templateStr);
    
    /**
     * 获取转换后的结果
     * @return 转换结果
     */
    const std::string& getResult() const { return m_result; }
    
    /**
     * 打印参数列表
     */
    void print() const;
    
    /**
     * 清空参数列表
     */
    void clear();
    
    /**
     * 查找指定占位符的参数
     * @param placeholder 占位符
     * @return 参数索引，如果未找到返回 -1
     */
    int findPlaceholder(const std::string& placeholder) const;
    
    /**
     * 替换占位符
     * @param placeholder 占位符
     * @param value 替换值
     */
    void replacePlaceholder(const std::string& placeholder, const std::string& value);
    
    /**
     * 检查是否包含指定占位符
     * @param placeholder 占位符
     * @return 是否包含
     */
    bool containsPlaceholder(const std::string& placeholder) const;
    
    /**
     * 获取所有占位符
     * @return 占位符列表
     */
    std::vector<std::string> getPlaceholders() const;
    
    /**
     * 验证参数完整性
     * @return 是否完整
     */
    bool validate() const;

private:
    std::vector<AtomArg> m_args;
    std::string m_result;
    
    /**
     * 处理模板字符串中的占位符
     * @param templateStr 模板字符串
     * @return 处理后的字符串
     */
    std::string processTemplate(const std::string& templateStr);
    
    /**
     * 替换字符串中的占位符
     * @param str 字符串
     * @param placeholder 占位符
     * @param value 替换值
     * @return 替换后的字符串
     */
    std::string replaceAll(const std::string& str, const std::string& placeholder, const std::string& value);
};

} // namespace CJMOD

#endif // CJMOD_ARG_H