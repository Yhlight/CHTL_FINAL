#ifndef TEMPLATE_SYSTEM_H
#define TEMPLATE_SYSTEM_H

#include "CHTLParser.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * 模板类型枚举
 */
enum class TemplateType {
    STYLE,      // 样式组模板
    ELEMENT,    // 元素模板
    VAR         // 变量组模板
};

/**
 * 自定义类型枚举
 */
enum class CustomType {
    STYLE,      // 自定义样式组
    ELEMENT,    // 自定义元素
    VAR         // 自定义变量组
};

/**
 * 导入类型枚举
 */
enum class ImportType {
    HTML,       // HTML文件
    STYLE,      // CSS文件
    JAVASCRIPT, // JavaScript文件
    CHTL,       // CHTL文件
    CJMOD       // CJMOD文件
};

/**
 * 模板基类
 */
class Template {
public:
    std::string name;
    TemplateType type;
    std::map<std::string, std::string> properties;
    std::vector<std::unique_ptr<ASTNode>> children;
    
    Template(const std::string& n, TemplateType t) : name(n), type(t) {}
    virtual ~Template() = default;
    
    virtual std::string expand(const std::map<std::string, std::string>& params = {}) = 0;
    virtual void addChild(std::unique_ptr<ASTNode> child);
};

/**
 * 样式组模板
 */
class StyleTemplate : public Template {
public:
    StyleTemplate(const std::string& n) : Template(n, TemplateType::STYLE) {}
    
    std::string expand(const std::map<std::string, std::string>& params = {}) override;
};

/**
 * 元素模板
 */
class ElementTemplate : public Template {
public:
    ElementTemplate(const std::string& n) : Template(n, TemplateType::ELEMENT) {}
    
    std::string expand(const std::map<std::string, std::string>& params = {}) override;
};

/**
 * 变量组模板
 */
class VarTemplate : public Template {
public:
    VarTemplate(const std::string& n) : Template(n, TemplateType::VAR) {}
    
    std::string expand(const std::map<std::string, std::string>& params = {}) override;
};

/**
 * 自定义基类
 */
class Custom {
public:
    std::string name;
    CustomType type;
    std::map<std::string, std::string> properties;
    std::vector<std::unique_ptr<ASTNode>> children;
    std::vector<std::string> specializations;
    
    Custom(const std::string& n, CustomType t) : name(n), type(t) {}
    virtual ~Custom() = default;
    
    virtual std::string expand(const std::map<std::string, std::string>& params = {}) = 0;
    virtual void addChild(std::unique_ptr<ASTNode> child);
    virtual void addSpecialization(const std::string& spec);
};

/**
 * 自定义样式组
 */
class CustomStyle : public Custom {
public:
    CustomStyle(const std::string& n) : Custom(n, CustomType::STYLE) {}
    
    std::string expand(const std::map<std::string, std::string>& params = {}) override;
};

/**
 * 自定义元素
 */
class CustomElement : public Custom {
public:
    CustomElement(const std::string& n) : Custom(n, CustomType::ELEMENT) {}
    
    std::string expand(const std::map<std::string, std::string>& params = {}) override;
};

/**
 * 自定义变量组
 */
class CustomVar : public Custom {
public:
    CustomVar(const std::string& n) : Custom(n, CustomType::VAR) {}
    
    std::string expand(const std::map<std::string, std::string>& params = {}) override;
};

/**
 * 导入信息
 */
struct ImportInfo {
    ImportType type;
    std::string path;
    std::string alias;
    std::string namespace;
    
    ImportInfo(ImportType t, const std::string& p, const std::string& a = "", const std::string& ns = "")
        : type(t), path(p), alias(a), namespace(ns) {}
};

/**
 * 命名空间
 */
class Namespace {
public:
    std::string name;
    std::map<std::string, std::unique_ptr<Template>> templates;
    std::map<std::string, std::unique_ptr<Custom>> customs;
    std::map<std::string, std::unique_ptr<ImportInfo>> imports;
    
    Namespace(const std::string& n) : name(n) {}
    
    void addTemplate(std::unique_ptr<Template> template);
    void addCustom(std::unique_ptr<Custom> custom);
    void addImport(std::unique_ptr<ImportInfo> import);
    
    Template* getTemplate(const std::string& name);
    Custom* getCustom(const std::string& name);
    ImportInfo* getImport(const std::string& name);
};

/**
 * 模板系统管理器
 */
class TemplateSystem {
public:
    TemplateSystem();
    ~TemplateSystem();
    
    /**
     * 注册模板
     * @param template 模板
     */
    void registerTemplate(std::unique_ptr<Template> template);
    
    /**
     * 注册自定义
     * @param custom 自定义
     */
    void registerCustom(std::unique_ptr<Custom> custom);
    
    /**
     * 注册导入
     * @param import 导入信息
     */
    void registerImport(std::unique_ptr<ImportInfo> import);
    
    /**
     * 注册命名空间
     * @param namespace 命名空间
     */
    void registerNamespace(std::unique_ptr<Namespace> namespace);
    
    /**
     * 获取模板
     * @param name 模板名
     * @param namespace 命名空间（可选）
     * @return 模板指针
     */
    Template* getTemplate(const std::string& name, const std::string& namespace = "");
    
    /**
     * 获取自定义
     * @param name 自定义名
     * @param namespace 命名空间（可选）
     * @return 自定义指针
     */
    Custom* getCustom(const std::string& name, const std::string& namespace = "");
    
