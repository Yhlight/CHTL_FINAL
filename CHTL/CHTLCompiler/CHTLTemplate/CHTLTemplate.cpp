#include "CHTLTemplate.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// CHTLTemplate implementation
void CHTLTemplate::addContent(std::shared_ptr<CHTLNode> node) {
    if (node) {
        content.push_back(node);
    }
}

void CHTLTemplate::addModification(const TemplateModification& modification) {
    modifications.push_back(modification);
}

void CHTLTemplate::setParameter(const std::string& key, const std::string& value) {
    parameters[key] = value;
}

void CHTLTemplate::addDependency(const std::string& dependency) {
    dependencies.insert(dependency);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplate::processTemplate(const std::map<std::string, std::string>& parameters) {
    // Merge template parameters with provided parameters
    std::map<std::string, std::string> final_params = this->parameters;
    for (const auto& param : parameters) {
        final_params[param.first] = param.second;
    }
    
    // Apply parameter substitution
    std::vector<std::shared_ptr<CHTLNode>> processed_content = content; // TODO: Implement parameter substitution
    
    // Apply modifications
    return applyModifications(processed_content);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplate::applyModifications(const std::vector<std::shared_ptr<CHTLNode>>& base_content) {
    std::vector<std::shared_ptr<CHTLNode>> result = base_content;
    
    for (const auto& modification : modifications) {
        switch (modification.type) {
            case InheritanceType::Inherit:
                // Inherit from parent template
                // This would be implemented based on the parent template
                break;
            case InheritanceType::Delete:
                // TODO: Implement delete modification
                break;
            case InheritanceType::Insert:
                // TODO: Implement insert modification
                break;
            case InheritanceType::After:
                // TODO: Implement after modification
                break;
            case InheritanceType::Before:
                // TODO: Implement before modification
                break;
            case InheritanceType::Replace:
                // TODO: Implement replace modification
                break;
            default:
                break;
        }
    }
    
    return result;
}

bool CHTLTemplate::isValid() const {
    return !name.empty() && !content.empty();
}

std::vector<std::string> CHTLTemplate::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (name.empty()) {
        errors.push_back("Template name is empty");
    }
    
    if (content.empty()) {
        errors.push_back("Template content is empty");
    }
    
    return errors;
}

// StyleTemplate implementation
std::string StyleTemplate::generateCSS(const std::map<std::string, std::string>& parameters) {
    std::ostringstream css;
    
    std::vector<std::shared_ptr<CHTLNode>> processed_content = processTemplate(parameters);
    
    for (const auto& node : processed_content) {
        if (node) {
            css << node->toHTML() << "\n";
        }
    }
    
    return css.str();
}

std::vector<std::string> StyleTemplate::getCSSProperties() const {
    std::vector<std::string> properties;
    
    for (const auto& node : content) {
        if (node && node->node_type == NodeType::Style) {
            // Extract CSS properties from style nodes
            // This is a simplified implementation
            std::string style_text = node->toHTML();
            // Parse CSS properties from style_text
            // Implementation would depend on the specific CSS parsing logic
        }
    }
    
    return properties;
}

std::vector<std::string> StyleTemplate::getCSSSelectors() const {
    std::vector<std::string> selectors;
    
    for (const auto& node : content) {
        if (node && node->node_type == NodeType::Style) {
            // Extract CSS selectors from style nodes
            // This is a simplified implementation
            std::string style_text = node->toHTML();
            // Parse CSS selectors from style_text
            // Implementation would depend on the specific CSS parsing logic
        }
    }
    
    return selectors;
}

// ElementTemplate implementation
std::string ElementTemplate::generateHTML(const std::map<std::string, std::string>& parameters) {
    std::ostringstream html;
    
    std::vector<std::shared_ptr<CHTLNode>> processed_content = processTemplate(parameters);
    
    for (const auto& node : processed_content) {
        if (node) {
            html << node->toHTML();
        }
    }
    
    return html.str();
}

std::vector<std::string> ElementTemplate::getElementNames() const {
    std::vector<std::string> element_names;
    
    for (const auto& node : content) {
        if (node && node->node_type == NodeType::Element) {
            auto element_node = std::static_pointer_cast<ElementNode>(node);
            element_names.push_back(element_node->tag_name);
        }
    }
    
    return element_names;
}

std::vector<std::string> ElementTemplate::getRequiredAttributes() const {
    std::vector<std::string> attributes;
    
    for (const auto& node : content) {
        if (node && node->node_type == NodeType::Element) {
            auto element_node = std::static_pointer_cast<ElementNode>(node);
            for (const auto& attr : element_node->attributes) {
                attributes.push_back(attr->attribute_name);
            }
        }
    }
    
    return attributes;
}

// VarTemplate implementation
std::string VarTemplate::generateValue(const std::map<std::string, std::string>& parameters) {
    std::vector<std::shared_ptr<CHTLNode>> processed_content = processTemplate(parameters);
    
    if (!processed_content.empty() && processed_content[0]) {
        return processed_content[0]->toHTML();
    }
    
    return "";
}

std::vector<std::string> VarTemplate::getVariableNames() const {
    std::vector<std::string> variable_names;
    
    // Extract variable names from parameters
    for (const auto& param : parameters) {
        variable_names.push_back(param.first);
    }
    
    return variable_names;
}

std::string VarTemplate::getDefaultValue() const {
    auto it = parameters.find("default");
    return (it != parameters.end()) ? it->second : "";
}

// CHTLTemplateManager implementation
CHTLTemplateManager::CHTLTemplateManager() {
}

void CHTLTemplateManager::registerTemplate(std::shared_ptr<CHTLTemplate> template_def) {
    if (!template_def) return;
    
    std::string key = generateTemplateKey(template_def->name, template_def->type);
    templates[key] = template_def;
    
    // Register in specific template maps
    switch (template_def->type) {
        case TemplateType::Style:
            style_templates[template_def->name] = std::static_pointer_cast<StyleTemplate>(template_def);
            break;
        case TemplateType::Element:
            element_templates[template_def->name] = std::static_pointer_cast<ElementTemplate>(template_def);
            break;
        case TemplateType::Var:
            var_templates[template_def->name] = std::static_pointer_cast<VarTemplate>(template_def);
            break;
    }
    
    // Rebuild dependency graph
    buildDependencyGraph();
}

void CHTLTemplateManager::registerStyleTemplate(std::shared_ptr<StyleTemplate> template_def) {
    if (template_def) {
        registerTemplate(template_def);
    }
}

void CHTLTemplateManager::registerElementTemplate(std::shared_ptr<ElementTemplate> template_def) {
    if (template_def) {
        registerTemplate(template_def);
    }
}

void CHTLTemplateManager::registerVarTemplate(std::shared_ptr<VarTemplate> template_def) {
    if (template_def) {
        registerTemplate(template_def);
    }
}

std::shared_ptr<CHTLTemplate> CHTLTemplateManager::resolveTemplate(const std::string& name, TemplateType type) {
    return findTemplate(name, type);
}

std::shared_ptr<StyleTemplate> CHTLTemplateManager::resolveStyleTemplate(const std::string& name) {
    auto it = style_templates.find(name);
    return (it != style_templates.end()) ? it->second : nullptr;
}

std::shared_ptr<ElementTemplate> CHTLTemplateManager::resolveElementTemplate(const std::string& name) {
    auto it = element_templates.find(name);
    return (it != element_templates.end()) ? it->second : nullptr;
}

std::shared_ptr<VarTemplate> CHTLTemplateManager::resolveVarTemplate(const std::string& name) {
    auto it = var_templates.find(name);
    return (it != var_templates.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processTemplate(const std::string& name, TemplateType type, const std::map<std::string, std::string>& parameters) {
    auto template_def = resolveTemplate(name, type);
    if (!template_def) {
        return {};
    }
    
    return template_def->processTemplate(parameters);
}

std::string CHTLTemplateManager::processStyleTemplate(const std::string& name, const std::map<std::string, std::string>& parameters) {
    auto template_def = resolveStyleTemplate(name);
    if (!template_def) {
        return "";
    }
    
    return template_def->generateCSS(parameters);
}

std::string CHTLTemplateManager::processElementTemplate(const std::string& name, const std::map<std::string, std::string>& parameters) {
    auto template_def = resolveElementTemplate(name);
    if (!template_def) {
        return "";
    }
    
    return template_def->generateHTML(parameters);
}

std::string CHTLTemplateManager::processVarTemplate(const std::string& name, const std::map<std::string, std::string>& parameters) {
    auto template_def = resolveVarTemplate(name);
    if (!template_def) {
        return "";
    }
    
    return template_def->generateValue(parameters);
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processInheritance(std::shared_ptr<CHTLTemplate> template_def, const std::map<std::string, std::string>& parameters) {
    if (!template_def) return {};
    
    // Process the template
    std::vector<std::shared_ptr<CHTLNode>> result = template_def->processTemplate(parameters);
    
    // Process inheritance if parent template exists
    if (!template_def->parent_template.empty()) {
        auto parent_template = findTemplate(template_def->parent_template, template_def->type);
        if (parent_template) {
            std::vector<std::shared_ptr<CHTLNode>> parent_content = parent_template->processTemplate(parameters);
            result = applyInheritanceModifications(parent_content, template_def->modifications);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::applyInheritanceModifications(const std::vector<std::shared_ptr<CHTLNode>>& base_content, const std::vector<TemplateModification>& modifications) {
    std::vector<std::shared_ptr<CHTLNode>> result = base_content;
    
    for (const auto& modification : modifications) {
        switch (modification.type) {
            case InheritanceType::Inherit:
                result = processInheritModification(result, modification);
                break;
            case InheritanceType::Delete:
                result = processDeleteModification(result, modification);
                break;
            case InheritanceType::Insert:
                result = processInsertModification(result, modification);
                break;
            case InheritanceType::After:
                result = processAfterModification(result, modification);
                break;
            case InheritanceType::Before:
                result = processBeforeModification(result, modification);
                break;
            case InheritanceType::Replace:
                result = processReplaceModification(result, modification);
                break;
            default:
                break;
        }
    }
    
    return result;
}

std::vector<std::string> CHTLTemplateManager::getTemplateDependencies(const std::string& name, TemplateType type) {
    auto template_def = findTemplate(name, type);
    if (!template_def) {
        return {};
    }
    
    std::vector<std::string> dependencies;
    for (const auto& dep : template_def->dependencies) {
        dependencies.push_back(dep);
    }
    
    return dependencies;
}

std::vector<std::string> CHTLTemplateManager::getCircularDependencies() {
    std::vector<std::string> circular_deps;
    std::set<std::string> visited;
    std::set<std::string> recursion_stack;
    
    for (const auto& template_pair : templates) {
        if (hasCircularDependency(template_pair.first, visited, recursion_stack)) {
            circular_deps.push_back(template_pair.first);
        }
    }
    
    return circular_deps;
}

std::map<std::string, std::vector<std::string>> CHTLTemplateManager::getDependencyGraph() {
    std::map<std::string, std::vector<std::string>> result;
    for (const auto& pair : dependency_graph) {
        result[pair.first] = std::vector<std::string>(pair.second.begin(), pair.second.end());
    }
    return result;
}

bool CHTLTemplateManager::validateTemplate(const std::string& name, TemplateType type) {
    auto template_def = findTemplate(name, type);
    if (!template_def) {
        return false;
    }
    
    return template_def->isValid();
}

std::vector<std::string> CHTLTemplateManager::getTemplateValidationErrors(const std::string& name, TemplateType type) {
    auto template_def = findTemplate(name, type);
    if (!template_def) {
        return {"Template not found"};
    }
    
    return template_def->getValidationErrors();
}

bool CHTLTemplateManager::validateAllTemplates() {
    for (const auto& template_pair : templates) {
        if (!template_pair.second->isValid()) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> CHTLTemplateManager::getAllTemplateNames(TemplateType type) {
    std::vector<std::string> names;
    
    switch (type) {
        case TemplateType::Style:
            for (const auto& pair : style_templates) {
                names.push_back(pair.first);
            }
            break;
        case TemplateType::Element:
            for (const auto& pair : element_templates) {
                names.push_back(pair.first);
            }
            break;
        case TemplateType::Var:
            for (const auto& pair : var_templates) {
                names.push_back(pair.first);
            }
            break;
    }
    
    return names;
}

std::vector<std::string> CHTLTemplateManager::getAllTemplateNames() {
    std::vector<std::string> names;
    
    for (const auto& pair : templates) {
        names.push_back(pair.first);
    }
    
    return names;
}

bool CHTLTemplateManager::templateExists(const std::string& name, TemplateType type) {
    return findTemplate(name, type) != nullptr;
}

void CHTLTemplateManager::removeTemplate(const std::string& name, TemplateType type) {
    std::string key = generateTemplateKey(name, type);
    templates.erase(key);
    
    switch (type) {
        case TemplateType::Style:
            style_templates.erase(name);
            break;
        case TemplateType::Element:
            element_templates.erase(name);
            break;
        case TemplateType::Var:
            var_templates.erase(name);
            break;
    }
    
    buildDependencyGraph();
}

void CHTLTemplateManager::clearTemplates() {
    templates.clear();
    style_templates.clear();
    element_templates.clear();
    var_templates.clear();
    dependency_graph.clear();
}

void CHTLTemplateManager::buildDependencyGraph() {
    dependency_graph.clear();
    
    for (const auto& template_pair : templates) {
        std::set<std::string> deps;
        for (const auto& dep : template_pair.second->dependencies) {
            deps.insert(dep);
        }
        dependency_graph[template_pair.first] = deps;
    }
}

bool CHTLTemplateManager::hasCircularDependency(const std::string& template_name, std::set<std::string>& visited, std::set<std::string>& recursion_stack) {
    if (recursion_stack.find(template_name) != recursion_stack.end()) {
        return true; // Circular dependency found
    }
    
    if (visited.find(template_name) != visited.end()) {
        return false; // Already processed
    }
    
    visited.insert(template_name);
    recursion_stack.insert(template_name);
    
    auto it = dependency_graph.find(template_name);
    if (it != dependency_graph.end()) {
        for (const auto& dep : it->second) {
            if (hasCircularDependency(dep, visited, recursion_stack)) {
                return true;
            }
        }
    }
    
    recursion_stack.erase(template_name);
    return false;
}

std::vector<std::string> CHTLTemplateManager::topologicalSort() {
    std::vector<std::string> result;
    std::map<std::string, int> in_degree;
    
    // Calculate in-degrees
    for (const auto& pair : dependency_graph) {
        in_degree[pair.first] = 0;
    }
    
    for (const auto& pair : dependency_graph) {
        for (const auto& dep : pair.second) {
            in_degree[dep]++;
        }
    }
    
    // Find nodes with no incoming edges
    std::vector<std::string> queue;
    for (const auto& pair : in_degree) {
        if (pair.second == 0) {
            queue.push_back(pair.first);
        }
    }
    
    // Process queue
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.erase(queue.begin());
        result.push_back(current);
        
        // Update in-degrees of dependent nodes
        auto it = dependency_graph.find(current);
        if (it != dependency_graph.end()) {
            for (const auto& dep : it->second) {
                in_degree[dep]--;
                if (in_degree[dep] == 0) {
                    queue.push_back(dep);
                }
            }
        }
    }
    
    return result;
}

std::string CHTLTemplateManager::generateTemplateKey(const std::string& name, TemplateType type) {
    std::ostringstream key;
    key << static_cast<int>(type) << ":" << name;
    return key.str();
}

std::shared_ptr<CHTLTemplate> CHTLTemplateManager::findTemplate(const std::string& name, TemplateType type) {
    std::string key = generateTemplateKey(name, type);
    auto it = templates.find(key);
    return (it != templates.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::cloneNodes(const std::vector<std::shared_ptr<CHTLNode>>& nodes) {
    std::vector<std::shared_ptr<CHTLNode>> cloned;
    
    for (const auto& node : nodes) {
        if (node) {
            // Simple cloning - in a real implementation, this would be more sophisticated
            cloned.push_back(node);
        }
    }
    
    return cloned;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::applyParameterSubstitution(const std::vector<std::shared_ptr<CHTLNode>>& nodes, const std::map<std::string, std::string>& /* parameters */) {
    // Simple parameter substitution
    // In a real implementation, this would traverse the AST and substitute parameters
    return nodes;
}

std::string CHTLTemplateManager::substituteParameters(const std::string& text, const std::map<std::string, std::string>& parameters) {
    std::string result = text;
    
    for (const auto& param : parameters) {
        std::string placeholder = "{{" + param.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), param.second);
            pos += param.second.length();
        }
    }
    
    return result;
}

// Modification processing methods
std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processInheritModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& /* modification */) {
    // Inherit modification - add content from parent template
    return content;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processDeleteModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification) {
    // Delete modification - remove specified content
    std::vector<std::shared_ptr<CHTLNode>> result;
    
    for (const auto& node : content) {
        if (node && node->value != modification.target) {
            result.push_back(node);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processInsertModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification) {
    // Insert modification - add new content
    std::vector<std::shared_ptr<CHTLNode>> result = content;
    
    // Create a simple text node for the inserted content
    auto insert_node = std::make_shared<TextNode>(modification.value);
    result.push_back(insert_node);
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processAfterModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification) {
    // After modification - add content after specified target
    std::vector<std::shared_ptr<CHTLNode>> result;
    
    for (const auto& node : content) {
        result.push_back(node);
        if (node && node->value == modification.target) {
            auto after_node = std::make_shared<TextNode>(modification.value);
            result.push_back(after_node);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processBeforeModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification) {
    // Before modification - add content before specified target
    std::vector<std::shared_ptr<CHTLNode>> result;
    
    for (const auto& node : content) {
        if (node && node->value == modification.target) {
            auto before_node = std::make_shared<TextNode>(modification.value);
            result.push_back(before_node);
        }
        result.push_back(node);
    }
    
    return result;
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateManager::processReplaceModification(const std::vector<std::shared_ptr<CHTLNode>>& content, const TemplateModification& modification) {
    // Replace modification - replace specified content
    std::vector<std::shared_ptr<CHTLNode>> result;
    
    for (const auto& node : content) {
        if (node && node->value == modification.target) {
            auto replace_node = std::make_shared<TextNode>(modification.value);
            result.push_back(replace_node);
        } else {
            result.push_back(node);
        }
    }
    
    return result;
}

// CHTLTemplateProcessor implementation
CHTLTemplateProcessor::CHTLTemplateProcessor(std::shared_ptr<CHTLTemplateManager> manager) : template_manager(manager) {
}

std::string CHTLTemplateProcessor::processTemplateReference(const std::string& reference, const std::map<std::string, std::string>& context) {
    if (!isTemplateReference(reference)) {
        return reference;
    }
    
    std::string template_name = parseTemplateReference(reference);
    std::map<std::string, std::string> params = parseTemplateParameters(reference);
    
    // Merge with context
    std::map<std::string, std::string> final_params = context;
    for (const auto& param : params) {
        final_params[param.first] = param.second;
    }
    
    // Try to resolve as different template types
    auto style_template = template_manager->resolveStyleTemplate(template_name);
    if (style_template) {
        return style_template->generateCSS(final_params);
    }
    
    auto element_template = template_manager->resolveElementTemplate(template_name);
    if (element_template) {
        return element_template->generateHTML(final_params);
    }
    
    auto var_template = template_manager->resolveVarTemplate(template_name);
    if (var_template) {
        return var_template->generateValue(final_params);
    }
    
    return reference; // Template not found
}

std::vector<std::shared_ptr<CHTLNode>> CHTLTemplateProcessor::processTemplateNode(std::shared_ptr<TemplateNode> template_node, const std::map<std::string, std::string>& context) {
    if (!template_node) return {};
    
    std::map<std::string, std::string> params = context;
    
    // Process based on template type
    if (template_node->template_type == "Style") {
        auto style_template = template_manager->resolveStyleTemplate(template_node->template_name);
        if (style_template) {
            return style_template->processTemplate(params);
        }
    } else if (template_node->template_type == "Element") {
        auto element_template = template_manager->resolveElementTemplate(template_node->template_name);
        if (element_template) {
            return element_template->processTemplate(params);
        }
    } else if (template_node->template_type == "Var") {
        auto var_template = template_manager->resolveVarTemplate(template_node->template_name);
        if (var_template) {
            return var_template->processTemplate(params);
        }
    }
    
    return {};
}

std::map<std::string, std::string> CHTLTemplateProcessor::resolveParameters(const std::map<std::string, std::string>& template_params, const std::map<std::string, std::string>& context) {
    std::map<std::string, std::string> resolved = context;
    
    for (const auto& param : template_params) {
        resolved[param.first] = param.second;
    }
    
    return resolved;
}

std::string CHTLTemplateProcessor::resolveParameterValue(const std::string& param_name, const std::map<std::string, std::string>& context) {
    auto it = context.find(param_name);
    return (it != context.end()) ? it->second : "";
}

void CHTLTemplateProcessor::pushContext(const std::map<std::string, std::string>& context) {
    context_stack.push_back(context);
}

void CHTLTemplateProcessor::popContext() {
    if (!context_stack.empty()) {
        context_stack.pop_back();
    }
}

std::map<std::string, std::string> CHTLTemplateProcessor::getCurrentContext() {
    if (context_stack.empty()) {
        return {};
    }
    return context_stack.back();
}

std::string CHTLTemplateProcessor::parseTemplateReference(const std::string& reference) {
    // Simple template reference parsing
    // Format: @TemplateName or @TemplateName(param1=value1,param2=value2)
    if (reference.empty() || reference[0] != '@') {
        return "";
    }
    
    size_t paren_pos = reference.find('(');
    if (paren_pos == std::string::npos) {
        return reference.substr(1); // Remove @
    }
    
    return reference.substr(1, paren_pos - 1);
}

std::map<std::string, std::string> CHTLTemplateProcessor::parseTemplateParameters(const std::string& reference) {
    std::map<std::string, std::string> params;
    
    size_t paren_start = reference.find('(');
    if (paren_start == std::string::npos) {
        return params;
    }
    
    size_t paren_end = reference.find(')', paren_start);
    if (paren_end == std::string::npos) {
        return params;
    }
    
    std::string param_string = reference.substr(paren_start + 1, paren_end - paren_start - 1);
    
    // Simple parameter parsing
    std::istringstream stream(param_string);
    std::string param;
    
    while (std::getline(stream, param, ',')) {
        size_t equal_pos = param.find('=');
        if (equal_pos != std::string::npos) {
            std::string key = param.substr(0, equal_pos);
            std::string value = param.substr(equal_pos + 1);
            
            // Remove quotes if present
            if (value.length() >= 2 && 
                ((value[0] == '"' && value.back() == '"') ||
                 (value[0] == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.length() - 2);
            }
            
            params[key] = value;
        }
    }
    
    return params;
}

bool CHTLTemplateProcessor::isTemplateReference(const std::string& text) {
    return !text.empty() && text[0] == '@';
}

} // namespace CHTL