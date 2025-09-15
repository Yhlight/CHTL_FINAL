#pragma once

#include <chrono>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <map>
#include <string>
#include <iostream>

namespace CHTL {

/**
 * 编译状态枚举
 */
enum class CompilationStatus {
    IDLE,           // 空闲
    RUNNING,        // 运行中
    COMPLETED,      // 完成
    FAILED,         // 失败
    TIMEOUT,        // 超时
    KILLED,         // 被杀死
    MEMORY_LIMIT    // 内存限制
};

/**
 * 编译统计信息
 */
struct CompilationStats {
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    std::chrono::milliseconds duration;
    size_t peak_memory_usage;
    size_t current_memory_usage;
    size_t files_processed;
    size_t lines_processed;
    size_t errors_count;
    size_t warnings_count;
    CompilationStatus status;
    
    CompilationStats() 
        : peak_memory_usage(0)
        , current_memory_usage(0)
        , files_processed(0)
        , lines_processed(0)
        , errors_count(0)
        , warnings_count(0)
        , status(CompilationStatus::IDLE) {
    }
};

/**
 * 内存监视配置
 */
struct MemoryMonitorConfig {
    size_t max_memory_mb;           // 最大内存使用量(MB)
    size_t warning_threshold_mb;    // 警告阈值(MB)
    size_t critical_threshold_mb;   // 危险阈值(MB)
    bool auto_kill_enabled;         // 是否启用自动杀死
    bool memory_tracking_enabled;   // 是否启用内存跟踪
    
    MemoryMonitorConfig() 
        : max_memory_mb(1024)       // 默认1GB
        , warning_threshold_mb(512)  // 默认512MB警告
        , critical_threshold_mb(896) // 默认896MB危险
        , auto_kill_enabled(true)
        , memory_tracking_enabled(true) {
    }
};

/**
 * 时间监视配置
 */
struct TimeMonitorConfig {
    std::chrono::milliseconds max_compile_time;     // 最大编译时间
    std::chrono::milliseconds warning_time;         // 警告时间
    std::chrono::milliseconds critical_time;        // 危险时间
    bool auto_kill_enabled;                         // 是否启用自动杀死
    bool time_tracking_enabled;                     // 是否启用时间跟踪
    
    TimeMonitorConfig() 
        : max_compile_time(std::chrono::minutes(10))    // 默认10分钟
        , warning_time(std::chrono::minutes(5))         // 默认5分钟警告
        , critical_time(std::chrono::minutes(8))        // 默认8分钟危险
        , auto_kill_enabled(true)
        , time_tracking_enabled(true) {
    }
};

/**
 * 编译监视器配置
 */
struct CompilationMonitorConfig {
    MemoryMonitorConfig memory_config;
    TimeMonitorConfig time_config;
    bool enable_logging;                    // 是否启用日志
    bool enable_notifications;              // 是否启用通知
    std::string log_file_path;              // 日志文件路径
    std::function<void(const std::string&)> notification_callback;  // 通知回调
    
    CompilationMonitorConfig() 
        : enable_logging(true)
        , enable_notifications(true)
        , log_file_path("compilation_monitor.log") {
    }
};

/**
 * 内存监视器
 */
class MemoryMonitor {
public:
    MemoryMonitor(const MemoryMonitorConfig& config);
    ~MemoryMonitor();
    
    // 内存监视
    void startMonitoring();
    void stopMonitoring();
    bool isMonitoring() const;
    
    // 内存使用量获取
    size_t getCurrentMemoryUsage() const;
    size_t getPeakMemoryUsage() const;
    double getMemoryUsagePercentage() const;
    
    // 内存状态检查
    bool isMemoryUsageNormal() const;
    bool isMemoryUsageWarning() const;
    bool isMemoryUsageCritical() const;
    bool isMemoryUsageExceeded() const;
    
    // 配置管理
    void updateConfig(const MemoryMonitorConfig& config);
    MemoryMonitorConfig getConfig() const;
    
    // 统计信息
    void updateStats(CompilationStats& stats) const;
    void resetStats();
    
