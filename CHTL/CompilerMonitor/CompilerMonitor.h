#ifndef COMPILER_MONITOR_H
#define COMPILER_MONITOR_H

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>

namespace CHTL {

// 编译监控器配置
struct CompilerMonitorConfig {
    bool enableTimeMonitoring = true;
    bool enableMemoryMonitoring = true;
    bool enableAutoKill = false;
    
    // 时间限制（毫秒）
    long long maxCompilationTime = 30000; // 30秒
    
    // 内存限制（字节）
    long long maxMemoryUsage = 2LL * 1024 * 1024 * 1024; // 2GB
    
    // 监控间隔（毫秒）
    int monitoringInterval = 100; // 100ms
    
    // 统计保留时间（秒）
    int statisticsRetentionTime = 3600; // 1小时
};

// 编译统计信息
struct CompilationStatistics {
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    long long durationMs = 0;
    long long peakMemoryUsage = 0;
    long long currentMemoryUsage = 0;
    bool wasKilled = false;
    std::string killReason;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    // 计算持续时间
    void calculateDuration() {
        if (endTime > startTime) {
            durationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        }
    }
};

// 编译监控器
class CompilerMonitor {
public:
    CompilerMonitor();
    CompilerMonitor(const CompilerMonitorConfig& config);
    ~CompilerMonitor();
    
    // 配置管理
    void setConfig(const CompilerMonitorConfig& config);
    CompilerMonitorConfig getConfig() const;
    
    // 监控控制
    bool startMonitoring(const std::string& processId = "");
    void stopMonitoring();
    bool isMonitoring() const;
    
    // 统计信息
    CompilationStatistics getCurrentStatistics() const;
    std::vector<CompilationStatistics> getAllStatistics() const;
    void clearStatistics();
    
    // 内存监控
    long long getCurrentMemoryUsage() const;
    long long getPeakMemoryUsage() const;
    
    // 时间监控
    long long getElapsedTime() const;
    
    // 自动终止
    void enableAutoKill(bool enable);
    void setMemoryLimit(long long limit);
    void setTimeLimit(long long limit);
    
    // 回调函数
    void setMemoryWarningCallback(std::function<void(long long)> callback);
    void setTimeWarningCallback(std::function<void(long long)> callback);
    void setKillCallback(std::function<void(const std::string&)> callback);
    
    // 调试和诊断
    std::string getDebugInfo() const;
    std::string generateReport() const;
    
private:
    CompilerMonitorConfig config;
    mutable std::mutex monitorMutex;
    
    std::atomic<bool> monitoring;
    std::atomic<bool> shouldStop;
    std::thread monitoringThread;
    
    CompilationStatistics currentStats;
    std::vector<CompilationStatistics> allStats;
    
    std::function<void(long long)> memoryWarningCallback;
    std::function<void(long long)> timeWarningCallback;
    std::function<void(const std::string&)> killCallback;
    
    // 内部方法
    void monitoringLoop();
    void updateMemoryUsage();
    void checkLimits();
    void killProcess(const std::string& reason);
    long long getProcessMemoryUsage() const;
    void cleanup();
};

// 编译监控器管理器
class CompilerMonitorManager {
public:
    static CompilerMonitorManager& getInstance();
    
    // 监控器管理
    std::shared_ptr<CompilerMonitor> createMonitor(const std::string& name = "");
    std::shared_ptr<CompilerMonitor> getMonitor(const std::string& name);
    void removeMonitor(const std::string& name);
    std::vector<std::string> getMonitorNames() const;
    
    // 全局配置
    void setGlobalConfig(const CompilerMonitorConfig& config);
    CompilerMonitorConfig getGlobalConfig() const;
    
    // 全局统计
    std::map<std::string, CompilationStatistics> getAllMonitorStatistics() const;
    void clearAllStatistics();
    
    // 调试
    std::string getManagerDebugInfo() const;
    
private:
    CompilerMonitorManager() = default;
    ~CompilerMonitorManager() = default;
    CompilerMonitorManager(const CompilerMonitorManager&) = delete;
    CompilerMonitorManager& operator=(const CompilerMonitorManager&) = delete;
    
    mutable std::mutex managerMutex;
    std::map<std::string, std::shared_ptr<CompilerMonitor>> monitors;
    CompilerMonitorConfig globalConfig;
    std::atomic<int> nextMonitorId{1};
};

} // namespace CHTL

#endif // COMPILER_MONITOR_H