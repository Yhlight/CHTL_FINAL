#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <variant>
#include <set>

namespace chtl {
namespace custom_system {

// 自定义类型
enum class CustomType {
    STYLE_GROUP,    // 样式组自定义
    ELEMENT,        // 元素自定义
    VARIABLE_GROUP  // 变量组自定义
};

// 自定义操作类型
enum class CustomOperation {
    ADD,            // 添加
    INSERT,         // 插入
    DELETE,         // 删除
    MODIFY,         // 修改
    SPECIALIZE      // 特例化
};

// 样式组自定义
struct StyleGroupCustom {
    std::string name;
    std::string baseTemplate;
    std::vector<std::string> properties;
    std::set<std::string> deletedProperties;
    std::set<std::string> deletedInheritance;
    bool unvalued;  // 无值样式组
    
    StyleGroupCustom(const std::string& n = "") : name(n), unvalued(false) {}
};

// 元素自定义
struct ElementCustom {
    std::string name;
    std::string baseTemplate;
    std::vector<std::string> addedElements;
    std::vector<std::string> insertedElements;
    std::set<std::string> deletedElements;
    std::set<std::string> deletedInheritance;
    
    ElementCustom(const std::string& n = "") : name(n) {}
};

// 变量组自定义
struct VariableGroupCustom {
    std::string name;
    std::string baseTemplate;
    std::map<std::string, std::string> addedVariables;
    std::map<std::string, std::string> modifiedVariables;
    std::set<std::string> deletedVariables;
    std::set<std::string> deletedInheritance;
    
    VariableGroupCustom(const std::string& n = "") : name(n) {}
};

// 自定义实例
struct CustomInstance {
    std::string customName;
    std::string instanceName;
    CustomType type;
    std::map<std::string, std::string> parameters;
    
    CustomInstance(const std::string& cName, const std::string& iName, CustomType t)
        : customName(cName), instanceName(iName), type(t) {}
};

// 自定义解析器
class CustomParser {
public:
    CustomParser(const std::string& input);
    
    std::vector<std::shared_ptr<CustomInstance>> parse();
    
private:
    std::string input_;
    size_t position_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::shared_ptr<StyleGroupCustom> parseStyleGroupCustom();
    std::shared_ptr<ElementCustom> parseElementCustom();
    std::shared_ptr<VariableGroupCustom> parseVariableGroupCustom();
    
    std::string parseString();
    std::string parseIdentifier();
    std::vector<std::string> parsePropertyList();
    std::vector<std::string> parseElementList();
    std::map<std::string, std::string> parseVariableMap();
    
    bool isKeyword(const std::string& word) const;
    bool isCustomKeyword(const std::string& word) const;
};

// 自定义编译器
class CustomCompiler {
public:
    CustomCompiler();
    
    std::string compile(const std::vector<std::shared_ptr<CustomInstance>>& instances);
    std::string compileStyleGroupCustom(const StyleGroupCustom& custom, const CustomInstance& instance);
    std::string compileElementCustom(const ElementCustom& custom, const CustomInstance& instance);
    std::string compileVariableGroupCustom(const VariableGroupCustom& custom, const CustomInstance& instance);
    
    void addCustom(const std::string& name, std::shared_ptr<StyleGroupCustom> custom);
    void addCustom(const std::string& name, std::shared_ptr<ElementCustom> custom);
    void addCustom(const std::string& name, std::shared_ptr<VariableGroupCustom> custom);
    
    std::shared_ptr<StyleGroupCustom> getStyleGroupCustom(const std::string& name);
    std::shared_ptr<ElementCustom> getElementCustom(const std::string& name);
    std::shared_ptr<VariableGroupCustom> getVariableGroupCustom(const std::string& name);
    
private:
    std::map<std::string, std::shared_ptr<StyleGroupCustom>> styleGroupCustoms_;
    std::map<std::string, std::shared_ptr<ElementCustom>> elementCustoms_;
    std::map<std::string, std::shared_ptr<VariableGroupCustom>> variableGroupCustoms_;
    
    std::string resolveParameters(const std::string& content, const std::map<std::string, std::string>& parameters);
    std::string processCustomOperations(const std::string& baseContent, const std::string& customContent, CustomType type);
    std::string mergeCustomizations(const std::string& base, const std::string& custom);
};

// 自定义管理器
class CustomManager {
public:
    CustomManager();
    
    void addCustom(const std::string& name, std::shared_ptr<StyleGroupCustom> custom);
    void addCustom(const std::string& name, std::shared_ptr<ElementCustom> custom);
    void addCustom(const std::string& name, std::shared_ptr<VariableGroupCustom> custom);
    
    std::shared_ptr<StyleGroupCustom> getStyleGroupCustom(const std::string& name);
    std::shared_ptr<ElementCustom> getElementCustom(const std::string& name);
    std::shared_ptr<VariableGroupCustom> getVariableGroupCustom(const std::string& name);
    
    std::string generateCode(const std::vector<std::shared_ptr<CustomInstance>>& instances);
    std::string generateStyleGroupCode(const StyleGroupCustom& custom, const CustomInstance& instance);
    std::string generateElementCode(const ElementCustom& custom, const CustomInstance& instance);
    std::string generateVariableGroupCode(const VariableGroupCustom& custom, const CustomInstance& instance);
    
    void clear();
    size_t getCustomCount() const;
    
private:
    CustomCompiler compiler_;
    
    std::string processCustomInheritance(const std::string& customName, const std::string& content, CustomType type);
    std::string resolveCustomDependencies(const std::string& content);
    std::string validateCustomParameters(const std::string& customName, const std::map<std::string, std::string>& parameters, CustomType type);
};

} // namespace custom_system
} // namespace chtl