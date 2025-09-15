#ifndef CHTLJSGENERATOR_H
#define CHTLJSGENERATOR_H

#include "../CHTLJSNode/CHTLJSBaseNode.h"
#include "../CHTLJSContext/CHTLJSContext.h"
#include <string>
#include <memory>

namespace CHTLJS {

/**
 * CHTL JS代码生成器
 * 负责将AST转换为JavaScript代码
 */
class CHTLJSGenerator {
public:
    CHTLJSGenerator(std::shared_ptr<CHTLJSContext> context);
    ~CHTLJSGenerator();
    
    // 主要方法
    std::string generate(std::shared_ptr<CHTLJSBaseNode> ast);
    std::string generateScriptLoader(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateListen(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateAnimate(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateRouter(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateVir(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateINeverAway(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateUtilThen(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generatePrintMylove(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateSelector(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateResponsiveValue(std::shared_ptr<CHTLJSBaseNode> node);
    std::string generateBlock(std::shared_ptr<CHTLJSBaseNode> node);
    
    // 辅助方法
    void setIndentation(int level);
    std::string getIndentation() const;
    void increaseIndentation();
    void decreaseIndentation();

private:
    std::shared_ptr<CHTLJSContext> context_;
    int indentation_level_;
    std::string indentation_string_;
    
    // 内部辅助方法
    std::string generateFunctionCall(const std::string& functionName, 
                                   const std::map<std::string, std::string>& stringParams,
                                   const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& nodeParams);
    std::string generateParameters(const std::map<std::string, std::string>& stringParams,
                                 const std::map<std::string, std::shared_ptr<CHTLJSBaseNode>>& nodeParams);
    std::string escapeString(const std::string& str);
};

} // namespace CHTLJS

#endif // CHTLJSGENERATOR_H