#include "CHTL/CompilationMonitor.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <random>
#include <ctime>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/resource.h>
#include <fstream>
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/task.h>
#endif

namespace CHTL {

// MemoryMonitor 实现
MemoryMonitor::MemoryMonitor(const MemoryMonitorConfig& config) 
    : config_(config)
    , monitoring_(false)
    , peak_memory_usage_(0)
    , current_memory_usage_(0) {
}

MemoryMonitor::~MemoryMonitor() {
    stopMonitoring();
}

void MemoryMonitor::startMonitoring() {
    if (monitoring_.load()) {
        return;
    }
    
    monitoring_.store(true);
    peak_memory_usage_.store(0);
    current_memory_usage_.store(0);
    
    monitoring_thread_ = std::thread(&MemoryMonitor::monitoringLoop, this);
}

void MemoryMonitor::stopMonitoring() {
    if (!monitoring_.load()) {
        return;
    }
    
    monitoring_.store(false);
    
    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }
}

bool MemoryMonitor::isMonitoring() const {
    return monitoring_.load();
}

size_t MemoryMonitor::getCurrentMemoryUsage() const {
    return current_memory_usage_.load();
}

size_t MemoryMonitor::getPeakMemoryUsage() const {
    return peak_memory_usage_.load();
}

double MemoryMonitor::getMemoryUsagePercentage() const {
    size_t current = getCurrentMemoryUsage();
    size_t max_memory = config_.max_memory_mb * 1024 * 1024;
    return static_cast<double>(current) / max_memory * 100.0;
}

bool MemoryMonitor::isMemoryUsageNormal() const {
    size_t current = getCurrentMemoryUsage();
    size_t warning_threshold = config_.warning_threshold_mb * 1024 * 1024;
    return current < warning_threshold;
}

bool MemoryMonitor::isMemoryUsageWarning() const {
    size_t current = getCurrentMemoryUsage();
    size_t warning_threshold = config_.warning_threshold_mb * 1024 * 1024;
    size_t critical_threshold = config_.critical_threshold_mb * 1024 * 1024;
    return current >= warning_threshold && current < critical_threshold;
}

bool MemoryMonitor::isMemoryUsageCritical() const {
    size_t current = getCurrentMemoryUsage();
    size_t critical_threshold = config_.critical_threshold_mb * 1024 * 1024;
    size_t max_memory = config_.max_memory_mb * 1024 * 1024;
    return current >= critical_threshold && current < max_memory;
}

bool MemoryMonitor::isMemoryUsageExceeded() const {
    size_t current = getCurrentMemoryUsage();
    size_t max_memory = config_.max_memory_mb * 1024 * 1024;
    return current >= max_memory;
}

void MemoryMonitor::updateConfig(const MemoryMonitorConfig& config) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(config_mutex_));
    config_ = config;
}

MemoryMonitorConfig MemoryMonitor::getConfig() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(config_mutex_));
    return config_;
}

void MemoryMonitor::updateStats(CompilationStats& stats) const {
    stats.current_memory_usage = getCurrentMemoryUsage();
    stats.peak_memory_usage = getPeakMemoryUsage();
}

void MemoryMonitor::resetStats() {
    peak_memory_usage_.store(0);
    current_memory_usage_.store(0);
}

void MemoryMonitor::setMemoryWarningCallback(std::function<void(size_t)> callback) {
    memory_warning_callback_ = callback;
}

void MemoryMonitor::setMemoryCriticalCallback(std::function<void(size_t)> callback) {
    memory_critical_callback_ = callback;
}

void MemoryMonitor::setMemoryExceededCallback(std::function<void(size_t)> callback) {
    memory_exceeded_callback_ = callback;
}

