#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 模板类型枚举
 */
enum class TemplateType {
    STYLE,      // 样式组模板
    ELEMENT,    // 元素模板
    VAR         // 变量组模板
};

/**
 * @brief 模板基类
 */
class Template {
public:
    Template(TemplateType type, const std::string& name, int line = 0, int column = 0);
    virtual ~Template() = default;

    TemplateType getType() const { return m_type; }
    std::string getName() const { return m_name; }
    int getLine() const { return m_line; }
    int getColumn() const { return m_column; }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getAttributes() const;
    
    // 子模板管理
    void addChild(std::shared_ptr<Template> child);
    const std::vector<std::shared_ptr<Template>>& getChildren() const;
    
    // 继承管理
    void addInheritance(const std::string& templateName);
    const std::vector<std::string>& getInheritances() const;
    
    // 访问者模式
    virtual void accept(class TemplateVisitor& visitor) = 0;

protected:
    TemplateType m_type;
    std::string m_name;
    int m_line;
    int m_column;
    std::unordered_map<std::string, std::string> m_attributes;
    std::vector<std::shared_ptr<Template>> m_children;
    std::vector<std::string> m_inheritances;
};

/**
 * @brief 样式组模板
 */
class StyleTemplate : public Template {
public:
    StyleTemplate(const std::string& name, int line = 0, int column = 0);
    
    void accept(TemplateVisitor& visitor) override;
};

/**
 * @brief 元素模板
 */
class ElementTemplate : public Template {
public:
    ElementTemplate(const std::string& name, int line = 0, int column = 0);
    
    void accept(TemplateVisitor& visitor) override;
};

/**
 * @brief 变量组模板
 */
class VarTemplate : public Template {
public:
    VarTemplate(const std::string& name, int line = 0, int column = 0);
    
    void accept(TemplateVisitor& visitor) override;
};

/**
 * @brief 模板访问者接口
 */
class TemplateVisitor {
public:
    virtual ~TemplateVisitor() = default;
    
    virtual void visit(StyleTemplate& template_) = 0;
    virtual void visit(ElementTemplate& template_) = 0;
    virtual void visit(VarTemplate& template_) = 0;
};

/**
 * @brief 模板管理器
 */
class TemplateManager {
public:
    explicit TemplateManager(std::shared_ptr<CHTLContext> context);
    ~TemplateManager() = default;

    // 模板注册
    void registerTemplate(std::shared_ptr<Template> template_);
    void registerTemplate(const std::string& name, std::shared_ptr<Template> template_);
    
    // 模板查找
    std::shared_ptr<Template> getTemplate(const std::string& name) const;
    std::shared_ptr<Template> getTemplate(const std::string& name, TemplateType type) const;
    
    // 模板列表
    std::vector<std::shared_ptr<Template>> getTemplates(TemplateType type) const;
    std::vector<std::shared_ptr<Template>> getAllTemplates() const;
    
    // 模板继承处理
    void resolveInheritances();
    bool hasCircularInheritance(const std::string& templateName, 
                               const std::string& targetName) const;
    
    // 模板应用
    std::string applyTemplate(const std::string& templateName, 
                            const std::unordered_map<std::string, std::string>& variables) const;
    
    // 清理
    void clear();
    void removeTemplate(const std::string& name);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<Template>> m_templates;
    std::unordered_map<TemplateType, std::vector<std::shared_ptr<Template>>> m_templatesByType;
    
    // 辅助方法
    void resolveTemplateInheritance(std::shared_ptr<Template> template_);
    std::string applyStyleTemplate(std::shared_ptr<StyleTemplate> template_, 
                                 const std::unordered_map<std::string, std::string>& variables) const;
    std::string applyElementTemplate(std::shared_ptr<ElementTemplate> template_, 
                                   const std::unordered_map<std::string, std::string>& variables) const;
    std::string applyVarTemplate(std::shared_ptr<VarTemplate> template_, 
                               const std::unordered_map<std::string, std::string>& variables) const;
};

/**
 * @brief 模板解析器
 */
class TemplateParser {
public:
    explicit TemplateParser(std::shared_ptr<CHTLContext> context);
    ~TemplateParser() = default;

    // 解析模板
    std::shared_ptr<Template> parseTemplate(std::shared_ptr<TemplateNode> node);
    std::vector<std::shared_ptr<Template>> parseTemplates(std::shared_ptr<BaseNode> root);
    
    // 解析继承
    void parseInheritance(std::shared_ptr<Template> template_, std::shared_ptr<TemplateNode> node);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::shared_ptr<StyleTemplate> parseStyleTemplate(std::shared_ptr<TemplateNode> node);
    std::shared_ptr<ElementTemplate> parseElementTemplate(std::shared_ptr<TemplateNode> node);
    std::shared_ptr<VarTemplate> parseVarTemplate(std::shared_ptr<TemplateNode> node);
};

/**
 * @brief 模板生成器
 */
class TemplateGenerator : public TemplateVisitor {
public:
    explicit TemplateGenerator(std::shared_ptr<CHTLContext> context);
    ~TemplateGenerator() = default;

    // 生成代码
    std::string generate(std::shared_ptr<Template> template_);
    std::string generateAll(const std::vector<std::shared_ptr<Template>>& templates);
    
    // TemplateVisitor 接口实现
    void visit(StyleTemplate& template_) override;
    void visit(ElementTemplate& template_) override;
    void visit(VarTemplate& template_) override;

private:
    std::shared_ptr<CHTLContext> m_context;
    std::string m_output;
    
    // 辅助方法
    std::string generateStyleTemplate(StyleTemplate& template_);
    std::string generateElementTemplate(ElementTemplate& template_);
    std::string generateVarTemplate(VarTemplate& template_);
    std::string generateAttributes(const std::unordered_map<std::string, std::string>& attributes);
};

} // namespace CHTL