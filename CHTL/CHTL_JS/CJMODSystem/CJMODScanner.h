#ifndef CJMOD_SCANNER_H
#define CJMOD_SCANNER_H

#include "CJMODArg.h"
#include "CJMODSyntax.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <any>
#include <functional>

namespace CHTL_JS {

// CJMOD扫描器类
class CJMODScanner {
public:
    // 构造函数和析构函数
    CJMODScanner();
    ~CJMODScanner();
    
    // 基本扫描功能
    static std::vector<CJMODArg> scan(const std::vector<CJMODArg>& args, const std::string& keyword = "");
    static std::vector<CJMODArg> scan(const std::string& code, const std::string& keyword = "");
    static std::vector<CJMODArg> scanFile(const std::string& filePath, const std::string& keyword = "");
    
    // 双指针扫描
    static std::vector<CJMODArg> doublePointerScan(const std::string& code, const std::string& keyword);
    static std::vector<CJMODArg> doublePointerScan(const std::vector<CJMODArg>& args, const std::string& keyword);
    
    // 前置截取
    static std::string preExtract(const std::string& code, const std::string& keyword);
    static std::vector<CJMODArg> preExtractScan(const std::string& code, const std::string& keyword);
    
    // 关键字检测
    static bool hasKeyword(const std::string& code, const std::string& keyword);
    static std::vector<size_t> findKeywordPositions(const std::string& code, const std::string& keyword);
    static std::vector<std::string> extractKeywords(const std::string& code);
    
    // 代码片段处理
    static std::vector<std::string> extractFragments(const std::string& code, const std::string& keyword);
    static std::vector<std::string> extractFragmentsByBoundary(const std::string& code, const std::string& startBoundary, const std::string& endBoundary);
    static std::vector<std::string> extractFragmentsByPattern(const std::string& code, const std::string& pattern);
    
    // 参数处理
    static std::vector<CJMODArg> processArgs(const std::vector<CJMODArg>& args, const std::string& keyword);
    static std::vector<CJMODArg> processArgsByType(const std::vector<CJMODArg>& args, const std::string& type);
    static std::vector<CJMODArg> processArgsByPattern(const std::vector<CJMODArg>& args, const std::string& pattern);
    
    // 代码验证
    static bool validateCode(const std::string& code);
    static bool validateArgs(const std::vector<CJMODArg>& args);
    static bool validateKeyword(const std::string& keyword);
    
    // 代码转换
    static std::string convertCode(const std::string& code, const std::string& fromType, const std::string& toType);
    static std::vector<CJMODArg> convertArgs(const std::vector<CJMODArg>& args, const std::string& fromType, const std::string& toType);
    
    // 代码格式化
    static std::string formatCode(const std::string& code);
    static std::string formatArgs(const std::vector<CJMODArg>& args);
    
    // 代码压缩
    static std::string minifyCode(const std::string& code);
    static std::string minifyArgs(const std::vector<CJMODArg>& args);
    
    // 代码美化
    static std::string beautifyCode(const std::string& code);
    static std::string beautifyArgs(const std::vector<CJMODArg>& args);
    
    // 统计信息
    static std::map<std::string, int> getStatistics(const std::string& code);
    static std::map<std::string, int> getArgsStatistics(const std::vector<CJMODArg>& args);
    
    // 调试功能
    static std::string debugCode(const std::string& code);
    static std::string debugArgs(const std::vector<CJMODArg>& args);
    static std::string debugScanResult(const std::vector<CJMODArg>& result);
    
    // 工具函数
    static std::string codeTypeToString(const std::string& type);
    static std::string stringToCodeType(const std::string& type);
    static bool isValidCodeType(const std::string& type);
    static bool isValidKeyword(const std::string& keyword);
    
    // 序列化
    static std::string toJSON(const std::vector<CJMODArg>& args);
    static std::string toXML(const std::vector<CJMODArg>& args);
    static std::string toYAML(const std::vector<CJMODArg>& args);
    static std::string toString(const std::vector<CJMODArg>& args);
    static std::string toDebugString(const std::vector<CJMODArg>& args);
    
    // 反序列化
    static std::vector<CJMODArg> fromJSON(const std::string& json);
    static std::vector<CJMODArg> fromXML(const std::string& xml);
    static std::vector<CJMODArg> fromYAML(const std::string& yaml);
    static std::vector<CJMODArg> fromString(const std::string& str);
    
    // 克隆
    static std::vector<CJMODArg> clone(const std::vector<CJMODArg>& args);
    static std::vector<CJMODArg> deepClone(const std::vector<CJMODArg>& args);
    
    // 比较
    static bool equals(const std::vector<CJMODArg>& args1, const std::vector<CJMODArg>& args2);
    static bool equals(const std::vector<CJMODArg>& args, const std::string& code);
    
    // 格式化
    static std::string format(const std::vector<CJMODArg>& args);
    static std::string minify(const std::vector<CJMODArg>& args);
    static std::string beautify(const std::vector<CJMODArg>& args);

private:
    // 内部方法
    static void initialize();
    static void cleanup();
    static bool validateInput(const std::string& code);
    static bool validateInput(const std::vector<CJMODArg>& args);
    
