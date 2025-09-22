#ifndef CHTL_GENERATOR_H
#define CHTL_GENERATOR_H

#include "../CHTLNode/Visitor.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../../CHTL JS/CHTLJSNode/DelegateNode.h"
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <set>
#include "../Config/Configuration.h"

namespace CHTL {

struct CompilationResult {
    std::string html;
    std::string css;
    std::string js;
};

class CHTLGenerator : public Visitor {
public:
    explicit CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, std::shared_ptr<Configuration> config);
    CompilationResult generate(BaseNode* root, bool use_html5_doctype);

    void visit(ElementNode& node) override;
    void visit(TextNode& node) override;
    void visit(StyleNode& node) override;
    void visit(OriginNode& node) override;
    void visit(ScriptNode& node) override;
    void visit(IfNode& node) override;

private:
    void generateReactiveStyleJS(ElementNode& node, const std::string& style_property, Expr* expr);
    void generateReactiveAttributeJS(ElementNode& node, const std::string& attr_key, Expr* expr);

    std::shared_ptr<Configuration> config;
    std::stringstream html_output;
    std::stringstream css_output;
    std::stringstream js_output;
    const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates;
    BaseNode* doc_root = nullptr;
    std::map<std::string, std::vector<CHTL_JS::DelegateNode>> delegate_registry;
    std::set<std::string> defined_reactive_vars;
};

} // namespace CHTL

#endif // CHTL_GENERATOR_H
