#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <set>
#include <chrono>
#include <any>

namespace CHTL_JS {

// 事件类型枚举
enum class EventType {
    UNKNOWN = 0,
    CLICK,
    DOUBLE_CLICK,
    MOUSE_OVER,
    MOUSE_OUT,
    MOUSE_ENTER,
    MOUSE_LEAVE,
    MOUSE_DOWN,
    MOUSE_UP,
    MOUSE_MOVE,
    KEY_DOWN,
    KEY_UP,
    KEY_PRESS,
    FOCUS,
    BLUR,
    CHANGE,
    INPUT,
    SUBMIT,
    RESET,
    LOAD,
    UNLOAD,
    RESIZE,
    SCROLL,
    CUSTOM
};

// 事件修饰符枚举
enum class EventModifier {
    NONE = 0,
    ONCE,           // 只触发一次
    PREVENT,        // 阻止默认行为
    STOP,           // 停止事件冒泡
    CAPTURE,        // 捕获阶段
    PASSIVE,        // 被动监听
    DELEGATE,       // 事件委托
    THROTTLE,       // 节流
    DEBOUNCE,       // 防抖
    IMMEDIATE,      // 立即执行
    LAZY            // 延迟执行
};

// 事件选项结构
struct EventOptions {
    bool once = false;
    bool preventDefault = false;
    bool stopPropagation = false;
    bool capture = false;
    bool passive = false;
    bool delegate = false;
    int throttle = 0;           // 节流间隔(ms)
    int debounce = 0;           // 防抖间隔(ms)
    bool immediate = false;     // 立即执行
    bool lazy = false;          // 延迟执行
    int delay = 0;              // 延迟时间(ms)
    int timeout = 0;            // 超时时间(ms)
    int retry = 0;              // 重试次数
    std::string selector;       // 委托选择器
    std::string condition;      // 执行条件
    std::string namespace_;     // 命名空间
    std::map<std::string, std::any> data; // 自定义数据
};

// 事件处理器函数类型
using EventHandler = std::function<void(const std::string&, const std::map<std::string, std::any>&)>;
using EventHandlerWithEvent = std::function<void(const std::string&, const std::map<std::string, std::any>&, const std::any&)>;
using EventHandlerWithTarget = std::function<void(const std::string&, const std::map<std::string, std::any>&, const std::any&, const std::any&)>;

// 事件处理器包装器
class EventHandlerWrapper {
public:
    EventHandlerWrapper() = default;
    EventHandlerWrapper(EventHandler handler);
    EventHandlerWrapper(EventHandlerWithEvent handler);
    EventHandlerWrapper(EventHandlerWithTarget handler);
    
    void operator()(const std::string& eventType, const std::map<std::string, std::any>& data, 
                   const std::any& event = std::any{}, const std::any& target = std::any{}) const;
    
    bool isValid() const;
    std::string getType() const;
    
private:
    std::function<void(const std::string&, const std::map<std::string, std::any>&, const std::any&, const std::any&)> handler;
    std::string type;
};

// 事件监听器类
class CHTLJSEnhancedListener {
public:
    CHTLJSEnhancedListener();
    CHTLJSEnhancedListener(const std::string& eventType, EventHandlerWrapper handler, const EventOptions& options = EventOptions{});
    CHTLJSEnhancedListener(EventType eventType, EventHandlerWrapper handler, const EventOptions& options = EventOptions{});
    ~CHTLJSEnhancedListener();
    
    // 基本属性
    void setEventType(const std::string& eventType);
    void setEventType(EventType eventType);
    std::string getEventType() const;
    EventType getEventTypeEnum() const;
    
    void setHandler(EventHandlerWrapper handler);
    EventHandlerWrapper getHandler() const;
    
    void setOptions(const EventOptions& options);
    EventOptions getOptions() const;
    
    void setSelector(const std::string& selector);
    std::string getSelector() const;
    
    void setTarget(const std::string& target);
    std::string getTarget() const;
    
    void setCondition(const std::string& condition);
    std::string getCondition() const;
    
    void setNamespace(const std::string& namespace_);
    std::string getNamespace() const;
    
    // 修饰符管理
    void addModifier(EventModifier modifier);
    void removeModifier(EventModifier modifier);
    void clearModifiers();
    bool hasModifier(EventModifier modifier) const;
    std::vector<EventModifier> getModifiers() const;
    
    // 选项管理
    void setOnce(bool once);
    bool isOnce() const;
    
    void setPreventDefault(bool prevent);
    bool shouldPreventDefault() const;
    
