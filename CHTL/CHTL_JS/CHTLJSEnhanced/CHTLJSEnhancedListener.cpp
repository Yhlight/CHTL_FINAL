#include "CHTLJSEnhancedListener.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <thread>
#include <future>

namespace CHTL_JS {

// EventHandlerWrapper 实现
EventHandlerWrapper::EventHandlerWrapper(EventHandler handler) 
    : handler([handler](const std::string& eventType, const std::map<std::string, std::any>& data, 
                       const std::any& event, const std::any& target) {
        handler(eventType, data);
    }), type("basic") {
}

EventHandlerWrapper::EventHandlerWrapper(EventHandlerWithEvent handler) 
    : handler([handler](const std::string& eventType, const std::map<std::string, std::any>& data, 
                       const std::any& event, const std::any& target) {
        handler(eventType, data, event);
    }), type("with_event") {
}

EventHandlerWrapper::EventHandlerWrapper(EventHandlerWithTarget handler) 
    : handler(handler), type("with_target") {
}

void EventHandlerWrapper::operator()(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                    const std::any& event, const std::any& target) const {
    if (handler) {
        handler(eventType, data, event, target);
    }
}

bool EventHandlerWrapper::isValid() const {
    return static_cast<bool>(handler);
}

std::string EventHandlerWrapper::getType() const {
    return type;
}

// CHTLJSEnhancedListener 实现
CHTLJSEnhancedListener::CHTLJSEnhancedListener() 
    : eventType(""), eventTypeEnum(EventType::UNKNOWN), active(true), enabled(true), 
      paused(false), executed(false), executionCount(0), totalExecutionTime(0), lastExecutionTime(0) {
    lastExecuted = std::chrono::steady_clock::now();
    nextExecution = lastExecuted;
}

CHTLJSEnhancedListener::CHTLJSEnhancedListener(const std::string& eventType, EventHandlerWrapper handler, const EventOptions& options) 
    : eventType(eventType), eventTypeEnum(EventType::UNKNOWN), handler(handler), options(options),
      active(true), enabled(true), paused(false), executed(false), executionCount(0), 
      totalExecutionTime(0), lastExecutionTime(0) {
    lastExecuted = std::chrono::steady_clock::now();
    nextExecution = lastExecuted;
}

CHTLJSEnhancedListener::CHTLJSEnhancedListener(EventType eventType, EventHandlerWrapper handler, const EventOptions& options) 
    : eventType(""), eventTypeEnum(eventType), handler(handler), options(options),
      active(true), enabled(true), paused(false), executed(false), executionCount(0), 
      totalExecutionTime(0), lastExecutionTime(0) {
    lastExecuted = std::chrono::steady_clock::now();
    nextExecution = lastExecuted;
}

CHTLJSEnhancedListener::~CHTLJSEnhancedListener() = default;

// 基本属性实现
void CHTLJSEnhancedListener::setEventType(const std::string& eventType) {
    this->eventType = eventType;
    this->eventTypeEnum = EventType::UNKNOWN;
}

void CHTLJSEnhancedListener::setEventType(EventType eventType) {
    this->eventTypeEnum = eventType;
    this->eventType = "";
}

std::string CHTLJSEnhancedListener::getEventType() const {
    return eventType;
}

EventType CHTLJSEnhancedListener::getEventTypeEnum() const {
    return eventTypeEnum;
}

void CHTLJSEnhancedListener::setHandler(EventHandlerWrapper handler) {
    this->handler = handler;
}

EventHandlerWrapper CHTLJSEnhancedListener::getHandler() const {
    return handler;
}

void CHTLJSEnhancedListener::setOptions(const EventOptions& options) {
    this->options = options;
}

EventOptions CHTLJSEnhancedListener::getOptions() const {
    return options;
}

void CHTLJSEnhancedListener::setSelector(const std::string& selector) {
    options.selector = selector;
}

std::string CHTLJSEnhancedListener::getSelector() const {
    return options.selector;
}

void CHTLJSEnhancedListener::setTarget(const std::string& target) {
    // 简化的目标设置
}

std::string CHTLJSEnhancedListener::getTarget() const {
    return "";
}

void CHTLJSEnhancedListener::setCondition(const std::string& condition) {
    options.condition = condition;
}

std::string CHTLJSEnhancedListener::getCondition() const {
    return options.condition;
}

void CHTLJSEnhancedListener::setNamespace(const std::string& namespace_) {
    options.namespace_ = namespace_;
}

std::string CHTLJSEnhancedListener::getNamespace() const {
    return options.namespace_;
}

// 修饰符管理实现
void CHTLJSEnhancedListener::addModifier(EventModifier modifier) {
    if (std::find(modifiers.begin(), modifiers.end(), modifier) == modifiers.end()) {
        modifiers.push_back(modifier);
    }
}

void CHTLJSEnhancedListener::removeModifier(EventModifier modifier) {
    modifiers.erase(std::remove(modifiers.begin(), modifiers.end(), modifier), modifiers.end());
}

void CHTLJSEnhancedListener::clearModifiers() {
    modifiers.clear();
}

bool CHTLJSEnhancedListener::hasModifier(EventModifier modifier) const {
    return std::find(modifiers.begin(), modifiers.end(), modifier) != modifiers.end();
}

std::vector<EventModifier> CHTLJSEnhancedListener::getModifiers() const {
    return modifiers;
}

// 选项管理实现
void CHTLJSEnhancedListener::setOnce(bool once) {
    options.once = once;
}

bool CHTLJSEnhancedListener::isOnce() const {
    return options.once;
}

void CHTLJSEnhancedListener::setPreventDefault(bool prevent) {
    options.preventDefault = prevent;
}

bool CHTLJSEnhancedListener::shouldPreventDefault() const {
    return options.preventDefault;
}

void CHTLJSEnhancedListener::setStopPropagation(bool stop) {
    options.stopPropagation = stop;
}

bool CHTLJSEnhancedListener::shouldStopPropagation() const {
    return options.stopPropagation;
}

void CHTLJSEnhancedListener::setCapture(bool capture) {
    options.capture = capture;
}

bool CHTLJSEnhancedListener::isCapture() const {
    return options.capture;
}

void CHTLJSEnhancedListener::setPassive(bool passive) {
    options.passive = passive;
}

bool CHTLJSEnhancedListener::isPassive() const {
    return options.passive;
}

void CHTLJSEnhancedListener::setDelegate(bool delegate) {
    options.delegate = delegate;
}

bool CHTLJSEnhancedListener::isDelegate() const {
    return options.delegate;
}

void CHTLJSEnhancedListener::setThrottle(int throttle) {
    options.throttle = throttle;
}

int CHTLJSEnhancedListener::getThrottle() const {
    return options.throttle;
}

void CHTLJSEnhancedListener::setDebounce(int debounce) {
    options.debounce = debounce;
}

int CHTLJSEnhancedListener::getDebounce() const {
    return options.debounce;
}

void CHTLJSEnhancedListener::setImmediate(bool immediate) {
    options.immediate = immediate;
}

bool CHTLJSEnhancedListener::isImmediate() const {
    return options.immediate;
}

void CHTLJSEnhancedListener::setLazy(bool lazy) {
    options.lazy = lazy;
}

bool CHTLJSEnhancedListener::isLazy() const {
    return options.lazy;
}

void CHTLJSEnhancedListener::setDelay(int delay) {
    options.delay = delay;
}

int CHTLJSEnhancedListener::getDelay() const {
    return options.delay;
}

void CHTLJSEnhancedListener::setTimeout(int timeout) {
    options.timeout = timeout;
}

int CHTLJSEnhancedListener::getTimeout() const {
    return options.timeout;
}

void CHTLJSEnhancedListener::setRetry(int retry) {
    options.retry = retry;
}

int CHTLJSEnhancedListener::getRetry() const {
    return options.retry;
}

// 数据管理实现
void CHTLJSEnhancedListener::setData(const std::map<std::string, std::any>& data) {
    this->data = data;
}

std::map<std::string, std::any> CHTLJSEnhancedListener::getData() const {
    return data;
}

void CHTLJSEnhancedListener::addData(const std::string& key, const std::any& value) {
    data[key] = value;
}

std::any CHTLJSEnhancedListener::getData(const std::string& key) const {
    auto it = data.find(key);
    return it != data.end() ? it->second : std::any{};
}

bool CHTLJSEnhancedListener::hasData(const std::string& key) const {
    return data.find(key) != data.end();
}

void CHTLJSEnhancedListener::removeData(const std::string& key) {
    data.erase(key);
}

void CHTLJSEnhancedListener::clearData() {
    data.clear();
}

// 状态管理实现
void CHTLJSEnhancedListener::setActive(bool active) {
    this->active = active;
}

bool CHTLJSEnhancedListener::isActive() const {
    return active;
}

void CHTLJSEnhancedListener::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool CHTLJSEnhancedListener::isEnabled() const {
    return enabled;
}

void CHTLJSEnhancedListener::setPaused(bool paused) {
    this->paused = paused;
}

bool CHTLJSEnhancedListener::isPaused() const {
    return paused;
}

void CHTLJSEnhancedListener::setExecuted(bool executed) {
    this->executed = executed;
}

bool CHTLJSEnhancedListener::isExecuted() const {
    return executed;
}

// 执行控制实现
void CHTLJSEnhancedListener::execute(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                    const std::any& event, const std::any& target) {
    if (!canExecute()) {
        return;
    }
    
    auto start = std::chrono::steady_clock::now();
    
    if (hasModifier(EventModifier::THROTTLE)) {
        executeWithThrottle(eventType, data, event, target);
    } else if (hasModifier(EventModifier::DEBOUNCE)) {
        executeWithDebounce(eventType, data, event, target);
    } else if (hasModifier(EventModifier::IMMEDIATE)) {
        executeImmediate(eventType, data, event, target);
    } else if (hasModifier(EventModifier::LAZY)) {
        executeLazy(eventType, data, event, target);
    } else if (options.delay > 0) {
        executeWithDelay(eventType, data, event, target);
    } else {
        executeHandler(eventType, data, event, target);
    }
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    updateExecutionTime(duration);
    executionCount++;
    executed = true;
    lastExecuted = end;
    
    if (options.once) {
        active = false;
    }
}

bool CHTLJSEnhancedListener::canExecute() const {
    return active && enabled && !paused && handler.isValid() && checkTiming();
}

bool CHTLJSEnhancedListener::shouldExecute(const std::string& eventType, const std::map<std::string, std::any>& data) const {
    return canExecute() && checkCondition(eventType, data) && checkModifiers();
}

// 时间管理实现
void CHTLJSEnhancedListener::setLastExecuted(std::chrono::steady_clock::time_point time) {
    lastExecuted = time;
}

std::chrono::steady_clock::time_point CHTLJSEnhancedListener::getLastExecuted() const {
    return lastExecuted;
}

void CHTLJSEnhancedListener::setNextExecution(std::chrono::steady_clock::time_point time) {
    nextExecution = time;
}

std::chrono::steady_clock::time_point CHTLJSEnhancedListener::getNextExecution() const {
    return nextExecution;
}

bool CHTLJSEnhancedListener::isThrottled() const {
    return checkThrottle();
}

bool CHTLJSEnhancedListener::isDebounced() const {
    return checkDebounce();
}

bool CHTLJSEnhancedListener::isDelayed() const {
    return checkDelay();
}

bool CHTLJSEnhancedListener::isTimedOut() const {
    return checkTimeout();
}

// 验证实现
bool CHTLJSEnhancedListener::isValid() const {
    return validateListener() && validateHandler() && validateOptions() && validateModifiers() && validateData();
}

bool CHTLJSEnhancedListener::isComplete() const {
    return !eventType.empty() && handler.isValid();
}

std::vector<std::string> CHTLJSEnhancedListener::validate() const {
    std::vector<std::string> errors;
    
    if (!validateListener()) {
        errors.push_back("Listener validation failed");
    }
    
    if (!validateHandler()) {
        errors.push_back("Handler validation failed");
    }
    
    if (!validateOptions()) {
        errors.push_back("Options validation failed");
    }
    
    if (!validateModifiers()) {
        errors.push_back("Modifiers validation failed");
    }
    
    if (!validateData()) {
        errors.push_back("Data validation failed");
    }
    
    return errors;
}

// 比较实现
bool CHTLJSEnhancedListener::equals(std::shared_ptr<CHTLJSEnhancedListener> other) const {
    return other && compareListener(other);
}

bool CHTLJSEnhancedListener::equals(const CHTLJSEnhancedListener& other) const {
    return compareListener(other);
}

// 克隆实现
std::shared_ptr<CHTLJSEnhancedListener> CHTLJSEnhancedListener::clone() const {
    auto cloned = std::make_shared<CHTLJSEnhancedListener>();
    copyTo(cloned);
    return cloned;
}

std::shared_ptr<CHTLJSEnhancedListener> CHTLJSEnhancedListener::deepClone() const {
    auto cloned = std::make_shared<CHTLJSEnhancedListener>();
    deepCopyTo(cloned);
    return cloned;
}

// 转换实现
std::string CHTLJSEnhancedListener::toJavaScript() const {
    return generateJavaScript();
}

std::string CHTLJSEnhancedListener::toCSS() const {
    return generateCSS();
}

std::string CHTLJSEnhancedListener::toCHTLJS() const {
    return generateCHTLJS();
}

std::string CHTLJSEnhancedListener::toString() const {
    return eventType;
}

std::string CHTLJSEnhancedListener::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSEnhancedListener{eventType='" << eventType
        << "', active=" << active
        << ", enabled=" << enabled
        << ", paused=" << paused
        << ", executed=" << executed
        << ", executionCount=" << executionCount << "}";
    return oss.str();
}

// 格式化实现
std::string CHTLJSEnhancedListener::format() const {
    return formatListener();
}

std::string CHTLJSEnhancedListener::minify() const {
    return minifyListener();
}

std::string CHTLJSEnhancedListener::beautify() const {
    return beautifyListener();
}

// 统计实现
int CHTLJSEnhancedListener::getExecutionCount() const {
    return executionCount;
}

std::chrono::milliseconds CHTLJSEnhancedListener::getTotalExecutionTime() const {
    return totalExecutionTime;
}

std::chrono::milliseconds CHTLJSEnhancedListener::getAverageExecutionTime() const {
    return executionCount > 0 ? 
        std::chrono::milliseconds(totalExecutionTime.count() / executionCount) : 
        std::chrono::milliseconds(0);
}

std::chrono::milliseconds CHTLJSEnhancedListener::getLastExecutionTime() const {
    return lastExecutionTime;
}

// 重置实现
void CHTLJSEnhancedListener::reset() {
    active = true;
    enabled = true;
    paused = false;
    executed = false;
    executionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    lastExecutionTime = std::chrono::milliseconds(0);
    lastExecuted = std::chrono::steady_clock::now();
    nextExecution = lastExecuted;
}

void CHTLJSEnhancedListener::resetStats() {
    executionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    lastExecutionTime = std::chrono::milliseconds(0);
}

// 验证辅助实现
bool CHTLJSEnhancedListener::validateListener() const {
    return !eventType.empty() || eventTypeEnum != EventType::UNKNOWN;
}

bool CHTLJSEnhancedListener::validateHandler() const {
    return handler.isValid();
}

bool CHTLJSEnhancedListener::validateOptions() const {
    return options.throttle >= 0 && options.debounce >= 0 && 
           options.delay >= 0 && options.timeout >= 0 && options.retry >= 0;
}

bool CHTLJSEnhancedListener::validateModifiers() const {
    return true; // 简化的修饰符验证
}

bool CHTLJSEnhancedListener::validateData() const {
    return true; // 简化的数据验证
}

// 比较辅助实现
bool CHTLJSEnhancedListener::compareListener(std::shared_ptr<CHTLJSEnhancedListener> other) const {
    return compareListener(*other);
}

bool CHTLJSEnhancedListener::compareListener(const CHTLJSEnhancedListener& other) const {
    return eventType == other.eventType &&
           eventTypeEnum == other.eventTypeEnum &&
           compareHandler(other) &&
           compareOptions(other) &&
           compareModifiers(other) &&
           compareData(other);
}

bool CHTLJSEnhancedListener::compareHandler(std::shared_ptr<CHTLJSEnhancedListener> other) const {
    return compareHandler(*other);
}

bool CHTLJSEnhancedListener::compareHandler(const CHTLJSEnhancedListener& other) const {
    return handler.getType() == other.handler.getType();
}

bool CHTLJSEnhancedListener::compareOptions(std::shared_ptr<CHTLJSEnhancedListener> other) const {
    return compareOptions(*other);
}

bool CHTLJSEnhancedListener::compareOptions(const CHTLJSEnhancedListener& other) const {
    return options.once == other.options.once &&
           options.preventDefault == other.options.preventDefault &&
           options.stopPropagation == other.options.stopPropagation &&
           options.capture == other.options.capture &&
           options.passive == other.options.passive &&
           options.delegate == other.options.delegate &&
           options.throttle == other.options.throttle &&
           options.debounce == other.options.debounce &&
           options.immediate == other.options.immediate &&
           options.lazy == other.options.lazy &&
           options.delay == other.options.delay &&
           options.timeout == other.options.timeout &&
           options.retry == other.options.retry &&
           options.selector == other.options.selector &&
           options.condition == other.options.condition &&
           options.namespace_ == other.options.namespace_;
}

bool CHTLJSEnhancedListener::compareModifiers(std::shared_ptr<CHTLJSEnhancedListener> other) const {
    return compareModifiers(*other);
}

bool CHTLJSEnhancedListener::compareModifiers(const CHTLJSEnhancedListener& other) const {
    return modifiers == other.modifiers;
}

bool CHTLJSEnhancedListener::compareData(std::shared_ptr<CHTLJSEnhancedListener> other) const {
    return compareData(*other);
}

bool CHTLJSEnhancedListener::compareData(const CHTLJSEnhancedListener& other) const {
    if (data.size() != other.data.size()) {
        return false;
    }
    
    for (const auto& pair : data) {
        auto it = other.data.find(pair.first);
        if (it == other.data.end()) {
            return false;
        }
        // 由于std::any不能直接比较，我们只比较键
        // 在实际应用中，可能需要更复杂的比较逻辑
    }
    
    return true;
}

// 克隆辅助实现
void CHTLJSEnhancedListener::copyTo(std::shared_ptr<CHTLJSEnhancedListener> target) const {
    target->eventType = eventType;
    target->eventTypeEnum = eventTypeEnum;
    target->handler = handler;
    target->options = options;
    target->modifiers = modifiers;
    target->data = data;
    target->active = active;
    target->enabled = enabled;
    target->paused = paused;
    target->executed = executed;
    target->executionCount = executionCount;
    target->totalExecutionTime = totalExecutionTime;
    target->lastExecutionTime = lastExecutionTime;
    target->lastExecuted = lastExecuted;
    target->nextExecution = nextExecution;
}

void CHTLJSEnhancedListener::deepCopyTo(std::shared_ptr<CHTLJSEnhancedListener> target) const {
    copyTo(target);
    // 深度复制数据
    target->data = data;
}

// 生成辅助实现
std::string CHTLJSEnhancedListener::generateJavaScript() const {
    std::ostringstream oss;
    oss << "addEventListener('" << eventType << "', ";
    
    if (options.capture) {
        oss << "true";
    } else if (options.passive) {
        oss << "{ passive: true }";
    } else {
        oss << "false";
    }
    
    oss << ");";
    return oss.str();
}

std::string CHTLJSEnhancedListener::generateCSS() const {
    return "";
}

std::string CHTLJSEnhancedListener::generateCHTLJS() const {
    std::ostringstream oss;
    oss << "listen('" << eventType << "'";
    
    if (!options.selector.empty()) {
        oss << ", '" << options.selector << "'";
    }
    
    oss << ");";
    return oss.str();
}

// 格式化辅助实现
std::string CHTLJSEnhancedListener::formatListener() const {
    return eventType;
}

std::string CHTLJSEnhancedListener::minifyListener() const {
    return eventType;
}

std::string CHTLJSEnhancedListener::beautifyListener() const {
    return eventType;
}

// 时间辅助方法实现
bool CHTLJSEnhancedListener::checkThrottle() const {
    if (options.throttle <= 0) return true;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastExecuted);
    return elapsed.count() >= options.throttle;
}

