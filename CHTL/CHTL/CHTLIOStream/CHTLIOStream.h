#ifndef CHTL_IOSTREAM_H
#define CHTL_IOSTREAM_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

namespace CHTL {

class CHTLIOStream {
private:
    std::string inputPath;
    std::string outputPath;
    std::stringstream buffer;
    bool debugMode;

public:
    CHTLIOStream();
    CHTLIOStream(const std::string& input, const std::string& output);
    virtual ~CHTLIOStream() = default;
    
    // 基本属性
    const std::string& getInputPath() const { return inputPath; }
    void setInputPath(const std::string& path) { inputPath = path; }
    
    const std::string& getOutputPath() const { return outputPath; }
    void setOutputPath(const std::string& path) { outputPath = path; }
    
    // 输入操作
    std::string readInput();
    std::string readFile(const std::string& filename);
    bool hasInput() const;
    
    // 输出操作
    void writeOutput(const std::string& content);
    void writeFile(const std::string& filename, const std::string& content);
    void appendOutput(const std::string& content);
    
    // 缓冲区操作
    void clearBuffer();
    std::string getBufferContent() const;
    void setBufferContent(const std::string& content);
    
    // 模式设置
    bool isDebugMode() const { return debugMode; }
    void setDebugMode(bool mode) { debugMode = mode; }
    
    // 工具方法
    bool fileExists(const std::string& filename) const;
    std::string getFileExtension(const std::string& filename) const;
    std::string getFileName(const std::string& path) const;
    std::string getDirectory(const std::string& path) const;
    
    // 字符串表示
    std::string toString() const;
    std::string toDebugString() const;
    
    // 序列化
    std::string toJSON() const;
    std::string toXML() const;
    std::string toYAML() const;
    
    // 克隆
    std::shared_ptr<CHTLIOStream> clone() const;
    
    // 比较
    bool operator==(const CHTLIOStream& other) const;
    bool operator!=(const CHTLIOStream& other) const;
};

} // namespace CHTL

#endif // CHTL_IOSTREAM_H