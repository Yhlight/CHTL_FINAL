#pragma once

#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLNode/CustomNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 自定义系统管理器类
 * 
 * 负责管理自定义样式组、元素、变量组的特例化功能
 */
class CustomManager {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit CustomManager(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~CustomManager();
    
    /**
     * @brief 注册自定义样式组
     * @param customStyle 自定义样式组节点
     * @return 是否注册成功
     */
    bool registerCustomStyle(std::shared_ptr<CustomNode> customStyle);
    
    /**
     * @brief 注册自定义元素
     * @param customElement 自定义元素节点
     * @return 是否注册成功
     */
    bool registerCustomElement(std::shared_ptr<CustomNode> customElement);
    
    /**
     * @brief 注册自定义变量组
     * @param customVar 自定义变量组节点
     * @return 是否注册成功
     */
    bool registerCustomVar(std::shared_ptr<CustomNode> customVar);
    
    /**
     * @brief 获取自定义样式组
     * @param name 样式组名称
     * @return 自定义样式组节点
     */
    std::shared_ptr<CustomNode> getCustomStyle(const std::string& name) const;
    
    /**
     * @brief 获取自定义元素
     * @param name 元素名称
     * @return 自定义元素节点
     */
    std::shared_ptr<CustomNode> getCustomElement(const std::string& name) const;
    
    /**
     * @brief 获取自定义变量组
     * @param name 变量组名称
     * @return 自定义变量组节点
     */
    std::shared_ptr<CustomNode> getCustomVar(const std::string& name) const;
    
    /**
     * @brief 应用自定义样式组特例化
     * @param customStyle 自定义样式组
     * @param specializations 特例化操作
     * @return 应用后的样式组内容
     */
    std::string applyStyleSpecialization(std::shared_ptr<CustomNode> customStyle, 
                                       const std::vector<SpecializationOperation>& specializations);
    
    /**
     * @brief 应用自定义元素特例化
     * @param customElement 自定义元素
     * @param specializations 特例化操作
     * @return 应用后的元素内容
     */
    std::string applyElementSpecialization(std::shared_ptr<CustomNode> customElement, 
                                         const std::vector<SpecializationOperation>& specializations);
    
    /**
     * @brief 应用自定义变量组特例化
     * @param customVar 自定义变量组
     * @param specializations 特例化操作
     * @return 应用后的变量组内容
     */
    std::string applyVarSpecialization(std::shared_ptr<CustomNode> customVar, 
                                     const std::vector<SpecializationOperation>& specializations);
    
    /**
     * @brief 处理无值样式组
     * @param customStyle 自定义样式组
     * @param values 提供的值
     * @return 填充后的样式组内容
     */
    std::string processValuelessStyle(std::shared_ptr<CustomNode> customStyle, 
                                    const std::unordered_map<std::string, std::string>& values);
    
    /**
     * @brief 处理索引访问
     * @param customElement 自定义元素
     * @param index 索引
     * @return 指定索引的元素内容
     */
    std::string processIndexAccess(std::shared_ptr<CustomNode> customElement, size_t index);
    
    /**
     * @brief 处理元素插入
     * @param customElement 自定义元素
     * @param insertOp 插入操作
     * @return 插入后的元素内容
     */
    std::string processElementInsertion(std::shared_ptr<CustomNode> customElement, 
                                      const InsertOperation& insertOp);
    
    /**
     * @brief 处理元素删除
     * @param customElement 自定义元素
     * @param deleteOp 删除操作
     * @return 删除后的元素内容
     */
    std::string processElementDeletion(std::shared_ptr<CustomNode> customElement, 
                                     const DeleteOperation& deleteOp);
    
    /**
     * @brief 清除所有自定义内容
     */
    void clear();
    
    /**
     * @brief 获取自定义样式组数量
     * @return 样式组数量
     */
    size_t getCustomStyleCount() const;
    
    /**
     * @brief 获取自定义元素数量
     * @return 元素数量
     */
    size_t getCustomElementCount() const;
    
    /**
     * @brief 获取自定义变量组数量
     * @return 变量组数量
     */
    size_t getCustomVarCount() const;
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 处理属性删除
     * @param content 内容
     * @param properties 要删除的属性列表
     * @return 处理后的内容
     */
    std::string processPropertyDeletion(const std::string& content, 
                                      const std::vector<std::string>& properties);
    
    /**
     * @brief 处理继承删除
     * @param content 内容
     * @param inheritance 要删除的继承
     * @return 处理后的内容
     */
    std::string processInheritanceDeletion(const std::string& content, 
                                         const std::string& inheritance);
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * @brief 添加警告信息
     * @param message 警告信息
     */
    void addWarning(const std::string& message);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> m_customStyles;
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> m_customElements;
    std::unordered_map<std::string, std::shared_ptr<CustomNode>> m_customVars;
    bool m_debugMode;
    
    // 统计信息
    size_t m_registeredStyles;
    size_t m_registeredElements;
    size_t m_registeredVars;
    size_t m_specializationCount;
    size_t m_successfulSpecializations;
    size_t m_failedSpecializations;
};

} // namespace CHTL