void MemoryMonitor::monitoringLoop() {
    while (monitoring_.load()) {
        size_t current_usage = getProcessMemoryUsage();
        current_memory_usage_.store(current_usage);
        
        size_t peak = peak_memory_usage_.load();
        if (current_usage > peak) {
            peak_memory_usage_.store(current_usage);
        }
        
        checkMemoryThresholds();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

size_t MemoryMonitor::getProcessMemoryUsage() const {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize;
    }
#elif defined(__linux__)
    std::ifstream status_file("/proc/self/status");
    std::string line;
    while (std::getline(status_file, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            std::istringstream iss(line.substr(6));
            size_t value;
            std::string unit;
            iss >> value >> unit;
            if (unit == "kB") {
                return value * 1024;
            }
        }
    }
#elif defined(__APPLE__)
    task_t task = mach_task_self();
    struct task_basic_info info;
    mach_msg_type_number_t size = TASK_BASIC_INFO_COUNT;
    if (task_info(task, TASK_BASIC_INFO, (task_info_t)&info, &size) == KERN_SUCCESS) {
        return info.resident_size;
    }
#endif
    return 0;
}

void MemoryMonitor::checkMemoryThresholds() {
    size_t current = getCurrentMemoryUsage();
    
    if (isMemoryUsageExceeded()) {
        notifyMemoryExceeded(current);
    } else if (isMemoryUsageCritical()) {
        notifyMemoryCritical(current);
    } else if (isMemoryUsageWarning()) {
        notifyMemoryWarning(current);
    }
}

void MemoryMonitor::notifyMemoryWarning(size_t usage) {
    if (memory_warning_callback_) {
        memory_warning_callback_(usage);
    }
}

void MemoryMonitor::notifyMemoryCritical(size_t usage) {
    if (memory_critical_callback_) {
        memory_critical_callback_(usage);
    }
}

void MemoryMonitor::notifyMemoryExceeded(size_t usage) {
    if (memory_exceeded_callback_) {
        memory_exceeded_callback_(usage);
    }
}

// TimeMonitor 实现
TimeMonitor::TimeMonitor(const TimeMonitorConfig& config) 
    : config_(config)
    , timing_(false)
    , paused_(false)
    , paused_duration_(std::chrono::milliseconds(0)) {
}

TimeMonitor::~TimeMonitor() {
    stopTiming();
}

void TimeMonitor::startTiming() {
    if (timing_.load()) {
        return;
    }
    
    timing_.store(true);
    paused_.store(false);
    start_time_ = std::chrono::steady_clock::now();
    paused_duration_ = std::chrono::milliseconds(0);
}

void TimeMonitor::stopTiming() {
    timing_.store(false);
    paused_.store(false);
}

void TimeMonitor::pauseTiming() {
    if (!timing_.load() || paused_.load()) {
        return;
    }
    
    paused_.store(true);
    pause_start_ = std::chrono::steady_clock::now();
}

void TimeMonitor::resumeTiming() {
    if (!timing_.load() || !paused_.load()) {
        return;
    }
    
    auto pause_end = std::chrono::steady_clock::now();
    auto pause_duration = std::chrono::duration_cast<std::chrono::milliseconds>(pause_end - pause_start_);
    paused_duration_ += pause_duration;
    paused_.store(false);
}

bool TimeMonitor::isTiming() const {
    return timing_.load();
}

std::chrono::milliseconds TimeMonitor::getElapsedTime() const {
    if (!timing_.load()) {
        return std::chrono::milliseconds(0);
    }
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_);
    
    if (paused_.load()) {
        auto pause_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - pause_start_);
        elapsed -= pause_duration;
    }
    
    elapsed -= paused_duration_;
    return elapsed;
}

std::chrono::milliseconds TimeMonitor::getRemainingTime() const {
    auto elapsed = getElapsedTime();
    auto remaining = config_.max_compile_time - elapsed;
    return std::max(remaining, std::chrono::milliseconds(0));
}

double TimeMonitor::getTimeProgress() const {
    auto elapsed = getElapsedTime();
    auto max_time = config_.max_compile_time;
    return std::min(static_cast<double>(elapsed.count()) / max_time.count(), 1.0);
}

bool TimeMonitor::isTimeNormal() const {
    auto elapsed = getElapsedTime();
    return elapsed < config_.warning_time;
}

bool TimeMonitor::isTimeWarning() const {
    auto elapsed = getElapsedTime();
    return elapsed >= config_.warning_time && elapsed < config_.critical_time;
}

bool TimeMonitor::isTimeCritical() const {
    auto elapsed = getElapsedTime();
    return elapsed >= config_.critical_time && elapsed < config_.max_compile_time;
}

bool TimeMonitor::isTimeExceeded() const {
    auto elapsed = getElapsedTime();
    return elapsed >= config_.max_compile_time;
}

void TimeMonitor::updateConfig(const TimeMonitorConfig& config) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(config_mutex_));
    config_ = config;
}

TimeMonitorConfig TimeMonitor::getConfig() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(config_mutex_));
    return config_;
}

void TimeMonitor::updateStats(CompilationStats& stats) const {
    stats.start_time = start_time_;
    stats.end_time = std::chrono::steady_clock::now();
    stats.duration = getElapsedTime();
}

