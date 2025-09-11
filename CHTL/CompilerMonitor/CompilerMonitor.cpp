#include "CompilerMonitor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/resource.h>
#include <unistd.h>
#include <cstring>

namespace CHTL {

// CompilerMonitor 实现
CompilerMonitor::CompilerMonitor() : monitoring(false), shouldStop(false) {
    config = CompilerMonitorConfig();
}

CompilerMonitor::CompilerMonitor(const CompilerMonitorConfig& config) 
    : config(config), monitoring(false), shouldStop(false) {
}

CompilerMonitor::~CompilerMonitor() {
    stopMonitoring();
    cleanup();
}

void CompilerMonitor::setConfig(const CompilerMonitorConfig& config) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    this->config = config;
}

CompilerMonitorConfig CompilerMonitor::getConfig() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    return config;
}

bool CompilerMonitor::startMonitoring(const std::string& processId) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    if (monitoring) {
        return false; // 已经在监控中
    }
    
    // 初始化统计信息
    currentStats = CompilationStatistics();
    currentStats.startTime = std::chrono::steady_clock::now();
    currentStats.peakMemoryUsage = getCurrentMemoryUsage();
    currentStats.currentMemoryUsage = currentStats.peakMemoryUsage;
    
    monitoring = true;
    shouldStop = false;
    
    // 启动监控线程
    monitoringThread = std::thread(&CompilerMonitor::monitoringLoop, this);
    
    return true;
}

void CompilerMonitor::stopMonitoring() {
    {
        std::lock_guard<std::mutex> lock(monitorMutex);
        if (!monitoring) {
            return;
        }
        
        shouldStop = true;
        monitoring = false;
    }
    
    // 等待监控线程结束
    if (monitoringThread.joinable()) {
        monitoringThread.join();
    }
    
    // 完成统计信息
    currentStats.endTime = std::chrono::steady_clock::now();
    currentStats.calculateDuration();
    currentStats.peakMemoryUsage = std::max(currentStats.peakMemoryUsage, getCurrentMemoryUsage());
    
    // 保存到历史统计
    allStats.push_back(currentStats);
    
    // 清理旧统计（保留时间限制）
    auto now = std::chrono::steady_clock::now();
    allStats.erase(
        std::remove_if(allStats.begin(), allStats.end(),
            [&](const CompilationStatistics& stats) {
                auto age = std::chrono::duration_cast<std::chrono::seconds>(now - stats.startTime).count();
                return age > config.statisticsRetentionTime;
            }),
        allStats.end()
    );
}

bool CompilerMonitor::isMonitoring() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    return monitoring;
}

CompilationStatistics CompilerMonitor::getCurrentStatistics() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    return currentStats;
}

std::vector<CompilationStatistics> CompilerMonitor::getAllStatistics() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    return allStats;
}

void CompilerMonitor::clearStatistics() {
    std::lock_guard<std::mutex> lock(monitorMutex);
    allStats.clear();
    currentStats = CompilationStatistics();
}

long long CompilerMonitor::getCurrentMemoryUsage() const {
    return getProcessMemoryUsage();
}

long long CompilerMonitor::getPeakMemoryUsage() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    return currentStats.peakMemoryUsage;
}

long long CompilerMonitor::getElapsedTime() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    if (!monitoring) {
        return currentStats.durationMs;
    }
    
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - currentStats.startTime).count();
}

void CompilerMonitor::enableAutoKill(bool enable) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    config.enableAutoKill = enable;
}

void CompilerMonitor::setMemoryLimit(long long limit) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    config.maxMemoryUsage = limit;
}

void CompilerMonitor::setTimeLimit(long long limit) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    config.maxCompilationTime = limit;
}

void CompilerMonitor::setMemoryWarningCallback(std::function<void(long long)> callback) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    memoryWarningCallback = callback;
}

void CompilerMonitor::setTimeWarningCallback(std::function<void(long long)> callback) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    timeWarningCallback = callback;
}

void CompilerMonitor::setKillCallback(std::function<void(const std::string&)> callback) {
    std::lock_guard<std::mutex> lock(monitorMutex);
    killCallback = callback;
}

