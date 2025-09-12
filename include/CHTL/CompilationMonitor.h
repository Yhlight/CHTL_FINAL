#pragma once

#include <chrono>
#include <memory>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <map>
#include <string>

namespace CHTL {

/**
 * 编译统计信息
 */
struct CompilationStats {
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    std::chrono::milliseconds duration;
    size_t peak_memory_usage;
    size_t current_memory_usage;
    bool was_killed;
    std::string reason;
    
    CompilationStats() : peak_memory_usage(0), current_memory_usage(0), was_killed(false) {
        start_time = std::chrono::steady_clock::now();
    }
    
    void updateMemoryUsage(size_t usage) {
        current_memory_usage = usage;
        if (usage > peak_memory_usage) {
            peak_memory_usage = usage;
        }
    }
    
    void finish() {
        end_time = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    }
    
    double getDurationSeconds() const {
        return duration.count() / 1000.0;
    }
    
    double getPeakMemoryMB() const {
        return peak_memory_usage / (1024.0 * 1024.0);
    }
    
    double getCurrentMemoryMB() const {
        return current_memory_usage / (1024.0 * 1024.0);
    }
};

/**
 * 编译监视器配置
 */
struct MonitorConfig {
    std::chrono::milliseconds max_compilation_time;
    size_t max_memory_usage_mb;
    bool enable_memory_monitoring;
    bool enable_time_monitoring;
    bool auto_kill_on_limit;
    std::chrono::milliseconds memory_check_interval;
    std::chrono::milliseconds time_check_interval;
    
    MonitorConfig() 
        : max_compilation_time(std::chrono::minutes(5))
        , max_memory_usage_mb(512)
        , enable_memory_monitoring(true)
        , enable_time_monitoring(true)
        , auto_kill_on_limit(true)
        , memory_check_interval(std::chrono::milliseconds(100))
        , time_check_interval(std::chrono::milliseconds(1000))
    {}
};

/**
 * 编译监视器回调
 */
class CompilationMonitorCallback {
public:
    virtual ~CompilationMonitorCallback() = default;
    
    virtual void onCompilationStart(const std::string& task_id) {}
    virtual void onCompilationProgress(const std::string& task_id, const CompilationStats& stats) {}
    virtual void onCompilationComplete(const std::string& task_id, const CompilationStats& stats) {}
    virtual void onCompilationKilled(const std::string& task_id, const CompilationStats& stats, const std::string& reason) {}
    virtual void onMemoryWarning(const std::string& task_id, size_t current_usage, size_t max_usage) {}
    virtual void onTimeWarning(const std::string& task_id, std::chrono::milliseconds current_time, std::chrono::milliseconds max_time) {}
};

/**
 * 编译监视器
 */
class CompilationMonitor {
public:
    CompilationMonitor();
    ~CompilationMonitor();
    
    // 配置
    void setConfig(const MonitorConfig& config);
    const MonitorConfig& getConfig() const;
    
    // 回调设置
    void setCallback(std::shared_ptr<CompilationMonitorCallback> callback);
    void removeCallback();
    
    // 监视控制
    std::string startMonitoring(const std::string& task_name = "");
    void stopMonitoring(const std::string& task_id);
    void stopAllMonitoring();
    
    // 状态查询
    bool isMonitoring(const std::string& task_id) const;
    bool isAnyMonitoring() const;
    std::vector<std::string> getActiveTasks() const;
    
    // 统计信息
    CompilationStats getStats(const std::string& task_id) const;
    std::map<std::string, CompilationStats> getAllStats() const;
    
    // 手动控制
    void killTask(const std::string& task_id, const std::string& reason = "Manual kill");
    void killAllTasks(const std::string& reason = "Manual kill all");
    
    // 清理
    void clearStats();
    void clearCompletedTasks();
    
    // 静态方法
    static size_t getCurrentMemoryUsage();
    static std::chrono::steady_clock::time_point getCurrentTime();

private:
    MonitorConfig config_;
    std::shared_ptr<CompilationMonitorCallback> callback_;
    
    mutable std::mutex stats_mutex_;
    std::map<std::string, CompilationStats> active_tasks_;
    std::map<std::string, CompilationStats> completed_tasks_;
    
    std::atomic<bool> monitoring_enabled_;
    std::thread monitoring_thread_;
    std::condition_variable monitoring_cv_;
    std::mutex monitoring_mutex_;
    
