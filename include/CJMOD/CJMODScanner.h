#ifndef CJMOD_SCANNER_H
#define CJMOD_SCANNER_H

#include "Arg.h"
#include <string>
#include <vector>

namespace CJMOD {

/**
 * 统一扫描器用于 CJMOD API 的接口
 */
class CJMODScanner {
public:
    /**
     * 扫描语法片段
     * @param args 参数对象
     * @param keyword 扫描的关键字
     * @return 扫描结果
     */
    static Arg scan(const Arg& args, const std::string& keyword);
    
    /**
     * 扫描语法片段（重载版本）
     * @param args 参数对象
     * @return 扫描结果
     */
    static Arg scan(const Arg& args);
    
    /**
     * 设置扫描模式
     * @param mode 扫描模式
     */
    static void setScanMode(int mode);
    
    /**
     * 启用/禁用调试模式
     * @param enabled 是否启用调试
     */
    static void setDebugMode(bool enabled);

private:
    static int s_scanMode;
    static bool s_debugMode;
    
    /**
     * 双指针扫描算法
     * @param content 要扫描的内容
     * @param keyword 关键字
     * @return 扫描结果
     */
    static std::vector<std::string> doublePointerScan(const std::string& content, const std::string& keyword);
    
    /**
     * 前置截取处理
     * @param content 原始内容
     * @return 处理后的内容
     */
    static std::string preprocessContent(const std::string& content);
    
    /**
     * 检查是否为关键字
     * @param content 内容
     * @param pos 位置
     * @param keyword 关键字
     * @return 是否为关键字
     */
    static bool isKeyword(const std::string& content, size_t pos, const std::string& keyword);
    
    /**
     * 查找关键字位置
     * @param content 内容
     * @param keyword 关键字
     * @param startPos 开始位置
     * @return 关键字位置
     */
    static size_t findKeyword(const std::string& content, const std::string& keyword, size_t startPos);
    
    /**
     * 调试输出
     * @param message 调试消息
     */
    static void debugOutput(const std::string& message);
};

} // namespace CJMOD

#endif // CJMOD_SCANNER_H