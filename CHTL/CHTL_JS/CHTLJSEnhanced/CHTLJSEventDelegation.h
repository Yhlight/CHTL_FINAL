#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <set>
#include <chrono>
#include <any>
#include "CHTLJSEnhancedListener.h"

namespace CHTL_JS {

// 委托策略枚举
enum class DelegationStrategy {
    NONE = 0,
    BUBBLE,         // 冒泡委托
    CAPTURE,        // 捕获委托
    DIRECT,         // 直接委托
    SELECTIVE,      // 选择性委托
    CONDITIONAL,    // 条件委托
    LAZY,           // 延迟委托
    EAGER           // 急切委托
};

// 委托选项结构
struct DelegationOptions {
    DelegationStrategy strategy = DelegationStrategy::BUBBLE;
    std::string rootSelector;   // 根选择器
    std::string targetSelector; // 目标选择器
    std::string condition;      // 委托条件
    bool stopPropagation = false;
    bool preventDefault = false;
    int maxDepth = -1;          // 最大深度
    int timeout = 0;            // 超时时间
    bool lazy = false;          // 延迟加载
    bool cache = true;          // 缓存结果
    std::map<std::string, std::any> data; // 自定义数据
};

// 委托事件信息
struct DelegationEventInfo {
    std::string eventType;
    std::any originalEvent;
    std::any target;
    std::any currentTarget;
    std::string rootSelector;
    std::string targetSelector;
    std::string condition;
    std::map<std::string, std::any> data;
    int depth;
    bool isDelegated;
    std::chrono::steady_clock::time_point timestamp;
};

// 委托处理器函数类型
using DelegationHandler = std::function<void(const DelegationEventInfo&)>;
using DelegationFilter = std::function<bool(const DelegationEventInfo&)>;
using DelegationTransformer = std::function<DelegationEventInfo(const DelegationEventInfo&)>;

// 事件委托类
class CHTLJSEventDelegation {
public:
    CHTLJSEventDelegation();
    CHTLJSEventDelegation(const std::string& rootSelector, const std::string& targetSelector, 
                          DelegationHandler handler, const DelegationOptions& options = DelegationOptions{});
    ~CHTLJSEventDelegation();
    
    // 基本属性
    void setRootSelector(const std::string& rootSelector);
    std::string getRootSelector() const;
    
    void setTargetSelector(const std::string& targetSelector);
    std::string getTargetSelector() const;
    
    void setHandler(DelegationHandler handler);
    DelegationHandler getHandler() const;
    
    void setOptions(const DelegationOptions& options);
    DelegationOptions getOptions() const;
    
    void setStrategy(DelegationStrategy strategy);
    DelegationStrategy getStrategy() const;
    
    void setCondition(const std::string& condition);
    std::string getCondition() const;
    
    // 选项管理
    void setStopPropagation(bool stop);
    bool shouldStopPropagation() const;
    
    void setPreventDefault(bool prevent);
    bool shouldPreventDefault() const;
    
    void setMaxDepth(int maxDepth);
    int getMaxDepth() const;
    
    void setTimeout(int timeout);
    int getTimeout() const;
    
    void setLazy(bool lazy);
    bool isLazy() const;
    
    void setCache(bool cache);
    bool isCache() const;
    
    // 数据管理
    void setData(const std::map<std::string, std::any>& data);
    std::map<std::string, std::any> getData() const;
    
    void addData(const std::string& key, const std::any& value);
    std::any getData(const std::string& key) const;
    bool hasData(const std::string& key) const;
    void removeData(const std::string& key);
    void clearData();
    
    // 过滤器管理
    void addFilter(DelegationFilter filter);
    void removeFilter(DelegationFilter filter);
    void clearFilters();
    std::vector<DelegationFilter> getFilters() const;
    
    // 转换器管理
    void addTransformer(DelegationTransformer transformer);
    void removeTransformer(DelegationTransformer transformer);
    void clearTransformers();
    std::vector<DelegationTransformer> getTransformers() const;
    