    // 事件回调
    void setMemoryWarningCallback(std::function<void(size_t)> callback);
    void setMemoryCriticalCallback(std::function<void(size_t)> callback);
    void setMemoryExceededCallback(std::function<void(size_t)> callback);
    
private:
    MemoryMonitorConfig config_;
    std::atomic<bool> monitoring_;
    std::atomic<size_t> peak_memory_usage_;
    std::atomic<size_t> current_memory_usage_;
    std::thread monitoring_thread_;
    std::mutex config_mutex_;
    
    // 回调函数
    std::function<void(size_t)> memory_warning_callback_;
    std::function<void(size_t)> memory_critical_callback_;
    std::function<void(size_t)> memory_exceeded_callback_;
    
    // 辅助函数
    void monitoringLoop();
    size_t getProcessMemoryUsage() const;
    void checkMemoryThresholds();
    void notifyMemoryWarning(size_t usage);
    void notifyMemoryCritical(size_t usage);
    void notifyMemoryExceeded(size_t usage);
};

/**
 * 时间监视器
 */
class TimeMonitor {
public:
    TimeMonitor(const TimeMonitorConfig& config);
    ~TimeMonitor();
    
    // 时间监视
    void startTiming();
    void stopTiming();
    void pauseTiming();
    void resumeTiming();
    bool isTiming() const;
    
    // 时间获取
    std::chrono::milliseconds getElapsedTime() const;
    std::chrono::milliseconds getRemainingTime() const;
    double getTimeProgress() const;
    
    // 时间状态检查
    bool isTimeNormal() const;
    bool isTimeWarning() const;
    bool isTimeCritical() const;
    bool isTimeExceeded() const;
    
    // 配置管理
    void updateConfig(const TimeMonitorConfig& config);
    TimeMonitorConfig getConfig() const;
    
    // 统计信息
    void updateStats(CompilationStats& stats) const;
    void resetStats();
    
    // 事件回调
    void setTimeWarningCallback(std::function<void(std::chrono::milliseconds)> callback);
    void setTimeCriticalCallback(std::function<void(std::chrono::milliseconds)> callback);
    void setTimeExceededCallback(std::function<void(std::chrono::milliseconds)> callback);
    
private:
    TimeMonitorConfig config_;
    std::atomic<bool> timing_;
    std::atomic<bool> paused_;
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point pause_start_;
    std::chrono::milliseconds paused_duration_;
    std::mutex config_mutex_;
    
    // 回调函数
    std::function<void(std::chrono::milliseconds)> time_warning_callback_;
    std::function<void(std::chrono::milliseconds)> time_critical_callback_;
    std::function<void(std::chrono::milliseconds)> time_exceeded_callback_;
    
    // 辅助函数
    void checkTimeThresholds();
    void notifyTimeWarning(std::chrono::milliseconds elapsed);
    void notifyTimeCritical(std::chrono::milliseconds elapsed);
    void notifyTimeExceeded(std::chrono::milliseconds elapsed);
};

/**
 * 编译监视器
 */
class CompilationMonitor {
public:
    CompilationMonitor(const CompilationMonitorConfig& config);
    ~CompilationMonitor();
    
    // 编译监视
    void startCompilation();
    void stopCompilation();
    void pauseCompilation();
    void resumeCompilation();
    void killCompilation();
    
    // 状态查询
    CompilationStatus getStatus() const;
    bool isCompiling() const;
    bool isPaused() const;
    bool isKilled() const;
    
    // 统计信息
    CompilationStats getStats() const;
    void updateStats(const CompilationStats& stats);
    void resetStats();
    
    // 配置管理
    void updateConfig(const CompilationMonitorConfig& config);
    CompilationMonitorConfig getConfig() const;
    
    // 监视器管理
    MemoryMonitor& getMemoryMonitor();
    TimeMonitor& getTimeMonitor();
    
    // 事件处理
    void setCompilationStartCallback(std::function<void()> callback);
    void setCompilationCompleteCallback(std::function<void(CompilationStatus)> callback);
    void setCompilationErrorCallback(std::function<void(const std::string&)> callback);
    void setCompilationKillCallback(std::function<void()> callback);
    
    // 日志和通知
    void logMessage(const std::string& message);
    void notifyUser(const std::string& message);
    
