#include "CHTLCustom.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLCustomManager::CHTLCustomManager() {
    // Initialize custom manager
}

CHTLCustomManager::~CHTLCustomManager() {
    // Cleanup
}

bool CHTLCustomManager::addCustomSpecialization(const CustomSpecialization& specialization) {
    if (!validateCustomSpecialization(specialization)) {
        return false;
    }
    
    std::string key = generateCustomKey(specialization.specialization_name, specialization.type);
    
    switch (specialization.type) {
        case CustomType::StyleGroup:
            style_group_specializations[key] = std::make_shared<CustomSpecialization>(specialization);
            break;
        case CustomType::Element:
            element_specializations[key] = std::make_shared<CustomSpecialization>(specialization);
            break;
        case CustomType::VarGroup:
            var_group_specializations[key] = std::make_shared<CustomSpecialization>(specialization);
            break;
        default:
            return false;
    }
    
    return true;
}

bool CHTLCustomManager::removeCustomSpecialization(const std::string& name, CustomType type) {
    std::string key = generateCustomKey(name, type);
    
    switch (type) {
        case CustomType::StyleGroup:
            return style_group_specializations.erase(key) > 0;
        case CustomType::Element:
            return element_specializations.erase(key) > 0;
        case CustomType::VarGroup:
            return var_group_specializations.erase(key) > 0;
        default:
            return false;
    }
}

std::shared_ptr<CustomSpecialization> CHTLCustomManager::findCustomSpecialization(const std::string& name, CustomType type) {
    std::string key = generateCustomKey(name, type);
    
    switch (type) {
        case CustomType::StyleGroup:
            {
                auto it = style_group_specializations.find(key);
                return (it != style_group_specializations.end()) ? it->second : nullptr;
            }
        case CustomType::Element:
            {
                auto it = element_specializations.find(key);
                return (it != element_specializations.end()) ? it->second : nullptr;
            }
        case CustomType::VarGroup:
            {
                auto it = var_group_specializations.find(key);
                return (it != var_group_specializations.end()) ? it->second : nullptr;
            }
        default:
            return nullptr;
    }
}

bool CHTLCustomManager::addCustomElement(const std::string& name, const CustomElementReference& element) {
    if (!validateCustomElement(element)) {
        return false;
    }
    
    custom_elements[name] = std::make_shared<CustomElementReference>(element);
    return true;
}

bool CHTLCustomManager::removeCustomElement(const std::string& name) {
    return custom_elements.erase(name) > 0;
}

std::shared_ptr<CustomElementReference> CHTLCustomManager::findCustomElement(const std::string& name) {
    auto it = custom_elements.find(name);
    return (it != custom_elements.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::processCustomElement(const std::string& name, const std::map<std::string, std::string>& parameters) {
    auto element = findCustomElement(name);
    if (!element) {
        return {};
    }
    
    // Process the custom element with parameters
    return processSpecializationContent(element->content, parameters);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::processCustomSpecialization(const std::string& name, CustomType type, const std::map<std::string, std::string>& parameters) {
    auto specialization = findCustomSpecialization(name, type);
    if (!specialization) {
        return {};
    }
    
    return applyCustomSpecialization(*specialization, parameters);
}

bool CHTLCustomManager::validateCustomSpecialization(const CustomSpecialization& specialization) {
    // Validate specialization name
    if (specialization.specialization_name.empty()) {
        return false;
    }
    
    // Validate target name
    if (specialization.target_name.empty()) {
        return false;
    }
    
    // Validate operation
    switch (specialization.operation) {
        case CustomOperation::Specialize:
        case CustomOperation::Delete:
        case CustomOperation::Insert:
        case CustomOperation::Replace:
        case CustomOperation::Modify:
            break;
        default:
            return false;
    }
    
    return true;
}

bool CHTLCustomManager::validateCustomElement(const CustomElementReference& element) {
    // Validate element name
    if (element.element_name.empty()) {
        return false;
    }
    
    // Validate specialization name
    if (element.specialization_name.empty()) {
        return false;
    }
    
    return true;
}

std::vector<std::string> CHTLCustomManager::getCustomElementNames() const {
    std::vector<std::string> names;
    for (const auto& pair : custom_elements) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> CHTLCustomManager::getCustomSpecializationNames(CustomType type) const {
    std::vector<std::string> names;
    
    switch (type) {
        case CustomType::StyleGroup:
            for (const auto& pair : style_group_specializations) {
                names.push_back(pair.first);
            }
            break;
        case CustomType::Element:
            for (const auto& pair : element_specializations) {
                names.push_back(pair.first);
            }
            break;
        case CustomType::VarGroup:
            for (const auto& pair : var_group_specializations) {
                names.push_back(pair.first);
            }
            break;
        default:
            break;
    }
    
    return names;
}

void CHTLCustomManager::clearAllCustomizations() {
    style_group_specializations.clear();
    element_specializations.clear();
    var_group_specializations.clear();
    custom_elements.clear();
}

std::string CHTLCustomManager::generateCustomKey(const std::string& name, CustomType type) {
    std::stringstream ss;
    ss << static_cast<int>(type) << ":" << name;
    return ss.str();
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::applyCustomSpecialization(const CustomSpecialization& specialization, const std::map<std::string, std::string>& parameters) {
    std::vector<std::shared_ptr<CHTLNode>> result = specialization.content;
    
    // Apply parameter substitution
    result = processSpecializationContent(result, parameters);
    
    // Apply operations based on type
    switch (specialization.operation) {
        case CustomOperation::Delete:
            result = processDeleteOperations(result, specialization);
            break;
        case CustomOperation::Insert:
            result = processInsertOperations(result, specialization);
            break;
        case CustomOperation::Replace:
            result = processReplaceOperations(result, specialization);
            break;
        case CustomOperation::Modify:
            result = processModifyOperations(result, specialization);
            break;
        case CustomOperation::Specialize:
        default:
            // No additional processing needed
            break;
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::processSpecializationContent(const std::vector<std::shared_ptr<CHTLNode>>& content, const std::map<std::string, std::string>& parameters) {
    // Simple parameter substitution
    // In a real implementation, this would traverse the AST and substitute parameters
    return content;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::processDeleteOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization) {
    std::vector<std::shared_ptr<CHTLNode>> result = content;
    
    // Process delete attributes
    for (const auto& attr : specialization.delete_attributes) {
        // TODO: Implement attribute deletion
    }
    
    // Process delete inheritance
    for (const auto& inherit : specialization.delete_inheritance) {
        // TODO: Implement inheritance deletion
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::processInsertOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization) {
    std::vector<std::shared_ptr<CHTLNode>> result = content;
    
    // Process insert elements
    for (const auto& pair : specialization.insert_elements) {
        // TODO: Implement element insertion
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::processReplaceOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization) {
    std::vector<std::shared_ptr<CHTLNode>> result = content;
    
    // Process replace elements
    for (const auto& pair : specialization.replace_elements) {
        // TODO: Implement element replacement
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLCustomManager::processModifyOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization) {
    std::vector<std::shared_ptr<CHTLNode>> result = content;
    
    // Process modify operations
    // This would combine delete, insert, and replace operations
    result = processDeleteOperations(result, specialization);
    result = processInsertOperations(result, specialization);
    result = processReplaceOperations(result, specialization);
    
    return result;
}

} // namespace CHTL