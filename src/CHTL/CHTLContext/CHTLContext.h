#ifndef CHTL_CONTEXT_H
#define CHTL_CONTEXT_H

#include <string>
#include <vector>
#include <map>

class CHTLContext {
public:
    using StyleProperties = std::map<std::string, std::string>;

    void addStyleTemplate(const std::string& name, const StyleProperties& properties);
    const StyleProperties& getStyleTemplate(const std::string& name) const;
    bool hasStyleTemplate(const std::string& name) const;

private:
    std::map<std::string, StyleProperties> styleTemplates;
};

#endif // CHTL_CONTEXT_H
