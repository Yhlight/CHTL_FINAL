#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

class CHTLErrorHandler {
public:
    enum class ErrorType {
        SYNTAX_ERROR,
        SEMANTIC_ERROR,
        TYPE_ERROR,
        RUNTIME_ERROR,
        WARNING,
        INFO
    };
    
    enum class ErrorSeverity {
        LOW,
        MEDIUM,
        HIGH,
        CRITICAL
    };
    
    struct ErrorInfo {
        ErrorType type;
        ErrorSeverity severity;
        std::string message;
        std::string file;
        int line;
        int column;
        std::string code_snippet;
        std::string suggestion;
        std::chrono::system_clock::time_point timestamp;
    };
    
    struct DiagnosticReport {
        std::vector<ErrorInfo> errors;
        std::vector<ErrorInfo> warnings;
        std::vector<ErrorInfo> info;
        int total_errors;
        int total_warnings;
        int total_info;
        bool has_errors;
        bool has_warnings;
    };
    
    CHTLErrorHandler() {
        initializeErrorMessages();
        initializeSuggestions();
    }
    
    void reportError(ErrorType type, ErrorSeverity severity, const std::string& message,
                    const std::string& file = "", int line = 0, int column = 0,
                    const std::string& code_snippet = "", const std::string& suggestion = "") {
        ErrorInfo error;
        error.type = type;
        error.severity = severity;
        error.message = message;
        error.file = file;
        error.line = line;
        error.column = column;
        error.code_snippet = code_snippet;
        error.suggestion = suggestion;
        error.timestamp = std::chrono::system_clock::now();
        
        errors_.push_back(error);
    }
    
    void reportWarning(ErrorType type, const std::string& message,
                      const std::string& file = "", int line = 0, int column = 0,
                      const std::string& code_snippet = "", const std::string& suggestion = "") {
        reportError(type, ErrorSeverity::LOW, message, file, line, column, code_snippet, suggestion);
    }
    
    void reportInfo(ErrorType type, const std::string& message,
                   const std::string& file = "", int line = 0, int column = 0,
                   const std::string& code_snippet = "", const std::string& suggestion = "") {
        reportError(type, ErrorSeverity::LOW, message, file, line, column, code_snippet, suggestion);
    }
    
    DiagnosticReport generateReport() {
        DiagnosticReport report;
        
        for (const auto& error : errors_) {
            switch (error.severity) {
                case ErrorSeverity::LOW:
                    if (error.type == ErrorType::WARNING) {
                        report.warnings.push_back(error);
                    } else if (error.type == ErrorType::INFO) {
                        report.info.push_back(error);
                    }
                    break;
                case ErrorSeverity::MEDIUM:
                case ErrorSeverity::HIGH:
                case ErrorSeverity::CRITICAL:
                    report.errors.push_back(error);
                    break;
            }
        }
        
        report.total_errors = report.errors.size();
        report.total_warnings = report.warnings.size();
        report.total_info = report.info.size();
        report.has_errors = !report.errors.empty();
        report.has_warnings = !report.warnings.empty();
        
        return report;
    }
    
    void printReport(const DiagnosticReport& report) {
        std::cout << "\n=== CHTL Diagnostic Report ===\n\n";
        
        if (report.has_errors) {
            std::cout << "❌ ERRORS (" << report.total_errors << "):\n";
            for (const auto& error : report.errors) {
                printError(error);
            }
            std::cout << "\n";
        }
        
        if (report.has_warnings) {
            std::cout << "⚠️  WARNINGS (" << report.total_warnings << "):\n";
            for (const auto& warning : report.warnings) {
                printError(warning);
            }
            std::cout << "\n";
        }
        
        if (!report.info.empty()) {
            std::cout << "ℹ️  INFO (" << report.total_info << "):\n";
            for (const auto& info : report.info) {
                printError(info);
            }
            std::cout << "\n";
        }
        
        if (!report.has_errors && !report.has_warnings) {
            std::cout << "✅ No errors or warnings found!\n\n";
        }
        
        std::cout << "Summary: " << report.total_errors << " errors, " 
                  << report.total_warnings << " warnings, " 
                  << report.total_info << " info messages\n";
    }
    
    void clearErrors() {
        errors_.clear();
    }
    
    bool hasErrors() const {
        return std::any_of(errors_.begin(), errors_.end(),
                          [](const ErrorInfo& error) {
                              return error.severity != ErrorSeverity::LOW || 
                                     error.type == ErrorType::WARNING;
                          });
    }
    
    bool hasCriticalErrors() const {
        return std::any_of(errors_.begin(), errors_.end(),
                          [](const ErrorInfo& error) {
                              return error.severity == ErrorSeverity::CRITICAL;
                          });
    }
    
    std::vector<ErrorInfo> getErrors() const {
        return errors_;
    }
    
