#include "CHTLJSEventDelegation.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <thread>
#include <future>

namespace CHTL_JS {

// CHTLJSEventDelegation 实现
CHTLJSEventDelegation::CHTLJSEventDelegation() 
    : rootSelector(""), targetSelector(""), active(true), enabled(true), paused(false),
      executionCount(0), totalExecutionTime(0), lastExecutionTime(0) {
    lastExecuted = std::chrono::steady_clock::now();
}

CHTLJSEventDelegation::CHTLJSEventDelegation(const std::string& rootSelector, const std::string& targetSelector, 
                                            DelegationHandler handler, const DelegationOptions& options) 
    : rootSelector(rootSelector), targetSelector(targetSelector), handler(handler), options(options),
      active(true), enabled(true), paused(false), executionCount(0), totalExecutionTime(0), lastExecutionTime(0) {
    lastExecuted = std::chrono::steady_clock::now();
}

CHTLJSEventDelegation::~CHTLJSEventDelegation() = default;

// 基本属性实现
void CHTLJSEventDelegation::setRootSelector(const std::string& rootSelector) {
    this->rootSelector = rootSelector;
}

std::string CHTLJSEventDelegation::getRootSelector() const {
    return rootSelector;
}

void CHTLJSEventDelegation::setTargetSelector(const std::string& targetSelector) {
    this->targetSelector = targetSelector;
}

std::string CHTLJSEventDelegation::getTargetSelector() const {
    return targetSelector;
}

void CHTLJSEventDelegation::setHandler(DelegationHandler handler) {
    this->handler = handler;
}

DelegationHandler CHTLJSEventDelegation::getHandler() const {
    return handler;
}

void CHTLJSEventDelegation::setOptions(const DelegationOptions& options) {
    this->options = options;
}

DelegationOptions CHTLJSEventDelegation::getOptions() const {
    return options;
}

void CHTLJSEventDelegation::setStrategy(DelegationStrategy strategy) {
    options.strategy = strategy;
}

DelegationStrategy CHTLJSEventDelegation::getStrategy() const {
    return options.strategy;
}

void CHTLJSEventDelegation::setCondition(const std::string& condition) {
    options.condition = condition;
}

std::string CHTLJSEventDelegation::getCondition() const {
    return options.condition;
}

// 选项管理实现
void CHTLJSEventDelegation::setStopPropagation(bool stop) {
    options.stopPropagation = stop;
}

bool CHTLJSEventDelegation::shouldStopPropagation() const {
    return options.stopPropagation;
}

void CHTLJSEventDelegation::setPreventDefault(bool prevent) {
    options.preventDefault = prevent;
}

bool CHTLJSEventDelegation::shouldPreventDefault() const {
    return options.preventDefault;
}

void CHTLJSEventDelegation::setMaxDepth(int maxDepth) {
    options.maxDepth = maxDepth;
}

int CHTLJSEventDelegation::getMaxDepth() const {
    return options.maxDepth;
}

void CHTLJSEventDelegation::setTimeout(int timeout) {
    options.timeout = timeout;
}

int CHTLJSEventDelegation::getTimeout() const {
    return options.timeout;
}

void CHTLJSEventDelegation::setLazy(bool lazy) {
    options.lazy = lazy;
}

bool CHTLJSEventDelegation::isLazy() const {
    return options.lazy;
}

void CHTLJSEventDelegation::setCache(bool cache) {
    options.cache = cache;
}

bool CHTLJSEventDelegation::isCache() const {
    return options.cache;
}

// 数据管理实现
void CHTLJSEventDelegation::setData(const std::map<std::string, std::any>& data) {
    this->data = data;
}

std::map<std::string, std::any> CHTLJSEventDelegation::getData() const {
    return data;
}

void CHTLJSEventDelegation::addData(const std::string& key, const std::any& value) {
    data[key] = value;
}

std::any CHTLJSEventDelegation::getData(const std::string& key) const {
    auto it = data.find(key);
    return it != data.end() ? it->second : std::any{};
}

bool CHTLJSEventDelegation::hasData(const std::string& key) const {
    return data.find(key) != data.end();
}

void CHTLJSEventDelegation::removeData(const std::string& key) {
    data.erase(key);
}

void CHTLJSEventDelegation::clearData() {
    data.clear();
}

// 过滤器管理实现
void CHTLJSEventDelegation::addFilter(DelegationFilter filter) {
    if (filter) {
        filters.push_back(filter);
    }
}

void CHTLJSEventDelegation::removeFilter(DelegationFilter filter) {
    filters.erase(std::remove(filters.begin(), filters.end(), filter), filters.end());
}

void CHTLJSEventDelegation::clearFilters() {
    filters.clear();
}

std::vector<DelegationFilter> CHTLJSEventDelegation::getFilters() const {
    return filters;
}

// 转换器管理实现
void CHTLJSEventDelegation::addTransformer(DelegationTransformer transformer) {
    if (transformer) {
        transformers.push_back(transformer);
    }
}

void CHTLJSEventDelegation::removeTransformer(DelegationTransformer transformer) {
    transformers.erase(std::remove(transformers.begin(), transformers.end(), transformer), transformers.end());
}

void CHTLJSEventDelegation::clearTransformers() {
    transformers.clear();
}

std::vector<DelegationTransformer> CHTLJSEventDelegation::getTransformers() const {
    return transformers;
}

// 状态管理实现
void CHTLJSEventDelegation::setActive(bool active) {
    this->active = active;
}

bool CHTLJSEventDelegation::isActive() const {
    return active;
}

void CHTLJSEventDelegation::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool CHTLJSEventDelegation::isEnabled() const {
    return enabled;
}

void CHTLJSEventDelegation::setPaused(bool paused) {
    this->paused = paused;
}

bool CHTLJSEventDelegation::isPaused() const {
    return paused;
}

// 执行控制实现
void CHTLJSEventDelegation::execute(const DelegationEventInfo& eventInfo) {
    if (!canExecute()) {
        return;
    }
    
    auto start = std::chrono::steady_clock::now();
    
    if (shouldExecute(eventInfo)) {
        DelegationEventInfo transformedEvent = transformEvent(eventInfo);
        executeHandler(transformedEvent);
        
        executionCount++;
        lastExecuted = std::chrono::steady_clock::now();
    }
    
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    updateExecutionTime(duration);
}

bool CHTLJSEventDelegation::canExecute() const {
    return active && enabled && !paused && handler;
}

bool CHTLJSEventDelegation::shouldExecute(const DelegationEventInfo& eventInfo) const {
    return canExecute() && 
           checkCondition(eventInfo) && 
           checkFilters(eventInfo) && 
           checkDepth(eventInfo) && 
           checkTimeout(eventInfo);
}

// 验证实现
bool CHTLJSEventDelegation::isValid() const {
    return validateDelegation() && validateHandler() && validateOptions() && 
           validateFilters() && validateTransformers() && validateData();
}

bool CHTLJSEventDelegation::isComplete() const {
    return !rootSelector.empty() && !targetSelector.empty() && handler;
}

std::vector<std::string> CHTLJSEventDelegation::validate() const {
    std::vector<std::string> errors;
    
    if (!validateDelegation()) {
        errors.push_back("Delegation validation failed");
    }
    
    if (!validateHandler()) {
        errors.push_back("Handler validation failed");
    }
    
    if (!validateOptions()) {
        errors.push_back("Options validation failed");
    }
    
    if (!validateFilters()) {
        errors.push_back("Filters validation failed");
    }
    
    if (!validateTransformers()) {
        errors.push_back("Transformers validation failed");
    }
    
    if (!validateData()) {
        errors.push_back("Data validation failed");
    }
    
    return errors;
}

// 比较实现
bool CHTLJSEventDelegation::equals(std::shared_ptr<CHTLJSEventDelegation> other) const {
    return other && compareDelegation(other);
}

bool CHTLJSEventDelegation::equals(const CHTLJSEventDelegation& other) const {
    return compareDelegation(other);
}

// 克隆实现
std::shared_ptr<CHTLJSEventDelegation> CHTLJSEventDelegation::clone() const {
    auto cloned = std::make_shared<CHTLJSEventDelegation>();
    copyTo(cloned);
    return cloned;
}

std::shared_ptr<CHTLJSEventDelegation> CHTLJSEventDelegation::deepClone() const {
    auto cloned = std::make_shared<CHTLJSEventDelegation>();
    deepCopyTo(cloned);
    return cloned;
}

// 转换实现
std::string CHTLJSEventDelegation::toJavaScript() const {
    return generateJavaScript();
}

std::string CHTLJSEventDelegation::toCSS() const {
    return generateCSS();
}

std::string CHTLJSEventDelegation::toCHTLJS() const {
    return generateCHTLJS();
}

std::string CHTLJSEventDelegation::toString() const {
    return rootSelector + " -> " + targetSelector;
}

std::string CHTLJSEventDelegation::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSEventDelegation{rootSelector='" << rootSelector
        << "', targetSelector='" << targetSelector
        << "', strategy=" << static_cast<int>(options.strategy)
        << ", active=" << active
        << ", enabled=" << enabled
        << ", paused=" << paused
        << ", executionCount=" << executionCount << "}";
    return oss.str();
}

// 格式化实现
std::string CHTLJSEventDelegation::format() const {
    return formatDelegation();
}

std::string CHTLJSEventDelegation::minify() const {
    return minifyDelegation();
}

std::string CHTLJSEventDelegation::beautify() const {
    return beautifyDelegation();
}

// 统计实现
int CHTLJSEventDelegation::getExecutionCount() const {
    return executionCount;
}

std::chrono::milliseconds CHTLJSEventDelegation::getTotalExecutionTime() const {
    return totalExecutionTime;
}

std::chrono::milliseconds CHTLJSEventDelegation::getAverageExecutionTime() const {
    return executionCount > 0 ? 
        std::chrono::milliseconds(totalExecutionTime.count() / executionCount) : 
        std::chrono::milliseconds(0);
}

std::chrono::milliseconds CHTLJSEventDelegation::getLastExecutionTime() const {
    return lastExecutionTime;
}

// 重置实现
void CHTLJSEventDelegation::reset() {
    active = true;
    enabled = true;
    paused = false;
    executionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    lastExecutionTime = std::chrono::milliseconds(0);
    lastExecuted = std::chrono::steady_clock::now();
}

void CHTLJSEventDelegation::resetStats() {
    executionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    lastExecutionTime = std::chrono::milliseconds(0);
}

// 验证辅助实现
bool CHTLJSEventDelegation::validateDelegation() const {
    return !rootSelector.empty() && !targetSelector.empty();
}

bool CHTLJSEventDelegation::validateHandler() const {
    return static_cast<bool>(handler);
}

bool CHTLJSEventDelegation::validateOptions() const {
    return options.maxDepth >= -1 && options.timeout >= 0;
}

bool CHTLJSEventDelegation::validateFilters() const {
    return true; // 简化的过滤器验证
}

bool CHTLJSEventDelegation::validateTransformers() const {
    return true; // 简化的转换器验证
}

bool CHTLJSEventDelegation::validateData() const {
    return true; // 简化的数据验证
}

// 比较辅助实现
bool CHTLJSEventDelegation::compareDelegation(std::shared_ptr<CHTLJSEventDelegation> other) const {
    return compareDelegation(*other);
}

bool CHTLJSEventDelegation::compareDelegation(const CHTLJSEventDelegation& other) const {
    return rootSelector == other.rootSelector &&
           targetSelector == other.targetSelector &&
           compareHandler(other) &&
           compareOptions(other) &&
           compareFilters(other) &&
           compareTransformers(other) &&
           compareData(other);
}

bool CHTLJSEventDelegation::compareHandler(std::shared_ptr<CHTLJSEventDelegation> other) const {
    return compareHandler(*other);
}

bool CHTLJSEventDelegation::compareHandler(const CHTLJSEventDelegation& other) const {
    return static_cast<bool>(handler) == static_cast<bool>(other.handler);
}

bool CHTLJSEventDelegation::compareOptions(std::shared_ptr<CHTLJSEventDelegation> other) const {
    return compareOptions(*other);
}

bool CHTLJSEventDelegation::compareOptions(const CHTLJSEventDelegation& other) const {
    return options.strategy == other.options.strategy &&
           options.rootSelector == other.options.rootSelector &&
           options.targetSelector == other.options.targetSelector &&
           options.condition == other.options.condition &&
           options.stopPropagation == other.options.stopPropagation &&
           options.preventDefault == other.options.preventDefault &&
           options.maxDepth == other.options.maxDepth &&
           options.timeout == other.options.timeout &&
           options.lazy == other.options.lazy &&
           options.cache == other.options.cache;
}

bool CHTLJSEventDelegation::compareFilters(std::shared_ptr<CHTLJSEventDelegation> other) const {
    return compareFilters(*other);
}

bool CHTLJSEventDelegation::compareFilters(const CHTLJSEventDelegation& other) const {
    return filters.size() == other.filters.size();
}

bool CHTLJSEventDelegation::compareTransformers(std::shared_ptr<CHTLJSEventDelegation> other) const {
    return compareTransformers(*other);
}

bool CHTLJSEventDelegation::compareTransformers(const CHTLJSEventDelegation& other) const {
    return transformers.size() == other.transformers.size();
}

bool CHTLJSEventDelegation::compareData(std::shared_ptr<CHTLJSEventDelegation> other) const {
    return compareData(*other);
}

bool CHTLJSEventDelegation::compareData(const CHTLJSEventDelegation& other) const {
    return data == other.data;
}

// 克隆辅助实现
void CHTLJSEventDelegation::copyTo(std::shared_ptr<CHTLJSEventDelegation> target) const {
    target->rootSelector = rootSelector;
    target->targetSelector = targetSelector;
    target->handler = handler;
    target->options = options;
    target->filters = filters;
    target->transformers = transformers;
    target->data = data;
    target->active = active;
    target->enabled = enabled;
    target->paused = paused;
    target->executionCount = executionCount;
    target->totalExecutionTime = totalExecutionTime;
    target->lastExecutionTime = lastExecutionTime;
    target->lastExecuted = lastExecuted;
}

void CHTLJSEventDelegation::deepCopyTo(std::shared_ptr<CHTLJSEventDelegation> target) const {
    copyTo(target);
    // 深度复制数据
    target->data = data;
}

// 生成辅助实现
std::string CHTLJSEventDelegation::generateJavaScript() const {
    std::ostringstream oss;
    oss << "delegate('" << rootSelector << "', '" << targetSelector << "', ";
    
    switch (options.strategy) {
        case DelegationStrategy::BUBBLE:
            oss << "'bubble'";
            break;
        case DelegationStrategy::CAPTURE:
            oss << "'capture'";
            break;
        case DelegationStrategy::DIRECT:
            oss << "'direct'";
            break;
        case DelegationStrategy::SELECTIVE:
            oss << "'selective'";
            break;
        case DelegationStrategy::CONDITIONAL:
            oss << "'conditional'";
            break;
        case DelegationStrategy::LAZY:
            oss << "'lazy'";
            break;
        case DelegationStrategy::EAGER:
            oss << "'eager'";
            break;
        default:
            oss << "'bubble'";
            break;
    }
    
    oss << ");";
    return oss.str();
}

std::string CHTLJSEventDelegation::generateCSS() const {
    return "";
}

std::string CHTLJSEventDelegation::generateCHTLJS() const {
    std::ostringstream oss;
    oss << "delegate('" << rootSelector << "', '" << targetSelector << "'";
    
    if (!options.condition.empty()) {
        oss << ", '" << options.condition << "'";
    }
    
    oss << ");";
    return oss.str();
}

// 格式化辅助实现
std::string CHTLJSEventDelegation::formatDelegation() const {
    return toString();
}

std::string CHTLJSEventDelegation::minifyDelegation() const {
    return toString();
}

std::string CHTLJSEventDelegation::beautifyDelegation() const {
    return toString();
}

// 执行辅助方法实现
bool CHTLJSEventDelegation::checkCondition(const DelegationEventInfo& eventInfo) const {
    if (options.condition.empty()) return true;
    
    // 简化的条件检查
    return true;
}

bool CHTLJSEventDelegation::checkFilters(const DelegationEventInfo& eventInfo) const {
    for (const auto& filter : filters) {
        if (filter && !filter(eventInfo)) {
            return false;
        }
    }
    return true;
}

bool CHTLJSEventDelegation::checkDepth(const DelegationEventInfo& eventInfo) const {
    if (options.maxDepth < 0) return true;
    return eventInfo.depth <= options.maxDepth;
}

bool CHTLJSEventDelegation::checkTimeout(const DelegationEventInfo& eventInfo) const {
    if (options.timeout <= 0) return true;
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - eventInfo.timestamp);
    return elapsed.count() < options.timeout;
}