    // 状态管理
    void setActive(bool active);
    bool isActive() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setPaused(bool paused);
    bool isPaused() const;
    
    // 执行控制
    void execute(const DelegationEventInfo& eventInfo);
    
    bool canExecute() const;
    bool shouldExecute(const DelegationEventInfo& eventInfo) const;
    
    // 验证
    bool isValid() const;
    bool isComplete() const;
    std::vector<std::string> validate() const;
    
    // 比较
    bool equals(std::shared_ptr<CHTLJSEventDelegation> other) const;
    bool equals(const CHTLJSEventDelegation& other) const;
    
    // 克隆
    std::shared_ptr<CHTLJSEventDelegation> clone() const;
    std::shared_ptr<CHTLJSEventDelegation> deepClone() const;
    
    // 转换
    std::string toJavaScript() const;
    std::string toCSS() const;
    std::string toCHTLJS() const;
    std::string toString() const;
    std::string toDebugString() const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;
    
    // 统计
    int getExecutionCount() const;
    std::chrono::milliseconds getTotalExecutionTime() const;
    std::chrono::milliseconds getAverageExecutionTime() const;
    std::chrono::milliseconds getLastExecutionTime() const;
    
    // 重置
    void reset();
    void resetStats();
    
private:
    std::string rootSelector;
    std::string targetSelector;
    DelegationHandler handler;
    DelegationOptions options;
    std::vector<DelegationFilter> filters;
    std::vector<DelegationTransformer> transformers;
    std::map<std::string, std::any> data;
    
    bool active;
    bool enabled;
    bool paused;
    
    int executionCount;
    std::chrono::milliseconds totalExecutionTime;
    std::chrono::milliseconds lastExecutionTime;
    std::chrono::steady_clock::time_point lastExecuted;
    
    // 辅助方法
    bool validateDelegation() const;
    bool validateHandler() const;
    bool validateOptions() const;
    bool validateFilters() const;
    bool validateTransformers() const;
    bool validateData() const;
    
    bool compareDelegation(std::shared_ptr<CHTLJSEventDelegation> other) const;
    bool compareDelegation(const CHTLJSEventDelegation& other) const;
    bool compareHandler(std::shared_ptr<CHTLJSEventDelegation> other) const;
    bool compareHandler(const CHTLJSEventDelegation& other) const;
    bool compareOptions(std::shared_ptr<CHTLJSEventDelegation> other) const;
    bool compareOptions(const CHTLJSEventDelegation& other) const;
    bool compareFilters(std::shared_ptr<CHTLJSEventDelegation> other) const;
    bool compareFilters(const CHTLJSEventDelegation& other) const;
    bool compareTransformers(std::shared_ptr<CHTLJSEventDelegation> other) const;
    bool compareTransformers(const CHTLJSEventDelegation& other) const;
    bool compareData(std::shared_ptr<CHTLJSEventDelegation> other) const;
    bool compareData(const CHTLJSEventDelegation& other) const;
    
    void copyTo(std::shared_ptr<CHTLJSEventDelegation> target) const;
    void deepCopyTo(std::shared_ptr<CHTLJSEventDelegation> target) const;
    
    std::string generateJavaScript() const;
    std::string generateCSS() const;
    std::string generateCHTLJS() const;
    
    std::string formatDelegation() const;
    std::string minifyDelegation() const;
    std::string beautifyDelegation() const;
    
    // 执行辅助方法
    bool checkCondition(const DelegationEventInfo& eventInfo) const;
    bool checkFilters(const DelegationEventInfo& eventInfo) const;
    bool checkDepth(const DelegationEventInfo& eventInfo) const;
    bool checkTimeout(const DelegationEventInfo& eventInfo) const;
    
    DelegationEventInfo transformEvent(const DelegationEventInfo& eventInfo) const;
    void executeHandler(const DelegationEventInfo& eventInfo);
    
