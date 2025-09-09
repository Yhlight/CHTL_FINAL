#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "CHTLNode/BaseNode.hpp"
#include "CHTLLexer/Lexer.hpp"
#include "CHTLParser/Parser.hpp"

namespace CHTL {

/**
 * @brief 模板解析器
 * 
 * 解析模板定义和实例化语法
 */
class TemplateParser {
public:
    TemplateParser(Lexer& lexer, Parser& parser);
    
    // 解析模板定义
    std::shared_ptr<BaseNode> parseTemplateDefinition();
    
    // 解析模板实例化
    std::shared_ptr<BaseNode> parseTemplateInstantiation();
    
    // 解析样式组模板
    std::shared_ptr<BaseNode> parseStyleGroupTemplate();
    
    // 解析元素模板
    std::shared_ptr<BaseNode> parseElementTemplate();
    
    // 解析变量组模板
    std::shared_ptr<BaseNode> parseVariableGroupTemplate();
    
    // 解析模板参数
    std::map<std::string, std::string> parseTemplateParameters();
    
    // 解析模板特例化
    std::shared_ptr<BaseNode> parseTemplateSpecialization();
    
private:
    Lexer& lexer_;
    Parser& parser_;
    
    // 解析模板继承
    std::shared_ptr<BaseNode> parseTemplateInheritance();
    
    // 解析模板删除操作
    std::vector<std::string> parseDeleteOperations();
    
    // 解析模板插入操作
    std::shared_ptr<BaseNode> parseInsertOperation();
    
    // 解析模板替换操作
    std::shared_ptr<BaseNode> parseReplaceOperation();
};

} // namespace CHTL