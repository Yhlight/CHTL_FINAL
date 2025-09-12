#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief 特例化操作类型
 */
enum class SpecializationType {
    DELETE_PROPERTY,      // 删除属性
    DELETE_INHERITANCE,   // 删除继承
    ADD_PROPERTY,         // 添加属性
    MODIFY_PROPERTY,      // 修改属性
    INSERT_ELEMENT,       // 插入元素
    DELETE_ELEMENT,       // 删除元素
    MODIFY_ELEMENT,       // 修改元素
    ADD_VALUE,            // 添加值
    MODIFY_VALUE          // 修改值
};

/**
 * @brief 插入位置
 */
enum class InsertPosition {
    BEFORE,               // 在指定元素之前
    AFTER,                // 在指定元素之后
    REPLACE,              // 替换指定元素
    AT_TOP,               // 在顶部
    AT_BOTTOM             // 在底部
};

/**
 * @brief 特例化操作基类
 */
struct SpecializationOperation {
    SpecializationType type;
    std::string target;   // 目标名称
    std::string value;    // 操作值
    
    SpecializationOperation(SpecializationType t, const std::string& target, const std::string& val = "")
        : type(t), target(target), value(val) {}
};

/**
 * @brief 属性删除操作
 */
struct PropertyDeletionOperation : public SpecializationOperation {
    std::vector<std::string> properties;  // 要删除的属性列表
    
    PropertyDeletionOperation(const std::vector<std::string>& props)
        : SpecializationOperation(SpecializationType::DELETE_PROPERTY, "", ""), properties(props) {}
};

/**
 * @brief 继承删除操作
 */
struct InheritanceDeletionOperation : public SpecializationOperation {
    std::string inheritance;  // 要删除的继承
    
    InheritanceDeletionOperation(const std::string& inh)
        : SpecializationOperation(SpecializationType::DELETE_INHERITANCE, "", ""), inheritance(inh) {}
};

/**
 * @brief 插入操作
 */
struct InsertOperation {
    InsertPosition position;           // 插入位置
    std::string targetElement;         // 目标元素
    size_t targetIndex;                // 目标索引
    std::string content;               // 插入内容
    
    InsertOperation(InsertPosition pos, const std::string& target, size_t index, const std::string& cont)
        : position(pos), targetElement(target), targetIndex(index), content(cont) {}
};

/**
 * @brief 删除操作
 */
struct DeleteOperation {
    std::string targetElement;         // 目标元素
    size_t targetIndex;                // 目标索引
    bool deleteInheritance;            // 是否删除继承
    
    DeleteOperation(const std::string& target, size_t index, bool delInh = false)
        : targetElement(target), targetIndex(index), deleteInheritance(delInh) {}
};

/**
 * @brief 值填充操作
 */
struct ValueFillOperation {
    std::string propertyName;          // 属性名称
    std::string value;                 // 填充值
    
    ValueFillOperation(const std::string& prop, const std::string& val)
        : propertyName(prop), value(val) {}
};

} // namespace CHTL