#pragma once

#include "CHTLTemplate.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 自定义类型枚举
 */
enum class CustomType {
    STYLE,      // 自定义样式组
    ELEMENT,    // 自定义元素
    VAR         // 自定义变量组
};

/**
 * @brief 特例化操作类型
 */
enum class SpecializationType {
    DELETE,     // 删除操作
    INSERT,     // 插入操作
    REPLACE,    // 替换操作
    MODIFY      // 修改操作
};

/**
 * @brief 特例化操作
 */
class Specialization {
public:
    Specialization(SpecializationType type, const std::string& target, 
                   const std::string& content = "", int line = 0, int column = 0);
    ~Specialization() = default;

    SpecializationType getType() const { return m_type; }
    std::string getTarget() const { return m_target; }
    std::string getContent() const { return m_content; }
    int getLine() const { return m_line; }
    int getColumn() const { return m_column; }
    
    void setContent(const std::string& content) { m_content = content; }

private:
    SpecializationType m_type;
    std::string m_target;
    std::string m_content;
    int m_line;
    int m_column;
};

/**
 * @brief 自定义基类
 */
class Custom : public Template {
public:
    Custom(CustomType type, const std::string& name, int line = 0, int column = 0);
    virtual ~Custom() = default;

    CustomType getCustomType() const { return m_customType; }
    
    // 特例化操作管理
    void addSpecialization(std::shared_ptr<Specialization> spec);
    const std::vector<std::shared_ptr<Specialization>>& getSpecializations() const;
    void clearSpecializations();
    
    // 无值属性管理（用于无值样式组）
    void addUnvaluedAttribute(const std::string& attribute);
    const std::vector<std::string>& getUnvaluedAttributes() const;
    bool hasUnvaluedAttribute(const std::string& attribute) const;
    
    // 访问者模式
    virtual void accept(class CustomVisitor& visitor) = 0;

protected:
    CustomType m_customType;
    std::vector<std::shared_ptr<Specialization>> m_specializations;
    std::vector<std::string> m_unvaluedAttributes;
};

/**
 * @brief 自定义样式组
 */
class CustomStyle : public Custom {
public:
    CustomStyle(const std::string& name, int line = 0, int column = 0);
    
    void accept(CustomVisitor& visitor) override;
};

/**
 * @brief 自定义元素
 */
class CustomElement : public Custom {
public:
    CustomElement(const std::string& name, int line = 0, int column = 0);
    
    // 索引访问
    std::shared_ptr<BaseNode> getElementByIndex(size_t index) const;
    void setElementByIndex(size_t index, std::shared_ptr<BaseNode> element);
    
    // 元素操作
    void insertElement(size_t position, std::shared_ptr<BaseNode> element);
    void removeElement(size_t index);
    void removeElement(const std::string& selector);
    
    void accept(CustomVisitor& visitor) override;
};

/**
 * @brief 自定义变量组
 */
class CustomVar : public Custom {
public:
    CustomVar(const std::string& name, int line = 0, int column = 0);
    
    void accept(CustomVisitor& visitor) override;
};

/**
 * @brief 自定义访问者接口
 */
class CustomVisitor {
public:
    virtual ~CustomVisitor() = default;
    
    virtual void visit(CustomStyle& custom) = 0;
    virtual void visit(CustomElement& custom) = 0;
    virtual void visit(CustomVar& custom) = 0;
};

/**
 * @brief 自定义管理器
 */
class CustomManager {
public:
    explicit CustomManager(std::shared_ptr<CHTLContext> context);
    ~CustomManager() = default;

    // 自定义注册
    void registerCustom(std::shared_ptr<Custom> custom);
    void registerCustom(const std::string& name, std::shared_ptr<Custom> custom);
    
    // 自定义查找
    std::shared_ptr<Custom> getCustom(const std::string& name) const;
    std::shared_ptr<Custom> getCustom(const std::string& name, CustomType type) const;
    
    // 自定义列表
    std::vector<std::shared_ptr<Custom>> getCustoms(CustomType type) const;
    std::vector<std::shared_ptr<Custom>> getAllCustoms() const;
    
    // 特例化处理
    void processSpecializations();
    std::string applySpecializations(const std::string& customName, 
                                   const std::unordered_map<std::string, std::string>& variables) const;
    
    // 清理
    void clear();
    void removeCustom(const std::string& name);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<Custom>> m_customs;
    std::unordered_map<CustomType, std::vector<std::shared_ptr<Custom>>> m_customsByType;
    
    // 辅助方法
    void processCustomSpecializations(std::shared_ptr<Custom> custom);
    std::string applyStyleSpecializations(std::shared_ptr<CustomStyle> custom, 
                                        const std::unordered_map<std::string, std::string>& variables) const;
    std::string applyElementSpecializations(std::shared_ptr<CustomElement> custom, 
                                          const std::unordered_map<std::string, std::string>& variables) const;
    std::string applyVarSpecializations(std::shared_ptr<CustomVar> custom, 
                                      const std::unordered_map<std::string, std::string>& variables) const;
};

/**
 * @brief 自定义解析器
 */
class CustomParser {
public:
    explicit CustomParser(std::shared_ptr<CHTLContext> context);
    ~CustomParser() = default;

    // 解析自定义
    std::shared_ptr<Custom> parseCustom(std::shared_ptr<CustomNode> node);
    std::vector<std::shared_ptr<Custom>> parseCustoms(std::shared_ptr<BaseNode> root);
    
    // 解析特例化操作
    std::vector<std::shared_ptr<Specialization>> parseSpecializations(std::shared_ptr<CustomNode> node);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::shared_ptr<CustomStyle> parseCustomStyle(std::shared_ptr<CustomNode> node);
    std::shared_ptr<CustomElement> parseCustomElement(std::shared_ptr<CustomNode> node);
    std::shared_ptr<CustomVar> parseCustomVar(std::shared_ptr<CustomNode> node);
    std::shared_ptr<Specialization> parseSpecialization(std::shared_ptr<BaseNode> node);
};

/**
 * @brief 自定义生成器
 */
class CustomGenerator : public CustomVisitor {
public:
    explicit CustomGenerator(std::shared_ptr<CHTLContext> context);
    ~CustomGenerator() = default;

    // 生成代码
    std::string generate(std::shared_ptr<Custom> custom);
    std::string generateAll(const std::vector<std::shared_ptr<Custom>>& customs);
    
    // CustomVisitor 接口实现
    void visit(CustomStyle& custom) override;
    void visit(CustomElement& custom) override;
    void visit(CustomVar& custom) override;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_output;
    
    // 辅助方法
    std::string generateCustomStyle(CustomStyle& custom);
    std::string generateCustomElement(CustomElement& custom);
    std::string generateCustomVar(CustomVar& custom);
    std::string generateSpecializations(const std::vector<std::shared_ptr<Specialization>>& specializations);
};

} // namespace CHTL