    void updateExecutionTime(std::chrono::milliseconds duration);
};

// 事件委托管理器
class CHTLJSEventDelegationManager {
public:
    CHTLJSEventDelegationManager();
    ~CHTLJSEventDelegationManager();
    
    // 委托管理
    void addDelegation(std::shared_ptr<CHTLJSEventDelegation> delegation);
    void removeDelegation(std::shared_ptr<CHTLJSEventDelegation> delegation);
    void removeDelegation(const std::string& rootSelector);
    void removeDelegation(const std::string& rootSelector, const std::string& targetSelector);
    void clearDelegations();
    void clearDelegations(const std::string& rootSelector);
    
    // 查找委托
    std::vector<std::shared_ptr<CHTLJSEventDelegation>> getDelegations() const;
    std::vector<std::shared_ptr<CHTLJSEventDelegation>> getDelegations(const std::string& rootSelector) const;
    std::vector<std::shared_ptr<CHTLJSEventDelegation>> getDelegations(const std::string& rootSelector, const std::string& targetSelector) const;
    
    std::shared_ptr<CHTLJSEventDelegation> getFirstDelegation(const std::string& rootSelector) const;
    std::shared_ptr<CHTLJSEventDelegation> getFirstDelegation(const std::string& rootSelector, const std::string& targetSelector) const;
    
    // 事件处理
    void handleEvent(const std::string& eventType, const std::any& event, const std::any& target);
    void handleEvent(const DelegationEventInfo& eventInfo);
    
    // 批量操作
    void handleAllEvents(const std::map<std::string, std::vector<std::any>>& events);
    void pauseAll();
    void resumeAll();
    void enableAll();
    void disableAll();
    
    // 状态管理
    void setActive(bool active);
    bool isActive() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setPaused(bool paused);
    bool isPaused() const;
    
    // 验证
    bool isValid() const;
    std::vector<std::string> validate() const;
    
    // 统计
    size_t getDelegationCount() const;
    size_t getDelegationCount(const std::string& rootSelector) const;
    
    int getTotalExecutionCount() const;
    std::chrono::milliseconds getTotalExecutionTime() const;
    std::chrono::milliseconds getAverageExecutionTime() const;
    
    // 重置
    void reset();
    void resetStats();
    
    // 转换
    std::string toJavaScript() const;
    std::string toCSS() const;
    std::string toCHTLJS() const;
    std::string toString() const;
    std::string toDebugString() const;
    
    // 格式化
    std::string format() const;
    std::string minify() const;
    std::string beautify() const;
    
private:
    std::vector<std::shared_ptr<CHTLJSEventDelegation>> delegations;
    std::map<std::string, std::vector<std::shared_ptr<CHTLJSEventDelegation>>> delegationsByRoot;
    std::map<std::string, std::vector<std::shared_ptr<CHTLJSEventDelegation>>> delegationsByTarget;
    
    bool active;
    bool enabled;
    bool paused;
    
    int totalExecutionCount;
    std::chrono::milliseconds totalExecutionTime;
    
    // 辅助方法
    bool validateManager() const;
    bool validateDelegations() const;
    
    void updateDelegationIndex(std::shared_ptr<CHTLJSEventDelegation> delegation);
    void removeDelegationFromIndex(std::shared_ptr<CHTLJSEventDelegation> delegation);
    
    std::string generateJavaScript() const;
    std::string generateCSS() const;
    std::string generateCHTLJS() const;
    
    std::string formatManager() const;
    std::string minifyManager() const;
    std::string beautifyManager() const;
    
    void updateStats();
    
    // 事件处理辅助方法
    DelegationEventInfo createEventInfo(const std::string& eventType, const std::any& event, const std::any& target) const;
    std::vector<std::shared_ptr<CHTLJSEventDelegation>> findMatchingDelegations(const DelegationEventInfo& eventInfo) const;
    bool matchesSelector(const std::string& selector, const std::any& target) const;
    bool matchesCondition(const std::string& condition, const DelegationEventInfo& eventInfo) const;
};

} // namespace CHTL_JS