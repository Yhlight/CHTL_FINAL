#ifndef CHTL_JS_ENHANCED_SELECTOR_H
#define CHTL_JS_ENHANCED_SELECTOR_H

#include "CHTLJSNode/CHTLJSBaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL_JS {

enum class SelectorType {
    TAG,           // 标签选择器
    CLASS,         // 类选择器
    ID,            // ID选择器
    ATTRIBUTE,     // 属性选择器
    PSEUDO_CLASS,  // 伪类选择器
    PSEUDO_ELEMENT, // 伪元素选择器
    DESCENDANT,    // 后代选择器
    CHILD,         // 子选择器
    ADJACENT,      // 相邻选择器
    GENERAL,       // 通用选择器
    ENHANCED,      // 增强选择器
    DYNAMIC_CONDITION, // 动态条件表达式
    UNKNOWN
};

class CHTLJSEnhancedSelector : public std::enable_shared_from_this<CHTLJSEnhancedSelector> {
private:
    std::string selector;
    SelectorType type;
    std::string tagName;
    std::string className;
    std::string idName;
    std::string attributeName;
    std::string attributeValue;
    std::string pseudoClass;
    std::string pseudoElement;
    std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> children;
    std::shared_ptr<CHTLJSEnhancedSelector> parent;
    
    // 选择器属性
    std::map<std::string, std::string> attributes;
    std::vector<std::string> modifiers;
    bool computed;
    bool optional;
    int specificity;
    
    // 动态条件表达式
    std::string conditionExpression;
    std::string trueValue;
    std::string falseValue;
    
    // 位置信息
    int line;
    int column;
    int position;

public:
    CHTLJSEnhancedSelector();
    CHTLJSEnhancedSelector(const std::string& selector);
    virtual ~CHTLJSEnhancedSelector() = default;
    
    // 基本属性
    const std::string& getSelector() const { return selector; }
    void setSelector(const std::string& selector);
    
    SelectorType getType() const { return type; }
    void setType(SelectorType type) { this->type = type; }
    
    const std::string& getTagName() const { return tagName; }
    void setTagName(const std::string& tagName) { this->tagName = tagName; }
    
    const std::string& getClassName() const { return className; }
    void setClassName(const std::string& className) { this->className = className; }
    
    const std::string& getIdName() const { return idName; }
    void setIdName(const std::string& idName) { this->idName = idName; }
    
    const std::string& getAttributeName() const { return attributeName; }
    void setAttributeName(const std::string& attributeName) { this->attributeName = attributeName; }
    
    const std::string& getAttributeValue() const { return attributeValue; }
    void setAttributeValue(const std::string& attributeValue) { this->attributeValue = attributeValue; }
    
    const std::string& getPseudoClass() const { return pseudoClass; }
    void setPseudoClass(const std::string& pseudoClass) { this->pseudoClass = pseudoClass; }
    
    const std::string& getPseudoElement() const { return pseudoElement; }
    void setPseudoElement(const std::string& pseudoElement) { this->pseudoElement = pseudoElement; }
    
    // 子选择器管理
    void addChild(std::shared_ptr<CHTLJSEnhancedSelector> child);
    void removeChild(std::shared_ptr<CHTLJSEnhancedSelector> child);
    void removeChild(size_t index);
    void clearChildren();
    
    std::shared_ptr<CHTLJSEnhancedSelector> getChild(size_t index) const;
    std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> getChildren() const { return children; }
    size_t getChildCount() const { return children.size(); }
    
    // 父选择器管理
    std::shared_ptr<CHTLJSEnhancedSelector> getParent() const { return parent; }
    void setParent(std::shared_ptr<CHTLJSEnhancedSelector> parent) { this->parent = parent; }
    
    // 属性管理
    void setAttribute(const std::string& key, const std::string& value);
    std::string getAttribute(const std::string& key) const;
    bool hasAttribute(const std::string& key) const;
    void removeAttribute(const std::string& key);
    const std::map<std::string, std::string>& getAttributes() const { return attributes; }
    
    // 修饰符管理
    void addModifier(const std::string& modifier);
    void removeModifier(const std::string& modifier);
    void clearModifiers();
    const std::vector<std::string>& getModifiers() const { return modifiers; }
    bool hasModifier(const std::string& modifier) const;
    
    // 选择器属性
    bool isComputed() const { return computed; }
    void setComputed(bool computed) { this->computed = computed; }
    
    // 动态条件表达式
    void setDynamicCondition(const std::string& condition, const std::string& trueVal, const std::string& falseVal);
    std::string getConditionExpression() const { return conditionExpression; }
    std::string getTrueValue() const { return trueValue; }
    std::string getFalseValue() const { return falseValue; }
    bool isDynamicCondition() const { return type == SelectorType::DYNAMIC_CONDITION; }
    
    bool isOptional() const { return optional; }
    void setOptional(bool optional) { this->optional = optional; }
    
    int getSpecificity() const { return specificity; }
    void setSpecificity(int specificity) { this->specificity = specificity; }
    
    // 位置信息
    int getLine() const { return line; }
    void setLine(int line) { this->line = line; }
    
    int getColumn() const { return column; }
    void setColumn(int column) { this->column = column; }
    
    int getPosition() const { return position; }
    void setPosition(int position) { this->position = position; }
    
    void setLocation(int line, int column, int position);
    