    // 监视线程
    void monitoringLoop();
    void checkMemoryUsage();
    void checkTimeUsage();
    void updateTaskStats(const std::string& task_id);
    
    // 内存使用获取
    size_t getProcessMemoryUsage() const;
    
    // 任务管理
    std::string generateTaskId() const;
    void addTask(const std::string& task_id, const std::string& task_name);
    void removeTask(const std::string& task_id);
    void moveTaskToCompleted(const std::string& task_id);
    
    // 回调调用
    void notifyCompilationStart(const std::string& task_id);
    void notifyCompilationProgress(const std::string& task_id, const CompilationStats& stats);
    void notifyCompilationComplete(const std::string& task_id, const CompilationStats& stats);
    void notifyCompilationKilled(const std::string& task_id, const CompilationStats& stats, const std::string& reason);
    void notifyMemoryWarning(const std::string& task_id, size_t current_usage, size_t max_usage);
    void notifyTimeWarning(const std::string& task_id, std::chrono::milliseconds current_time, std::chrono::milliseconds max_time);
};

/**
 * 编译监视器管理器
 */
class CompilationMonitorManager {
public:
    CompilationMonitorManager();
    ~CompilationMonitorManager();
    
    // 单例访问
    static CompilationMonitorManager& getInstance();
    
    // 监视器管理
    CompilationMonitor& getMonitor();
    void setGlobalConfig(const MonitorConfig& config);
    void setGlobalCallback(std::shared_ptr<CompilationMonitorCallback> callback);
    
    // 便捷方法
    std::string startTask(const std::string& task_name = "");
    void stopTask(const std::string& task_id);
    void stopAllTasks();
    
    // 统计信息
    CompilationStats getTaskStats(const std::string& task_id) const;
    std::map<std::string, CompilationStats> getAllTaskStats() const;
    
    // 清理
    void clearAllStats();
    void clearCompletedTasks();

private:
    std::unique_ptr<CompilationMonitor> monitor_;
    static std::unique_ptr<CompilationMonitorManager> instance_;
    static std::mutex instance_mutex_;
};

/**
 * RAII编译监视器
 */
class ScopedCompilationMonitor {
public:
    ScopedCompilationMonitor(const std::string& task_name = "");
    ~ScopedCompilationMonitor();
    
    // 禁用拷贝
    ScopedCompilationMonitor(const ScopedCompilationMonitor&) = delete;
    ScopedCompilationMonitor& operator=(const ScopedCompilationMonitor&) = delete;
    
    // 移动构造
    ScopedCompilationMonitor(ScopedCompilationMonitor&& other) noexcept;
    ScopedCompilationMonitor& operator=(ScopedCompilationMonitor&& other) noexcept;
    
    // 状态查询
    const std::string& getTaskId() const;
    bool isActive() const;
    CompilationStats getStats() const;
    
    // 手动控制
    void finish();
    void kill(const std::string& reason = "Manual kill");

private:
    std::string task_id_;
    bool active_;
    CompilationMonitor& monitor_;
};

/**
 * 默认编译监视器回调实现
 */
class DefaultCompilationMonitorCallback : public CompilationMonitorCallback {
public:
    DefaultCompilationMonitorCallback();
    ~DefaultCompilationMonitorCallback() = default;
    
    void onCompilationStart(const std::string& task_id) override;
    void onCompilationProgress(const std::string& task_id, const CompilationStats& stats) override;
    void onCompilationComplete(const std::string& task_id, const CompilationStats& stats) override;
    void onCompilationKilled(const std::string& task_id, const CompilationStats& stats, const std::string& reason) override;
    void onMemoryWarning(const std::string& task_id, size_t current_usage, size_t max_usage) override;
    void onTimeWarning(const std::string& task_id, std::chrono::milliseconds current_time, std::chrono::milliseconds max_time) override;
    
    // 配置
    void setVerbose(bool verbose);
    void setShowProgress(bool show_progress);
    void setShowWarnings(bool show_warnings);
    
private:
    bool verbose_;
    bool show_progress_;
    bool show_warnings_;
    
    void printStats(const CompilationStats& stats);
    void printMemoryWarning(size_t current_usage, size_t max_usage);
    void printTimeWarning(std::chrono::milliseconds current_time, std::chrono::milliseconds max_time);
};

} // namespace CHTL