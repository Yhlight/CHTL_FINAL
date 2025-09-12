#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace CHTL {

/**
 * @brief CJMOD API类型枚举
 */
enum class CJMODAPIType {
    VIR,            // 虚拟元素
    LISTEN,         // 事件监听
    ANIMATE,        // 动画
    ROUTER,         // 路由
    DELEGATE,       // 委托
    FILELOADER,     // 文件加载器
    UNKNOWN         // 未知类型
};

/**
 * @brief CJMOD API参数类型
 */
enum class CJMODAPIParamType {
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    OBJECT,         // 对象
    ARRAY,          // 数组
    FUNCTION,       // 函数
    UNKNOWN         // 未知类型
};

/**
 * @brief CJMOD API参数
 */
struct CJMODAPIParam {
    std::string name;
    CJMODAPIParamType type;
    std::string defaultValue;
    bool required;
    std::string description;
    
    CJMODAPIParam(const std::string& n, CJMODAPIParamType t, const std::string& def = "", bool req = false, const std::string& desc = "")
        : name(n), type(t), defaultValue(def), required(req), description(desc) {}
};

/**
 * @brief CJMOD API函数
 */
struct CJMODAPIFunction {
    std::string name;
    CJMODAPIType type;
    std::vector<CJMODAPIParam> parameters;
    std::string returnType;
    std::string description;
    std::function<std::string(const std::vector<std::string>&)> handler;
    
    CJMODAPIFunction(const std::string& n, CJMODAPIType t, const std::string& desc = "")
        : name(n), type(t), description(desc) {}
};

/**
 * @brief CJMOD API类
 * 
 * 提供CHTL JS语法扩展能力
 */
class CJMODAPI {
public:
    /**
     * @brief 构造函数
     */
    CJMODAPI();
    
    /**
     * @brief 析构函数
     */
    ~CJMODAPI();
    
    /**
     * @brief 注册API函数
     * @param func API函数
     * @return 是否注册成功
     */
    bool registerFunction(const CJMODAPIFunction& func);
    
    /**
     * @brief 获取API函数
     * @param name 函数名称
     * @return API函数指针
     */
    const CJMODAPIFunction* getFunction(const std::string& name) const;
    
    /**
     * @brief 检查API函数是否存在
     * @param name 函数名称
     * @return 是否存在
     */
    bool hasFunction(const std::string& name) const;
    
    /**
     * @brief 获取所有API函数
     * @return API函数列表
     */
    const std::vector<CJMODAPIFunction>& getAllFunctions() const;
    
    /**
     * @brief 获取指定类型的API函数
     * @param type API类型
     * @return API函数列表
     */
    std::vector<CJMODAPIFunction> getFunctionsByType(CJMODAPIType type) const;
    
    /**
     * @brief 调用API函数
     * @param name 函数名称
     * @param args 参数列表
     * @return 调用结果
     */
    std::string callFunction(const std::string& name, const std::vector<std::string>& args);
    
    /**
     * @brief 验证API函数参数
     * @param name 函数名称
     * @param args 参数列表
     * @return 验证结果
     */
    bool validateParameters(const std::string& name, const std::vector<std::string>& args) const;
    
    /**
     * @brief 获取API文档
     * @return API文档字符串
     */
    std::string getAPIDocumentation() const;
    
    /**
     * @brief 获取指定类型的API文档
     * @param type API类型
     * @return API文档字符串
     */
    std::string getAPIDocumentation(CJMODAPIType type) const;
    
    /**
     * @brief 清除所有API函数
     */
    void clear();
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 初始化默认API函数
     */
    void initializeDefaultFunctions();
    
    /**
     * @brief 添加VIR API函数
     */
    void addVIRFunctions();
    
    /**
     * @brief 添加LISTEN API函数
     */
    void addLISTENFunctions();
    
    /**
     * @brief 添加ANIMATE API函数
     */
    void addANIMATEFunctions();
    
    /**
     * @brief 添加ROUTER API函数
     */
    void addROUTERFunctions();
    
    /**
     * @brief 添加DELEGATE API函数
     */
    void addDELEGATEFunctions();
    
    /**
     * @brief 添加FILELOADER API函数
     */
    void addFILELOADERFunctions();
    
    /**
     * @brief 获取参数类型名称
     * @param type 参数类型
     * @return 类型名称
     */
    static std::string getParamTypeName(CJMODAPIParamType type);
    
    /**
     * @brief 获取API类型名称
     * @param type API类型
     * @return 类型名称
     */
    static std::string getAPITypeName(CJMODAPIType type);

private:
    std::vector<CJMODAPIFunction> m_functions;
    std::unordered_map<std::string, size_t> m_functionMap;
    bool m_debugMode;
    
    // 统计信息
    size_t m_totalFunctions;
    size_t m_functionCalls;
    size_t m_successfulCalls;
    size_t m_failedCalls;
};

} // namespace CHTL