DelegationEventInfo CHTLJSEventDelegation::transformEvent(const DelegationEventInfo& eventInfo) const {
    DelegationEventInfo transformed = eventInfo;
    
    for (const auto& transformer : transformers) {
        if (transformer) {
            transformed = transformer(transformed);
        }
    }
    
    return transformed;
}

void CHTLJSEventDelegation::executeHandler(const DelegationEventInfo& eventInfo) {
    if (handler) {
        handler(eventInfo);
    }
}

void CHTLJSEventDelegation::updateExecutionTime(std::chrono::milliseconds duration) {
    totalExecutionTime += duration;
    lastExecutionTime = duration;
}

// CHTLJSEventDelegationManager 实现
CHTLJSEventDelegationManager::CHTLJSEventDelegationManager() 
    : active(true), enabled(true), paused(false), totalExecutionCount(0), totalExecutionTime(0) {
}

CHTLJSEventDelegationManager::~CHTLJSEventDelegationManager() = default;

// 委托管理实现
void CHTLJSEventDelegationManager::addDelegation(std::shared_ptr<CHTLJSEventDelegation> delegation) {
    if (delegation && delegation->isValid()) {
        delegations.push_back(delegation);
        updateDelegationIndex(delegation);
    }
}

void CHTLJSEventDelegationManager::removeDelegation(std::shared_ptr<CHTLJSEventDelegation> delegation) {
    if (delegation) {
        removeDelegationFromIndex(delegation);
        delegations.erase(std::remove(delegations.begin(), delegations.end(), delegation), delegations.end());
    }
}

