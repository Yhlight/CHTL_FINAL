#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * 编译器调度器
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    /**
     * 调度编译过程
     * @param sourcePath 源文件路径
     * @param outputDir 输出目录
     * @return 是否成功
     */
    bool dispatch(const std::string& sourcePath, const std::string& outputDir);
    
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
    std::map<std::string, std::string> options;
    std::vector<std::string> errors;
    
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
     * 处理CHTL编译
     * @param sourcePath 源文件路径
     * @param outputDir 输出目录
     * @return 是否成功
     */
    bool processCHTLCompilation(const std::string& sourcePath, const std::string& outputDir);
    
    /**
     * 处理CHTL JS编译
     * @param sourcePath 源文件路径
     * @param outputDir 输出目录
     * @return 是否成功
     */
    bool processCHTLJSCompilation(const std::string& sourcePath, const std::string& outputDir);
    
    /**
     * 处理代码合并
     * @param outputDir 输出目录
     * @return 是否成功
     */
    bool processCodeMerging(const std::string& outputDir);
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H