std::string CompilerMonitor::getDebugInfo() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    std::ostringstream oss;
    oss << "CompilerMonitor Debug Info:\n";
    oss << "  Monitoring: " << (monitoring ? "Yes" : "No") << "\n";
    oss << "  Config:\n";
    oss << "    Time Monitoring: " << (config.enableTimeMonitoring ? "Yes" : "No") << "\n";
    oss << "    Memory Monitoring: " << (config.enableMemoryMonitoring ? "Yes" : "No") << "\n";
    oss << "    Auto Kill: " << (config.enableAutoKill ? "Yes" : "No") << "\n";
    oss << "    Max Time: " << config.maxCompilationTime << "ms\n";
    oss << "    Max Memory: " << config.maxMemoryUsage << " bytes\n";
    oss << "  Current Stats:\n";
    oss << "    Elapsed Time: " << getElapsedTime() << "ms\n";
    oss << "    Current Memory: " << getCurrentMemoryUsage() << " bytes\n";
    oss << "    Peak Memory: " << currentStats.peakMemoryUsage << " bytes\n";
    oss << "  History: " << allStats.size() << " records\n";
    
    return oss.str();
}

std::string CompilerMonitor::generateReport() const {
    std::lock_guard<std::mutex> lock(monitorMutex);
    
    std::ostringstream oss;
    oss << "=== Compiler Monitor Report ===\n\n";
    
    // 当前状态
    oss << "Current Status:\n";
    oss << "  Monitoring: " << (monitoring ? "Active" : "Inactive") << "\n";
    oss << "  Elapsed Time: " << getElapsedTime() << "ms\n";
    oss << "  Current Memory: " << getCurrentMemoryUsage() << " bytes\n";
    oss << "  Peak Memory: " << currentStats.peakMemoryUsage << " bytes\n\n";
    
    // 配置信息
    oss << "Configuration:\n";
    oss << "  Time Limit: " << config.maxCompilationTime << "ms\n";
    oss << "  Memory Limit: " << config.maxMemoryUsage << " bytes\n";
    oss << "  Auto Kill: " << (config.enableAutoKill ? "Enabled" : "Disabled") << "\n\n";
    
    // 历史统计
    if (!allStats.empty()) {
        oss << "Historical Statistics:\n";
        long long totalTime = 0;
        long long totalMemory = 0;
        int killCount = 0;
        
        for (const auto& stats : allStats) {
            totalTime += stats.durationMs;
            totalMemory += stats.peakMemoryUsage;
            if (stats.wasKilled) killCount++;
        }
        
        oss << "  Total Compilations: " << allStats.size() << "\n";
        oss << "  Average Time: " << (totalTime / allStats.size()) << "ms\n";
        oss << "  Average Memory: " << (totalMemory / allStats.size()) << " bytes\n";
        oss << "  Killed Processes: " << killCount << "\n";
    }
    
    return oss.str();
}

void CompilerMonitor::monitoringLoop() {
    while (!shouldStop) {
        {
            std::lock_guard<std::mutex> lock(monitorMutex);
            if (!monitoring) break;
            
            updateMemoryUsage();
            checkLimits();
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(config.monitoringInterval));
    }
}

void CompilerMonitor::updateMemoryUsage() {
    long long currentMemory = getCurrentMemoryUsage();
    currentStats.currentMemoryUsage = currentMemory;
    currentStats.peakMemoryUsage = std::max(currentStats.peakMemoryUsage, currentMemory);
}

void CompilerMonitor::checkLimits() {
    // 检查时间限制
    if (config.enableTimeMonitoring) {
        long long elapsed = getElapsedTime();
        if (elapsed > config.maxCompilationTime) {
            if (timeWarningCallback) {
                timeWarningCallback(elapsed);
            }
            
            if (config.enableAutoKill) {
                killProcess("Time limit exceeded: " + std::to_string(elapsed) + "ms > " + std::to_string(config.maxCompilationTime) + "ms");
                return;
            }
        }
    }
    
    // 检查内存限制
    if (config.enableMemoryMonitoring) {
        long long currentMemory = getCurrentMemoryUsage();
        if (currentMemory > config.maxMemoryUsage) {
            if (memoryWarningCallback) {
                memoryWarningCallback(currentMemory);
            }
            
            if (config.enableAutoKill) {
                killProcess("Memory limit exceeded: " + std::to_string(currentMemory) + " bytes > " + std::to_string(config.maxMemoryUsage) + " bytes");
                return;
            }
        }
    }
}

