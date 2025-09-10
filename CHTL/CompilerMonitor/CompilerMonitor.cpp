#include "CompilerMonitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <algorithm>

namespace CHTL {

// CompilerMonitor 实现
CompilerMonitor::CompilerMonitor() 
    : monitoring(false), terminated(false), shouldStop(false), 
      processId(0), processKilled(false) {
}

CompilerMonitor::~CompilerMonitor() {
    stopMonitoring();
}

void CompilerMonitor::setConfig(const MonitorConfig& config) {
    this->config = config;
}

MonitorConfig CompilerMonitor::getConfig() const {
    return config;
}

bool CompilerMonitor::startMonitoring(std::function<void()> compileFunction) {
    if (monitoring.load()) {
        return false; // 已经在监视中
    }
    
    reset();
    monitoring.store(true);
    shouldStop.store(false);
    terminated.store(false);
    startTime = std::chrono::steady_clock::now();
    
    // 启动监视线程
    monitorThread = std::thread(&CompilerMonitor::monitorLoop, this);
    
    if (config.enableMemoryMonitoring) {
        memoryMonitorThread = std::thread(&CompilerMonitor::memoryMonitorLoop, this);
    }
    
    try {
        // 执行编译函数
        compileFunction();
        
        // 编译完成，停止监视
        stopMonitoring();
        return true;
    } catch (const std::exception& e) {
        handleError("Compilation error: " + std::string(e.what()));
        stopMonitoring();
        return false;
    }
}

void CompilerMonitor::stopMonitoring() {
    if (!monitoring.load()) {
        return;
    }
    
    shouldStop.store(true);
    cv.notify_all();
    
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
    
    if (memoryMonitorThread.joinable()) {
        memoryMonitorThread.join();
    }
    
    monitoring.store(false);
}

void CompilerMonitor::updateStats(const CompileStats& stats) {
    std::lock_guard<std::mutex> lock(statsMutex);
    currentStats = stats;
    
    if (config.onStatsUpdate) {
        config.onStatsUpdate(currentStats);
    }
}

CompileStats CompilerMonitor::getCurrentStats() const {
    std::lock_guard<std::mutex> lock(statsMutex);
    return currentStats;
}

bool CompilerMonitor::shouldTerminate() const {
    return terminated.load() || shouldStop.load();
}

void CompilerMonitor::forceTerminate() {
    terminated.store(true);
    shouldStop.store(true);
    
    if (config.enableAutoKill && processId > 0) {
        killProcess();
    }
    
    if (config.onKill) {
        config.onKill();
    }
}

void CompilerMonitor::reset() {
    std::lock_guard<std::mutex> lock(statsMutex);
    currentStats = CompileStats();
    processId = 0;
    processKilled = false;
    terminated.store(false);
    shouldStop.store(false);
}

bool CompilerMonitor::isMonitoring() const {
    return monitoring.load();
}

bool CompilerMonitor::isTerminated() const {
    return terminated.load();
}

void CompilerMonitor::setProcessId(pid_t pid) {
    processId = pid;
}

size_t CompilerMonitor::getCurrentMemoryUsage() const {
    return getProcessMemoryUsage();
}

std::chrono::milliseconds CompilerMonitor::getElapsedTime() const {
    if (!monitoring.load()) {
        return std::chrono::milliseconds(0);
    }
    
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
}

void CompilerMonitor::monitorLoop() {
    while (!shouldStop.load() && !terminated.load()) {
        // 检查编译时间
        if (config.enableTimeMonitoring) {
            checkCompileTime();
        }
        
        // 检查内存使用
        if (config.enableMemoryMonitoring) {
            checkMemoryUsage();
        }
        
        // 等待检查间隔
        std::unique_lock<std::mutex> lock(cvMutex);
        cv.wait_for(lock, config.checkInterval, [this] { 
            return shouldStop.load() || terminated.load(); 
        });
    }
}

void CompilerMonitor::memoryMonitorLoop() {
    while (!shouldStop.load() && !terminated.load()) {
        updateMemoryStats();
        
        std::this_thread::sleep_for(config.checkInterval);
    }
}

void CompilerMonitor::checkMemoryUsage() {
    size_t currentMemory = getCurrentMemoryUsage();
    
    if (currentMemory > config.maxMemoryUsage) {
        handleMemoryLimit();
    }
    
    // 更新统计信息
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.memoryUsage = currentMemory;
        if (currentMemory > currentStats.peakMemoryUsage) {
            currentStats.peakMemoryUsage = currentMemory;
        }
    }
}

void CompilerMonitor::checkCompileTime() {
    auto elapsed = getElapsedTime();
    
    if (elapsed > config.maxCompileTime) {
        handleTimeout();
    }
    
    // 更新统计信息
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.compileTime = elapsed;
    }
}

bool CompilerMonitor::killProcess() {
    if (processId <= 0 || processKilled.load()) {
        return false;
    }
    
    try {
        // 尝试优雅终止
        if (kill(processId, SIGTERM) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            
            // 检查进程是否还存在
            if (kill(processId, 0) == 0) {
                // 强制终止
                kill(processId, SIGKILL);
            }
            
            processKilled.store(true);
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to kill process " << processId << ": " << e.what() << std::endl;
    }
    
    return false;
}

size_t CompilerMonitor::getProcessMemoryUsage() const {
    if (processId <= 0) {
        return 0;
    }
    
    try {
        std::ifstream statusFile("/proc/" + std::to_string(processId) + "/status");
        if (!statusFile.is_open()) {
            return 0;
        }
        
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.substr(0, 6) == "VmRSS:") {
                std::istringstream iss(line);
                std::string key, value, unit;
                iss >> key >> value >> unit;
                
                size_t memoryKB = std::stoul(value);
                return memoryKB * 1024; // 转换为字节
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Failed to get memory usage: " << e.what() << std::endl;
    }
    
    return 0;
}

void CompilerMonitor::updateMemoryStats() {
    size_t currentMemory = getCurrentMemoryUsage();
    
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.memoryUsage = currentMemory;
        if (currentMemory > currentStats.peakMemoryUsage) {
            currentStats.peakMemoryUsage = currentMemory;
        }
    }
}

