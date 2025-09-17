#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <string>
#include <vector>

namespace CHTL {
namespace Util {

/**
 * ZIP工具类
 */
class ZipUtil {
public:
    /**
     * 压缩文件或目录
     * @param source 源路径
     * @param destination ZIP文件路径
     * @return 是否成功
     */
    static bool compress(const std::string& source, const std::string& destination);
    
    /**
     * 解压ZIP文件
     * @param zipPath ZIP文件路径
     * @param destination 解压目标路径
     * @return 是否成功
     */
    static bool extract(const std::string& zipPath, const std::string& destination);
    
    /**
     * 列出ZIP文件内容
     * @param zipPath ZIP文件路径
     * @return 文件列表
     */
    static std::vector<std::string> listFiles(const std::string& zipPath);
    
    /**
     * 检查ZIP文件是否有效
     * @param zipPath ZIP文件路径
     * @return 是否有效
     */
    static bool isValid(const std::string& zipPath);
};

} // namespace Util
} // namespace CHTL

#endif // ZIP_UTIL_H