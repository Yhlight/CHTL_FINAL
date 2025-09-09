#ifndef CHTL_CONSTRAINT_H
#define CHTL_CONSTRAINT_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// Constraint types
enum class ConstraintType {
    Precise,    // Exact constraint
    Type,       // Type-based constraint
    Global      // Global constraint
};

// Constraint scope
enum class ConstraintScope {
    Element,
    Attribute,
    Style,
    Template,
    Custom,
    Import,
    Namespace
};

// Constraint definition
struct ConstraintDefinition {
    ConstraintType type;
    ConstraintScope scope;
    std::string target_name;
    std::string constraint_expression;
    std::vector<std::string> except_list;
    std::map<std::string, std::string> parameters;
    bool is_active;
    int priority;
};

// Constraint violation
struct ConstraintViolation {
    std::string constraint_name;
    std::string target_name;
    std::string violation_message;
    std::string violation_type;
    size_t line_number;
    size_t column_number;
    std::string suggested_fix;
};

// Constraint node for AST
class ConstraintNode : public CHTLNode {
public:
    ConstraintDefinition definition;
    
    ConstraintNode(const ConstraintDefinition& def, size_t l = 0, size_t c = 0);
    virtual ~ConstraintNode() = default;
    
    const ConstraintDefinition& getDefinition() const { return definition; }
    virtual std::string toHTML() const override;
};

// Constraint manager for handling constraints
class CHTLConstraintManager {
public:
    CHTLConstraintManager();
    ~CHTLConstraintManager();
    
    // Constraint management
    bool addConstraint(const ConstraintDefinition& constraint);
    bool removeConstraint(const std::string& constraint_name);
    bool updateConstraint(const std::string& constraint_name, const ConstraintDefinition& constraint);
    bool enableConstraint(const std::string& constraint_name);
    bool disableConstraint(const std::string& constraint_name);
    
    // Constraint validation
    std::vector<ConstraintViolation> validateElement(const std::string& element_name, const std::map<std::string, std::string>& attributes = {});
    std::vector<ConstraintViolation> validateAttribute(const std::string& attribute_name, const std::string& attribute_value);
    std::vector<ConstraintViolation> validateStyle(const std::string& style_name, const std::map<std::string, std::string>& properties = {});
    std::vector<ConstraintViolation> validateTemplate(const std::string& template_name, const std::map<std::string, std::string>& parameters = {});
    std::vector<ConstraintViolation> validateCustom(const std::string& custom_name, const std::map<std::string, std::string>& parameters = {});
    std::vector<ConstraintViolation> validateImport(const std::string& import_path, const std::string& import_type);
    std::vector<ConstraintViolation> validateNamespace(const std::string& namespace_name, const std::vector<std::string>& symbols = {});
    
    // Global validation
    std::vector<ConstraintViolation> validateAll();
    std::vector<ConstraintViolation> validateScope(ConstraintScope scope);
    
    // Constraint queries
    std::shared_ptr<ConstraintDefinition> getConstraint(const std::string& constraint_name) const;
    std::vector<std::string> getConstraintNames() const;
    std::vector<std::string> getConstraintNames(ConstraintScope scope) const;
    std::vector<std::string> getConstraintNames(ConstraintType type) const;
    bool constraintExists(const std::string& constraint_name) const;
    
    // Exception handling
    bool addException(const std::string& constraint_name, const std::string& exception_name);
    bool removeException(const std::string& constraint_name, const std::string& exception_name);
    bool hasException(const std::string& constraint_name, const std::string& exception_name) const;
    std::vector<std::string> getExceptions(const std::string& constraint_name) const;
    
    // Constraint processing
    std::string processConstraintExpression(const std::string& expression, const std::map<std::string, std::string>& context = {});
    bool evaluateConstraint(const ConstraintDefinition& constraint, const std::map<std::string, std::string>& context = {});
    std::string generateConstraintViolationMessage(const ConstraintDefinition& constraint, const std::string& target_name);
    
    // Validation
    bool validateConstraint(const ConstraintDefinition& constraint) const;
    bool validateConstraintExpression(const std::string& expression) const;
    bool validateException(const std::string& exception_name) const;
    
    // Utility methods
    void clearAllConstraints();
    void clearViolations();
    std::vector<ConstraintViolation> getViolations() const;
    bool hasViolations() const;
    
    // Constraint statistics
    int getConstraintCount() const;
    int getViolationCount() const;
    int getActiveConstraintCount() const;
    std::map<ConstraintScope, int> getConstraintCountByScope() const;
    std::map<ConstraintType, int> getConstraintCountByType() const;
    
private:
    std::map<std::string, std::shared_ptr<ConstraintDefinition>> constraints;
    std::vector<ConstraintViolation> violations;
    
    // Helper methods
    std::string generateConstraintKey(const std::string& name) const;
    bool isTargetInExceptionList(const std::string& target_name, const std::vector<std::string>& exception_list) const;
    std::string parseConstraintExpression(const std::string& expression);
    bool evaluateBooleanExpression(const std::string& expression, const std::map<std::string, std::string>& context);
    std::string substituteVariables(const std::string& expression, const std::map<std::string, std::string>& context);
    std::vector<std::string> extractVariables(const std::string& expression);
    bool isValidConstraintName(const std::string& name) const;
    bool isValidExceptionName(const std::string& name) const;
    std::string generateViolationMessage(const ConstraintDefinition& constraint, const std::string& target_name, const std::string& violation_type);
    void addViolation(const ConstraintDefinition& constraint, const std::string& target_name, const std::string& violation_type, size_t line = 0, size_t column = 0);
};

} // namespace CHTL

#endif // CHTL_CONSTRAINT_H