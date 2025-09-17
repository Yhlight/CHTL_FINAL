#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

namespace CHTL {
namespace Util {

/**
 * 文件系统工具类
 */
class FileSystem {
public:
    /**
     * 检查文件是否存在
     * @param path 文件路径
     * @return 是否存在
     */
    static bool exists(const std::string& path);
    
    /**
     * 检查是否为目录
     * @param path 路径
     * @return 是否为目录
     */
    static bool isDirectory(const std::string& path);
    
    /**
     * 检查是否为文件
     * @param path 路径
     * @return 是否为文件
     */
    static bool isFile(const std::string& path);
    
    /**
     * 创建目录
     * @param path 目录路径
     * @return 是否成功
     */
    static bool createDirectory(const std::string& path);
    
    /**
     * 读取文件内容
     * @param path 文件路径
     * @return 文件内容
     */
    static std::string readFile(const std::string& path);
    
    /**
     * 写入文件内容
     * @param path 文件路径
     * @param content 内容
     * @return 是否成功
     */
    static bool writeFile(const std::string& path, const std::string& content);
    
    /**
     * 获取文件扩展名
     * @param path 文件路径
     * @return 扩展名
     */
    static std::string getExtension(const std::string& path);
    
    /**
     * 获取文件名（不含路径）
     * @param path 文件路径
     * @return 文件名
     */
    static std::string getFilename(const std::string& path);
    
    /**
     * 获取目录路径
     * @param path 文件路径
     * @return 目录路径
     */
    static std::string getDirectory(const std::string& path);
    
    /**
     * 列出目录内容
     * @param path 目录路径
     * @return 文件列表
     */
    static std::vector<std::string> listDirectory(const std::string& path);
    
    /**
     * 删除文件
     * @param path 文件路径
     * @return 是否成功
     */
    static bool removeFile(const std::string& path);
    
    /**
     * 删除目录
     * @param path 目录路径
     * @return 是否成功
     */
    static bool removeDirectory(const std::string& path);
};

} // namespace Util
} // namespace CHTL

#endif // FILE_SYSTEM_H