#ifndef CHTL_JS_LOADER_H
#define CHTL_JS_LOADER_H

#include "CHTLJSContext.h"
#include "CHTLJSNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL JS文件加载器
 */
class CHTLJSLoader {
public:
    CHTLJSLoader();
    ~CHTLJSLoader();
    
    /**
     * 加载CHTL JS文件
     * @param filePath 文件路径
     * @param context 编译上下文
     * @return 根节点
     */
    std::unique_ptr<CHTLJSBaseNode> loadFile(const std::string& filePath, 
                                            CHTLJSContext& context);
    
    /**
     * 加载CHTL JS字符串
     * @param content 文件内容
     * @param context 编译上下文
     * @return 根节点
     */
    std::unique_ptr<CHTLJSBaseNode> loadString(const std::string& content, 
                                              CHTLJSContext& context);
    
    /**
     * 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool fileExists(const std::string& filePath) const;
    
    /**
     * 获取文件内容
     * @param filePath 文件路径
     * @return 文件内容
     */
    std::string readFile(const std::string& filePath) const;

private:
    /**
     * 解析文件内容
     * @param content 文件内容
     * @param context 编译上下文
     * @return 根节点
     */
    std::unique_ptr<CHTLJSBaseNode> parseContent(const std::string& content, 
                                                CHTLJSContext& context);
    
    /**
     * 处理导入
     * @param importNode 导入节点
     * @param context 编译上下文
     */
    void processImport(CHTLJSImportNode* importNode, CHTLJSContext& context);
    
    /**
     * 处理模板
     * @param templateNode 模板节点
     * @param context 编译上下文
     */
    void processTemplate(CHTLJSTemplateNode* templateNode, CHTLJSContext& context);
    
    /**
     * 处理自定义
     * @param customNode 自定义节点
     * @param context 编译上下文
     */
    void processCustom(CHTLJSCustomNode* customNode, CHTLJSContext& context);
};

} // namespace CHTL

#endif // CHTL_JS_LOADER_H