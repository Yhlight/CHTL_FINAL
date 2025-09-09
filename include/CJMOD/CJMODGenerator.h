#ifndef CJMOD_GENERATOR_H
#define CJMOD_GENERATOR_H

#include "Arg.h"
#include <string>
#include <vector>

namespace CJMOD {

/**
 * 生成器用于 CJMOD API 的接口
 */
class CJMODGenerator {
public:
    /**
     * 导出最终的代码
     * @param args 参数对象
     * @return 生成的代码
     */
    static std::string exportResult(const Arg& args);
    
    /**
     * 导出最终的代码（重载版本）
     * @param args 参数对象
     * @param templateStr 模板字符串
     * @return 生成的代码
     */
    static std::string exportResult(const Arg& args, const std::string& templateStr);
    
    /**
     * 设置输出格式
     * @param format 输出格式
     */
    static void setOutputFormat(const std::string& format);
    
    /**
     * 启用/禁用调试模式
     * @param enabled 是否启用调试
     */
    static void setDebugMode(bool enabled);
    
    /**
     * 设置代码风格
     * @param style 代码风格
     */
    static void setCodeStyle(const std::string& style);

private:
    static std::string s_outputFormat;
    static bool s_debugMode;
    static std::string s_codeStyle;
    
    /**
     * 格式化代码
     * @param code 原始代码
     * @return 格式化后的代码
     */
    static std::string formatCode(const std::string& code);
    
    /**
     * 应用代码风格
     * @param code 原始代码
     * @return 应用风格后的代码
     */
    static std::string applyCodeStyle(const std::string& code);
    
    /**
     * 生成缩进
     * @param level 缩进级别
     * @return 缩进字符串
     */
    static std::string generateIndent(int level);
    
    /**
     * 调试输出
     * @param message 调试消息
     */
    static void debugOutput(const std::string& message);
};

} // namespace CJMOD

#endif // CJMOD_GENERATOR_H