    void exportReport(const DiagnosticReport& report, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "CHTL Diagnostic Report\n";
        file << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        if (report.has_errors) {
            file << "ERRORS (" << report.total_errors << "):\n";
            for (const auto& error : report.errors) {
                exportError(file, error);
            }
            file << "\n";
        }
        
        if (report.has_warnings) {
            file << "WARNINGS (" << report.total_warnings << "):\n";
            for (const auto& warning : report.warnings) {
                exportError(file, warning);
            }
            file << "\n";
        }
        
        if (!report.info.empty()) {
            file << "INFO (" << report.total_info << "):\n";
            for (const auto& info : report.info) {
                exportError(file, info);
            }
            file << "\n";
        }
        
        file << "Summary: " << report.total_errors << " errors, " 
             << report.total_warnings << " warnings, " 
             << report.total_info << " info messages\n";
        
        file.close();
        std::cout << "Diagnostic report exported to: " << filename << std::endl;
    }
    
private:
    std::vector<ErrorInfo> errors_;
    std::map<ErrorType, std::string> error_messages_;
    std::map<ErrorType, std::string> suggestions_;
    
    void initializeErrorMessages() {
        error_messages_[ErrorType::SYNTAX_ERROR] = "Syntax error";
        error_messages_[ErrorType::SEMANTIC_ERROR] = "Semantic error";
        error_messages_[ErrorType::TYPE_ERROR] = "Type error";
        error_messages_[ErrorType::RUNTIME_ERROR] = "Runtime error";
        error_messages_[ErrorType::WARNING] = "Warning";
        error_messages_[ErrorType::INFO] = "Information";
    }
    
    void initializeSuggestions() {
        suggestions_[ErrorType::SYNTAX_ERROR] = "Check syntax and try again";
        suggestions_[ErrorType::SEMANTIC_ERROR] = "Check semantic rules and fix the issue";
        suggestions_[ErrorType::TYPE_ERROR] = "Check types and ensure compatibility";
        suggestions_[ErrorType::RUNTIME_ERROR] = "Check runtime conditions and fix the issue";
        suggestions_[ErrorType::WARNING] = "Consider fixing this warning";
        suggestions_[ErrorType::INFO] = "This is informational only";
    }
    
    void printError(const ErrorInfo& error) {
        std::cout << "  ";
        
        // 打印错误类型和严重程度
        switch (error.severity) {
            case ErrorSeverity::CRITICAL:
                std::cout << "🔴 CRITICAL ";
                break;
            case ErrorSeverity::HIGH:
                std::cout << "🔴 HIGH ";
                break;
            case ErrorSeverity::MEDIUM:
                std::cout << "🟡 MEDIUM ";
                break;
            case ErrorSeverity::LOW:
                std::cout << "🟢 LOW ";
                break;
        }
        
        std::cout << error_messages_[error.type] << ": ";
        std::cout << error.message;
        
        if (!error.file.empty()) {
            std::cout << " (" << error.file;
            if (error.line > 0) {
                std::cout << ":" << error.line;
                if (error.column > 0) {
                    std::cout << ":" << error.column;
                }
            }
            std::cout << ")";
        }
        
        std::cout << "\n";
        
        if (!error.code_snippet.empty()) {
            std::cout << "    Code: " << error.code_snippet << "\n";
        }
        
        if (!error.suggestion.empty()) {
            std::cout << "    Suggestion: " << error.suggestion << "\n";
        }
        
        std::cout << "\n";
    }
    
    void exportError(std::ofstream& file, const ErrorInfo& error) {
        file << "  ";
        
        switch (error.severity) {
            case ErrorSeverity::CRITICAL:
                file << "CRITICAL ";
                break;
            case ErrorSeverity::HIGH:
                file << "HIGH ";
                break;
            case ErrorSeverity::MEDIUM:
                file << "MEDIUM ";
                break;
            case ErrorSeverity::LOW:
                file << "LOW ";
                break;
        }
        
        file << error_messages_[error.type] << ": ";
        file << error.message;
        
        if (!error.file.empty()) {
            file << " (" << error.file;
            if (error.line > 0) {
                file << ":" << error.line;
                if (error.column > 0) {
                    file << ":" << error.column;
                }
            }
            file << ")";
        }
        
        file << "\n";
        
        if (!error.code_snippet.empty()) {
            file << "    Code: " << error.code_snippet << "\n";
        }
        
        if (!error.suggestion.empty()) {
            file << "    Suggestion: " << error.suggestion << "\n";
        }
        
        file << "\n";
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto tm = *std::localtime(&time_t);
        
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// 示例使用
int main() {
    CHTLErrorHandler handler;
    
    // 模拟一些错误
    handler.reportError(CHTLErrorHandler::ErrorType::SYNTAX_ERROR, 
                       CHTLErrorHandler::ErrorSeverity::HIGH,
                       "Unexpected token '}' at end of template",
                       "test.chtl", 15, 25,
                       "[Template] @Style MyButton {",
                       "Check for missing closing brace or semicolon");
    
    handler.reportWarning(CHTLErrorHandler::ErrorType::WARNING,
                         "Unused variable 'temp'",
                         "test.chtl", 23, 10,
                         "var temp = 'unused';",
                         "Remove unused variable or use it");
    
    handler.reportInfo(CHTLErrorHandler::ErrorType::INFO,
                      "Template 'MyButton' compiled successfully",
                      "test.chtl", 15, 1);
    
    // 生成并打印报告
    auto report = handler.generateReport();
    handler.printReport(report);
    
    // 导出报告
    handler.exportReport(report, "diagnostic_report.txt");
    
    return 0;
}