void TimeMonitor::resetStats() {
    start_time_ = std::chrono::steady_clock::now();
    paused_duration_ = std::chrono::milliseconds(0);
}

void TimeMonitor::setTimeWarningCallback(std::function<void(std::chrono::milliseconds)> callback) {
    time_warning_callback_ = callback;
}

void TimeMonitor::setTimeCriticalCallback(std::function<void(std::chrono::milliseconds)> callback) {
    time_critical_callback_ = callback;
}

void TimeMonitor::setTimeExceededCallback(std::function<void(std::chrono::milliseconds)> callback) {
    time_exceeded_callback_ = callback;
}

void TimeMonitor::checkTimeThresholds() {
    auto elapsed = getElapsedTime();
    
    if (isTimeExceeded()) {
        notifyTimeExceeded(elapsed);
    } else if (isTimeCritical()) {
        notifyTimeCritical(elapsed);
    } else if (isTimeWarning()) {
        notifyTimeWarning(elapsed);
    }
}

void TimeMonitor::notifyTimeWarning(std::chrono::milliseconds elapsed) {
    if (time_warning_callback_) {
        time_warning_callback_(elapsed);
    }
}

void TimeMonitor::notifyTimeCritical(std::chrono::milliseconds elapsed) {
    if (time_critical_callback_) {
        time_critical_callback_(elapsed);
    }
}

void TimeMonitor::notifyTimeExceeded(std::chrono::milliseconds elapsed) {
    if (time_exceeded_callback_) {
        time_exceeded_callback_(elapsed);
    }
}

// CompilationMonitor 实现
CompilationMonitor::CompilationMonitor(const CompilationMonitorConfig& config) 
    : config_(config)
    , status_(CompilationStatus::IDLE)
    , auto_kill_enabled_(true) {
    initializeMonitors();
    setupMonitorCallbacks();
}

CompilationMonitor::~CompilationMonitor() {
    stopCompilation();
}

void CompilationMonitor::startCompilation() {
    if (status_.load() == CompilationStatus::RUNNING) {
        return;
    }
    
    status_.store(CompilationStatus::RUNNING);
    stats_ = CompilationStats();
    stats_.start_time = std::chrono::steady_clock::now();
    stats_.status = CompilationStatus::RUNNING;
    
    if (config_.memory_config.memory_tracking_enabled) {
        memory_monitor_->startMonitoring();
    }
    
    if (config_.time_config.time_tracking_enabled) {
        time_monitor_->startTiming();
    }
    
    if (compilation_start_callback_) {
        compilation_start_callback_();
    }
    
    logMessage("Compilation started");
}

void CompilationMonitor::stopCompilation() {
    if (status_.load() == CompilationStatus::IDLE) {
        return;
    }
    
    status_.store(CompilationStatus::COMPLETED);
    stats_.end_time = std::chrono::steady_clock::now();
    stats_.duration = std::chrono::duration_cast<std::chrono::milliseconds>(stats_.end_time - stats_.start_time);
    stats_.status = CompilationStatus::COMPLETED;
    
    if (memory_monitor_->isMonitoring()) {
        memory_monitor_->stopMonitoring();
    }
    
    if (time_monitor_->isTiming()) {
        time_monitor_->stopTiming();
    }
    
    memory_monitor_->updateStats(stats_);
    time_monitor_->updateStats(stats_);
    
    if (compilation_complete_callback_) {
        compilation_complete_callback_(CompilationStatus::COMPLETED);
    }
    
    logMessage("Compilation completed");
}

void CompilationMonitor::pauseCompilation() {
    if (status_.load() != CompilationStatus::RUNNING) {
        return;
    }
    
    if (time_monitor_->isTiming()) {
        time_monitor_->pauseTiming();
    }
    
    logMessage("Compilation paused");
}

void CompilationMonitor::resumeCompilation() {
    if (status_.load() != CompilationStatus::RUNNING) {
        return;
    }
    
    if (time_monitor_->isTiming()) {
        time_monitor_->resumeTiming();
    }
    
    logMessage("Compilation resumed");
}

void CompilationMonitor::killCompilation() {
    if (status_.load() == CompilationStatus::IDLE) {
        return;
    }
    
    status_.store(CompilationStatus::KILLED);
    stats_.end_time = std::chrono::steady_clock::now();
    stats_.duration = std::chrono::duration_cast<std::chrono::milliseconds>(stats_.end_time - stats_.start_time);
    stats_.status = CompilationStatus::KILLED;
    
    if (memory_monitor_->isMonitoring()) {
        memory_monitor_->stopMonitoring();
    }
    
    if (time_monitor_->isTiming()) {
        time_monitor_->stopTiming();
    }
    
    if (compilation_kill_callback_) {
        compilation_kill_callback_();
    }
    
    logMessage("Compilation killed");
}

