#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include "CHTLContext.h"
#include "CHTLNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL文件加载器
 */
class CHTLLoader {
public:
    CHTLLoader();
    ~CHTLLoader();
    
    /**
     * 加载CHTL文件
     * @param filePath 文件路径
     * @param context 编译上下文
     * @return 根节点
     */
    std::unique_ptr<BaseNode> loadFile(const std::string& filePath, 
                                      CHTLContext& context);
    
    /**
     * 加载CHTL字符串
     * @param content 文件内容
     * @param context 编译上下文
     * @return 根节点
     */
    std::unique_ptr<BaseNode> loadString(const std::string& content, 
                                        CHTLContext& context);
    
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
    std::unique_ptr<BaseNode> parseContent(const std::string& content, 
                                          CHTLContext& context);
    
    /**
     * 处理节点
     * @param node 节点
     * @param context 编译上下文
     */
    void processNode(BaseNode* node, CHTLContext& context);
    
    /**
     * 处理导入
     * @param importNode 导入节点
     * @param context 编译上下文
     */
    void processImport(ImportNode* importNode, CHTLContext& context);
    
    /**
     * 处理模板
     * @param templateNode 模板节点
     * @param context 编译上下文
     */
    void processTemplate(TemplateNode* templateNode, CHTLContext& context);
    
    /**
     * 处理自定义
     * @param customNode 自定义节点
     * @param context 编译上下文
     */
    void processCustom(CustomNode* customNode, CHTLContext& context);
};

} // namespace CHTL

#endif // CHTL_LOADER_H