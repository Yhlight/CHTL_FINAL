#include "CHTL/CompilationMonitor.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <random>
#include <iomanip>

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

// CompilationMonitor 实现
CompilationMonitor::CompilationMonitor() 
    : monitoring_enabled_(false) {
}

CompilationMonitor::~CompilationMonitor() {
    stopAllMonitoring();
}

void CompilationMonitor::setConfig(const MonitorConfig& config) {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    config_ = config;
}

const MonitorConfig& CompilationMonitor::getConfig() const {
    return config_;
}

void CompilationMonitor::setCallback(std::shared_ptr<CompilationMonitorCallback> callback) {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    callback_ = callback;
}

void CompilationMonitor::removeCallback() {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    callback_.reset();
}

std::string CompilationMonitor::startMonitoring(const std::string& task_name) {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    std::string task_id = generateTaskId();
    addTask(task_id, task_name);
    
    if (!monitoring_enabled_) {
        monitoring_enabled_ = true;
        monitoring_thread_ = std::thread(&CompilationMonitor::monitoringLoop, this);
    }
    
    notifyCompilationStart(task_id);
    return task_id;
}

void CompilationMonitor::stopMonitoring(const std::string& task_id) {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    auto it = active_tasks_.find(task_id);
    if (it != active_tasks_.end()) {
        it->second.finish();
        notifyCompilationComplete(task_id, it->second);
        moveTaskToCompleted(task_id);
    }
    
    if (active_tasks_.empty()) {
        monitoring_enabled_ = false;
        monitoring_cv_.notify_all();
    }
}

void CompilationMonitor::stopAllMonitoring() {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    for (auto& pair : active_tasks_) {
        pair.second.finish();
        notifyCompilationComplete(pair.first, pair.second);
    }
    
    completed_tasks_.insert(active_tasks_.begin(), active_tasks_.end());
    active_tasks_.clear();
    
    monitoring_enabled_ = false;
    monitoring_cv_.notify_all();
    
    if (monitoring_thread_.joinable()) {
        monitoring_thread_.join();
    }
}

bool CompilationMonitor::isMonitoring(const std::string& task_id) const {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    return active_tasks_.find(task_id) != active_tasks_.end();
}

bool CompilationMonitor::isAnyMonitoring() const {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    return !active_tasks_.empty();
}

std::vector<std::string> CompilationMonitor::getActiveTasks() const {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    std::vector<std::string> tasks;
    for (const auto& pair : active_tasks_) {
        tasks.push_back(pair.first);
    }
    return tasks;
}

CompilationStats CompilationMonitor::getStats(const std::string& task_id) const {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    auto active_it = active_tasks_.find(task_id);
    if (active_it != active_tasks_.end()) {
        return active_it->second;
    }
    
    auto completed_it = completed_tasks_.find(task_id);
    if (completed_it != completed_tasks_.end()) {
        return completed_it->second;
    }
    
    return CompilationStats();
}

std::map<std::string, CompilationStats> CompilationMonitor::getAllStats() const {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    std::map<std::string, CompilationStats> all_stats;
    all_stats.insert(active_tasks_.begin(), active_tasks_.end());
    all_stats.insert(completed_tasks_.begin(), completed_tasks_.end());
    
    return all_stats;
}

void CompilationMonitor::killTask(const std::string& task_id, const std::string& reason) {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    auto it = active_tasks_.find(task_id);
    if (it != active_tasks_.end()) {
        it->second.was_killed = true;
        it->second.reason = reason;
        it->second.finish();
        notifyCompilationKilled(task_id, it->second, reason);
        moveTaskToCompleted(task_id);
    }
}

void CompilationMonitor::killAllTasks(const std::string& reason) {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    for (auto& pair : active_tasks_) {
        pair.second.was_killed = true;
        pair.second.reason = reason;
        pair.second.finish();
        notifyCompilationKilled(pair.first, pair.second, reason);
    }
    
    completed_tasks_.insert(active_tasks_.begin(), active_tasks_.end());
    active_tasks_.clear();
}

void CompilationMonitor::clearStats() {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    active_tasks_.clear();
    completed_tasks_.clear();
}

