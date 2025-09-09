#pragma once

#include "BaseNode.hpp"
#include <string>
#include <vector>
#include <set>

namespace CHTL {

/**
 * @brief 约束节点
 * 
 * 根据 CHTL.md 文档第 971-1006 行实现
 * 支持精确约束、类型约束和全局约束
 */
class ConstraintNode : public BaseNode {
public:
    enum class ConstraintType {
        PRECISE,    // 精确约束
        TYPE,       // 类型约束
        GLOBAL      // 全局约束
    };
    
    ConstraintNode(ConstraintType type, size_t line = 0, size_t column = 0);
    
    // 获取约束类型
    ConstraintType getConstraintType() const { return constraintType_; }
    
    // 设置约束类型
    void setConstraintType(ConstraintType type) { constraintType_ = type; }
    
    // 添加精确约束（HTML元素、自定义与模板对象）
    void addPreciseConstraint(const std::string& constraint);
    
    // 获取精确约束列表
    const std::vector<std::string>& getPreciseConstraints() const { return preciseConstraints_; }
    
    // 检查是否被精确约束
    bool isPreciseConstrained(const std::string& element) const;
    
    // 添加类型约束（@Html, [Custom], [Template]）
    void addTypeConstraint(const std::string& constraint);
    
    // 获取类型约束列表
    const std::vector<std::string>& getTypeConstraints() const { return typeConstraints_; }
    
    // 检查是否被类型约束
    bool isTypeConstrained(const std::string& type) const;
    
    // 添加全局约束
    void addGlobalConstraint(const std::string& constraint);
    
    // 获取全局约束列表
    const std::vector<std::string>& getGlobalConstraints() const { return globalConstraints_; }
    
    // 检查是否被全局约束
    bool isGlobalConstrained(const std::string& constraint) const;
    
    // 验证元素是否违反约束
    bool validateElement(const std::string& element) const;
    
    // 验证类型是否违反约束
    bool validateType(const std::string& type) const;
    
    // 转换为 HTML
    std::string toHTML() const override;
    
    // 转换为字符串
    std::string toString() const override;
    
private:
    ConstraintType constraintType_;
    std::vector<std::string> preciseConstraints_;
    std::vector<std::string> typeConstraints_;
    std::vector<std::string> globalConstraints_;
    
    // 解析约束字符串
    std::vector<std::string> parseConstraintString(const std::string& constraintStr) const;
};

} // namespace CHTL