bool CHTLJSEnhancedListener::checkDebounce() const {
    if (options.debounce <= 0) return true;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastExecuted);
    return elapsed.count() >= options.debounce;
}

bool CHTLJSEnhancedListener::checkDelay() const {
    if (options.delay <= 0) return true;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastExecuted);
    return elapsed.count() >= options.delay;
}

bool CHTLJSEnhancedListener::checkTimeout() const {
    if (options.timeout <= 0) return true;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastExecuted);
    return elapsed.count() < options.timeout;
}

void CHTLJSEnhancedListener::updateExecutionTime(std::chrono::milliseconds duration) {
    totalExecutionTime += duration;
    lastExecutionTime = duration;
}

void CHTLJSEnhancedListener::updateNextExecution() {
    if (options.throttle > 0) {
        nextExecution = lastExecuted + std::chrono::milliseconds(options.throttle);
    } else if (options.debounce > 0) {
        nextExecution = lastExecuted + std::chrono::milliseconds(options.debounce);
    } else {
        nextExecution = lastExecuted;
    }
}

// 执行辅助方法实现
bool CHTLJSEnhancedListener::checkCondition(const std::string& eventType, const std::map<std::string, std::any>& data) const {
    if (options.condition.empty()) return true;
    
    // 简化的条件检查
    return true;
}