void CompilationMonitor::clearCompletedTasks() {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    completed_tasks_.clear();
}

void CompilationMonitor::monitoringLoop() {
    while (monitoring_enabled_) {
        std::unique_lock<std::mutex> lock(monitoring_mutex_);
        
        if (active_tasks_.empty()) {
            monitoring_cv_.wait(lock, [this] { return !monitoring_enabled_ || !active_tasks_.empty(); });
            continue;
        }
        
        lock.unlock();
        
        checkMemoryUsage();
        checkTimeUsage();
        
        std::this_thread::sleep_for(config_.memory_check_interval);
    }
}

void CompilationMonitor::checkMemoryUsage() {
    if (!config_.enable_memory_monitoring) {
        return;
    }
    
    size_t current_usage = getProcessMemoryUsage();
    size_t max_usage = config_.max_memory_usage_mb * 1024 * 1024;
    
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    for (auto& pair : active_tasks_) {
        pair.second.updateMemoryUsage(current_usage);
        
        if (current_usage > max_usage) {
            notifyMemoryWarning(pair.first, current_usage, max_usage);
            
            if (config_.auto_kill_on_limit) {
                pair.second.was_killed = true;
                pair.second.reason = "Memory limit exceeded";
                pair.second.finish();
                notifyCompilationKilled(pair.first, pair.second, "Memory limit exceeded");
                moveTaskToCompleted(pair.first);
            }
        }
    }
}

void CompilationMonitor::checkTimeUsage() {
    if (!config_.enable_time_monitoring) {
        return;
    }
    
    auto current_time = std::chrono::steady_clock::now();
    
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    for (auto& pair : active_tasks_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - pair.second.start_time);
        
        if (elapsed > config_.max_compilation_time) {
            notifyTimeWarning(pair.first, elapsed, config_.max_compilation_time);
            
            if (config_.auto_kill_on_limit) {
                pair.second.was_killed = true;
                pair.second.reason = "Time limit exceeded";
                pair.second.finish();
                notifyCompilationKilled(pair.first, pair.second, "Time limit exceeded");
                moveTaskToCompleted(pair.first);
            }
        }
    }
}

void CompilationMonitor::updateTaskStats(const std::string& task_id) {
    std::lock_guard<std::mutex> lock(monitoring_mutex_);
    
    auto it = active_tasks_.find(task_id);
    if (it != active_tasks_.end()) {
        size_t current_usage = getProcessMemoryUsage();
        it->second.updateMemoryUsage(current_usage);
        notifyCompilationProgress(task_id, it->second);
    }
}

size_t CompilationMonitor::getProcessMemoryUsage() const {
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
            std::istringstream iss(line);
            std::string key, value, unit;
            iss >> key >> value >> unit;
            return std::stoul(value) * 1024; // Convert from KB to bytes
        }
    }
#elif defined(__APPLE__)
    task_t task = MACH_PORT_NULL;
    if (task_for_pid(mach_task_self(), getpid(), &task) == KERN_SUCCESS) {
        task_vm_info_data_t vm_info;
        mach_msg_type_number_t count = TASK_VM_INFO_COUNT;
        if (task_info(task, TASK_VM_INFO, (task_info_t)&vm_info, &count) == KERN_SUCCESS) {
            return vm_info.phys_footprint;
        }
    }
#endif
    return 0;
}

std::string CompilationMonitor::generateTaskId() const {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    
    return "task_" + std::to_string(dis(gen));
}

void CompilationMonitor::addTask(const std::string& task_id, const std::string& task_name) {
    CompilationStats stats;
    stats.start_time = std::chrono::steady_clock::now();
    active_tasks_[task_id] = stats;
}

void CompilationMonitor::removeTask(const std::string& task_id) {
    active_tasks_.erase(task_id);
}

void CompilationMonitor::moveTaskToCompleted(const std::string& task_id) {
    auto it = active_tasks_.find(task_id);
    if (it != active_tasks_.end()) {
        completed_tasks_[task_id] = it->second;
        active_tasks_.erase(it);
    }
}

void CompilationMonitor::notifyCompilationStart(const std::string& task_id) {
    if (callback_) {
        callback_->onCompilationStart(task_id);
    }
}

