#include "CHTLConstraint.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

ConstraintNode::ConstraintNode(const ConstraintDefinition& def, size_t l, size_t c)
    : CHTLNode(NodeType::Configuration, def.target_name, l, c), definition(def) {
}

std::string ConstraintNode::toHTML() const {
    // Constraint nodes don't generate HTML directly
    // They are processed by the constraint manager
    return "";
}

CHTLConstraintManager::CHTLConstraintManager() {
    // Initialize constraint manager
}

CHTLConstraintManager::~CHTLConstraintManager() {
    // Cleanup
}

bool CHTLConstraintManager::addConstraint(const ConstraintDefinition& constraint) {
    if (!validateConstraint(constraint)) {
        return false;
    }
    
    std::string key = generateConstraintKey(constraint.target_name);
    if (constraints.find(key) != constraints.end()) {
        return false; // Constraint already exists
    }
    
    constraints[key] = std::make_shared<ConstraintDefinition>(constraint);
    return true;
}

bool CHTLConstraintManager::removeConstraint(const std::string& constraint_name) {
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return false;
    }
    
    constraints.erase(it);
    return true;
}

bool CHTLConstraintManager::updateConstraint(const std::string& constraint_name, const ConstraintDefinition& constraint) {
    if (!validateConstraint(constraint)) {
        return false;
    }
    
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return false;
    }
    
    it->second = std::make_shared<ConstraintDefinition>(constraint);
    return true;
}

bool CHTLConstraintManager::enableConstraint(const std::string& constraint_name) {
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return false;
    }
    
    it->second->is_active = true;
    return true;
}