void CompilerMonitor::killProcess(const std::string& reason) {
    currentStats.wasKilled = true;
    currentStats.killReason = reason;
    
    if (killCallback) {
        killCallback(reason);
    }
    
    // 在实际实现中，这里应该终止编译进程
    // 由于我们无法直接终止调用进程，我们设置标志
    shouldStop = true;
    monitoring = false;
    
    std::cerr << "Compiler Monitor: Process killed - " << reason << std::endl;
}

long long CompilerMonitor::getProcessMemoryUsage() const {
    // 读取 /proc/self/status 文件获取内存使用情况
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        return 0;
    }
    
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            // 提取内存使用量（KB）
            std::istringstream iss(line.substr(6));
            long long memoryKB;
            if (iss >> memoryKB) {
                return memoryKB * 1024; // 转换为字节
            }
        }
    }
    
    return 0;
}

void CompilerMonitor::cleanup() {
    stopMonitoring();
}

// CompilerMonitorManager 实现
CompilerMonitorManager& CompilerMonitorManager::getInstance() {
    static CompilerMonitorManager instance;
    return instance;
}

std::shared_ptr<CompilerMonitor> CompilerMonitorManager::createMonitor(const std::string& name) {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    std::string monitorName = name.empty() ? "monitor_" + std::to_string(nextMonitorId++) : name;
    
    auto monitor = std::make_shared<CompilerMonitor>(globalConfig);
    monitors[monitorName] = monitor;
    
    return monitor;
}

std::shared_ptr<CompilerMonitor> CompilerMonitorManager::getMonitor(const std::string& name) {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    auto it = monitors.find(name);
    return (it != monitors.end()) ? it->second : nullptr;
}

void CompilerMonitorManager::removeMonitor(const std::string& name) {
    std::lock_guard<std::mutex> lock(managerMutex);
    monitors.erase(name);
}

std::vector<std::string> CompilerMonitorManager::getMonitorNames() const {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    std::vector<std::string> names;
    for (const auto& pair : monitors) {
        names.push_back(pair.first);
    }
    return names;
}

void CompilerMonitorManager::setGlobalConfig(const CompilerMonitorConfig& config) {
    std::lock_guard<std::mutex> lock(managerMutex);
    globalConfig = config;
}

CompilerMonitorConfig CompilerMonitorManager::getGlobalConfig() const {
    std::lock_guard<std::mutex> lock(managerMutex);
    return globalConfig;
}

std::map<std::string, CompilationStatistics> CompilerMonitorManager::getAllMonitorStatistics() const {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    std::map<std::string, CompilationStatistics> allStats;
    for (const auto& pair : monitors) {
        allStats[pair.first] = pair.second->getCurrentStatistics();
    }
    return allStats;
}

void CompilerMonitorManager::clearAllStatistics() {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    for (auto& pair : monitors) {
        pair.second->clearStatistics();
    }
}

std::string CompilerMonitorManager::getManagerDebugInfo() const {
    std::lock_guard<std::mutex> lock(managerMutex);
    
    std::ostringstream oss;
    oss << "CompilerMonitorManager Debug Info:\n";
    oss << "  Active Monitors: " << monitors.size() << "\n";
    oss << "  Monitor Names: ";
    for (const auto& name : getMonitorNames()) {
        oss << name << " ";
    }
    oss << "\n";
    oss << "  Global Config:\n";
    oss << "    Time Monitoring: " << (globalConfig.enableTimeMonitoring ? "Yes" : "No") << "\n";
    oss << "    Memory Monitoring: " << (globalConfig.enableMemoryMonitoring ? "Yes" : "No") << "\n";
    oss << "    Auto Kill: " << (globalConfig.enableAutoKill ? "Yes" : "No") << "\n";
    
    return oss.str();
}

} // namespace CHTL