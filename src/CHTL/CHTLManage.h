#ifndef CHTL_MANAGE_H
#define CHTL_MANAGE_H

#include "CHTLContext.h"
#include "CHTLNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * CHTL管理器
 */
class CHTLManage {
public:
    CHTLManage();
    ~CHTLManage();
    
    /**
     * 编译CHTL文件
     * @param filePath 文件路径
     * @param outputDir 输出目录
     * @return 是否成功
     */
    bool compileFile(const std::string& filePath, const std::string& outputDir);
    
    /**
     * 编译CHTL字符串
     * @param content 文件内容
     * @param outputDir 输出目录
     * @return 是否成功
     */
    bool compileString(const std::string& content, const std::string& outputDir);
    
    /**
     * 设置编译选项
     * @param key 选项名
     * @param value 选项值
     */
    void setOption(const std::string& key, const std::string& value);
    
    /**
     * 获取编译选项
     * @param key 选项名
     * @return 选项值
     */
    std::string getOption(const std::string& key) const;
    
    /**
     * 获取编译错误
     * @return 错误列表
     */
    const std::vector<std::string>& getErrors() const;
    
    /**
     * 清空错误
     */
    void clearErrors();

private:
    CHTLContext context;
    std::vector<std::string> errors;
    std::map<std::string, std::string> options;
    
    /**
     * 添加错误
     * @param error 错误信息
     */
    void addError(const std::string& error);
    
    /**
     * 验证编译选项
     * @return 是否有效
     */
    bool validateOptions() const;
    
    /**
     * 处理编译过程
     * @param rootNode 根节点
     * @param outputDir 输出目录
     * @return 是否成功
     */
    bool processCompilation(BaseNode* rootNode, const std::string& outputDir);
};

} // namespace CHTL

#endif // CHTL_MANAGE_H