    // 自动杀死功能
    void enableAutoKill(bool enabled);
    bool isAutoKillEnabled() const;
    void checkAndKillIfNeeded();
    
    // 报告生成
    std::string generateReport() const;
    void saveReportToFile(const std::string& filepath) const;
    
private:
    CompilationMonitorConfig config_;
    std::atomic<CompilationStatus> status_;
    std::atomic<bool> auto_kill_enabled_;
    CompilationStats stats_;
    
    // 监视器
    std::unique_ptr<MemoryMonitor> memory_monitor_;
    std::unique_ptr<TimeMonitor> time_monitor_;
    
    // 线程安全
    mutable std::mutex stats_mutex_;
    mutable std::mutex config_mutex_;
    
    // 回调函数
    std::function<void()> compilation_start_callback_;
    std::function<void(CompilationStatus)> compilation_complete_callback_;
    std::function<void(const std::string&)> compilation_error_callback_;
    std::function<void()> compilation_kill_callback_;
    
    // 辅助函数
    void initializeMonitors();
    void setupMonitorCallbacks();
    void checkThresholds();
    void handleMemoryWarning(size_t usage);
    void handleMemoryCritical(size_t usage);
    void handleMemoryExceeded(size_t usage);
    void handleTimeWarning(std::chrono::milliseconds elapsed);
    void handleTimeCritical(std::chrono::milliseconds elapsed);
    void handleTimeExceeded(std::chrono::milliseconds elapsed);
    void performAutoKill();
    std::string formatDuration(std::chrono::milliseconds duration) const;
    std::string formatMemorySize(size_t bytes) const;
};

/**
 * 编译监视器管理器
 */
class CompilationMonitorManager {
public:
    CompilationMonitorManager();
    ~CompilationMonitorManager();
    
    // 监视器管理
    std::shared_ptr<CompilationMonitor> createMonitor(const CompilationMonitorConfig& config);
    std::shared_ptr<CompilationMonitor> getMonitor(const std::string& id);
    void removeMonitor(const std::string& id);
    void removeAllMonitors();
    
    // 全局配置
    void setGlobalConfig(const CompilationMonitorConfig& config);
    CompilationMonitorConfig getGlobalConfig() const;
    
    // 全局状态
    std::vector<std::string> getActiveMonitorIds() const;
    size_t getActiveMonitorCount() const;
    bool hasActiveMonitors() const;
    
    // 全局统计
    std::map<std::string, CompilationStats> getAllStats() const;
    std::string generateGlobalReport() const;
    void saveGlobalReportToFile(const std::string& filepath) const;
    
    // 全局控制
    void pauseAllMonitors();
    void resumeAllMonitors();
    void killAllMonitors();
    void resetAllStats();
    
private:
    CompilationMonitorConfig global_config_;
    std::map<std::string, std::shared_ptr<CompilationMonitor>> monitors_;
    mutable std::mutex monitors_mutex_;
    
    // 辅助函数
    std::string generateMonitorId() const;
    void cleanupInactiveMonitors();
};

/**
 * 编译监视器工具类
 */
class CompilationMonitorUtils {
public:
    // 内存工具
    static size_t getProcessMemoryUsage();
    static size_t getSystemMemoryTotal();
    static size_t getSystemMemoryAvailable();
    static double getSystemMemoryUsagePercentage();
    
    // 时间工具
    static std::chrono::milliseconds getCurrentTime();
    static std::string formatTime(std::chrono::milliseconds time);
    static std::string formatDuration(std::chrono::milliseconds duration);
    
    // 系统工具
    static std::string getProcessId();
    static std::string getSystemInfo();
    static bool isProcessRunning(const std::string& pid);
    
    // 文件工具
    static bool writeToFile(const std::string& filepath, const std::string& content);
    static std::string readFromFile(const std::string& filepath);
    static bool fileExists(const std::string& filepath);
    
    // 日志工具
    static void logToFile(const std::string& filepath, const std::string& message);
    static std::vector<std::string> readLogFile(const std::string& filepath);
    static void clearLogFile(const std::string& filepath);
    
private:
    CompilationMonitorUtils() = delete;
};

} // namespace CHTL