    void setStopPropagation(bool stop);
    bool shouldStopPropagation() const;
    
    void setCapture(bool capture);
    bool isCapture() const;
    
    void setPassive(bool passive);
    bool isPassive() const;
    
    void setDelegate(bool delegate);
    bool isDelegate() const;
    
    void setThrottle(int throttle);
    int getThrottle() const;
    
    void setDebounce(int debounce);
    int getDebounce() const;
    
    void setImmediate(bool immediate);
    bool isImmediate() const;
    
    void setLazy(bool lazy);
    bool isLazy() const;
    
    void setDelay(int delay);
    int getDelay() const;
    
    void setTimeout(int timeout);
    int getTimeout() const;
    
    void setRetry(int retry);
    int getRetry() const;
    
    // 数据管理
    void setData(const std::map<std::string, std::any>& data);
    std::map<std::string, std::any> getData() const;
    
    void addData(const std::string& key, const std::any& value);
    std::any getData(const std::string& key) const;
    bool hasData(const std::string& key) const;
    void removeData(const std::string& key);
    void clearData();
    
    // 状态管理
    void setActive(bool active);
    bool isActive() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    void setPaused(bool paused);
    bool isPaused() const;
    
    void setExecuted(bool executed);
    bool isExecuted() const;
    
    // 执行控制
    void execute(const std::string& eventType, const std::map<std::string, std::any>& data, 
                const std::any& event = std::any{}, const std::any& target = std::any{});
    
    bool canExecute() const;
    bool shouldExecute(const std::string& eventType, const std::map<std::string, std::any>& data) const;
    
    // 时间管理
    void setLastExecuted(std::chrono::steady_clock::time_point time);
    std::chrono::steady_clock::time_point getLastExecuted() const;
    
    void setNextExecution(std::chrono::steady_clock::time_point time);
    std::chrono::steady_clock::time_point getNextExecution() const;
    
    bool isThrottled() const;
    bool isDebounced() const;
    bool isDelayed() const;
    bool isTimedOut() const;
    
    // 验证
    bool isValid() const;
    bool isComplete() const;
    std::vector<std::string> validate() const;
    
    // 比较
    bool equals(std::shared_ptr<CHTLJSEnhancedListener> other) const;
    bool equals(const CHTLJSEnhancedListener& other) const;
    
    // 克隆
    std::shared_ptr<CHTLJSEnhancedListener> clone() const;
    std::shared_ptr<CHTLJSEnhancedListener> deepClone() const;
    
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
    std::string eventType;
    EventType eventTypeEnum;
    EventHandlerWrapper handler;
    EventOptions options;
    std::vector<EventModifier> modifiers;
    std::map<std::string, std::any> data;
    
    bool active;
    bool enabled;
    bool paused;
    bool executed;
    
    std::chrono::steady_clock::time_point lastExecuted;
    std::chrono::steady_clock::time_point nextExecution;
    
    int executionCount;
    std::chrono::milliseconds totalExecutionTime;
    std::chrono::milliseconds lastExecutionTime;
    
    // 辅助方法
    bool validateListener() const;
    bool validateHandler() const;
    bool validateOptions() const;
    bool validateModifiers() const;
    bool validateData() const;
    
    bool compareListener(std::shared_ptr<CHTLJSEnhancedListener> other) const;
    bool compareListener(const CHTLJSEnhancedListener& other) const;
    bool compareHandler(std::shared_ptr<CHTLJSEnhancedListener> other) const;
    bool compareHandler(const CHTLJSEnhancedListener& other) const;
    bool compareOptions(std::shared_ptr<CHTLJSEnhancedListener> other) const;
    bool compareOptions(const CHTLJSEnhancedListener& other) const;
    bool compareModifiers(std::shared_ptr<CHTLJSEnhancedListener> other) const;
    bool compareModifiers(const CHTLJSEnhancedListener& other) const;
    bool compareData(std::shared_ptr<CHTLJSEnhancedListener> other) const;
    bool compareData(const CHTLJSEnhancedListener& other) const;
    
    void copyTo(std::shared_ptr<CHTLJSEnhancedListener> target) const;
    void deepCopyTo(std::shared_ptr<CHTLJSEnhancedListener> target) const;
    
    std::string generateJavaScript() const;
    std::string generateCSS() const;
    std::string generateCHTLJS() const;
    
    std::string formatListener() const;
    std::string minifyListener() const;
    std::string beautifyListener() const;
    
