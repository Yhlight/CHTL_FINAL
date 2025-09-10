#ifndef CHTL_COMPILER_MONITOR_H
#define CHTL_COMPILER_MONITOR_H

#include <chrono>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <map>
#include <string>

namespace CHTL {

// 编译统计信息
struct CompileStats {
    std::chrono::milliseconds compileTime;
    size_t memoryUsage; // 字节
    size_t peakMemoryUsage; // 字节
    size_t filesProcessed;
    size_t linesProcessed;
    size_t tokensGenerated;
    std::string status;
    std::string errorMessage;
    
    CompileStats() : compileTime(0), memoryUsage(0), peakMemoryUsage(0), 
                    filesProcessed(0), linesProcessed(0), tokensGenerated(0) {}
};

// 编译监视器配置
struct MonitorConfig {
    std::chrono::milliseconds maxCompileTime{30000}; // 30秒
    size_t maxMemoryUsage{1024 * 1024 * 1024}; // 1GB
    bool enableMemoryMonitoring{true};
    bool enableTimeMonitoring{true};
    bool enableAutoKill{true};
    std::chrono::milliseconds checkInterval{100}; // 100ms
    std::function<void(const CompileStats&)> onStatsUpdate;
    std::function<void(const std::string&)> onError;
    std::function<void()> onKill;
    
    MonitorConfig() = default;
};

// 编译监视器
class CompilerMonitor {
public:
    CompilerMonitor();
    ~CompilerMonitor();
    
    // 配置监视器
    void setConfig(const MonitorConfig& config);
    MonitorConfig getConfig() const;
    
    // 开始监视编译过程
    bool startMonitoring(std::function<void()> compileFunction);
    
    // 停止监视
    void stopMonitoring();
    
    // 更新统计信息
    void updateStats(const CompileStats& stats);
    
    // 获取当前统计信息
    CompileStats getCurrentStats() const;
    
    // 检查是否应该终止编译
    bool shouldTerminate() const;
    
    // 强制终止编译
    void forceTerminate();
    
    // 重置监视器
    void reset();
    
    // 获取监视器状态
    bool isMonitoring() const;
    bool isTerminated() const;
    
    // 设置编译进程ID（用于外部进程监视）
    void setProcessId(pid_t pid);
    
    // 获取内存使用情况
    size_t getCurrentMemoryUsage() const;
    
    // 获取编译时间
    std::chrono::milliseconds getElapsedTime() const;
    
private:
    MonitorConfig config;
    std::atomic<bool> monitoring;
    std::atomic<bool> terminated;
    std::atomic<bool> shouldStop;
    
    mutable std::mutex statsMutex;
    CompileStats currentStats;
    std::chrono::steady_clock::time_point startTime;
    
    std::thread monitorThread;
    std::thread memoryMonitorThread;
    std::condition_variable cv;
    std::mutex cvMutex;
    
    pid_t processId;
    std::atomic<bool> processKilled;
    
    // 内部方法
    void monitorLoop();
    void memoryMonitorLoop();
    void checkMemoryUsage();
    void checkCompileTime();
    bool killProcess();
    size_t getProcessMemoryUsage() const;
    void updateMemoryStats();
    
    // 统计信息更新
    void updateCompileTime();
    void updateMemoryUsage();
    void updateFileStats(size_t files, size_t lines, size_t tokens);
    
    // 错误处理
    void handleError(const std::string& error);
    void handleTimeout();
    void handleMemoryLimit();
};

// 编译监视器工厂
class CompilerMonitorFactory {
public:
    static std::unique_ptr<CompilerMonitor> createMonitor();
    static std::unique_ptr<CompilerMonitor> createMonitor(const MonitorConfig& config);
    static MonitorConfig createDefaultConfig();
    static MonitorConfig createStrictConfig();
    static MonitorConfig createLooseConfig();
};

// 编译监视器管理器
class CompilerMonitorManager {
public:
    static CompilerMonitorManager& getInstance();
    
    // 注册监视器
    void registerMonitor(const std::string& name, std::shared_ptr<CompilerMonitor> monitor);
    
    // 获取监视器
    std::shared_ptr<CompilerMonitor> getMonitor(const std::string& name);
    
    // 移除监视器
    void removeMonitor(const std::string& name);
    
    // 获取所有监视器
    std::map<std::string, std::shared_ptr<CompilerMonitor>> getAllMonitors() const;
    
    // 清理所有监视器
    void clearAllMonitors();
    
private:
    CompilerMonitorManager() = default;
    std::map<std::string, std::shared_ptr<CompilerMonitor>> monitors;
    mutable std::mutex monitorsMutex;
};

} // namespace CHTL

#endif // CHTL_COMPILER_MONITOR_H