bool CHTLJSEnhancedListener::checkModifiers() const {
    return true; // 简化的修饰符检查
}

bool CHTLJSEnhancedListener::checkTiming() const {
    return checkThrottle() && checkDebounce() && checkDelay() && checkTimeout();
}

void CHTLJSEnhancedListener::executeHandler(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                           const std::any& event, const std::any& target) {
    handler(eventType, data, event, target);
}

void CHTLJSEnhancedListener::executeWithThrottle(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                                const std::any& event, const std::any& target) {
    if (checkThrottle()) {
        executeHandler(eventType, data, event, target);
    }
}

void CHTLJSEnhancedListener::executeWithDebounce(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                                const std::any& event, const std::any& target) {
    if (checkDebounce()) {
        executeHandler(eventType, data, event, target);
    }
}

void CHTLJSEnhancedListener::executeWithDelay(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                             const std::any& event, const std::any& target) {
    std::thread([this, eventType, data, event, target]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(options.delay));
        executeHandler(eventType, data, event, target);
    }).detach();
}

void CHTLJSEnhancedListener::executeImmediate(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                             const std::any& event, const std::any& target) {
    executeHandler(eventType, data, event, target);
}

void CHTLJSEnhancedListener::executeLazy(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                        const std::any& event, const std::any& target) {
    std::thread([this, eventType, data, event, target]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        executeHandler(eventType, data, event, target);
    }).detach();
}