bool CHTLConstraintManager::disableConstraint(const std::string& constraint_name) {
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return false;
    }
    
    it->second->is_active = false;
    return true;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateElement(const std::string& element_name, const std::map<std::string, std::string>& attributes) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != ConstraintScope::Element) {
            continue;
        }
        
        // Check if element is in exception list
        if (isTargetInExceptionList(element_name, constraint->except_list)) {
            continue;
        }
        
        // Check if constraint applies to this element
        if (constraint->type == ConstraintType::Precise && constraint->target_name != element_name) {
            continue;
        }
        
        // Evaluate constraint
        std::map<std::string, std::string> context;
        context["element_name"] = element_name;
        context["attributes"] = std::to_string(attributes.size());
        for (const auto& attr_pair : attributes) {
            context["attr_" + attr_pair.first] = attr_pair.second;
        }
        
        if (!evaluateConstraint(*constraint, context)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = element_name;
            violation.violation_message = generateConstraintViolationMessage(*constraint, element_name);
            violation.violation_type = "element_constraint";
            violation.suggested_fix = "Check element definition and attributes";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateAttribute(const std::string& attribute_name, const std::string& attribute_value) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != ConstraintScope::Attribute) {
            continue;
        }
        
        // Check if attribute is in exception list
        if (isTargetInExceptionList(attribute_name, constraint->except_list)) {
            continue;
        }
        
        // Check if constraint applies to this attribute
        if (constraint->type == ConstraintType::Precise && constraint->target_name != attribute_name) {
            continue;
        }
        
        // Evaluate constraint
        std::map<std::string, std::string> context;
        context["attribute_name"] = attribute_name;
        context["attribute_value"] = attribute_value;
        
        if (!evaluateConstraint(*constraint, context)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = attribute_name;
            violation.violation_message = generateConstraintViolationMessage(*constraint, attribute_name);
            violation.violation_type = "attribute_constraint";
            violation.suggested_fix = "Check attribute value and format";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateStyle(const std::string& style_name, const std::map<std::string, std::string>& properties) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != ConstraintScope::Style) {
            continue;
        }
        
        // Check if style is in exception list
        if (isTargetInExceptionList(style_name, constraint->except_list)) {
            continue;
        }
        
        // Check if constraint applies to this style
        if (constraint->type == ConstraintType::Precise && constraint->target_name != style_name) {
            continue;
        }
        
        // Evaluate constraint
        std::map<std::string, std::string> context;
        context["style_name"] = style_name;
        context["properties"] = std::to_string(properties.size());
        for (const auto& prop_pair : properties) {
            context["prop_" + prop_pair.first] = prop_pair.second;
        }
        
        if (!evaluateConstraint(*constraint, context)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = style_name;
            violation.violation_message = generateConstraintViolationMessage(*constraint, style_name);
            violation.violation_type = "style_constraint";
            violation.suggested_fix = "Check style definition and properties";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateTemplate(const std::string& template_name, const std::map<std::string, std::string>& parameters) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != ConstraintScope::Template) {
            continue;
        }
        
        // Check if template is in exception list
        if (isTargetInExceptionList(template_name, constraint->except_list)) {
            continue;
        }
        
        // Check if constraint applies to this template
        if (constraint->type == ConstraintType::Precise && constraint->target_name != template_name) {
            continue;
        }
        
        // Evaluate constraint
        std::map<std::string, std::string> context;
        context["template_name"] = template_name;
        context["parameters"] = std::to_string(parameters.size());
        for (const auto& param_pair : parameters) {
            context["param_" + param_pair.first] = param_pair.second;
        }
        
        if (!evaluateConstraint(*constraint, context)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = template_name;
            violation.violation_message = generateConstraintViolationMessage(*constraint, template_name);
            violation.violation_type = "template_constraint";
            violation.suggested_fix = "Check template definition and parameters";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateCustom(const std::string& custom_name, const std::map<std::string, std::string>& parameters) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != ConstraintScope::Custom) {
            continue;
        }
        
        // Check if custom is in exception list
        if (isTargetInExceptionList(custom_name, constraint->except_list)) {
            continue;
        }
        
        // Check if constraint applies to this custom
        if (constraint->type == ConstraintType::Precise && constraint->target_name != custom_name) {
            continue;
        }
        
        // Evaluate constraint
        std::map<std::string, std::string> context;
        context["custom_name"] = custom_name;
        context["parameters"] = std::to_string(parameters.size());
        for (const auto& param_pair : parameters) {
            context["param_" + param_pair.first] = param_pair.second;
        }
        
        if (!evaluateConstraint(*constraint, context)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = custom_name;
            violation.violation_message = generateConstraintViolationMessage(*constraint, custom_name);
            violation.violation_type = "custom_constraint";
            violation.suggested_fix = "Check custom definition and parameters";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateImport(const std::string& import_path, const std::string& import_type) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != ConstraintScope::Import) {
            continue;
        }
        
        // Check if import is in exception list
        if (isTargetInExceptionList(import_path, constraint->except_list)) {
            continue;
        }
        
        // Check if constraint applies to this import
        if (constraint->type == ConstraintType::Precise && constraint->target_name != import_path) {
            continue;
        }
        
        // Evaluate constraint
        std::map<std::string, std::string> context;
        context["import_path"] = import_path;
        context["import_type"] = import_type;
        
        if (!evaluateConstraint(*constraint, context)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = import_path;
            violation.violation_message = generateConstraintViolationMessage(*constraint, import_path);
            violation.violation_type = "import_constraint";
            violation.suggested_fix = "Check import path and type";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateNamespace(const std::string& namespace_name, const std::vector<std::string>& symbols) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != ConstraintScope::Namespace) {
            continue;
        }
        
        // Check if namespace is in exception list
        if (isTargetInExceptionList(namespace_name, constraint->except_list)) {
            continue;
        }
        
        // Check if constraint applies to this namespace
        if (constraint->type == ConstraintType::Precise && constraint->target_name != namespace_name) {
            continue;
        }
        
        // Evaluate constraint
        std::map<std::string, std::string> context;
        context["namespace_name"] = namespace_name;
        context["symbols"] = std::to_string(symbols.size());
        for (size_t i = 0; i < symbols.size(); ++i) {
            context["symbol_" + std::to_string(i)] = symbols[i];
        }
        
        if (!evaluateConstraint(*constraint, context)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = namespace_name;
            violation.violation_message = generateConstraintViolationMessage(*constraint, namespace_name);
            violation.violation_type = "namespace_constraint";
            violation.suggested_fix = "Check namespace definition and symbols";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateAll() {
    std::vector<ConstraintViolation> all_violations;
    
    // Validate all scopes
    for (int scope = 0; scope < static_cast<int>(ConstraintScope::Namespace) + 1; ++scope) {
        auto scope_violations = validateScope(static_cast<ConstraintScope>(scope));
        all_violations.insert(all_violations.end(), scope_violations.begin(), scope_violations.end());
    }
    
    return all_violations;
}

std::vector<ConstraintViolation> CHTLConstraintManager::validateScope(ConstraintScope scope) {
    std::vector<ConstraintViolation> violations;
    
    for (const auto& pair : constraints) {
        const auto& constraint = pair.second;
        if (!constraint->is_active || constraint->scope != scope) {
            continue;
        }
        
        // This is a simplified validation - in a real implementation,
        // this would validate all targets of the given scope
        // For now, we'll just check if the constraint is valid
        if (!validateConstraint(*constraint)) {
            ConstraintViolation violation;
            violation.constraint_name = constraint->target_name;
            violation.target_name = "constraint_definition";
            violation.violation_message = "Invalid constraint definition";
            violation.violation_type = "constraint_validation";
            violation.suggested_fix = "Fix constraint definition";
            violations.push_back(violation);
        }
    }
    
    return violations;
}

std::shared_ptr<ConstraintDefinition> CHTLConstraintManager::getConstraint(const std::string& constraint_name) const {
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    return (it != constraints.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLConstraintManager::getConstraintNames() const {
    std::vector<std::string> names;
    for (const auto& pair : constraints) {
        names.push_back(pair.second->target_name);
    }
    return names;
}

std::vector<std::string> CHTLConstraintManager::getConstraintNames(ConstraintScope scope) const {
    std::vector<std::string> names;
    for (const auto& pair : constraints) {
        if (pair.second->scope == scope) {
            names.push_back(pair.second->target_name);
        }
    }
    return names;
}

std::vector<std::string> CHTLConstraintManager::getConstraintNames(ConstraintType type) const {
    std::vector<std::string> names;
    for (const auto& pair : constraints) {
        if (pair.second->type == type) {
            names.push_back(pair.second->target_name);
        }
    }
    return names;
}

bool CHTLConstraintManager::constraintExists(const std::string& constraint_name) const {
    std::string key = generateConstraintKey(constraint_name);
    return constraints.find(key) != constraints.end();
}

bool CHTLConstraintManager::addException(const std::string& constraint_name, const std::string& exception_name) {
    if (!validateException(exception_name)) {
        return false;
    }
    
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return false;
    }
    
    it->second->except_list.push_back(exception_name);
    return true;
}

bool CHTLConstraintManager::removeException(const std::string& constraint_name, const std::string& exception_name) {
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return false;
    }
    
    auto& except_list = it->second->except_list;
    auto except_it = std::find(except_list.begin(), except_list.end(), exception_name);
    if (except_it == except_list.end()) {
        return false;
    }
    
    except_list.erase(except_it);
    return true;
}

bool CHTLConstraintManager::hasException(const std::string& constraint_name, const std::string& exception_name) const {
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    if (it == constraints.end()) {
        return false;
    }
    
    return std::find(it->second->except_list.begin(), it->second->except_list.end(), exception_name) != it->second->except_list.end();
}

std::vector<std::string> CHTLConstraintManager::getExceptions(const std::string& constraint_name) const {
    std::string key = generateConstraintKey(constraint_name);
    auto it = constraints.find(key);
    return (it != constraints.end()) ? it->second->except_list : std::vector<std::string>();
}

std::string CHTLConstraintManager::processConstraintExpression(const std::string& expression, const std::map<std::string, std::string>& context) {
    return substituteVariables(expression, context);
}

bool CHTLConstraintManager::evaluateConstraint(const ConstraintDefinition& constraint, const std::map<std::string, std::string>& context) {
    std::string processed_expression = processConstraintExpression(constraint.constraint_expression, context);
    return evaluateBooleanExpression(processed_expression, context);
}

std::string CHTLConstraintManager::generateConstraintViolationMessage(const ConstraintDefinition& constraint, const std::string& target_name) {
    return generateViolationMessage(constraint, target_name, "constraint_violation");
}

bool CHTLConstraintManager::validateConstraint(const ConstraintDefinition& constraint) const {
    // Validate constraint name
    if (constraint.target_name.empty()) {
        return false;
    }
    
    // Validate constraint expression
    if (!validateConstraintExpression(constraint.constraint_expression)) {
        return false;
    }
    
    // Validate type and scope
    switch (constraint.type) {
        case ConstraintType::Precise:
        case ConstraintType::Type:
        case ConstraintType::Global:
            break;
        default:
            return false;
    }
    
    switch (constraint.scope) {
        case ConstraintScope::Element:
        case ConstraintScope::Attribute:
        case ConstraintScope::Style:
        case ConstraintScope::Template:
        case ConstraintScope::Custom:
        case ConstraintScope::Import:
        case ConstraintScope::Namespace:
            break;
        default:
            return false;
    }
    
    return true;
}

bool CHTLConstraintManager::validateConstraintExpression(const std::string& expression) const {
    // Simple expression validation
    return !expression.empty();
}

bool CHTLConstraintManager::validateException(const std::string& exception_name) const {
    return isValidExceptionName(exception_name);
}

void CHTLConstraintManager::clearAllConstraints() {
    constraints.clear();
    violations.clear();
}

void CHTLConstraintManager::clearViolations() {
    violations.clear();
}

std::vector<ConstraintViolation> CHTLConstraintManager::getViolations() const {
    return violations;
}

bool CHTLConstraintManager::hasViolations() const {
    return !violations.empty();
}

int CHTLConstraintManager::getConstraintCount() const {
    return constraints.size();
}

int CHTLConstraintManager::getViolationCount() const {
    return violations.size();
}

int CHTLConstraintManager::getActiveConstraintCount() const {
    int count = 0;
    for (const auto& pair : constraints) {
        if (pair.second->is_active) {
            count++;
        }
    }
    return count;
}

std::map<ConstraintScope, int> CHTLConstraintManager::getConstraintCountByScope() const {
    std::map<ConstraintScope, int> counts;
    
    for (const auto& pair : constraints) {
        counts[pair.second->scope]++;
    }
    
    return counts;
}

std::map<ConstraintType, int> CHTLConstraintManager::getConstraintCountByType() const {
    std::map<ConstraintType, int> counts;
    
    for (const auto& pair : constraints) {
        counts[pair.second->type]++;
    }
    
    return counts;
}

std::string CHTLConstraintManager::generateConstraintKey(const std::string& name) const {
    return name;
}

bool CHTLConstraintManager::isTargetInExceptionList(const std::string& target_name, const std::vector<std::string>& exception_list) const {
    return std::find(exception_list.begin(), exception_list.end(), target_name) != exception_list.end();
}

std::string CHTLConstraintManager::parseConstraintExpression(const std::string& expression) {
    return expression;
}

bool CHTLConstraintManager::evaluateBooleanExpression(const std::string& /* expression */, const std::map<std::string, std::string>& /* context */) {
    // Simple boolean expression evaluation
    // This is a simplified implementation
    return true;
}

std::string CHTLConstraintManager::substituteVariables(const std::string& expression, const std::map<std::string, std::string>& context) {
    std::string result = expression;
    
    for (const auto& pair : context) {
        std::string placeholder = "${" + pair.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos += pair.second.length();
        }
    }
    
    return result;
}