void CHTLJSEventDelegationManager::removeDelegation(const std::string& rootSelector) {
    delegations.erase(std::remove_if(delegations.begin(), delegations.end(),
        [&rootSelector](const std::shared_ptr<CHTLJSEventDelegation>& delegation) {
            return delegation->getRootSelector() == rootSelector;
        }), delegations.end());
    
    delegationsByRoot.erase(rootSelector);
}

void CHTLJSEventDelegationManager::removeDelegation(const std::string& rootSelector, const std::string& targetSelector) {
    delegations.erase(std::remove_if(delegations.begin(), delegations.end(),
        [&rootSelector, &targetSelector](const std::shared_ptr<CHTLJSEventDelegation>& delegation) {
            return delegation->getRootSelector() == rootSelector && 
                   delegation->getTargetSelector() == targetSelector;
        }), delegations.end());
    
    auto& rootDelegations = delegationsByRoot[rootSelector];
    rootDelegations.erase(std::remove_if(rootDelegations.begin(), rootDelegations.end(),
        [&targetSelector](const std::shared_ptr<CHTLJSEventDelegation>& delegation) {
            return delegation->getTargetSelector() == targetSelector;
        }), rootDelegations.end());
}

void CHTLJSEventDelegationManager::clearDelegations() {
    delegations.clear();
    delegationsByRoot.clear();
    delegationsByTarget.clear();
}