// CHTLJSEnhancedListenerManager 实现
CHTLJSEnhancedListenerManager::CHTLJSEnhancedListenerManager() 
    : active(true), enabled(true), paused(false), totalExecutionCount(0), totalExecutionTime(0) {
}

CHTLJSEnhancedListenerManager::~CHTLJSEnhancedListenerManager() = default;

// 监听器管理实现
void CHTLJSEnhancedListenerManager::addListener(std::shared_ptr<CHTLJSEnhancedListener> listener) {
    if (listener && listener->isValid()) {
        listeners.push_back(listener);
        updateListenerIndex(listener);
    }
}

void CHTLJSEnhancedListenerManager::removeListener(std::shared_ptr<CHTLJSEnhancedListener> listener) {
    if (listener) {
        removeListenerFromIndex(listener);
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
    }
}

void CHTLJSEnhancedListenerManager::removeListener(const std::string& eventType) {
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
        [&eventType](const std::shared_ptr<CHTLJSEnhancedListener>& listener) {
            return listener->getEventType() == eventType;
        }), listeners.end());
    
    listenersByType.erase(eventType);
}

void CHTLJSEnhancedListenerManager::removeListener(const std::string& eventType, const std::string& namespace_) {
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
        [&eventType, &namespace_](const std::shared_ptr<CHTLJSEnhancedListener>& listener) {
            return listener->getEventType() == eventType && listener->getNamespace() == namespace_;
        }), listeners.end());
    
    auto& typeListeners = listenersByType[eventType];
    typeListeners.erase(std::remove_if(typeListeners.begin(), typeListeners.end(),
        [&namespace_](const std::shared_ptr<CHTLJSEnhancedListener>& listener) {
            return listener->getNamespace() == namespace_;
        }), typeListeners.end());
}