void CompilationMonitor::notifyCompilationProgress(const std::string& task_id, const CompilationStats& stats) {
    if (callback_) {
        callback_->onCompilationProgress(task_id, stats);
    }
}

void CompilationMonitor::notifyCompilationComplete(const std::string& task_id, const CompilationStats& stats) {
    if (callback_) {
        callback_->onCompilationComplete(task_id, stats);
    }
}

void CompilationMonitor::notifyCompilationKilled(const std::string& task_id, const CompilationStats& stats, const std::string& reason) {
    if (callback_) {
        callback_->onCompilationKilled(task_id, stats, reason);
    }
}

void CompilationMonitor::notifyMemoryWarning(const std::string& task_id, size_t current_usage, size_t max_usage) {
    if (callback_) {
        callback_->onMemoryWarning(task_id, current_usage, max_usage);
    }
}

void CompilationMonitor::notifyTimeWarning(const std::string& task_id, std::chrono::milliseconds current_time, std::chrono::milliseconds max_time) {
    if (callback_) {
        callback_->onTimeWarning(task_id, current_time, max_time);
    }
}

// CompilationMonitorManager 实现
std::unique_ptr<CompilationMonitorManager> CompilationMonitorManager::instance_ = nullptr;
std::mutex CompilationMonitorManager::instance_mutex_;

CompilationMonitorManager::CompilationMonitorManager() 
    : monitor_(std::make_unique<CompilationMonitor>()) {
}

CompilationMonitorManager::~CompilationMonitorManager() = default;

CompilationMonitorManager& CompilationMonitorManager::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_) {
        instance_ = std::make_unique<CompilationMonitorManager>();
    }
    return *instance_;
}

CompilationMonitor& CompilationMonitorManager::getMonitor() {
    return *monitor_;
}

void CompilationMonitorManager::setGlobalConfig(const MonitorConfig& config) {
    monitor_->setConfig(config);
}

void CompilationMonitorManager::setGlobalCallback(std::shared_ptr<CompilationMonitorCallback> callback) {
    monitor_->setCallback(callback);
}

std::string CompilationMonitorManager::startTask(const std::string& task_name) {
    return monitor_->startMonitoring(task_name);
}

void CompilationMonitorManager::stopTask(const std::string& task_id) {
    monitor_->stopMonitoring(task_id);
}

void CompilationMonitorManager::stopAllTasks() {
    monitor_->stopAllMonitoring();
}

CompilationStats CompilationMonitorManager::getTaskStats(const std::string& task_id) const {
    return monitor_->getStats(task_id);
}

std::map<std::string, CompilationStats> CompilationMonitorManager::getAllTaskStats() const {
    return monitor_->getAllStats();
}

void CompilationMonitorManager::clearAllStats() {
    monitor_->clearStats();
}

void CompilationMonitorManager::clearCompletedTasks() {
    monitor_->clearCompletedTasks();
}

// ScopedCompilationMonitor 实现
ScopedCompilationMonitor::ScopedCompilationMonitor(const std::string& task_name)
    : active_(true), monitor_(CompilationMonitorManager::getInstance().getMonitor()) {
    task_id_ = monitor_.startMonitoring(task_name);
}

ScopedCompilationMonitor::~ScopedCompilationMonitor() {
    if (active_) {
        monitor_.stopMonitoring(task_id_);
    }
}

ScopedCompilationMonitor::ScopedCompilationMonitor(ScopedCompilationMonitor&& other) noexcept
    : task_id_(std::move(other.task_id_))
    , active_(other.active_)
    , monitor_(other.monitor_) {
    other.active_ = false;
}

ScopedCompilationMonitor& ScopedCompilationMonitor::operator=(ScopedCompilationMonitor&& other) noexcept {
    if (this != &other) {
        if (active_) {
            monitor_.stopMonitoring(task_id_);
        }
        
        task_id_ = std::move(other.task_id_);
        active_ = other.active_;
        monitor_ = other.monitor_;
        
        other.active_ = false;
    }
    return *this;
}

const std::string& ScopedCompilationMonitor::getTaskId() const {
    return task_id_;
}

bool ScopedCompilationMonitor::isActive() const {
    return active_;
}