    // 双指针扫描内部方法
    static std::vector<CJMODArg> doublePointerScanInternal(const std::string& code, const std::string& keyword);
    static std::vector<CJMODArg> doublePointerScanArgsInternal(const std::vector<CJMODArg>& args, const std::string& keyword);
    
    // 前置截取内部方法
    static std::string preExtractInternal(const std::string& code, const std::string& keyword);
    static std::vector<CJMODArg> preExtractScanInternal(const std::string& code, const std::string& keyword);
    
    // 关键字检测内部方法
    static bool hasKeywordInternal(const std::string& code, const std::string& keyword);
    static std::vector<size_t> findKeywordPositionsInternal(const std::string& code, const std::string& keyword);
    static std::vector<std::string> extractKeywordsInternal(const std::string& code);
    
    // 代码片段处理内部方法
    static std::vector<std::string> extractFragmentsInternal(const std::string& code, const std::string& keyword);
    static std::vector<std::string> extractFragmentsByBoundaryInternal(const std::string& code, const std::string& startBoundary, const std::string& endBoundary);
    static std::vector<std::string> extractFragmentsByPatternInternal(const std::string& code, const std::string& pattern);
    
    // 参数处理内部方法
    static std::vector<CJMODArg> processArgsInternal(const std::vector<CJMODArg>& args, const std::string& keyword);
    static std::vector<CJMODArg> processArgsByTypeInternal(const std::vector<CJMODArg>& args, const std::string& type);
    static std::vector<CJMODArg> processArgsByPatternInternal(const std::vector<CJMODArg>& args, const std::string& pattern);
    
    // 代码验证内部方法
    static bool validateCodeInternal(const std::string& code);
    static bool validateArgsInternal(const std::vector<CJMODArg>& args);
    static bool validateKeywordInternal(const std::string& keyword);
    
    // 代码转换内部方法
    static std::string convertCodeInternal(const std::string& code, const std::string& fromType, const std::string& toType);
    static std::vector<CJMODArg> convertArgsInternal(const std::vector<CJMODArg>& args, const std::string& fromType, const std::string& toType);
    
    // 代码格式化内部方法
    static std::string formatCodeInternal(const std::string& code);
    static std::string formatArgsInternal(const std::vector<CJMODArg>& args);
    
    // 代码压缩内部方法
    static std::string minifyCodeInternal(const std::string& code);
    static std::string minifyArgsInternal(const std::vector<CJMODArg>& args);
    
    // 代码美化内部方法
    static std::string beautifyCodeInternal(const std::string& code);
    static std::string beautifyArgsInternal(const std::vector<CJMODArg>& args);
    
    // 统计信息内部方法
    static std::map<std::string, int> getStatisticsInternal(const std::string& code);
    static std::map<std::string, int> getArgsStatisticsInternal(const std::vector<CJMODArg>& args);
    
    // 调试功能内部方法
    static std::string debugCodeInternal(const std::string& code);
    static std::string debugArgsInternal(const std::vector<CJMODArg>& args);
    static std::string debugScanResultInternal(const std::vector<CJMODArg>& result);
    
    // 工具函数内部方法
    static std::string codeTypeToStringInternal(const std::string& type);
    static std::string stringToCodeTypeInternal(const std::string& type);
    static bool isValidCodeTypeInternal(const std::string& type);
    static bool isValidKeywordInternal(const std::string& keyword);
    
    // 序列化内部方法
    static std::string toJSONInternal(const std::vector<CJMODArg>& args);
    static std::string toXMLInternal(const std::vector<CJMODArg>& args);
    static std::string toYAMLInternal(const std::vector<CJMODArg>& args);
    static std::string toStringInternal(const std::vector<CJMODArg>& args);
    static std::string toDebugStringInternal(const std::vector<CJMODArg>& args);
    
    // 反序列化内部方法
    static std::vector<CJMODArg> fromJSONInternal(const std::string& json);
    static std::vector<CJMODArg> fromXMLInternal(const std::string& xml);
    static std::vector<CJMODArg> fromYAMLInternal(const std::string& yaml);
    static std::vector<CJMODArg> fromStringInternal(const std::string& str);
    
    // 克隆内部方法
    static std::vector<CJMODArg> cloneInternal(const std::vector<CJMODArg>& args);
    static std::vector<CJMODArg> deepCloneInternal(const std::vector<CJMODArg>& args);
    
    // 比较内部方法
    static bool equalsInternal(const std::vector<CJMODArg>& args1, const std::vector<CJMODArg>& args2);
    static bool equalsInternal(const std::vector<CJMODArg>& args, const std::string& code);
    
    // 格式化内部方法
    static std::string formatInternal(const std::vector<CJMODArg>& args);
    static std::string minifyInternal(const std::vector<CJMODArg>& args);
    static std::string beautifyInternal(const std::vector<CJMODArg>& args);
};

} // namespace CHTL_JS

#endif // CJMOD_SCANNER_H