void CHTLJSEnhancedListenerManager::clearListeners() {
    listeners.clear();
    listenersByType.clear();
    listenersByNamespace.clear();
}

void CHTLJSEnhancedListenerManager::clearListeners(const std::string& eventType) {
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
        [&eventType](const std::shared_ptr<CHTLJSEnhancedListener>& listener) {
            return listener->getEventType() == eventType;
        }), listeners.end());
    
    listenersByType.erase(eventType);
}

// 查找监听器实现
std::vector<std::shared_ptr<CHTLJSEnhancedListener>> CHTLJSEnhancedListenerManager::getListeners() const {
    return listeners;
}

std::vector<std::shared_ptr<CHTLJSEnhancedListener>> CHTLJSEnhancedListenerManager::getListeners(const std::string& eventType) const {
    auto it = listenersByType.find(eventType);
    return it != listenersByType.end() ? it->second : std::vector<std::shared_ptr<CHTLJSEnhancedListener>>();
}

std::vector<std::shared_ptr<CHTLJSEnhancedListener>> CHTLJSEnhancedListenerManager::getListeners(const std::string& eventType, const std::string& namespace_) const {
    auto listeners = getListeners(eventType);
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
        [&namespace_](const std::shared_ptr<CHTLJSEnhancedListener>& listener) {
            return listener->getNamespace() != namespace_;
        }), listeners.end());
    return listeners;
}