void CHTLJSEventDelegationManager::clearDelegations(const std::string& rootSelector) {
    delegations.erase(std::remove_if(delegations.begin(), delegations.end(),
        [&rootSelector](const std::shared_ptr<CHTLJSEventDelegation>& delegation) {
            return delegation->getRootSelector() == rootSelector;
        }), delegations.end());
    
    delegationsByRoot.erase(rootSelector);
}

// 查找委托实现
std::vector<std::shared_ptr<CHTLJSEventDelegation>> CHTLJSEventDelegationManager::getDelegations() const {
    return delegations;
}

std::vector<std::shared_ptr<CHTLJSEventDelegation>> CHTLJSEventDelegationManager::getDelegations(const std::string& rootSelector) const {
    auto it = delegationsByRoot.find(rootSelector);
    return it != delegationsByRoot.end() ? it->second : std::vector<std::shared_ptr<CHTLJSEventDelegation>>();
}

std::vector<std::shared_ptr<CHTLJSEventDelegation>> CHTLJSEventDelegationManager::getDelegations(const std::string& rootSelector, const std::string& targetSelector) const {
    auto delegations = getDelegations(rootSelector);
    delegations.erase(std::remove_if(delegations.begin(), delegations.end(),
        [&targetSelector](const std::shared_ptr<CHTLJSEventDelegation>& delegation) {
            return delegation->getTargetSelector() != targetSelector;
        }), delegations.end());
    return delegations;
}