CompilationStatus CompilationMonitor::getStatus() const {
    return status_.load();
}

bool CompilationMonitor::isCompiling() const {
    return status_.load() == CompilationStatus::RUNNING;
}

bool CompilationMonitor::isPaused() const {
    return time_monitor_->isTiming() && time_monitor_->isTiming();
}

bool CompilationMonitor::isKilled() const {
    return status_.load() == CompilationStatus::KILLED;
}

CompilationStats CompilationMonitor::getStats() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    return stats_;
}

void CompilationMonitor::updateStats(const CompilationStats& stats) {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    stats_ = stats;
}

void CompilationMonitor::resetStats() {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    stats_ = CompilationStats();
}

void CompilationMonitor::updateConfig(const CompilationMonitorConfig& config) {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(config_mutex_));
    config_ = config;
    memory_monitor_->updateConfig(config.memory_config);
    time_monitor_->updateConfig(config.time_config);
}

CompilationMonitorConfig CompilationMonitor::getConfig() const {
    std::lock_guard<std::mutex> lock(const_cast<std::mutex&>(config_mutex_));
    return config_;
}

MemoryMonitor& CompilationMonitor::getMemoryMonitor() {
    return *memory_monitor_;
}

TimeMonitor& CompilationMonitor::getTimeMonitor() {
    return *time_monitor_;
}

void CompilationMonitor::setCompilationStartCallback(std::function<void()> callback) {
    compilation_start_callback_ = callback;
}

void CompilationMonitor::setCompilationCompleteCallback(std::function<void(CompilationStatus)> callback) {
    compilation_complete_callback_ = callback;
}

void CompilationMonitor::setCompilationErrorCallback(std::function<void(const std::string&)> callback) {
    compilation_error_callback_ = callback;
}

void CompilationMonitor::setCompilationKillCallback(std::function<void()> callback) {
    compilation_kill_callback_ = callback;
}

void CompilationMonitor::logMessage(const std::string& message) {
    if (!config_.enable_logging) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";
    oss << message;
    
    CompilationMonitorUtils::logToFile(config_.log_file_path, oss.str());
}

void CompilationMonitor::notifyUser(const std::string& message) {
    if (!config_.enable_notifications) {
        return;
    }
    
    if (config_.notification_callback) {
        config_.notification_callback(message);
    } else {
        std::cout << "[NOTIFICATION] " << message << std::endl;
    }
}

void CompilationMonitor::enableAutoKill(bool enabled) {
    auto_kill_enabled_.store(enabled);
}

bool CompilationMonitor::isAutoKillEnabled() const {
    return auto_kill_enabled_.load();
}

void CompilationMonitor::checkAndKillIfNeeded() {
    if (!auto_kill_enabled_.load()) {
        return;
    }
    
    bool should_kill = false;
    std::string reason;
    
    if (memory_monitor_->isMemoryUsageExceeded()) {
        should_kill = true;
        reason = "Memory usage exceeded limit";
    } else if (time_monitor_->isTimeExceeded()) {
        should_kill = true;
        reason = "Compilation time exceeded limit";
    }
    
    if (should_kill) {
        logMessage("Auto-killing compilation: " + reason);
        notifyUser("Compilation auto-killed: " + reason);
        killCompilation();
    }
}

std::string CompilationMonitor::generateReport() const {
    std::ostringstream oss;
    
    auto stats = getStats();
    
    oss << "=== CHTL Compilation Monitor Report ===\n";
    oss << "Status: ";
    switch (stats.status) {
        case CompilationStatus::IDLE: oss << "IDLE"; break;
        case CompilationStatus::RUNNING: oss << "RUNNING"; break;
        case CompilationStatus::COMPLETED: oss << "COMPLETED"; break;
        case CompilationStatus::FAILED: oss << "FAILED"; break;
        case CompilationStatus::TIMEOUT: oss << "TIMEOUT"; break;
        case CompilationStatus::KILLED: oss << "KILLED"; break;
        case CompilationStatus::MEMORY_LIMIT: oss << "MEMORY_LIMIT"; break;
    }
    oss << "\n";
    
    oss << "Duration: " << formatDuration(stats.duration) << "\n";
    oss << "Peak Memory Usage: " << formatMemorySize(stats.peak_memory_usage) << "\n";
    oss << "Current Memory Usage: " << formatMemorySize(stats.current_memory_usage) << "\n";
    oss << "Files Processed: " << stats.files_processed << "\n";
    oss << "Lines Processed: " << stats.lines_processed << "\n";
    oss << "Errors: " << stats.errors_count << "\n";
    oss << "Warnings: " << stats.warnings_count << "\n";
    
    return oss.str();
}