std::vector<std::shared_ptr<CHTLJSEnhancedListener>> CHTLJSEnhancedListenerManager::getListeners(EventType eventType) const {
    // 简化的类型查找
    return std::vector<std::shared_ptr<CHTLJSEnhancedListener>>();
}

std::vector<std::shared_ptr<CHTLJSEnhancedListener>> CHTLJSEnhancedListenerManager::getListeners(EventType eventType, const std::string& namespace_) const {
    // 简化的类型查找
    return std::vector<std::shared_ptr<CHTLJSEnhancedListener>>();
}

std::shared_ptr<CHTLJSEnhancedListener> CHTLJSEnhancedListenerManager::getFirstListener(const std::string& eventType) const {
    auto listeners = getListeners(eventType);
    return listeners.empty() ? nullptr : listeners[0];
}

std::shared_ptr<CHTLJSEnhancedListener> CHTLJSEnhancedListenerManager::getFirstListener(const std::string& eventType, const std::string& namespace_) const {
    auto listeners = getListeners(eventType, namespace_);
    return listeners.empty() ? nullptr : listeners[0];
}

std::shared_ptr<CHTLJSEnhancedListener> CHTLJSEnhancedListenerManager::getFirstListener(EventType eventType) const {
    auto listeners = getListeners(eventType);
    return listeners.empty() ? nullptr : listeners[0];
}