std::shared_ptr<CHTLJSEventDelegation> CHTLJSEventDelegationManager::getFirstDelegation(const std::string& rootSelector) const {
    auto delegations = getDelegations(rootSelector);
    return delegations.empty() ? nullptr : delegations[0];
}

std::shared_ptr<CHTLJSEventDelegation> CHTLJSEventDelegationManager::getFirstDelegation(const std::string& rootSelector, const std::string& targetSelector) const {
    auto delegations = getDelegations(rootSelector, targetSelector);
    return delegations.empty() ? nullptr : delegations[0];
}

// 事件处理实现
void CHTLJSEventDelegationManager::handleEvent(const std::string& eventType, const std::any& event, const std::any& target) {
    if (!active || !enabled || paused) return;
    
    DelegationEventInfo eventInfo = createEventInfo(eventType, event, target);
    handleEvent(eventInfo);
}

void CHTLJSEventDelegationManager::handleEvent(const DelegationEventInfo& eventInfo) {
    if (!active || !enabled || paused) return;
    
    auto matchingDelegations = findMatchingDelegations(eventInfo);
    for (auto& delegation : matchingDelegations) {
        if (delegation && delegation->shouldExecute(eventInfo)) {
            delegation->execute(eventInfo);
            totalExecutionCount++;
        }
    }
    
    updateStats();
}

