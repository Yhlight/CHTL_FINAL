#ifndef CJMOD_ARG_H
#define CJMOD_ARG_H

#include "AtomArg.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace CHTL {
namespace CJMOD {

class Arg {
private:
    std::vector<std::shared_ptr<AtomArg>> args;
    std::unordered_map<std::string, std::function<std::string(const std::string&)>> bindings;
    std::unordered_map<std::string, int> matchCounters;
    std::string transformResult;
    
public:
    Arg();
    ~Arg() = default;
    
    // 参数管理
    void addToken(const std::string& token);
    void addArg(std::shared_ptr<AtomArg> arg);
    void removeArg(size_t index);
    void clear();
    
    // 参数访问
    std::shared_ptr<AtomArg> getArg(size_t index) const;
    std::string getToken(size_t index) const;
    size_t size() const;
    bool empty() const;
    
    // 参数绑定
    void bind(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    void bind(const std::string& placeholder, const std::string& value);
    void unbind(const std::string& placeholder);
    bool hasBinding(const std::string& placeholder) const;
    
    // 参数匹配
    std::string match(const std::string& placeholder, std::function<std::string(const std::string&)> func);
    std::string match(const std::string& placeholder, const std::string& value);
    void resetMatchCounters();
    
    // 参数填充
    void fillValue(const Arg& source);
    void fillValue(size_t index, const std::string& value);
    void fillValue(const std::string& placeholder, const std::string& value);
    
    // 参数转换
    void transform(const std::string& template);
    void transform(const std::string& template, const std::vector<std::string>& values);
    std::string getTransformResult() const;
    void clearTransform();
    
    // 参数查找
    std::vector<size_t> findPlaceholder(const std::string& placeholder) const;
    std::vector<std::string> getAllPlaceholders() const;
    std::vector<std::string> getAllTokens() const;
    
    // 参数验证
    bool validate() const;
    bool hasRequiredPlaceholders() const;
    bool hasOptionalPlaceholders() const;
    bool hasUnorderedPlaceholders() const;
    
    // 参数统计
    std::unordered_map<std::string, int> getPlaceholderCounts() const;
    std::unordered_map<std::string, int> getTokenCounts() const;
    
    // 参数操作
    void sortByPlaceholder();
    void sortByToken();
    void reverse();
    
    // 参数合并
    void merge(const Arg& other);
    void merge(const std::vector<std::string>& tokens);
    
    // 参数分割
    std::vector<Arg> splitByPlaceholder(const std::string& placeholder) const;
    std::vector<Arg> splitByToken(const std::string& token) const;
    
    // 参数过滤
    Arg filterByPlaceholder(const std::string& placeholder) const;
    Arg filterByToken(const std::string& token) const;
    Arg filterByIndex(const std::vector<size_t>& indices) const;
    
    // 参数替换
    void replacePlaceholder(const std::string& oldPlaceholder, const std::string& newPlaceholder);
    void replaceToken(const std::string& oldToken, const std::string& newToken);
    
    // 参数格式化
    std::string format() const;
    std::string format(const std::string& separator) const;
    std::string format(const std::string& prefix, const std::string& suffix, const std::string& separator) const;
    
    // 调试方法
    void print() const;
    void printBindings() const;
    void printMatchCounters() const;
    void printStatistics() const;
    
    // 操作符重载
    std::shared_ptr<AtomArg> operator[](size_t index) const;
    Arg& operator+=(const Arg& other);
    Arg operator+(const Arg& other) const;
    bool operator==(const Arg& other) const;
    bool operator!=(const Arg& other) const;
    
    // 迭代器支持
    std::vector<std::shared_ptr<AtomArg>>::iterator begin();
    std::vector<std::shared_ptr<AtomArg>>::iterator end();
    std::vector<std::shared_ptr<AtomArg>>::const_iterator begin() const;
    std::vector<std::shared_ptr<AtomArg>>::const_iterator end() const;
};

} // namespace CJMOD
} // namespace CHTL

#endif // CJMOD_ARG_H