std::shared_ptr<CHTLJSEnhancedListener> CHTLJSEnhancedListenerManager::getFirstListener(EventType eventType, const std::string& namespace_) const {
    auto listeners = getListeners(eventType, namespace_);
    return listeners.empty() ? nullptr : listeners[0];
}

// 事件触发实现
void CHTLJSEnhancedListenerManager::trigger(const std::string& eventType, const std::map<std::string, std::any>& data, 
                                           const std::any& event, const std::any& target) {
    if (!active || !enabled || paused) return;
    
    auto listeners = getListeners(eventType);
    for (auto& listener : listeners) {
        if (listener && listener->shouldExecute(eventType, data)) {
            listener->execute(eventType, data, event, target);
            totalExecutionCount++;
        }
    }
    
    updateStats();
}

void CHTLJSEnhancedListenerManager::trigger(EventType eventType, const std::map<std::string, std::any>& data, 
                                           const std::any& event, const std::any& target) {
    // 简化的类型触发
    trigger("", data, event, target);
}

// 批量操作实现
void CHTLJSEnhancedListenerManager::triggerAll(const std::map<std::string, std::map<std::string, std::any>>& events) {
    for (const auto& event : events) {
        trigger(event.first, event.second);
    }
}

void CHTLJSEnhancedListenerManager::pauseAll() {
    paused = true;
    for (auto& listener : listeners) {
        if (listener) {
            listener->setPaused(true);
        }
    }
}

void CHTLJSEnhancedListenerManager::resumeAll() {
    paused = false;
    for (auto& listener : listeners) {
        if (listener) {
            listener->setPaused(false);
        }
    }
}

void CHTLJSEnhancedListenerManager::enableAll() {
    enabled = true;
    for (auto& listener : listeners) {
        if (listener) {
            listener->setEnabled(true);
        }
    }
}

void CHTLJSEnhancedListenerManager::disableAll() {
    enabled = false;
    for (auto& listener : listeners) {
        if (listener) {
            listener->setEnabled(false);
        }
    }
}

// 状态管理实现
void CHTLJSEnhancedListenerManager::setActive(bool active) {
    this->active = active;
}

bool CHTLJSEnhancedListenerManager::isActive() const {
    return active;
}

void CHTLJSEnhancedListenerManager::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool CHTLJSEnhancedListenerManager::isEnabled() const {
    return enabled;
}

void CHTLJSEnhancedListenerManager::setPaused(bool paused) {
    this->paused = paused;
}

bool CHTLJSEnhancedListenerManager::isPaused() const {
    return paused;
}

// 验证实现
bool CHTLJSEnhancedListenerManager::isValid() const {
    return validateManager() && validateListeners();
}

std::vector<std::string> CHTLJSEnhancedListenerManager::validate() const {
    std::vector<std::string> errors;
    
    if (!validateManager()) {
        errors.push_back("Manager validation failed");
    }
    
    if (!validateListeners()) {
        errors.push_back("Listeners validation failed");
    }
    
    return errors;
}

// 统计实现
size_t CHTLJSEnhancedListenerManager::getListenerCount() const {
    return listeners.size();
}

size_t CHTLJSEnhancedListenerManager::getListenerCount(const std::string& eventType) const {
    return getListeners(eventType).size();
}

size_t CHTLJSEnhancedListenerManager::getListenerCount(EventType eventType) const {
    return getListeners(eventType).size();
}

int CHTLJSEnhancedListenerManager::getTotalExecutionCount() const {
    return totalExecutionCount;
}

std::chrono::milliseconds CHTLJSEnhancedListenerManager::getTotalExecutionTime() const {
    return totalExecutionTime;
}