    // 类型判断
    bool isTagSelector() const { return type == SelectorType::TAG; }
    bool isClassSelector() const { return type == SelectorType::CLASS; }
    bool isIdSelector() const { return type == SelectorType::ID; }
    bool isAttributeSelector() const { return type == SelectorType::ATTRIBUTE; }
    bool isPseudoClassSelector() const { return type == SelectorType::PSEUDO_CLASS; }
    bool isPseudoElementSelector() const { return type == SelectorType::PSEUDO_ELEMENT; }
    bool isDescendantSelector() const { return type == SelectorType::DESCENDANT; }
    bool isChildSelector() const { return type == SelectorType::CHILD; }
    bool isAdjacentSelector() const { return type == SelectorType::ADJACENT; }
    bool isGeneralSelector() const { return type == SelectorType::GENERAL; }
    bool isEnhancedSelector() const { return type == SelectorType::ENHANCED; }
    
    // 解析和生成
    bool parse(const std::string& selector);
    std::string toJavaScript() const;
    std::string toCSS() const;
    std::string toCHTLJS() const;
    std::string toString() const;
    std::string toDebugString() const;
    
    // 验证
    bool isValid() const;
    bool isComplete() const;
    std::vector<std::string> validate() const;
    
    // 比较
    bool equals(std::shared_ptr<CHTLJSEnhancedSelector> other) const;
    bool equals(const CHTLJSEnhancedSelector& other) const;
    
    // 克隆
    std::shared_ptr<CHTLJSEnhancedSelector> clone() const;
    std::shared_ptr<CHTLJSEnhancedSelector> deepClone() const;
    
    // 遍历
    void traverse(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor);
    void traverse(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) const;
    void traverseChildren(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor);
    void traverseChildren(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) const;
    
    // 查找
    std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> findSelectors(SelectorType type) const;
    std::vector<std::shared_ptr<CHTLJSEnhancedSelector>> findSelectors(std::function<bool(std::shared_ptr<CHTLJSEnhancedSelector>)> predicate) const;
    std::shared_ptr<CHTLJSEnhancedSelector> findFirstSelector(SelectorType type) const;
    std::shared_ptr<CHTLJSEnhancedSelector> findFirstSelector(std::function<bool(std::shared_ptr<CHTLJSEnhancedSelector>)> predicate) const;
    
    // 优化
    std::shared_ptr<CHTLJSEnhancedSelector> optimize() const;
    std::shared_ptr<CHTLJSEnhancedSelector> compress() const;
    std::shared_ptr<CHTLJSEnhancedSelector> decompress() const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;

private:
    // 解析辅助
    bool parseTagSelector(const std::string& selector);
    bool parseClassSelector(const std::string& selector);
    bool parseIdSelector(const std::string& selector);
    bool parseAttributeSelector(const std::string& selector);
    bool parsePseudoSelector(const std::string& selector);
    bool parseComplexSelector(const std::string& selector);
    
    // 类型推断
    SelectorType inferType(const std::string& selector) const;
    bool isTagName(const std::string& name) const;
    bool isClassName(const std::string& name) const;
    bool isIdName(const std::string& name) const;
    bool isAttributeName(const std::string& name) const;
    bool isPseudoClass(const std::string& name) const;
    bool isPseudoElement(const std::string& name) const;
    
    // 生成辅助
    std::string generateJavaScript() const;
    std::string generateCSS() const;
    std::string generateCHTLJS() const;
    std::string generateTagSelector() const;
    std::string generateClassSelector() const;
    std::string generateIdSelector() const;
    std::string generateAttributeSelector() const;
    std::string generatePseudoSelector() const;
    std::string generateComplexSelector() const;
    
    // 验证辅助
    bool validateSelector() const;
    bool validateChildren() const;
    bool validateAttributes() const;
    bool validateModifiers() const;
    
    // 比较辅助
    bool compareSelector(std::shared_ptr<CHTLJSEnhancedSelector> other) const;
    bool compareSelector(const CHTLJSEnhancedSelector& other) const;
    bool compareChildren(std::shared_ptr<CHTLJSEnhancedSelector> other) const;
    bool compareChildren(const CHTLJSEnhancedSelector& other) const;
    bool compareAttributes(std::shared_ptr<CHTLJSEnhancedSelector> other) const;
    bool compareAttributes(const CHTLJSEnhancedSelector& other) const;
    
    // 克隆辅助
    void copyTo(std::shared_ptr<CHTLJSEnhancedSelector> target) const;
    void deepCopyTo(std::shared_ptr<CHTLJSEnhancedSelector> target) const;
    
    // 遍历辅助
    void traverseSelector(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor);
    void traverseSelector(std::function<void(std::shared_ptr<CHTLJSEnhancedSelector>)> visitor) const;
    
    // 查找辅助
    void collectSelectors(SelectorType type, std::vector<std::shared_ptr<CHTLJSEnhancedSelector>>& result) const;
    void collectSelectors(std::function<bool(std::shared_ptr<CHTLJSEnhancedSelector>)> predicate, std::vector<std::shared_ptr<CHTLJSEnhancedSelector>>& result) const;
    
    // 优化辅助
    std::shared_ptr<CHTLJSEnhancedSelector> optimizeSelector() const;
    std::shared_ptr<CHTLJSEnhancedSelector> compressSelector() const;
    std::shared_ptr<CHTLJSEnhancedSelector> decompressSelector() const;
    
    // 格式化辅助
    std::string formatSelector() const;
    std::string minifySelector() const;
    std::string beautifySelector() const;
    
    // 计算特异性
    int calculateSpecificity() const;
    int calculateTagSpecificity() const;
    int calculateClassSpecificity() const;
    int calculateIdSpecificity() const;
    int calculateAttributeSpecificity() const;
    int calculatePseudoSpecificity() const;
};

} // namespace CHTL_JS

#endif // CHTL_JS_ENHANCED_SELECTOR_H