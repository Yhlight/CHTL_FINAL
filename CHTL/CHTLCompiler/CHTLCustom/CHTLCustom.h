#ifndef CHTL_CUSTOM_H
#define CHTL_CUSTOM_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// Custom specialization types
enum class CustomType {
    StyleGroup,
    Element,
    VarGroup
};

// Custom specialization operations
enum class CustomOperation {
    Specialize,
    Delete,
    Insert,
    Replace,
    Modify
};

// Custom specialization definition
struct CustomSpecialization {
    CustomType type;
    std::string target_name;
    std::string specialization_name;
    CustomOperation operation;
    std::map<std::string, std::string> parameters;
    std::vector<std::shared_ptr<CHTLNode>> content;
    std::vector<std::string> delete_attributes;
    std::vector<std::string> delete_inheritance;
    std::map<std::string, std::string> insert_elements;
    std::map<std::string, std::string> replace_elements;
};

// Custom element reference
struct CustomElementReference {
    std::string element_name;
    std::string specialization_name;
    std::map<std::string, std::string> parameters;
    std::vector<std::shared_ptr<CHTLNode>> content;
};

// Custom manager for handling customizations
class CHTLCustomManager {
public:
    CHTLCustomManager();
    ~CHTLCustomManager();
    
    // Custom specialization management
    bool addCustomSpecialization(const CustomSpecialization& specialization);
    bool removeCustomSpecialization(const std::string& name, CustomType type);
    std::shared_ptr<CustomSpecialization> findCustomSpecialization(const std::string& name, CustomType type);
    
    // Custom element management
    bool addCustomElement(const std::string& name, const CustomElementReference& element);
    bool removeCustomElement(const std::string& name);
    std::shared_ptr<CustomElementReference> findCustomElement(const std::string& name);
    
    // Custom processing
    std::vector<std::shared_ptr<CHTLNode>> processCustomElement(const std::string& name, const std::map<std::string, std::string>& parameters = {});
    std::vector<std::shared_ptr<CHTLNode>> processCustomSpecialization(const std::string& name, CustomType type, const std::map<std::string, std::string>& parameters = {});
    
    // Validation
    bool validateCustomSpecialization(const CustomSpecialization& specialization);
    bool validateCustomElement(const CustomElementReference& element);
    
    // Utility methods
    std::vector<std::string> getCustomElementNames() const;
    std::vector<std::string> getCustomSpecializationNames(CustomType type) const;
    void clearAllCustomizations();
    
private:
    std::map<std::string, std::shared_ptr<CustomSpecialization>> style_group_specializations;
    std::map<std::string, std::shared_ptr<CustomSpecialization>> element_specializations;
    std::map<std::string, std::shared_ptr<CustomSpecialization>> var_group_specializations;
    std::map<std::string, std::shared_ptr<CustomElementReference>> custom_elements;
    
    // Helper methods
    std::string generateCustomKey(const std::string& name, CustomType type);
    std::vector<std::shared_ptr<CHTLNode>> applyCustomSpecialization(const CustomSpecialization& specialization, const std::map<std::string, std::string>& parameters);
    std::vector<std::shared_ptr<CHTLNode>> processSpecializationContent(const std::vector<std::shared_ptr<CHTLNode>>& content, const std::map<std::string, std::string>& parameters);
    std::vector<std::shared_ptr<CHTLNode>> processDeleteOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization);
    std::vector<std::shared_ptr<CHTLNode>> processInsertOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization);
    std::vector<std::shared_ptr<CHTLNode>> processReplaceOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization);
    std::vector<std::shared_ptr<CHTLNode>> processModifyOperations(const std::vector<std::shared_ptr<CHTLNode>>& content, const CustomSpecialization& specialization);
};

} // namespace CHTL

#endif // CHTL_CUSTOM_H