std::chrono::milliseconds CHTLJSEnhancedListenerManager::getAverageExecutionTime() const {
    return totalExecutionCount > 0 ? 
        std::chrono::milliseconds(totalExecutionTime.count() / totalExecutionCount) : 
        std::chrono::milliseconds(0);
}

// 重置实现
void CHTLJSEnhancedListenerManager::reset() {
    active = true;
    enabled = true;
    paused = false;
    totalExecutionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    
    for (auto& listener : listeners) {
        if (listener) {
            listener->reset();
        }
    }
}

void CHTLJSEnhancedListenerManager::resetStats() {
    totalExecutionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    
    for (auto& listener : listeners) {
        if (listener) {
            listener->resetStats();
        }
    }
}

// 转换实现
std::string CHTLJSEnhancedListenerManager::toJavaScript() const {
    return generateJavaScript();
}

std::string CHTLJSEnhancedListenerManager::toCSS() const {
    return generateCSS();
}

std::string CHTLJSEnhancedListenerManager::toCHTLJS() const {
    return generateCHTLJS();
}

std::string CHTLJSEnhancedListenerManager::toString() const {
    return "CHTLJSEnhancedListenerManager";
}

std::string CHTLJSEnhancedListenerManager::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSEnhancedListenerManager{listenerCount=" << listeners.size()
        << ", active=" << active
        << ", enabled=" << enabled
        << ", paused=" << paused
        << ", totalExecutionCount=" << totalExecutionCount << "}";
    return oss.str();
}

// 格式化实现
std::string CHTLJSEnhancedListenerManager::format() const {
    return formatManager();
}

std::string CHTLJSEnhancedListenerManager::minify() const {
    return minifyManager();
}

std::string CHTLJSEnhancedListenerManager::beautify() const {
    return beautifyManager();
}

// 辅助方法实现
bool CHTLJSEnhancedListenerManager::validateManager() const {
    return true;
}

bool CHTLJSEnhancedListenerManager::validateListeners() const {
    for (const auto& listener : listeners) {
        if (!listener || !listener->isValid()) {
            return false;
        }
    }
    return true;
}

void CHTLJSEnhancedListenerManager::updateListenerIndex(std::shared_ptr<CHTLJSEnhancedListener> listener) {
    if (!listener) return;
    
    std::string eventType = listener->getEventType();
    if (!eventType.empty()) {
        listenersByType[eventType].push_back(listener);
    }
    
    std::string namespace_ = listener->getNamespace();
    if (!namespace_.empty()) {
        listenersByNamespace[namespace_].push_back(listener);
    }
}

void CHTLJSEnhancedListenerManager::removeListenerFromIndex(std::shared_ptr<CHTLJSEnhancedListener> listener) {
    if (!listener) return;
    
    std::string eventType = listener->getEventType();
    if (!eventType.empty()) {
        auto& typeListeners = listenersByType[eventType];
        typeListeners.erase(std::remove(typeListeners.begin(), typeListeners.end(), listener), typeListeners.end());
    }
    
    std::string namespace_ = listener->getNamespace();
    if (!namespace_.empty()) {
        auto& namespaceListeners = listenersByNamespace[namespace_];
        namespaceListeners.erase(std::remove(namespaceListeners.begin(), namespaceListeners.end(), listener), namespaceListeners.end());
    }
}

std::string CHTLJSEnhancedListenerManager::generateJavaScript() const {
    std::ostringstream oss;
    for (const auto& listener : listeners) {
        if (listener) {
            oss << listener->toJavaScript() << "\n";
        }
    }
    return oss.str();
}

std::string CHTLJSEnhancedListenerManager::generateCSS() const {
    return "";
}

std::string CHTLJSEnhancedListenerManager::generateCHTLJS() const {
    std::ostringstream oss;
    for (const auto& listener : listeners) {
        if (listener) {
            oss << listener->toCHTLJS() << "\n";
        }
    }
    return oss.str();
}

std::string CHTLJSEnhancedListenerManager::formatManager() const {
    return toString();
}

std::string CHTLJSEnhancedListenerManager::minifyManager() const {
    return toString();
}

std::string CHTLJSEnhancedListenerManager::beautifyManager() const {
    return toString();
}

void CHTLJSEnhancedListenerManager::updateStats() {
    totalExecutionTime = std::chrono::milliseconds(0);
    for (const auto& listener : listeners) {
        if (listener) {
            totalExecutionTime += listener->getTotalExecutionTime();
        }
    }
}

} // namespace CHTL_JS