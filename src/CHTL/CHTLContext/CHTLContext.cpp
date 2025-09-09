#include "CHTLContext.h"
#include <stdexcept>

void CHTLContext::addStyleTemplate(const std::string& name, const StyleProperties& properties) {
    styleTemplates[name] = properties;
}

const CHTLContext::StyleProperties& CHTLContext::getStyleTemplate(const std::string& name) const {
    auto it = styleTemplates.find(name);
    if (it == styleTemplates.end()) {
        throw std::runtime_error("Style template not found: " + name);
    }
    return it->second;
}

bool CHTLContext::hasStyleTemplate(const std::string& name) const {
    return styleTemplates.find(name) != styleTemplates.end();
}