void CompilerMonitor::updateCompileTime() {
    auto elapsed = getElapsedTime();
    
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.compileTime = elapsed;
    }
}

void CompilerMonitor::updateMemoryUsage() {
    size_t currentMemory = getCurrentMemoryUsage();
    
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.memoryUsage = currentMemory;
        if (currentMemory > currentStats.peakMemoryUsage) {
            currentStats.peakMemoryUsage = currentMemory;
        }
    }
}

void CompilerMonitor::updateFileStats(size_t files, size_t lines, size_t tokens) {
    std::lock_guard<std::mutex> lock(statsMutex);
    currentStats.filesProcessed = files;
    currentStats.linesProcessed = lines;
    currentStats.tokensGenerated = tokens;
}

void CompilerMonitor::handleError(const std::string& error) {
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.status = "error";
        currentStats.errorMessage = error;
    }
    
    if (config.onError) {
        config.onError(error);
    }
    
    std::cerr << "Compiler Monitor Error: " << error << std::endl;
}

void CompilerMonitor::handleTimeout() {
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.status = "timeout";
        currentStats.errorMessage = "Compilation timeout exceeded";
    }
    
    std::cerr << "Compiler Monitor: Compilation timeout exceeded (" 
              << config.maxCompileTime.count() << "ms)" << std::endl;
    
    if (config.enableAutoKill) {
        forceTerminate();
    }
}

void CompilerMonitor::handleMemoryLimit() {
    {
        std::lock_guard<std::mutex> lock(statsMutex);
        currentStats.status = "memory_limit";
        currentStats.errorMessage = "Memory usage limit exceeded";
    }
    
    std::cerr << "Compiler Monitor: Memory usage limit exceeded (" 
              << config.maxMemoryUsage / (1024 * 1024) << "MB)" << std::endl;
    
    if (config.enableAutoKill) {
        forceTerminate();
    }
}

// CompilerMonitorFactory 实现
std::unique_ptr<CompilerMonitor> CompilerMonitorFactory::createMonitor() {
    return std::make_unique<CompilerMonitor>();
}

std::unique_ptr<CompilerMonitor> CompilerMonitorFactory::createMonitor(const MonitorConfig& config) {
    auto monitor = std::make_unique<CompilerMonitor>();
    monitor->setConfig(config);
    return monitor;
}

MonitorConfig CompilerMonitorFactory::createDefaultConfig() {
    MonitorConfig config;
    config.maxCompileTime = std::chrono::milliseconds(30000); // 30秒
    config.maxMemoryUsage = 1024 * 1024 * 1024; // 1GB
    config.enableMemoryMonitoring = true;
    config.enableTimeMonitoring = true;
    config.enableAutoKill = true;
    config.checkInterval = std::chrono::milliseconds(100);
    return config;
}

MonitorConfig CompilerMonitorFactory::createStrictConfig() {
    MonitorConfig config;
    config.maxCompileTime = std::chrono::milliseconds(10000); // 10秒
    config.maxMemoryUsage = 512 * 1024 * 1024; // 512MB
    config.enableMemoryMonitoring = true;
    config.enableTimeMonitoring = true;
    config.enableAutoKill = true;
    config.checkInterval = std::chrono::milliseconds(50);
    return config;
}

MonitorConfig CompilerMonitorFactory::createLooseConfig() {
    MonitorConfig config;
    config.maxCompileTime = std::chrono::milliseconds(120000); // 2分钟
    config.maxMemoryUsage = 2 * 1024 * 1024 * 1024; // 2GB
    config.enableMemoryMonitoring = true;
    config.enableTimeMonitoring = true;
    config.enableAutoKill = false;
    config.checkInterval = std::chrono::milliseconds(200);
    return config;
}

// CompilerMonitorManager 实现
CompilerMonitorManager& CompilerMonitorManager::getInstance() {
    static CompilerMonitorManager instance;
    return instance;
}

void CompilerMonitorManager::registerMonitor(const std::string& name, std::shared_ptr<CompilerMonitor> monitor) {
    std::lock_guard<std::mutex> lock(monitorsMutex);
    monitors[name] = monitor;
}

std::shared_ptr<CompilerMonitor> CompilerMonitorManager::getMonitor(const std::string& name) {
    std::lock_guard<std::mutex> lock(monitorsMutex);
    auto it = monitors.find(name);
    if (it != monitors.end()) {
        return it->second;
    }
    return nullptr;
}

void CompilerMonitorManager::removeMonitor(const std::string& name) {
    std::lock_guard<std::mutex> lock(monitorsMutex);
    monitors.erase(name);
}

std::map<std::string, std::shared_ptr<CompilerMonitor>> CompilerMonitorManager::getAllMonitors() const {
    std::lock_guard<std::mutex> lock(monitorsMutex);
    return monitors;
}

void CompilerMonitorManager::clearAllMonitors() {
    std::lock_guard<std::mutex> lock(monitorsMutex);
    monitors.clear();
}

} // namespace CHTL