// 批量操作实现
void CHTLJSEventDelegationManager::handleAllEvents(const std::map<std::string, std::vector<std::any>>& events) {
    for (const auto& eventGroup : events) {
        for (const auto& event : eventGroup.second) {
            handleEvent(eventGroup.first, event, std::any{});
        }
    }
}

void CHTLJSEventDelegationManager::pauseAll() {
    paused = true;
    for (auto& delegation : delegations) {
        if (delegation) {
            delegation->setPaused(true);
        }
    }
}

void CHTLJSEventDelegationManager::resumeAll() {
    paused = false;
    for (auto& delegation : delegations) {
        if (delegation) {
            delegation->setPaused(false);
        }
    }
}

void CHTLJSEventDelegationManager::enableAll() {
    enabled = true;
    for (auto& delegation : delegations) {
        if (delegation) {
            delegation->setEnabled(true);
        }
    }
}

void CHTLJSEventDelegationManager::disableAll() {
    enabled = false;
    for (auto& delegation : delegations) {
        if (delegation) {
            delegation->setEnabled(false);
        }
    }
}

// 状态管理实现
void CHTLJSEventDelegationManager::setActive(bool active) {
    this->active = active;
}

bool CHTLJSEventDelegationManager::isActive() const {
    return active;
}

void CHTLJSEventDelegationManager::setEnabled(bool enabled) {
    this->enabled = enabled;
}

