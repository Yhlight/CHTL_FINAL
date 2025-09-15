#include "CHTL/CustomSystem/CustomManager.h"
#include "CHTL/CustomSystem/SpecializationOperations.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CustomManager::CustomManager(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_debugMode(false)
    , m_registeredStyles(0)
    , m_registeredElements(0)
    , m_registeredVars(0)
    , m_specializationCount(0)
    , m_successfulSpecializations(0)
    , m_failedSpecializations(0)
{
}

CustomManager::~CustomManager() {
}

bool CustomManager::registerCustomStyle(std::shared_ptr<CustomNode> customStyle) {
    if (!customStyle) {
        addError("Cannot register null custom style");
        return false;
    }
    
    std::string name = customStyle->getCustomName();
    if (name.empty()) {
        addError("Custom style name cannot be empty");
        return false;
    }
    
    if (m_customStyles.find(name) != m_customStyles.end()) {
        addWarning("Custom style '" + name + "' already exists, overwriting");
    }
    
    m_customStyles[name] = customStyle;
    m_registeredStyles++;
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Registered custom style: " << name << std::endl;
    }
    
    return true;
}

bool CustomManager::registerCustomElement(std::shared_ptr<CustomNode> customElement) {
    if (!customElement) {
        addError("Cannot register null custom element");
        return false;
    }
    
    std::string name = customElement->getCustomName();
    if (name.empty()) {
        addError("Custom element name cannot be empty");
        return false;
    }
    
    if (m_customElements.find(name) != m_customElements.end()) {
        addWarning("Custom element '" + name + "' already exists, overwriting");
    }
    
    m_customElements[name] = customElement;
    m_registeredElements++;
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Registered custom element: " << name << std::endl;
    }
    
    return true;
}

bool CustomManager::registerCustomVar(std::shared_ptr<CustomNode> customVar) {
    if (!customVar) {
        addError("Cannot register null custom var");
        return false;
    }
    
    std::string name = customVar->getCustomName();
    if (name.empty()) {
        addError("Custom var name cannot be empty");
        return false;
    }
    
    if (m_customVars.find(name) != m_customVars.end()) {
        addWarning("Custom var '" + name + "' already exists, overwriting");
    }
    
    m_customVars[name] = customVar;
    m_registeredVars++;
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Registered custom var: " << name << std::endl;
    }
    
    return true;
}

std::shared_ptr<CustomNode> CustomManager::getCustomStyle(const std::string& name) const {
    auto it = m_customStyles.find(name);
    return it != m_customStyles.end() ? it->second : nullptr;
}

std::shared_ptr<CustomNode> CustomManager::getCustomElement(const std::string& name) const {
    auto it = m_customElements.find(name);
    return it != m_customElements.end() ? it->second : nullptr;
}

std::shared_ptr<CustomNode> CustomManager::getCustomVar(const std::string& name) const {
    auto it = m_customVars.find(name);
    return it != m_customVars.end() ? it->second : nullptr;
}

std::string CustomManager::applyStyleSpecialization(std::shared_ptr<CustomNode> customStyle, 
                                                   const std::vector<SpecializationOperation>& specializations) {
    if (!customStyle) {
        addError("Cannot apply specialization to null custom style");
        return "";
    }
    
    m_specializationCount++;
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Applying style specialization to: " << customStyle->getCustomName() << std::endl;
    }
    
    try {
        std::string content = customStyle->getCustomContent();
        
        for (const auto& op : specializations) {
            switch (op.type) {
                case SpecializationType::DELETE_PROPERTY: {
                    // 处理属性删除
                    PropertyDeletionOperation* propOp = static_cast<PropertyDeletionOperation*>(const_cast<SpecializationOperation*>(&op));
                    content = processPropertyDeletion(content, propOp->properties);
                    break;
                }
                case SpecializationType::DELETE_INHERITANCE: {
                    // 处理继承删除
                    InheritanceDeletionOperation* inhOp = static_cast<InheritanceDeletionOperation*>(const_cast<SpecializationOperation*>(&op));
                    content = processInheritanceDeletion(content, inhOp->inheritance);
                    break;
                }
                case SpecializationType::ADD_PROPERTY:
                case SpecializationType::MODIFY_PROPERTY: {
                    // 处理属性添加/修改
                    // 这里需要实现属性添加/修改逻辑
                    break;
                }
                default:
                    addWarning("Unsupported specialization type for style");
                    break;
            }
        }
        
        m_successfulSpecializations++;
        return content;
        
    } catch (const std::exception& e) {
        addError("Style specialization failed: " + std::string(e.what()));
        m_failedSpecializations++;
        return "";
    }
}

std::string CustomManager::applyElementSpecialization(std::shared_ptr<CustomNode> customElement, 
                                                     const std::vector<SpecializationOperation>& specializations) {
    if (!customElement) {
        addError("Cannot apply specialization to null custom element");
        return "";
    }
    
    m_specializationCount++;
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Applying element specialization to: " << customElement->getCustomName() << std::endl;
    }
    
    try {
        std::string content = customElement->getCustomContent();
        
        for (const auto& op : specializations) {
            switch (op.type) {
                case SpecializationType::INSERT_ELEMENT: {
                    // 处理元素插入
                    // 这里需要实现元素插入逻辑
                    break;
                }
                case SpecializationType::DELETE_ELEMENT: {
                    // 处理元素删除
                    // 这里需要实现元素删除逻辑
                    break;
                }
                case SpecializationType::MODIFY_ELEMENT: {
                    // 处理元素修改
                    // 这里需要实现元素修改逻辑
                    break;
                }
                default:
                    addWarning("Unsupported specialization type for element");
                    break;
            }
        }
        
        m_successfulSpecializations++;
        return content;
        
    } catch (const std::exception& e) {
        addError("Element specialization failed: " + std::string(e.what()));
        m_failedSpecializations++;
        return "";
    }
}