void CompilationMonitor::saveReportToFile(const std::string& filepath) const {
    std::string report = generateReport();
    CompilationMonitorUtils::writeToFile(filepath, report);
}

void CompilationMonitor::initializeMonitors() {
    memory_monitor_ = std::make_unique<MemoryMonitor>(config_.memory_config);
    time_monitor_ = std::make_unique<TimeMonitor>(config_.time_config);
}

void CompilationMonitor::setupMonitorCallbacks() {
    memory_monitor_->setMemoryWarningCallback([this](size_t usage) {
        handleMemoryWarning(usage);
    });
    
    memory_monitor_->setMemoryCriticalCallback([this](size_t usage) {
        handleMemoryCritical(usage);
    });
    
    memory_monitor_->setMemoryExceededCallback([this](size_t usage) {
        handleMemoryExceeded(usage);
    });
    
    time_monitor_->setTimeWarningCallback([this](std::chrono::milliseconds elapsed) {
        handleTimeWarning(elapsed);
    });
    
    time_monitor_->setTimeCriticalCallback([this](std::chrono::milliseconds elapsed) {
        handleTimeCritical(elapsed);
    });
    
    time_monitor_->setTimeExceededCallback([this](std::chrono::milliseconds elapsed) {
        handleTimeExceeded(elapsed);
    });
}

void CompilationMonitor::checkThresholds() {
    checkAndKillIfNeeded();
}

void CompilationMonitor::handleMemoryWarning(size_t usage) {
    std::ostringstream oss;
    oss << "Memory warning: " << formatMemorySize(usage) << " used";
    logMessage(oss.str());
    notifyUser(oss.str());
}

void CompilationMonitor::handleMemoryCritical(size_t usage) {
    std::ostringstream oss;
    oss << "Memory critical: " << formatMemorySize(usage) << " used";
    logMessage(oss.str());
    notifyUser(oss.str());
}

void CompilationMonitor::handleMemoryExceeded(size_t usage) {
    std::ostringstream oss;
    oss << "Memory exceeded: " << formatMemorySize(usage) << " used";
    logMessage(oss.str());
    notifyUser(oss.str());
    
    if (auto_kill_enabled_.load()) {
        performAutoKill();
    }
}

void CompilationMonitor::handleTimeWarning(std::chrono::milliseconds elapsed) {
    std::ostringstream oss;
    oss << "Time warning: " << formatDuration(elapsed) << " elapsed";
    logMessage(oss.str());
    notifyUser(oss.str());
}

void CompilationMonitor::handleTimeCritical(std::chrono::milliseconds elapsed) {
    std::ostringstream oss;
    oss << "Time critical: " << formatDuration(elapsed) << " elapsed";
    logMessage(oss.str());
    notifyUser(oss.str());
}

void CompilationMonitor::handleTimeExceeded(std::chrono::milliseconds elapsed) {
    std::ostringstream oss;
    oss << "Time exceeded: " << formatDuration(elapsed) << " elapsed";
    logMessage(oss.str());
    notifyUser(oss.str());
    
    if (auto_kill_enabled_.load()) {
        performAutoKill();
    }
}

void CompilationMonitor::performAutoKill() {
    logMessage("Performing auto-kill due to resource limits");
    killCompilation();
}

std::string CompilationMonitor::formatDuration(std::chrono::milliseconds duration) const {
    auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration % std::chrono::hours(1));
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration % std::chrono::minutes(1));
    auto ms = duration % std::chrono::seconds(1);
    
    std::ostringstream oss;
    if (hours.count() > 0) {
        oss << hours.count() << "h ";
    }
    if (minutes.count() > 0) {
        oss << minutes.count() << "m ";
    }
    if (seconds.count() > 0) {
        oss << seconds.count() << "s ";
    }
    oss << ms.count() << "ms";
    
    return oss.str();
}

std::string CompilationMonitor::formatMemorySize(size_t bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit_index];
    return oss.str();
}

} // namespace CHTL