bool CHTLJSEventDelegationManager::isEnabled() const {
    return enabled;
}

void CHTLJSEventDelegationManager::setPaused(bool paused) {
    this->paused = paused;
}

bool CHTLJSEventDelegationManager::isPaused() const {
    return paused;
}

// 验证实现
bool CHTLJSEventDelegationManager::isValid() const {
    return validateManager() && validateDelegations();
}

std::vector<std::string> CHTLJSEventDelegationManager::validate() const {
    std::vector<std::string> errors;
    
    if (!validateManager()) {
        errors.push_back("Manager validation failed");
    }
    
    if (!validateDelegations()) {
        errors.push_back("Delegations validation failed");
    }
    
    return errors;
}

// 统计实现
size_t CHTLJSEventDelegationManager::getDelegationCount() const {
    return delegations.size();
}

size_t CHTLJSEventDelegationManager::getDelegationCount(const std::string& rootSelector) const {
    return getDelegations(rootSelector).size();
}

int CHTLJSEventDelegationManager::getTotalExecutionCount() const {
    return totalExecutionCount;
}

std::chrono::milliseconds CHTLJSEventDelegationManager::getTotalExecutionTime() const {
    return totalExecutionTime;
}

std::chrono::milliseconds CHTLJSEventDelegationManager::getAverageExecutionTime() const {
    return totalExecutionCount > 0 ? 
        std::chrono::milliseconds(totalExecutionTime.count() / totalExecutionCount) : 
        std::chrono::milliseconds(0);
}

// 重置实现
void CHTLJSEventDelegationManager::reset() {
    active = true;
    enabled = true;
    paused = false;
    totalExecutionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    
    for (auto& delegation : delegations) {
        if (delegation) {
            delegation->reset();
        }
    }
}

void CHTLJSEventDelegationManager::resetStats() {
    totalExecutionCount = 0;
    totalExecutionTime = std::chrono::milliseconds(0);
    
    for (auto& delegation : delegations) {
        if (delegation) {
            delegation->resetStats();
        }
    }
}

// 转换实现
std::string CHTLJSEventDelegationManager::toJavaScript() const {
    return generateJavaScript();
}

std::string CHTLJSEventDelegationManager::toCSS() const {
    return generateCSS();
}

std::string CHTLJSEventDelegationManager::toCHTLJS() const {
    return generateCHTLJS();
}

std::string CHTLJSEventDelegationManager::toString() const {
    return "CHTLJSEventDelegationManager";
}

std::string CHTLJSEventDelegationManager::toDebugString() const {
    std::ostringstream oss;
    oss << "CHTLJSEventDelegationManager{delegationCount=" << delegations.size()
        << ", active=" << active
        << ", enabled=" << enabled
        << ", paused=" << paused
        << ", totalExecutionCount=" << totalExecutionCount << "}";
    return oss.str();
}

// 格式化实现
std::string CHTLJSEventDelegationManager::format() const {
    return formatManager();
}

std::string CHTLJSEventDelegationManager::minify() const {
    return minifyManager();
}

std::string CHTLJSEventDelegationManager::beautify() const {
    return beautifyManager();
}

// 辅助方法实现
bool CHTLJSEventDelegationManager::validateManager() const {
    return true;
}

bool CHTLJSEventDelegationManager::validateDelegations() const {
    for (const auto& delegation : delegations) {
        if (!delegation || !delegation->isValid()) {
            return false;
        }
    }
    return true;
}

