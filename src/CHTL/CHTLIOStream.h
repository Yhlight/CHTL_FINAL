#ifndef CHTL_IOSTREAM_H
#define CHTL_IOSTREAM_H

#include <string>
#include <fstream>
#include <iostream>
#include <memory>

namespace CHTL {

/**
 * CHTL IO流
 */
class CHTLIOStream {
public:
    CHTLIOStream();
    ~CHTLIOStream();
    
    /**
     * 打开文件
     * @param filePath 文件路径
     * @param mode 打开模式
     * @return 是否成功
     */
    bool open(const std::string& filePath, std::ios_base::openmode mode = std::ios_base::in);
    
    /**
     * 关闭文件
     */
    void close();
    
    /**
     * 检查文件是否打开
     * @return 是否打开
     */
    bool isOpen() const;
    
    /**
     * 读取一行
     * @return 读取的行
     */
    std::string readLine();
    
    /**
     * 读取所有内容
     * @return 文件内容
     */
    std::string readAll();
    
    /**
     * 写入内容
     * @param content 内容
     * @return 是否成功
     */
    bool write(const std::string& content);
    
    /**
     * 写入一行
     * @param line 行内容
     * @return 是否成功
     */
    bool writeLine(const std::string& line);
    
    /**
     * 获取文件大小
     * @return 文件大小
     */
    size_t getFileSize() const;
    
    /**
     * 检查文件是否存在
     * @param filePath 文件路径
     * @return 是否存在
     */
    static bool fileExists(const std::string& filePath);
    
    /**
     * 创建目录
     * @param dirPath 目录路径
     * @return 是否成功
     */
    static bool createDirectory(const std::string& dirPath);

private:
    std::unique_ptr<std::fstream> fileStream;
    std::string currentFilePath;
};

} // namespace CHTL

#endif // CHTL_IOSTREAM_H