    // 时间辅助方法
    bool checkThrottle() const;
    bool checkDebounce() const;
    bool checkDelay() const;
    bool checkTimeout() const;
    
    void updateExecutionTime(std::chrono::milliseconds duration);
    void updateNextExecution();
    
    // 执行辅助方法
    bool checkCondition(const std::string& eventType, const std::map<std::string, std::any>& data) const;
    bool checkModifiers() const;
    bool checkTiming() const;
    
    void executeHandler(const std::string& eventType, const std::map<std::string, std::any>& data, 
                       const std::any& event, const std::any& target);
    void executeWithThrottle(const std::string& eventType, const std::map<std::string, std::any>& data, 
                            const std::any& event, const std::any& target);
    void executeWithDebounce(const std::string& eventType, const std::map<std::string, std::any>& data, 
                            const std::any& event, const std::any& target);
    void executeWithDelay(const std::string& eventType, const std::map<std::string, std::any>& data, 
                         const std::any& event, const std::any& target);
    void executeImmediate(const std::string& eventType, const std::map<std::string, std::any>& data, 
                         const std::any& event, const std::any& target);
    void executeLazy(const std::string& eventType, const std::map<std::string, std::any>& data, 
                    const std::any& event, const std::any& target);
};

// 事件监听器管理器
class CHTLJSEnhancedListenerManager {
public:
    CHTLJSEnhancedListenerManager();
    ~CHTLJSEnhancedListenerManager();
    
    // 监听器管理
    void addListener(std::shared_ptr<CHTLJSEnhancedListener> listener);
    void removeListener(std::shared_ptr<CHTLJSEnhancedListener> listener);
    void removeListener(const std::string& eventType);
    void removeListener(const std::string& eventType, const std::string& namespace_);
    void clearListeners();
    void clearListeners(const std::string& eventType);
    
    // 查找监听器
    std::vector<std::shared_ptr<CHTLJSEnhancedListener>> getListeners() const;
    std::vector<std::shared_ptr<CHTLJSEnhancedListener>> getListeners(const std::string& eventType) const;
    std::vector<std::shared_ptr<CHTLJSEnhancedListener>> getListeners(const std::string& eventType, const std::string& namespace_) const;
    std::vector<std::shared_ptr<CHTLJSEnhancedListener>> getListeners(EventType eventType) const;
    std::vector<std::shared_ptr<CHTLJSEnhancedListener>> getListeners(EventType eventType, const std::string& namespace_) const;
    
    std::shared_ptr<CHTLJSEnhancedListener> getFirstListener(const std::string& eventType) const;
    std::shared_ptr<CHTLJSEnhancedListener> getFirstListener(const std::string& eventType, const std::string& namespace_) const;
    std::shared_ptr<CHTLJSEnhancedListener> getFirstListener(EventType eventType) const;
    std::shared_ptr<CHTLJSEnhancedListener> getFirstListener(EventType eventType, const std::string& namespace_) const;
    
    // 事件触发
    void trigger(const std::string& eventType, const std::map<std::string, std::any>& data = {}, 
                const std::any& event = std::any{}, const std::any& target = std::any{});
    void trigger(EventType eventType, const std::map<std::string, std::any>& data = {}, 
                const std::any& event = std::any{}, const std::any& target = std::any{});
    
    // 批量操作
    void triggerAll(const std::map<std::string, std::map<std::string, std::any>>& events);
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
    size_t getListenerCount() const;
    size_t getListenerCount(const std::string& eventType) const;
    size_t getListenerCount(EventType eventType) const;
    
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
    std::vector<std::shared_ptr<CHTLJSEnhancedListener>> listeners;
    std::map<std::string, std::vector<std::shared_ptr<CHTLJSEnhancedListener>>> listenersByType;
    std::map<std::string, std::vector<std::shared_ptr<CHTLJSEnhancedListener>>> listenersByNamespace;
    
    bool active;
    bool enabled;
    bool paused;
    
    int totalExecutionCount;
    std::chrono::milliseconds totalExecutionTime;
    
    // 辅助方法
    bool validateManager() const;
    bool validateListeners() const;
    
    void updateListenerIndex(std::shared_ptr<CHTLJSEnhancedListener> listener);
    void removeListenerFromIndex(std::shared_ptr<CHTLJSEnhancedListener> listener);
    
    std::string generateJavaScript() const;
    std::string generateCSS() const;
    std::string generateCHTLJS() const;
    
    std::string formatManager() const;
    std::string minifyManager() const;
    std::string beautifyManager() const;
    
    void updateStats();
};

} // namespace CHTL_JS