void CHTLJSEventDelegationManager::updateDelegationIndex(std::shared_ptr<CHTLJSEventDelegation> delegation) {
    if (!delegation) return;
    
    std::string rootSelector = delegation->getRootSelector();
    if (!rootSelector.empty()) {
        delegationsByRoot[rootSelector].push_back(delegation);
    }
    
    std::string targetSelector = delegation->getTargetSelector();
    if (!targetSelector.empty()) {
        delegationsByTarget[targetSelector].push_back(delegation);
    }
}

void CHTLJSEventDelegationManager::removeDelegationFromIndex(std::shared_ptr<CHTLJSEventDelegation> delegation) {
    if (!delegation) return;
    
    std::string rootSelector = delegation->getRootSelector();
    if (!rootSelector.empty()) {
        auto& rootDelegations = delegationsByRoot[rootSelector];
        rootDelegations.erase(std::remove(rootDelegations.begin(), rootDelegations.end(), delegation), rootDelegations.end());
    }
    
    std::string targetSelector = delegation->getTargetSelector();
    if (!targetSelector.empty()) {
        auto& targetDelegations = delegationsByTarget[targetSelector];
        targetDelegations.erase(std::remove(targetDelegations.begin(), targetDelegations.end(), delegation), targetDelegations.end());
    }
}

std::string CHTLJSEventDelegationManager::generateJavaScript() const {
    std::ostringstream oss;
    for (const auto& delegation : delegations) {
        if (delegation) {
            oss << delegation->toJavaScript() << "\n";
        }
    }
    return oss.str();
}

std::string CHTLJSEventDelegationManager::generateCSS() const {
    return "";
}

std::string CHTLJSEventDelegationManager::generateCHTLJS() const {
    std::ostringstream oss;
    for (const auto& delegation : delegations) {
        if (delegation) {
            oss << delegation->toCHTLJS() << "\n";
        }
    }
    return oss.str();
}

std::string CHTLJSEventDelegationManager::formatManager() const {
    return toString();
}

std::string CHTLJSEventDelegationManager::minifyManager() const {
    return toString();
}

std::string CHTLJSEventDelegationManager::beautifyManager() const {
    return toString();
}

void CHTLJSEventDelegationManager::updateStats() {
    totalExecutionTime = std::chrono::milliseconds(0);
    for (const auto& delegation : delegations) {
        if (delegation) {
            totalExecutionTime += delegation->getTotalExecutionTime();
        }
    }
}

// 事件处理辅助方法实现
DelegationEventInfo CHTLJSEventDelegationManager::createEventInfo(const std::string& eventType, const std::any& event, const std::any& target) const {
    DelegationEventInfo eventInfo;
    eventInfo.eventType = eventType;
    eventInfo.originalEvent = event;
    eventInfo.target = target;
    eventInfo.currentTarget = target;
    eventInfo.depth = 0;
    eventInfo.isDelegated = true;
    eventInfo.timestamp = std::chrono::steady_clock::now();
    return eventInfo;
}

std::vector<std::shared_ptr<CHTLJSEventDelegation>> CHTLJSEventDelegationManager::findMatchingDelegations(const DelegationEventInfo& eventInfo) const {
    std::vector<std::shared_ptr<CHTLJSEventDelegation>> matching;
    
    for (const auto& delegation : delegations) {
        if (delegation && delegation->canExecute()) {
            // 简化的匹配逻辑
            if (matchesSelector(delegation->getRootSelector(), eventInfo.currentTarget) &&
                matchesSelector(delegation->getTargetSelector(), eventInfo.target)) {
                matching.push_back(delegation);
            }
        }
    }
    
    return matching;
}

bool CHTLJSEventDelegationManager::matchesSelector(const std::string& selector, const std::any& target) const {
    // 简化的选择器匹配
    return !selector.empty();
}

bool CHTLJSEventDelegationManager::matchesCondition(const std::string& condition, const DelegationEventInfo& eventInfo) const {
    // 简化的条件匹配
    return condition.empty() || condition == "true";
}

} // namespace CHTL_JS