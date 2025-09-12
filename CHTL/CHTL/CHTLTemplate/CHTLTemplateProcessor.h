#ifndef CHTL_TEMPLATE_PROCESSOR_H
#define CHTL_TEMPLATE_PROCESSOR_H

#include "../CHTLNode/TemplateNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLTemplateProcessor {
private:
    std::map<std::string, std::shared_ptr<TemplateNode>> templates;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    void addError(const std::string& error);
    void addWarning(const std::string& warning);

    // 辅助方法
    std::shared_ptr<TemplateNode> getTemplate(const std::string& name) const;
    void mergeTemplateProperties(std::shared_ptr<TemplateNode> target, std::shared_ptr<TemplateNode> source);
    void mergeTemplateContent(std::shared_ptr<TemplateNode> target, std::shared_ptr<TemplateNode> source);

public:
    CHTLTemplateProcessor();
    ~CHTLTemplateProcessor() = default;

    void registerTemplate(std::shared_ptr<TemplateNode> templateNode);
    void unregisterTemplate(const std::string& name);
    bool hasTemplate(const std::string& name) const;

    std::shared_ptr<BaseNode> processTemplate(const std::string& name, const std::map<std::string, std::string>& args = {});
    std::shared_ptr<BaseNode> instantiateTemplate(const std::string& name, const std::map<std::string, std::string>& args = {});

    std::shared_ptr<TemplateNode> combineTemplates(const std::string& template1Name, const std::string& template2Name);
    std::shared_ptr<TemplateNode> mergeTemplates(const std::string& template1Name, const std::string& template2Name);

    void applyInheritance(std::shared_ptr<TemplateNode> templateNode);
    void resolveInheritanceConflicts(std::shared_ptr<TemplateNode> templateNode);

    const std::vector<std::string>& getErrors() const;
    const std::vector<std::string>& getWarnings() const;
    void clearMessages();
    void reset();
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_PROCESSOR_H