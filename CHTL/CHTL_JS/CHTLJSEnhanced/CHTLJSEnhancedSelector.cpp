#include "CHTLJSEnhancedSelector.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL_JS {

CHTLJSEnhancedSelector::CHTLJSEnhancedSelector() 
    : type(SelectorType::UNKNOWN), isComputed(false), isOptional(false), 
      specificity(0), line(0), column(0), position(0) {
}

CHTLJSEnhancedSelector::CHTLJSEnhancedSelector(const std::string& selector) 
    : selector(selector), type(SelectorType::UNKNOWN), isComputed(false), isOptional(false),
      specificity(0), line(0), column(0), position(0) {
    parse(selector);
}

void CHTLJSEnhancedSelector::setSelector(const std::string& selector) {
    this->selector = selector;
    parse(selector);
}

// 子选择器管理实现
void CHTLJSEnhancedSelector::addChild(std::shared_ptr<CHTLJSEnhancedSelector> child) {
    if (child) {
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

void CHTLJSEnhancedSelector::removeChild(std::shared_ptr<CHTLJSEnhancedSelector> child) {
    auto it = std::find(children.begin(), children.end(), child);
    if (it != children.end()) {
        (*it)->setParent(nullptr);
        children.erase(it);
    }
}

void CHTLJSEnhancedSelector::removeChild(size_t index) {
    if (index < children.size()) {
        children[index]->setParent(nullptr);
        children.erase(children.begin() + index);
    }
}

void CHTLJSEnhancedSelector::clearChildren() {
    for (auto& child : children) {
        child->setParent(nullptr);
    }
    children.clear();
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::getChild(size_t index) const {
    return index < children.size() ? children[index] : nullptr;
}

void CHTLJSEnhancedSelector::setLocation(int line, int column, int position) {
    this->line = line;
    this->column = column;
    this->position = position;
}

// 属性管理实现
void CHTLJSEnhancedSelector::setAttribute(const std::string& key, const std::string& value) {
    attributes[key] = value;
}

std::string CHTLJSEnhancedSelector::getAttribute(const std::string& key) const {
    auto it = attributes.find(key);
    return it != attributes.end() ? it->second : "";
}

bool CHTLJSEnhancedSelector::hasAttribute(const std::string& key) const {
    return attributes.find(key) != attributes.end();
}

void CHTLJSEnhancedSelector::removeAttribute(const std::string& key) {
    attributes.erase(key);
}

// 修饰符管理实现
void CHTLJSEnhancedSelector::addModifier(const std::string& modifier) {
    if (std::find(modifiers.begin(), modifiers.end(), modifier) == modifiers.end()) {
        modifiers.push_back(modifier);
    }
}

void CHTLJSEnhancedSelector::removeModifier(const std::string& modifier) {
    modifiers.erase(std::remove(modifiers.begin(), modifiers.end(), modifier), modifiers.end());
}

void CHTLJSEnhancedSelector::clearModifiers() {
    modifiers.clear();
}

bool CHTLJSEnhancedSelector::hasModifier(const std::string& modifier) const {
    return std::find(modifiers.begin(), modifiers.end(), modifier) != modifiers.end();
}

// 解析和生成实现
bool CHTLJSEnhancedSelector::parse(const std::string& selector) {
    if (selector.empty()) {
        return false;
    }
    
    this->selector = selector;
    type = inferType(selector);
    
    switch (type) {
        case SelectorType::TAG:
            return parseTagSelector(selector);
        case SelectorType::CLASS:
            return parseClassSelector(selector);
        case SelectorType::ID:
            return parseIdSelector(selector);
        case SelectorType::ATTRIBUTE:
            return parseAttributeSelector(selector);
        case SelectorType::PSEUDO_CLASS:
        case SelectorType::PSEUDO_ELEMENT:
            return parsePseudoSelector(selector);
        case SelectorType::DESCENDANT:
        case SelectorType::CHILD:
        case SelectorType::ADJACENT:
        case SelectorType::GENERAL:
            return parseComplexSelector(selector);
        default:
            return false;
    }
}

std::string CHTLJSEnhancedSelector::toJavaScript() const {
    return generateJavaScript();
}

std::string CHTLJSEnhancedSelector::toCSS() const {
    return generateCSS();
}

std::string CHTLJSEnhancedSelector::toCHTLJS() const {
    return generateCHTLJS();
}

std::string CHTLJSEnhancedSelector::toString() const {
    return selector;
}

std::string CHTLJSEnhancedSelector::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSEnhancedSelector{type=" << static_cast<int>(type)
        << ", selector='" << selector
        << "', tagName='" << tagName
        << "', className='" << className
        << "', idName='" << idName
        << "', specificity=" << specificity << "}";
    return oss.str();
}

// 验证实现
bool CHTLJSEnhancedSelector::isValid() const {
    return validateSelector() && validateChildren() && validateAttributes() && validateModifiers();
}

bool CHTLJSEnhancedSelector::isComplete() const {
    return !selector.empty() && type != SelectorType::UNKNOWN;
}

std::vector<std::string> CHTLJSEnhancedSelector::validate() const {
    std::vector<std::string> errors;
    
    if (!validateSelector()) {
        errors.push_back("Selector validation failed");
    }
    
    if (!validateChildren()) {
        errors.push_back("Children validation failed");
    }
    
    if (!validateAttributes()) {
        errors.push_back("Attributes validation failed");
    }
    
    if (!validateModifiers()) {
        errors.push_back("Modifiers validation failed");
    }
    
    return errors;
}

// 比较实现
bool CHTLJSEnhancedSelector::equals(std::shared_ptr<CHTLJSEnhancedSelector> other) const {
    return other && compareSelector(other);
}

bool CHTLJSEnhancedSelector::equals(const CHTLJSEnhancedSelector& other) const {
    return compareSelector(other);
}

// 克隆实现
std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::clone() const {
    auto cloned = std::make_shared<CHTLJSEnhancedSelector>(selector);
    copyTo(cloned);
    return cloned;
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::deepClone() const {
    auto cloned = std::make_shared<CHTLJSEnhancedSelector>(selector);
    deepCopyTo(cloned);
    return cloned;
}

// 遍历实现
void CHTLJSEnhancedSelector::traverse(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) {
    traverseSelector(visitor);
}

void CHTLJSEnhancedSelector::traverse(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) const {
    traverseSelector(visitor);
}

void CHTLJSEnhancedSelector::traverseChildren(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) {
    for (auto& child : children) {
        if (child) {
            child->traverse(visitor);
        }
    }
}

void CHTLJSEnhancedSelector::traverseChildren(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) const {
    for (const auto& child : children) {
        if (child) {
            child->traverse(visitor);
        }
    }
}

// 查找实现
std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> CHTLJSEnhancedSelector::findSelectors(SelectorType type) const {
    std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> result;
    collectSelectors(type, result);
    return result;
}

std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> CHTLJSEnhancedSelector::findSelectors(std::function<bool(std::shared_ptr<CHTLJSEnhancedSelector>)> predicate) const {
    std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> result;
    collectSelectors(predicate, result);
    return result;
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::findFirstSelector(SelectorType type) const {
    auto selectors = findSelectors(type);
    return selectors.empty() ? nullptr : selectors[0];
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::findFirstSelector(std::function<bool(std::shared_ptr<CHTLJSEnhancedSelector>)> predicate) const {
    auto selectors = findSelectors(predicate);
    return selectors.empty() ? nullptr : selectors[0];
}

// 优化实现
std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::optimize() const {
    return optimizeSelector();
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::compress() const {
    return compressSelector();
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::decompress() const {
    return decompressSelector();
}

// 格式化实现
std::string CHTLJSEnhancedSelector::format() const {
    return formatSelector();
}

std::string CHTLJSEnhancedSelector::minify() const {
    return minifySelector();
}

std::string CHTLJSEnhancedSelector::beautify() const {
    return beautifySelector();
}

// 解析辅助实现
bool CHTLJSEnhancedSelector::parseTagSelector(const std::string& selector) {
    tagName = selector;
    return true;
}

bool CHTLJSEnhancedSelector::parseClassSelector(const std::string& selector) {
    if (selector[0] == '.') {
        className = selector.substr(1);
        return true;
    }
    return false;
}

bool CHTLJSEnhancedSelector::parseIdSelector(const std::string& selector) {
    if (selector[0] == '#') {
        idName = selector.substr(1);
        return true;
    }
    return false;
}

bool CHTLJSEnhancedSelector::parseAttributeSelector(const std::string& selector) {
    std::regex attrRegex(R"(\[([^=]+)(?:=([^\]]+))?\])");
    std::smatch match;
    
    if (std::regex_match(selector, match, attrRegex)) {
        attributeName = match[1].str();
        if (match.size() > 2) {
            attributeValue = match[2].str();
        }
        return true;
    }
    return false;
}

bool CHTLJSEnhancedSelector::parsePseudoSelector(const std::string& selector) {
    if (selector[0] == ':') {
        std::string pseudo = selector.substr(1);
        if (pseudo[0] == ':') {
            pseudoElement = pseudo.substr(1);
            type = SelectorType::PSEUDO_ELEMENT;
        } else {
            pseudoClass = pseudo;
            type = SelectorType::PSEUDO_CLASS;
        }
        return true;
    }
    return false;
}

bool CHTLJSEnhancedSelector::parseComplexSelector(const std::string& selector) {
    // 简化的复杂选择器解析
    std::vector<std::string> parts;
    std::string current;
    
    for (char c : selector) {
        if (c == ' ' || c == '>' || c == '+' || c == '~') {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    for (const auto& part : parts) {
        auto child = std::make_shared<CHTLJSEnhancedSelector>(part);
        addChild(child);
    }
    
    return true;
}

// 类型推断实现
SelectorType CHTLJSEnhancedSelector::inferType(const std::string& selector) const {
    if (selector.empty()) {
        return SelectorType::UNKNOWN;
    }
    
    if (selector[0] == '.') {
        return SelectorType::CLASS;
    } else if (selector[0] == '#') {
        return SelectorType::ID;
    } else if (selector[0] == '[') {
        return SelectorType::ATTRIBUTE;
    } else if (selector[0] == ':') {
        return SelectorType::PSEUDO_CLASS;
    } else if (isTagName(selector)) {
        return SelectorType::TAG;
    } else if (selector.find(' ') != std::string::npos || 
               selector.find('>') != std::string::npos ||
               selector.find('+') != std::string::npos ||
               selector.find('~') != std::string::npos) {
        return SelectorType::DESCENDANT;
    }
    
    return SelectorType::UNKNOWN;
}

bool CHTLJSEnhancedSelector::isTagName(const std::string& name) const {
    // 简化的标签名检查
    return !name.empty() && std::isalpha(name[0]) && 
           std::all_of(name.begin(), name.end(), [](char c) {
               return std::isalnum(c) || c == '-';
           });
}

bool CHTLJSEnhancedSelector::isClassName(const std::string& name) const {
    return !name.empty() && name[0] == '.';
}

bool CHTLJSEnhancedSelector::isIdName(const std::string& name) const {
    return !name.empty() && name[0] == '#';
}

bool CHTLJSEnhancedSelector::isAttributeName(const std::string& name) const {
    return !name.empty() && name[0] == '[' && name.back() == ']';
}

bool CHTLJSEnhancedSelector::isPseudoClass(const std::string& name) const {
    return !name.empty() && name[0] == ':' && name[1] != ':';
}

bool CHTLJSEnhancedSelector::isPseudoElement(const std::string& name) const {
    return !name.empty() && name[0] == ':' && name[1] == ':';
}

// 生成辅助实现
std::string CHTLJSEnhancedSelector::generateJavaScript() const {
    switch (type) {
        case SelectorType::TAG:
            return "document.getElementsByTagName('" + tagName + "')";
        case SelectorType::CLASS:
            return "document.getElementsByClassName('" + className + "')";
        case SelectorType::ID:
            return "document.getElementById('" + idName + "')";
        case SelectorType::ATTRIBUTE:
            return "document.querySelector('" + selector + "')";
        case SelectorType::PSEUDO_CLASS:
        case SelectorType::PSEUDO_ELEMENT:
            return "document.querySelector('" + selector + "')";
        case SelectorType::DESCENDANT:
        case SelectorType::CHILD:
        case SelectorType::ADJACENT:
        case SelectorType::GENERAL:
            return "document.querySelector('" + selector + "')";
        default:
            return "document.querySelector('" + selector + "')";
    }
}

std::string CHTLJSEnhancedSelector::generateCSS() const {
    return selector;
}

std::string CHTLJSEnhancedSelector::generateCHTLJS() const {
    return "{{" + selector + "}}";
}

std::string CHTLJSEnhancedSelector::generateTagSelector() const {
    return tagName;
}

std::string CHTLJSEnhancedSelector::generateClassSelector() const {
    return "." + className;
}

std::string CHTLJSEnhancedSelector::generateIdSelector() const {
    return "#" + idName;
}

std::string CHTLJSEnhancedSelector::generateAttributeSelector() const {
    return "[" + attributeName + (attributeValue.empty() ? "" : "=" + attributeValue) + "]";
}

std::string CHTLJSEnhancedSelector::generatePseudoSelector() const {
    if (!pseudoElement.empty()) {
        return "::" + pseudoElement;
    } else if (!pseudoClass.empty()) {
        return ":" + pseudoClass;
    }
    return "";
}

std::string CHTLJSEnhancedSelector::generateComplexSelector() const {
    std::ostringstream oss;
    for (size_t i = 0; i < children.size(); ++i) {
        if (i > 0) oss << " ";
        oss << children[i]->toString();
    }
    return oss.str();
}

// 验证辅助实现
bool CHTLJSEnhancedSelector::validateSelector() const {
    return !selector.empty() && type != SelectorType::UNKNOWN;
}

bool CHTLJSEnhancedSelector::validateChildren() const {
    for (const auto& child : children) {
        if (!child || !child->isValid()) {
            return false;
        }
    }
    return true;
}

bool CHTLJSEnhancedSelector::validateAttributes() const {
    // 简化的属性验证
    return true;
}

bool CHTLJSEnhancedSelector::validateModifiers() const {
    // 简化的修饰符验证
    return true;
}

// 比较辅助实现
bool CHTLJSEnhancedSelector::compareSelector(std::shared_ptr<CHTLJSEnhancedSelector> other) const {
    return compareSelector(*other);
}

bool CHTLJSEnhancedSelector::compareSelector(const CHTLJSEnhancedSelector& other) const {
    return type == other.type &&
           selector == other.selector &&
           tagName == other.tagName &&
           className == other.className &&
           idName == other.idName &&
           attributeName == other.attributeName &&
           attributeValue == other.attributeValue &&
           pseudoClass == other.pseudoClass &&
           pseudoElement == other.pseudoElement &&
           compareChildren(other) &&
           compareAttributes(other);
}

bool CHTLJSEnhancedSelector::compareChildren(std::shared_ptr<CHTLJSEnhancedSelector> other) const {
    return compareChildren(*other);
}

bool CHTLJSEnhancedSelector::compareChildren(const CHTLJSEnhancedSelector& other) const {
    if (children.size() != other.children.size()) {
        return false;
    }
    
    for (size_t i = 0; i < children.size(); ++i) {
        if (!children[i] || !other.children[i]) {
            return false;
        }
        if (!children[i]->equals(other.children[i])) {
            return false;
        }
    }
    
    return true;
}

bool CHTLJSEnhancedSelector::compareAttributes(std::shared_ptr<CHTLJSEnhancedSelector> other) const {
    return compareAttributes(*other);
}

bool CHTLJSEnhancedSelector::compareAttributes(const CHTLJSEnhancedSelector& other) const {
    return attributes == other.attributes;
}

// 克隆辅助实现
void CHTLJSEnhancedSelector::copyTo(std::shared_ptr<CHTLJSEnhancedSelector> target) const {
    target->type = type;
    target->tagName = tagName;
    target->className = className;
    target->idName = idName;
    target->attributeName = attributeName;
    target->attributeValue = attributeValue;
    target->pseudoClass = pseudoClass;
    target->pseudoElement = pseudoElement;
    target->attributes = attributes;
    target->modifiers = modifiers;
    target->isComputed = isComputed;
    target->isOptional = isOptional;
    target->specificity = specificity;
    target->line = line;
    target->column = column;
    target->position = position;
}

void CHTLJSEnhancedSelector::deepCopyTo(std::shared_ptr<CHTLJSEnhancedSelector> target) const {
    copyTo(target);
    
    for (const auto& child : children) {
        if (child) {
            auto clonedChild = child->deepClone();
            target->addChild(clonedChild);
        }
    }
}

// 遍历辅助实现
void CHTLJSEnhancedSelector::traverseSelector(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) {
    visitor(shared_from_this());
    traverseChildren(visitor);
}

void CHTLJSEnhancedSelector::traverseSelector(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) const {
    visitor(const_cast<CHTLJSEnhancedSelector*>(this)->shared_from_this());
    traverseChildren(visitor);
}

// 查找辅助实现
void CHTLJSEnhancedSelector::collectSelectors(SelectorType type, std::vector<std::shared_ptr<CHTLJSEnhancedSelector>>& result) const {
    if (this->type == type) {
        result.push_back(const_cast<CHTLJSEnhancedSelector*>(this)->shared_from_this());
    }
    for (const auto& child : children) {
        if (child) {
            child->collectSelectors(type, result);
        }
    }
}

void CHTLJSEnhancedSelector::collectSelectors(std::function<bool(std::shared_ptr<CHTLJSEnhancedSelector>)> predicate, std::vector<std::shared_ptr<CHTLJSEnhancedSelector>>& result) const {
    if (predicate(const_cast<CHTLJSEnhancedSelector*>(this)->shared_from_this())) {
        result.push_back(const_cast<CHTLJSEnhancedSelector*>(this)->shared_from_this());
    }
    for (const auto& child : children) {
        if (child) {
            child->collectSelectors(predicate, result);
        }
    }
}

// 优化辅助实现
std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::optimizeSelector() const {
    return clone();
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::compressSelector() const {
    return clone();
}

std::shared_ptr<CHTLJSEnhancedSelector> CHTLJSEnhancedSelector::decompressSelector() const {
    return clone();
}

// 格式化辅助实现
std::string CHTLJSEnhancedSelector::formatSelector() const {
    return selector;
}

std::string CHTLJSEnhancedSelector::minifySelector() const {
    return selector;
}

std::string CHTLJSEnhancedSelector::beautifySelector() const {
    return selector;
}

// 计算特异性
int CHTLJSEnhancedSelector::calculateSpecificity() const {
    return calculateTagSpecificity() + 
           calculateClassSpecificity() + 
           calculateIdSpecificity() + 
           calculateAttributeSpecificity() + 
           calculatePseudoSpecificity();
}

int CHTLJSEnhancedSelector::calculateTagSpecificity() const {
    return !tagName.empty() ? 1 : 0;
}

int CHTLJSEnhancedSelector::calculateClassSpecificity() const {
    return !className.empty() ? 10 : 0;
}

int CHTLJSEnhancedSelector::calculateIdSpecificity() const {
    return !idName.empty() ? 100 : 0;
}

int CHTLJSEnhancedSelector::calculateAttributeSpecificity() const {
    return !attributeName.empty() ? 10 : 0;
}

int CHTLJSEnhancedSelector::calculatePseudoSpecificity() const {
    return (!pseudoClass.empty() || !pseudoElement.empty()) ? 10 : 0;
}

} // namespace CHTL_JS