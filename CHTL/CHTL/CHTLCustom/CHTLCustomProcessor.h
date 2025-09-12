#ifndef CHTL_CUSTOM_PROCESSOR_H
#define CHTL_CUSTOM_PROCESSOR_H

#include "CHTLNode/CustomNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLCustomProcessor {
private:
    std::map<std::string, std::shared_ptr<CustomNode>> customNodes;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    void addError(const std::string& error);
    void addWarning(const std::string& warning);

    // 辅助方法
    std::shared_ptr<CustomNode> getCustomNode(const std::string& name) const;
    void mergeCustomProperties(std::shared_ptr<CustomNode> target, std::shared_ptr<CustomNode> source);
    void mergeCustomContent(std::shared_ptr<CustomNode> target, std::shared_ptr<CustomNode> source);

public:
    CHTLCustomProcessor();
    ~CHTLCustomProcessor() = default;

    void registerCustomNode(std::shared_ptr<CustomNode> customNode);
    void unregisterCustomNode(const std::string& name);
    bool hasCustomNode(const std::string& name) const;

    std::shared_ptr<BaseNode> processCustomNode(const std::string& name, const std::map<std::string, std::string>& args = {});
    std::shared_ptr<BaseNode> instantiateCustomNode(const std::string& name, const std::map<std::string, std::string>& args = {});

    std::shared_ptr<CustomNode> combineCustomNodes(const std::string& custom1Name, const std::string& custom2Name);
    std::shared_ptr<CustomNode> mergeCustomNodes(const std::string& custom1Name, const std::string& custom2Name);

    void applySpecializations(std::shared_ptr<CustomNode> customNode);
    void resolveSpecializationConflicts(std::shared_ptr<CustomNode> customNode);

    // 特例化操作
    void addSpecialization(const std::string& customName, const std::string& key, std::shared_ptr<BaseNode> specialization);
    void removeSpecialization(const std::string& customName, const std::string& key);
    std::shared_ptr<BaseNode> getSpecialization(const std::string& customName, const std::string& key) const;

    // 删除操作
    void deleteElement(const std::string& customName, const std::string& elementName);
    void deleteElementAt(const std::string& customName, int index);
    void deleteSpecialization(const std::string& customName, const std::string& key);

    // 插入操作
    void insertElement(const std::string& customName, std::shared_ptr<BaseNode> element);
    void insertElementAt(const std::string& customName, int index, std::shared_ptr<BaseNode> element);
    void insertSpecialization(const std::string& customName, const std::string& key, std::shared_ptr<BaseNode> specialization);

    // 索引访问
    std::shared_ptr<BaseNode> getIndexedElement(const std::string& customName, int index) const;
    void setIndexedElement(const std::string& customName, int index, std::shared_ptr<BaseNode> element);
    void removeIndexedElement(const std::string& customName, int index);

    // 自定义验证
    bool validateCustomNode(const std::string& name) const;
    bool validateCustomParameters(const std::string& name, const std::map<std::string, std::string>& args) const;
    bool validateCustomConstraints(const std::string& name) const;

    const std::vector<std::string>& getErrors() const { return errors; }
    const std::vector<std::string>& getWarnings() const { return warnings; }
    void clearMessages();
    void reset();
    std::string getDebugInfo() const;
};

} // namespace CHTL

#endif // CHTL_CUSTOM_PROCESSOR_H