CompilationStats ScopedCompilationMonitor::getStats() const {
    return monitor_.getStats(task_id_);
}

void ScopedCompilationMonitor::finish() {
    if (active_) {
        monitor_.stopMonitoring(task_id_);
        active_ = false;
    }
}

void ScopedCompilationMonitor::kill(const std::string& reason) {
    if (active_) {
        monitor_.killTask(task_id_, reason);
        active_ = false;
    }
}

// DefaultCompilationMonitorCallback 实现
DefaultCompilationMonitorCallback::DefaultCompilationMonitorCallback()
    : verbose_(false), show_progress_(true), show_warnings_(true) {
}

void DefaultCompilationMonitorCallback::onCompilationStart(const std::string& task_id) {
    if (verbose_) {
        std::cout << "[CompilationMonitor] Starting task: " << task_id << std::endl;
    }
}

void DefaultCompilationMonitorCallback::onCompilationProgress(const std::string& task_id, const CompilationStats& stats) {
    if (show_progress_ && verbose_) {
        printStats(stats);
    }
}

void DefaultCompilationMonitorCallback::onCompilationComplete(const std::string& task_id, const CompilationStats& stats) {
    if (verbose_) {
        std::cout << "[CompilationMonitor] Task completed: " << task_id << std::endl;
        printStats(stats);
    }
}

void DefaultCompilationMonitorCallback::onCompilationKilled(const std::string& task_id, const CompilationStats& stats, const std::string& reason) {
    std::cout << "[CompilationMonitor] Task killed: " << task_id << " - " << reason << std::endl;
    printStats(stats);
}

void DefaultCompilationMonitorCallback::onMemoryWarning(const std::string& task_id, size_t current_usage, size_t max_usage) {
    if (show_warnings_) {
        printMemoryWarning(current_usage, max_usage);
    }
}

void DefaultCompilationMonitorCallback::onTimeWarning(const std::string& task_id, std::chrono::milliseconds current_time, std::chrono::milliseconds max_time) {
    if (show_warnings_) {
        printTimeWarning(current_time, max_time);
    }
}

void DefaultCompilationMonitorCallback::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void DefaultCompilationMonitorCallback::setShowProgress(bool show_progress) {
    show_progress_ = show_progress;
}

void DefaultCompilationMonitorCallback::setShowWarnings(bool show_warnings) {
    show_warnings_ = show_warnings;
}

void DefaultCompilationMonitorCallback::printStats(const CompilationStats& stats) {
    std::cout << "[CompilationMonitor] Duration: " << std::fixed << std::setprecision(2) 
              << stats.getDurationSeconds() << "s" << std::endl;
    std::cout << "[CompilationMonitor] Peak Memory: " << std::fixed << std::setprecision(2) 
              << stats.getPeakMemoryMB() << "MB" << std::endl;
    std::cout << "[CompilationMonitor] Current Memory: " << std::fixed << std::setprecision(2) 
              << stats.getCurrentMemoryMB() << "MB" << std::endl;
    if (stats.was_killed) {
        std::cout << "[CompilationMonitor] Killed: " << stats.reason << std::endl;
    }
}

void DefaultCompilationMonitorCallback::printMemoryWarning(size_t current_usage, size_t max_usage) {
    std::cout << "[CompilationMonitor] Memory Warning: " 
              << (current_usage / (1024.0 * 1024.0)) << "MB / " 
              << (max_usage / (1024.0 * 1024.0)) << "MB" << std::endl;
}

void DefaultCompilationMonitorCallback::printTimeWarning(std::chrono::milliseconds current_time, std::chrono::milliseconds max_time) {
    std::cout << "[CompilationMonitor] Time Warning: " 
              << (current_time.count() / 1000.0) << "s / " 
              << (max_time.count() / 1000.0) << "s" << std::endl;
}

// 静态方法实现
size_t CompilationMonitor::getCurrentMemoryUsage() {
    CompilationMonitor monitor;
    return monitor.getProcessMemoryUsage();
}

std::chrono::steady_clock::time_point CompilationMonitor::getCurrentTime() {
    return std::chrono::steady_clock::now();
}

} // namespace CHTL