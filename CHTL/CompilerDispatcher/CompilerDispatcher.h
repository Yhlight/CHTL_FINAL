#ifndef COMPILER_DISPATCHER_H
#define COMPILER_DISPATCHER_H

#include <string>
#include <memory>
#include "Scanner/UnifiedScanner.h"

namespace CHTL {

/**
 * 编译器调度器
 * 负责协调各个编译器的编译过程
 */
class CompilerDispatcher {
public:
    CompilerDispatcher();
    ~CompilerDispatcher();
    
    /**
     * 编译CHTL文件
     * @param inputFile 输入文件路径
     * @return 编译结果（HTML代码）
     */
    std::string compile(const std::string& inputFile);
    
    /**
     * 编译CHTL代码
     * @param code 输入代码
     * @return 编译结果（HTML代码）
     */
    std::string compileCode(const std::string& code);
    
    /**
     * 设置是否输出默认结构
     * @param enable 是否启用
     */
    void setDefaultStruct(bool enable);
    
    /**
     * 设置内联模式
     * @param enable 是否启用
     */
    void setInlineMode(bool enable);
    
    /**
     * 设置内联CSS
     * @param enable 是否启用
     */
    void setInlineCSS(bool enable);
    
    /**
     * 设置内联JS
     * @param enable 是否启用
     */
    void setInlineJS(bool enable);
    
    /**
     * 设置输出文件
     * @param filename 文件名
     */
    void setOutputFile(const std::string& filename);

private:
    /**
     * 读取文件内容
     * @param filename 文件名
     * @return 文件内容
     */
    std::string readFile(const std::string& filename);
    
    /**
     * 写入文件内容
     * @param filename 文件名
     * @param content 内容
     */
    void writeFile(const std::string& filename, const std::string& content);
    
    /**
     * 处理编译选项
     * @param code 代码
     * @return 处理后的代码
     */
    std::string processCompilationOptions(const std::string& code);
    
    /**
     * 生成默认HTML结构
     * @param content 内容
     * @return 带结构的HTML
     */
    std::string generateDefaultStructure(const std::string& content);
    
    /**
     * 内联处理
     * @param html HTML内容
     * @param css CSS内容
     * @param js JS内容
     * @return 内联后的HTML
     */
    std::string inlineResources(const std::string& html, 
                               const std::string& css, 
                               const std::string& js);

private:
    std::unique_ptr<UnifiedScanner> m_scanner;
    
    // 编译选项
    bool m_defaultStruct;
    bool m_inlineMode;
    bool m_inlineCSS;
    bool m_inlineJS;
    std::string m_outputFile;
};

} // namespace CHTL

#endif // COMPILER_DISPATCHER_H