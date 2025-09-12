#ifndef CHTL_JS_CONTEXT_H
#define CHTL_JS_CONTEXT_H

#include <string>
#include <map>
#include <vector>
#include <memory>

namespace CHTL {

/**
 * CHTL JS编译上下文
 */
class CHTLJSContext {
public:
    CHTLJSContext();
    ~CHTLJSContext();
    
    /**
     * 设置源文件路径
     */
    void setSourcePath(const std::string& path);
    
    /**
     * 获取源文件路径
     */
    std::string getSourcePath() const;
    
    /**
     * 设置输出目录
     */
    void setOutputDir(const std::string& dir);
    
    /**
     * 获取输出目录
     */
    std::string getOutputDir() const;
    
    /**
     * 设置编译选项
     */
    void setOption(const std::string& key, const std::string& value);
    
    /**
     * 获取编译选项
     */
    std::string getOption(const std::string& key) const;
    
    /**
     * 添加全局变量
     */
    void addGlobalVariable(const std::string& name, const std::string& value);
    
    /**
     * 获取全局变量
     */
    std::string getGlobalVariable(const std::string& name) const;
    
    /**
     * 添加响应式变量
     */
    void addReactiveVariable(const std::string& name, const std::string& value);
    
    /**
     * 获取响应式变量
     */
    std::string getReactiveVariable(const std::string& name) const;
    
    /**
     * 清空上下文
     */
    void clear();

private:
    std::string sourcePath;
    std::string outputDir;
    std::map<std::string, std::string> options;
    std::map<std::string, std::string> globalVariables;
    std::map<std::string, std::string> reactiveVariables;
};

} // namespace CHTL

#endif // CHTL_JS_CONTEXT_H