std::string CustomManager::applyVarSpecialization(std::shared_ptr<CustomNode> customVar, 
                                                 const std::vector<SpecializationOperation>& specializations) {
    if (!customVar) {
        addError("Cannot apply specialization to null custom var");
        return "";
    }
    
    m_specializationCount++;
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Applying var specialization to: " << customVar->getCustomName() << std::endl;
    }
    
    try {
        std::string content = customVar->getCustomContent();
        
        for (const auto& op : specializations) {
            switch (op.type) {
                case SpecializationType::ADD_VALUE:
                case SpecializationType::MODIFY_VALUE: {
                    // 处理值添加/修改
                    // 这里需要实现值添加/修改逻辑
                    break;
                }
                default:
                    addWarning("Unsupported specialization type for var");
                    break;
            }
        }
        
        m_successfulSpecializations++;
        return content;
        
    } catch (const std::exception& e) {
        addError("Var specialization failed: " + std::string(e.what()));
        m_failedSpecializations++;
        return "";
    }
}

std::string CustomManager::processValuelessStyle(std::shared_ptr<CustomNode> customStyle, 
                                                const std::unordered_map<std::string, std::string>& values) {
    if (!customStyle) {
        addError("Cannot process valueless style for null custom style");
        return "";
    }
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Processing valueless style: " << customStyle->getCustomName() << std::endl;
    }
    
    // 这里需要实现无值样式组的处理逻辑
    // 根据提供的值填充样式组
    
    return customStyle->getCustomContent();
}

std::string CustomManager::processIndexAccess(std::shared_ptr<CustomNode> customElement, size_t index) {
    if (!customElement) {
        addError("Cannot process index access for null custom element");
        return "";
    }
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Processing index access for: " << customElement->getCustomName() 
                  << " at index: " << index << std::endl;
    }
    
    // 这里需要实现索引访问逻辑
    // 返回指定索引的元素内容
    
    return customElement->getCustomContent();
}

std::string CustomManager::processElementInsertion(std::shared_ptr<CustomNode> customElement, 
                                                  const InsertOperation& insertOp) {
    if (!customElement) {
        addError("Cannot process element insertion for null custom element");
        return "";
    }
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Processing element insertion for: " << customElement->getCustomName() << std::endl;
    }
    
    // 这里需要实现元素插入逻辑
    // 根据插入位置和内容插入元素
    
    return customElement->getCustomContent();
}

std::string CustomManager::processElementDeletion(std::shared_ptr<CustomNode> customElement, 
                                                 const DeleteOperation& deleteOp) {
    if (!customElement) {
        addError("Cannot process element deletion for null custom element");
        return "";
    }
    
    if (m_debugMode) {
        std::cout << "[CustomManager] Processing element deletion for: " << customElement->getCustomName() << std::endl;
    }
    
    // 这里需要实现元素删除逻辑
    // 根据删除操作删除指定元素
    
    return customElement->getCustomContent();
}

void CustomManager::clear() {
    m_customStyles.clear();
    m_customElements.clear();
    m_customVars.clear();
    m_registeredStyles = 0;
    m_registeredElements = 0;
    m_registeredVars = 0;
    m_specializationCount = 0;
    m_successfulSpecializations = 0;
    m_failedSpecializations = 0;
}

size_t CustomManager::getCustomStyleCount() const {
    return m_registeredStyles;
}

size_t CustomManager::getCustomElementCount() const {
    return m_registeredElements;
}

size_t CustomManager::getCustomVarCount() const {
    return m_registeredVars;
}

void CustomManager::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CustomManager::isDebugMode() const {
    return m_debugMode;
}

std::string CustomManager::getStatistics() const {
    std::ostringstream oss;
    oss << "CustomManager Statistics:\n";
    oss << "  Registered styles: " << m_registeredStyles << "\n";
    oss << "  Registered elements: " << m_registeredElements << "\n";
    oss << "  Registered vars: " << m_registeredVars << "\n";
    oss << "  Specialization count: " << m_specializationCount << "\n";
    oss << "  Successful specializations: " << m_successfulSpecializations << "\n";
    oss << "  Failed specializations: " << m_failedSpecializations << "\n";
    oss << "  Success rate: " << (m_specializationCount > 0 ? (m_successfulSpecializations * 100.0 / m_specializationCount) : 0) << "%\n";
    
    return oss.str();
}

std::string CustomManager::processPropertyDeletion(const std::string& content, 
                                                  const std::vector<std::string>& properties) {
    std::string result = content;
    
    for (const auto& prop : properties) {
        // 使用正则表达式删除属性
        std::regex propRegex(prop + "\\s*:\\s*[^;]+;?");
        result = std::regex_replace(result, propRegex, "");
    }
    
    return result;
}

std::string CustomManager::processInheritanceDeletion(const std::string& content, 
                                                     const std::string& inheritance) {
    std::string result = content;
    
    // 删除继承引用
    std::regex inhRegex(inheritance + "\\s*;?");
    result = std::regex_replace(result, inhRegex, "");
    
    return result;
}

void CustomManager::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("CustomManager Error: " + message);
    }
}

void CustomManager::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("CustomManager Warning: " + message);
    }
}

} // namespace CHTL