    /**
     * 获取导入信息
     * @param name 导入名
     * @param namespace 命名空间（可选）
     * @return 导入信息指针
     */
    ImportInfo* getImport(const std::string& name, const std::string& namespace = "");
    
    /**
     * 获取命名空间
     * @param name 命名空间名
     * @return 命名空间指针
     */
    Namespace* getNamespace(const std::string& name);
    
    /**
     * 展开模板
     * @param name 模板名
     * @param params 参数
     * @param namespace 命名空间（可选）
     * @return 展开后的代码
     */
    std::string expandTemplate(const std::string& name, 
                              const std::map<std::string, std::string>& params = {},
                              const std::string& namespace = "");
    
    /**
     * 展开自定义
     * @param name 自定义名
     * @param params 参数
     * @param namespace 命名空间（可选）
     * @return 展开后的代码
     */
    std::string expandCustom(const std::string& name, 
                            const std::map<std::string, std::string>& params = {},
                            const std::string& namespace = "");
    
    /**
     * 处理导入
     * @param import 导入信息
     * @return 导入的代码
     */
    std::string processImport(const ImportInfo& import);
    
    /**
     * 解析模板引用
     * @param reference 引用字符串
     * @return 解析结果
     */
    std::map<std::string, std::string> parseTemplateReference(const std::string& reference);
    
    /**
     * 解析自定义引用
     * @param reference 引用字符串
     * @return 解析结果
     */
    std::map<std::string, std::string> parseCustomReference(const std::string& reference);
    
    /**
     * 解析命名空间引用
     * @param reference 引用字符串
     * @return 解析结果
     */
    std::map<std::string, std::string> parseNamespaceReference(const std::string& reference);
    
    /**
     * 处理特例化
     * @param custom 自定义
     * @param specializations 特例化列表
     * @return 处理后的自定义
     */
    std::unique_ptr<Custom> processSpecialization(Custom* custom, 
                                                 const std::vector<std::string>& specializations);
    
    /**
     * 处理继承
     * @param child 子模板/自定义
     * @param parent 父模板/自定义
     * @return 继承后的模板/自定义
     */
    std::unique_ptr<Template> processInheritance(Template* child, Template* parent);
    std::unique_ptr<Custom> processInheritance(Custom* child, Custom* parent);
    
    /**
     * 处理删除操作
     * @param target 目标模板/自定义
     * @param deletions 删除列表
     * @return 处理后的模板/自定义
     */
    std::unique_ptr<Template> processDeletion(Template* target, 
                                            const std::vector<std::string>& deletions);
    std::unique_ptr<Custom> processDeletion(Custom* target, 
                                           const std::vector<std::string>& deletions);
    
    /**
     * 处理插入操作
     * @param target 目标自定义
     * @param position 插入位置
     * @param content 插入内容
     * @return 处理后的自定义
     */
    std::unique_ptr<Custom> processInsertion(Custom* target, 
                                           const std::string& position, 
                                           const std::string& content);
    
    /**
     * 验证模板引用
     * @param reference 引用字符串
     * @return 是否有效
     */
    bool validateTemplateReference(const std::string& reference);
    
    /**
     * 验证自定义引用
     * @param reference 引用字符串
     * @return 是否有效
     */
    bool validateCustomReference(const std::string& reference);
    
    /**
     * 验证命名空间引用
     * @param reference 引用字符串
     * @return 是否有效
     */
    bool validateNamespaceReference(const std::string& reference);
    
    /**
     * 获取所有模板
     * @return 模板映射
     */
    const std::map<std::string, std::unique_ptr<Template>>& getAllTemplates() const;
    
    /**
     * 获取所有自定义
     * @return 自定义映射
     */
    const std::map<std::string, std::unique_ptr<Custom>>& getAllCustoms() const;
    
    /**
     * 获取所有命名空间
     * @return 命名空间映射
     */
    const std::map<std::string, std::unique_ptr<Namespace>>& getAllNamespaces() const;
    
    /**
     * 清空所有数据
     */
    void clear();

private:
    std::map<std::string, std::unique_ptr<Template>> templates;
    std::map<std::string, std::unique_ptr<Custom>> customs;
    std::map<std::string, std::unique_ptr<ImportInfo>> imports;
    std::map<std::string, std::unique_ptr<Namespace>> namespaces;
    
    /**
     * 解析引用字符串
     * @param reference 引用字符串
     * @return 解析结果
     */
    std::map<std::string, std::string> parseReference(const std::string& reference);
    
    /**
     * 处理路径解析
     * @param path 路径
     * @return 解析后的路径
     */
    std::string resolvePath(const std::string& path);
    
    /**
     * 处理通配符导入
     * @param pattern 模式
     * @return 匹配的文件列表
     */
    std::vector<std::string> processWildcardImport(const std::string& pattern);
    
    /**
     * 处理模块解包
     * @param path 模块路径
     * @return 解包后的内容
     */
    std::string processModuleUnpack(const std::string& path);
    
    /**
     * 处理依赖解析
     * @param import 导入信息
     * @return 依赖列表
     */
    std::vector<std::string> resolveDependencies(const ImportInfo& import);
    
    /**
     * 处理循环依赖检测
     * @param import 导入信息
     * @return 是否检测到循环依赖
     */
    bool detectCircularDependency(const ImportInfo& import);
    
    /**
     * 处理版本兼容性
     * @param import 导入信息
     * @return 是否兼容
     */
    bool checkVersionCompatibility(const ImportInfo& import);
};

} // namespace CHTL

#endif // TEMPLATE_SYSTEM_H