std::vector<std::string> CHTLConstraintManager::extractVariables(const std::string& expression) {
    std::vector<std::string> variables;
    std::regex var_regex("\\$\\{([^}]+)\\}");
    std::sregex_iterator iter(expression.begin(), expression.end(), var_regex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        variables.push_back((*iter)[1].str());
    }
    
    return variables;
}

bool CHTLConstraintManager::isValidConstraintName(const std::string& name) const {
    return !name.empty() && std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

bool CHTLConstraintManager::isValidExceptionName(const std::string& name) const {
    return !name.empty() && std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

std::string CHTLConstraintManager::generateViolationMessage(const ConstraintDefinition& constraint, const std::string& target_name, const std::string& violation_type) {
    std::stringstream ss;
    ss << "Constraint violation: " << constraint.target_name 
       << " on " << target_name 
       << " (" << violation_type << ")";
    return ss.str();
}

void CHTLConstraintManager::addViolation(const ConstraintDefinition& constraint, const std::string& target_name, const std::string& violation_type, size_t line, size_t column) {
    ConstraintViolation violation;
    violation.constraint_name = constraint.target_name;
    violation.target_name = target_name;
    violation.violation_message = generateViolationMessage(constraint, target_name, violation_type);
    violation.violation_type = violation_type;
    violation.line_number = line;
    violation.column_number = column;
    violation.suggested_fix = "Check constraint definition and target";
    violations.push_back(violation);
}

} // namespace CHTL