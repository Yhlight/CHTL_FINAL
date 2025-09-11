#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CHTLLoader {
private:
    std::string basePath;
    std::vector<std::string> loadedFiles;
    std::vector<std::string> errors;

public:
    CHTLLoader();
    CHTLLoader(const std::string& basePath);
    virtual ~CHTLLoader() = default;
    
    // 基本属性
    const std::string& getBasePath() const { return basePath; }
    void setBasePath(const std::string& path) { basePath = path; }
    
    // 文件加载
    std::string loadFile(const std::string& filename);
    std::vector<std::string> loadFiles(const std::vector<std::string>& filenames);
    bool isFileLoaded(const std::string& filename) const;
    
    // 错误管理
    const std::vector<std::string>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }
    void clearErrors();
    
    // 加载历史
    const std::vector<std::string>& getLoadedFiles() const { return loadedFiles; }
    void clearLoadedFiles();
    
    // 工具方法
    std::string resolvePath(const std::string& filename) const;
    bool fileExists(const std::string& filename) const;
    std::string getFileExtension(const std::string& filename) const;
    
    // 字符串表示
    std::string toString() const;
    std::string toDebugString() const;
};

} // namespace CHTL

#endif // CHTL_LOADER_H