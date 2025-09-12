#ifndef CODE_MERGER_H
#define CODE_MERGER_H

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

/**
 * 代码合并器
 */
class CodeMerger {
public:
    CodeMerger();
    ~CodeMerger();
    
    /**
     * 合并HTML代码
     * @param htmlCode HTML代码
     * @return 合并后的HTML代码
     */
    std::string mergeHTML(const std::string& htmlCode);
    
    /**
     * 合并CSS代码
     * @param cssCode CSS代码
     * @return 合并后的CSS代码
     */
    std::string mergeCSS(const std::string& cssCode);
    
    /**
     * 合并JavaScript代码
     * @param jsCode JavaScript代码
     * @return 合并后的JavaScript代码
     */
    std::string mergeJavaScript(const std::string& jsCode);
    
    /**
     * 合并所有代码
     * @param htmlCode HTML代码
     * @param cssCode CSS代码
     * @param jsCode JavaScript代码
     * @return 合并后的完整代码
     */
    std::string mergeAll(const std::string& htmlCode, 
                        const std::string& cssCode, 
                        const std::string& jsCode);
    
    /**
     * 添加外部CSS文件
     * @param cssFile CSS文件路径
     */
    void addExternalCSS(const std::string& cssFile);
    
    /**
     * 添加外部JavaScript文件
     * @param jsFile JavaScript文件路径
     */
    void addExternalJS(const std::string& jsFile);
    
    /**
     * 设置合并选项
     * @param key 选项名
     * @param value 选项值
     */
    void setOption(const std::string& key, const std::string& value);
    
    /**
     * 获取合并选项
     * @param key 选项名
     * @return 选项值
     */
    std::string getOption(const std::string& key) const;
    
    /**
     * 清空外部文件
     */
    void clearExternalFiles();

private:
    std::vector<std::string> externalCSSFiles;
    std::vector<std::string> externalJSFiles;
    std::map<std::string, std::string> options;
    
    /**
     * 读取文件内容
     * @param filePath 文件路径
     * @return 文件内容
     */
    std::string readFile(const std::string& filePath) const;
    
    /**
     * 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    bool fileExists(const std::string& filePath) const;
    
    /**
     * 压缩CSS代码
     * @param cssCode CSS代码
     * @return 压缩后的CSS代码
     */
    std::string minifyCSS(const std::string& cssCode) const;
    
    /**
     * 压缩JavaScript代码
     * @param jsCode JavaScript代码
     * @return 压缩后的JavaScript代码
     */
    std::string minifyJS(const std::string& jsCode) const;
    
    /**
     * 压缩HTML代码
     * @param htmlCode HTML代码
     * @return 压缩后的HTML代码
     */
    std::string minifyHTML(const std::string& htmlCode) const;
};

} // namespace CHTL

#endif // CODE_MERGER_H