#include "CHTLCompilerDispatcher.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLJSLexer/CHTLJSLexer.h"
#include "CHTLJSParser/CHTLJSParser.h"
#include "CHTLJSFeatures/CHTLJSFeatures.h"
#include "CHTLJSDynamicExpressions/CHTLJSDynamicExpressions.h"
#include "CHTLModuleSystem/CHTLModuleSystem.h"
#include "CHTLCJMODAPI/CHTLCJMODAPI.h"
#include "CHTLUnifiedScanner/CHTLUnifiedScanner.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <algorithm>

namespace CHTL {

// CHTLCompiler implementation
CHTLCompiler::CHTLCompiler() {
    // Initialize with empty content - will be set during compilation
    lexer = std::make_unique<CHTLLexer>();
    parser = std::make_unique<CHTLParser>(std::vector<Token>());
    generator = std::make_unique<CHTLGenerator>();
}

CHTLCompiler::~CHTLCompiler() {
}

CompilerResult CHTLCompiler::compile(const CompilerConfig& config) {
    CompilerResult result;
    
    try {
        // Read input file
        std::ifstream file(config.input_file);
        if (!file.is_open()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Cannot open input file: " + config.input_file;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        // Tokenize
        lexer = std::make_unique<CHTLLexer>();
        auto tokens = lexer->tokenize(content);
        if (tokens.empty()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Failed to tokenize input";
            return result;
        }
        
        // Parse
        parser = std::make_unique<CHTLParser>(tokens);
        auto ast = parser->parse();
        if (!ast) {
            result.status = CompilerStatus::Error;
            result.error_message = "Failed to parse input";
            return result;
        }
        
        // Generate
        std::string output = generator->generateHTML(ast);
        if (output.empty()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Failed to generate output";
            return result;
        }
        
        // Write output file
        if (!config.output_file.empty()) {
            std::ofstream out_file(config.output_file);
            if (!out_file.is_open()) {
                result.status = CompilerStatus::Error;
                result.error_message = "Cannot open output file: " + config.output_file;
                return result;
            }
            out_file << output;
            out_file.close();
        }
        
        result.status = CompilerStatus::Completed;
        result.output = output;
        result.metadata["input_file"] = config.input_file;
        result.metadata["output_file"] = config.output_file;
        result.metadata["compiler_type"] = "CHTL";
        
    } catch (const std::exception& e) {
        result.status = CompilerStatus::Error;
        result.error_message = "Compilation error: " + std::string(e.what());
    }
    
    return result;
}

bool CHTLCompiler::canHandle(const CompilerType& type) const {
    return type == CompilerType::CHTL;
}

std::string CHTLCompiler::getVersion() const {
    return "1.0.0";
}

std::vector<std::string> CHTLCompiler::getSupportedExtensions() const {
    return {".chtl"};
}

// CHTLJSCompiler implementation
CHTLJSCompiler::CHTLJSCompiler() {
    // Initialize with empty content - will be set during compilation
    lexer = std::make_unique<CHTLJSLexer>("");
    parser = std::make_unique<CHTLJSParser>(std::vector<CHTLJSToken>());
    // Note: features and expressions will be initialized when needed
}

CHTLJSCompiler::~CHTLJSCompiler() {
}

CompilerResult CHTLJSCompiler::compile(const CompilerConfig& config) {
    CompilerResult result;
    
    try {
        // Read input file
        std::ifstream file(config.input_file);
        if (!file.is_open()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Cannot open input file: " + config.input_file;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        // Tokenize
        lexer = std::make_unique<CHTLJSLexer>(content);
        auto tokens = lexer->tokenize();
        if (tokens.empty()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Failed to tokenize input";
            return result;
        }
        
        // Parse
        parser = std::make_unique<CHTLJSParser>(tokens);
        auto ast = parser->parse();
        if (!ast) {
            result.status = CompilerStatus::Error;
            result.error_message = "Failed to parse input";
            return result;
        }
        
        // Process features (placeholder)
        std::string output = "// CHTL JS processed content\n" + content;
        if (output.empty()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Failed to process features";
            return result;
        }
        
        // Write output file
        if (!config.output_file.empty()) {
            std::ofstream out_file(config.output_file);
            if (!out_file.is_open()) {
                result.status = CompilerStatus::Error;
                result.error_message = "Cannot open output file: " + config.output_file;
                return result;
            }
            out_file << output;
            out_file.close();
        }
        
        result.status = CompilerStatus::Completed;
        result.output = output;
        result.metadata["input_file"] = config.input_file;
        result.metadata["output_file"] = config.output_file;
        result.metadata["compiler_type"] = "CHTLJS";
        
    } catch (const std::exception& e) {
        result.status = CompilerStatus::Error;
        result.error_message = "Compilation error: " + std::string(e.what());
    }
    
    return result;
}

bool CHTLJSCompiler::canHandle(const CompilerType& type) const {
    return type == CompilerType::CHTLJS;
}

std::string CHTLJSCompiler::getVersion() const {
    return "1.0.0";
}

std::vector<std::string> CHTLJSCompiler::getSupportedExtensions() const {
    return {".cjjs"};
}

// CSSCompiler implementation
CSSCompiler::CSSCompiler() {
}

CSSCompiler::~CSSCompiler() {
}

CompilerResult CSSCompiler::compile(const CompilerConfig& config) {
    CompilerResult result;
    
    try {
        // Read input file
        std::ifstream file(config.input_file);
        if (!file.is_open()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Cannot open input file: " + config.input_file;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        // Simple CSS processing (placeholder)
        std::string output = content;
        
        // Write output file
        if (!config.output_file.empty()) {
            std::ofstream out_file(config.output_file);
            if (!out_file.is_open()) {
                result.status = CompilerStatus::Error;
                result.error_message = "Cannot open output file: " + config.output_file;
                return result;
            }
            out_file << output;
            out_file.close();
        }
        
        result.status = CompilerStatus::Completed;
        result.output = output;
        result.metadata["input_file"] = config.input_file;
        result.metadata["output_file"] = config.output_file;
        result.metadata["compiler_type"] = "CSS";
        
    } catch (const std::exception& e) {
        result.status = CompilerStatus::Error;
        result.error_message = "Compilation error: " + std::string(e.what());
    }
    
    return result;
}

bool CSSCompiler::canHandle(const CompilerType& type) const {
    return type == CompilerType::CSS;
}

std::string CSSCompiler::getVersion() const {
    return "1.0.0";
}

std::vector<std::string> CSSCompiler::getSupportedExtensions() const {
    return {".css"};
}

// JavaScriptCompiler implementation
JavaScriptCompiler::JavaScriptCompiler() {
}

JavaScriptCompiler::~JavaScriptCompiler() {
}

CompilerResult JavaScriptCompiler::compile(const CompilerConfig& config) {
    CompilerResult result;
    
    try {
        // Read input file
        std::ifstream file(config.input_file);
        if (!file.is_open()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Cannot open input file: " + config.input_file;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        // Simple JavaScript processing (placeholder)
        std::string output = content;
        
        // Write output file
        if (!config.output_file.empty()) {
            std::ofstream out_file(config.output_file);
            if (!out_file.is_open()) {
                result.status = CompilerStatus::Error;
                result.error_message = "Cannot open output file: " + config.output_file;
                return result;
            }
            out_file << output;
            out_file.close();
        }
        
        result.status = CompilerStatus::Completed;
        result.output = output;
        result.metadata["input_file"] = config.input_file;
        result.metadata["output_file"] = config.output_file;
        result.metadata["compiler_type"] = "JavaScript";
        
    } catch (const std::exception& e) {
        result.status = CompilerStatus::Error;
        result.error_message = "Compilation error: " + std::string(e.what());
    }
    
    return result;
}

bool JavaScriptCompiler::canHandle(const CompilerType& type) const {
    return type == CompilerType::JavaScript;
}

std::string JavaScriptCompiler::getVersion() const {
    return "1.0.0";
}

std::vector<std::string> JavaScriptCompiler::getSupportedExtensions() const {
    return {".js"};
}

// HTMLCompiler implementation
HTMLCompiler::HTMLCompiler() {
}

HTMLCompiler::~HTMLCompiler() {
}

CompilerResult HTMLCompiler::compile(const CompilerConfig& config) {
    CompilerResult result;
    
    try {
        // Read input file
        std::ifstream file(config.input_file);
        if (!file.is_open()) {
            result.status = CompilerStatus::Error;
            result.error_message = "Cannot open input file: " + config.input_file;
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        file.close();
        
        // Simple HTML processing (placeholder)
        std::string output = content;
        
        // Write output file
        if (!config.output_file.empty()) {
            std::ofstream out_file(config.output_file);
            if (!out_file.is_open()) {
                result.status = CompilerStatus::Error;
                result.error_message = "Cannot open output file: " + config.output_file;
                return result;
            }
            out_file << output;
            out_file.close();
        }
        
        result.status = CompilerStatus::Completed;
        result.output = output;
        result.metadata["input_file"] = config.input_file;
        result.metadata["output_file"] = config.output_file;
        result.metadata["compiler_type"] = "HTML";
        
    } catch (const std::exception& e) {
        result.status = CompilerStatus::Error;
        result.error_message = "Compilation error: " + std::string(e.what());
    }
    
    return result;
}

bool HTMLCompiler::canHandle(const CompilerType& type) const {
    return type == CompilerType::HTML;
}

std::string HTMLCompiler::getVersion() const {
    return "1.0.0";
}

std::vector<std::string> HTMLCompiler::getSupportedExtensions() const {
    return {".html", ".htm"};
}

// MixedCompiler implementation
MixedCompiler::MixedCompiler() {
    chtl_compiler = std::make_unique<CHTLCompiler>();
    chtljs_compiler = std::make_unique<CHTLJSCompiler>();
    css_compiler = std::make_unique<CSSCompiler>();
    js_compiler = std::make_unique<JavaScriptCompiler>();
    html_compiler = std::make_unique<HTMLCompiler>();
}

MixedCompiler::~MixedCompiler() {
}

CompilerResult MixedCompiler::compile(const CompilerConfig& config) {
    CompilerResult result;
    
    try {
        // Detect file type and delegate to appropriate compiler
        std::string extension = "";
        size_t pos = config.input_file.find_last_of('.');
        if (pos != std::string::npos) {
            extension = config.input_file.substr(pos);
        }
        std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        
        if (extension == ".chtl") {
            return chtl_compiler->compile(config);
        } else if (extension == ".cjjs") {
            return chtljs_compiler->compile(config);
        } else if (extension == ".css") {
            return css_compiler->compile(config);
        } else if (extension == ".js") {
            return js_compiler->compile(config);
        } else if (extension == ".html" || extension == ".htm") {
            return html_compiler->compile(config);
        } else {
            result.status = CompilerStatus::Error;
            result.error_message = "Unsupported file type: " + extension;
            return result;
        }
        
    } catch (const std::exception& e) {
        result.status = CompilerStatus::Error;
        result.error_message = "Compilation error: " + std::string(e.what());
    }
    
    return result;
}

bool MixedCompiler::canHandle(const CompilerType& type) const {
    return type == CompilerType::Mixed;
}

std::string MixedCompiler::getVersion() const {
    return "1.0.0";
}

std::vector<std::string> MixedCompiler::getSupportedExtensions() const {
    return {".chtl", ".cjjs", ".css", ".js", ".html", ".htm"};
}

// CHTLCompilerDispatcher implementation
CHTLCompilerDispatcher::CHTLCompilerDispatcher() : debug_mode(false), verbose(false) {
    // Register default compilers
    registerCompiler(CompilerType::CHTL, std::make_unique<CHTLCompiler>());
    registerCompiler(CompilerType::CHTLJS, std::make_unique<CHTLJSCompiler>());
    registerCompiler(CompilerType::CSS, std::make_unique<CSSCompiler>());
    registerCompiler(CompilerType::JavaScript, std::make_unique<JavaScriptCompiler>());
    registerCompiler(CompilerType::HTML, std::make_unique<HTMLCompiler>());
    registerCompiler(CompilerType::Mixed, std::make_unique<MixedCompiler>());
    
    // Set up extension mappings
    setExtensionMapping(".chtl", CompilerType::CHTL);
    setExtensionMapping(".cjjs", CompilerType::CHTLJS);
    setExtensionMapping(".css", CompilerType::CSS);
    setExtensionMapping(".js", CompilerType::JavaScript);
    setExtensionMapping(".html", CompilerType::HTML);
    setExtensionMapping(".htm", CompilerType::HTML);
    
    // Initialize status
    for (auto& pair : compilers) {
        compiler_status[pair.first] = CompilerStatus::Idle;
        compilation_counts[pair.first] = 0;
        compilation_times[pair.first] = 0.0;
    }
}

CHTLCompilerDispatcher::~CHTLCompilerDispatcher() {
}

void CHTLCompilerDispatcher::registerCompiler(CompilerType type, std::unique_ptr<ICompiler> compiler) {
    compilers[type] = std::move(compiler);
    compiler_status[type] = CompilerStatus::Idle;
    compilation_counts[type] = 0;
    compilation_times[type] = 0.0;
}

void CHTLCompilerDispatcher::unregisterCompiler(CompilerType type) {
    compilers.erase(type);
    compiler_status.erase(type);
    compilation_counts.erase(type);
    compilation_times.erase(type);
}

ICompiler* CHTLCompilerDispatcher::getCompiler(CompilerType type) {
    auto it = compilers.find(type);
    return (it != compilers.end()) ? it->second.get() : nullptr;
}

std::vector<CompilerType> CHTLCompilerDispatcher::getAvailableCompilers() const {
    std::vector<CompilerType> types;
    for (const auto& pair : compilers) {
        types.push_back(pair.first);
    }
    return types;
}

CompilerResult CHTLCompilerDispatcher::compile(const CompilerConfig& config) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    updateCompilerStatus(config.type, CompilerStatus::Running);
    
    ICompiler* compiler = getCompiler(config.type);
    if (!compiler) {
        CompilerResult result;
        result.status = CompilerStatus::Error;
        result.error_message = "Compiler not found for type: " + std::to_string(static_cast<int>(config.type));
        updateCompilerStatus(config.type, CompilerStatus::Error);
        addCompilerError(config.type, result.error_message);
        return result;
    }
    
    CompilerResult result = compiler->compile(config);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    updateCompilationTime(config.type, duration.count() / 1000.0);
    
    updateCompilerStatus(config.type, result.status);
    updateCompilationCount(config.type);
    
    if (result.status == CompilerStatus::Error) {
        addCompilerError(config.type, result.error_message);
    }
    
    notifyCompilationCallback(config.type, result.status);
    
    return result;
}

CompilerResult CHTLCompilerDispatcher::compile(const std::string& input_file, const std::string& output_file, CompilerType type) {
    CompilerConfig config;
    config.type = type;
    config.input_file = input_file;
    config.output_file = output_file;
    config.debug_mode = debug_mode;
    config.verbose = verbose;
    
    return compile(config);
}

CompilerResult CHTLCompilerDispatcher::compile(const std::string& input_file, CompilerType type) {
    CompilerConfig config;
    config.type = type;
    config.input_file = input_file;
    config.debug_mode = debug_mode;
    config.verbose = verbose;
    
    return compile(config);
}

std::vector<CompilerResult> CHTLCompilerDispatcher::compileBatch(const std::vector<CompilerConfig>& configs) {
    std::vector<CompilerResult> results;
    
    for (const auto& config : configs) {
        results.push_back(compile(config));
    }
    
    return results;
}

std::vector<CompilerResult> CHTLCompilerDispatcher::compileDirectory(const std::string& directory, CompilerType type) {
    std::vector<CompilerResult> results;
    
    try {
        std::vector<std::string> extensions;
        if (type == CompilerType::Mixed) {
            extensions = {".chtl", ".cjjs", ".css", ".js", ".html", ".htm"};
        } else {
            ICompiler* compiler = getCompiler(type);
            if (compiler) {
                extensions = compiler->getSupportedExtensions();
            }
        }
        
        auto files = findFiles(directory, extensions);
        
        for (const auto& file : files) {
            CompilerConfig config;
            config.type = type;
            config.input_file = file;
            config.debug_mode = debug_mode;
            config.verbose = verbose;
            
            results.push_back(compile(config));
        }
        
    } catch (const std::exception& e) {
        CompilerResult result;
        result.status = CompilerStatus::Error;
        result.error_message = "Directory compilation error: " + std::string(e.what());
        results.push_back(result);
    }
    
    return results;
}

void CHTLCompilerDispatcher::setDebugMode(bool enabled) {
    debug_mode = enabled;
}

void CHTLCompilerDispatcher::setVerboseMode(bool enabled) {
    verbose = enabled;
}

void CHTLCompilerDispatcher::addSearchPath(const std::string& path) {
    search_paths.push_back(path);
}

void CHTLCompilerDispatcher::removeSearchPath(const std::string& path) {
    search_paths.erase(std::remove(search_paths.begin(), search_paths.end(), path), search_paths.end());
}

void CHTLCompilerDispatcher::setExtensionMapping(const std::string& extension, CompilerType type) {
    extension_map[extension] = type;
}

CompilerType CHTLCompilerDispatcher::detectCompilerType(const std::string& file_path) const {
    std::string extension = getFileExtension(file_path);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    auto it = extension_map.find(extension);
    if (it != extension_map.end()) {
        return it->second;
    }
    
    return CompilerType::CHTL; // Default
}

std::string CHTLCompilerDispatcher::getCompilerVersion(CompilerType type) const {
    auto it = compilers.find(type);
    ICompiler* compiler = (it != compilers.end()) ? it->second.get() : nullptr;
    return compiler ? compiler->getVersion() : "Unknown";
}

std::vector<std::string> CHTLCompilerDispatcher::getSupportedExtensions(CompilerType type) const {
    auto it = compilers.find(type);
    ICompiler* compiler = (it != compilers.end()) ? it->second.get() : nullptr;
    return compiler ? compiler->getSupportedExtensions() : std::vector<std::string>();
}

bool CHTLCompilerDispatcher::isFileSupported(const std::string& file_path) const {
    std::string extension = getFileExtension(file_path);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    return extension_map.find(extension) != extension_map.end();
}

CompilerStatus CHTLCompilerDispatcher::getCompilerStatus(CompilerType type) const {
    auto it = compiler_status.find(type);
    return (it != compiler_status.end()) ? it->second : CompilerStatus::Idle;
}

std::map<CompilerType, CompilerStatus> CHTLCompilerDispatcher::getAllCompilerStatus() const {
    return compiler_status;
}

void CHTLCompilerDispatcher::resetCompilerStatus(CompilerType type) {
    compiler_status[type] = CompilerStatus::Idle;
}

void CHTLCompilerDispatcher::resetAllCompilerStatus() {
    for (auto& pair : compiler_status) {
        pair.second = CompilerStatus::Idle;
    }
}

std::vector<std::string> CHTLCompilerDispatcher::getCompilerErrors(CompilerType type) const {
    auto it = compiler_errors.find(type);
    return (it != compiler_errors.end()) ? it->second : std::vector<std::string>();
}

std::map<CompilerType, std::vector<std::string>> CHTLCompilerDispatcher::getAllCompilerErrors() const {
    return compiler_errors;
}

void CHTLCompilerDispatcher::clearCompilerErrors(CompilerType type) {
    compiler_errors[type].clear();
}

void CHTLCompilerDispatcher::clearAllCompilerErrors() {
    for (auto& pair : compiler_errors) {
        pair.second.clear();
    }
}

std::map<CompilerType, int> CHTLCompilerDispatcher::getCompilationCounts() const {
    return compilation_counts;
}

std::map<CompilerType, double> CHTLCompilerDispatcher::getCompilationTimes() const {
    return compilation_times;
}

void CHTLCompilerDispatcher::resetStatistics() {
    for (auto& pair : compilation_counts) {
        pair.second = 0;
    }
    for (auto& pair : compilation_times) {
        pair.second = 0.0;
    }
}

void CHTLCompilerDispatcher::setCompilationCallback(std::function<void(CompilerType, CompilerStatus)> callback) {
    compilation_callback = callback;
}

void CHTLCompilerDispatcher::setErrorCallback(std::function<void(CompilerType, const std::string&)> callback) {
    error_callback = callback;
}

void CHTLCompilerDispatcher::setProgressCallback(std::function<void(CompilerType, int, int)> callback) {
    progress_callback = callback;
}

// Private methods
void CHTLCompilerDispatcher::updateCompilerStatus(CompilerType type, CompilerStatus status) {
    compiler_status[type] = status;
}

void CHTLCompilerDispatcher::addCompilerError(CompilerType type, const std::string& error) {
    compiler_errors[type].push_back(error);
}

void CHTLCompilerDispatcher::updateCompilationCount(CompilerType type) {
    compilation_counts[type]++;
}

void CHTLCompilerDispatcher::updateCompilationTime(CompilerType type, double time) {
    compilation_times[type] += time;
}

void CHTLCompilerDispatcher::notifyCompilationCallback(CompilerType type, CompilerStatus status) {
    if (compilation_callback) {
        compilation_callback(type, status);
    }
}

void CHTLCompilerDispatcher::notifyErrorCallback(CompilerType type, const std::string& error) {
    if (error_callback) {
        error_callback(type, error);
    }
}

void CHTLCompilerDispatcher::notifyProgressCallback(CompilerType type, int current, int total) {
    if (progress_callback) {
        progress_callback(type, current, total);
    }
}

std::string CHTLCompilerDispatcher::readFile(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLCompilerDispatcher::writeFile(const std::string& file_path, const std::string& content) const {
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

std::string CHTLCompilerDispatcher::getFileExtension(const std::string& file_path) const {
    size_t pos = file_path.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return file_path.substr(pos);
}

std::string CHTLCompilerDispatcher::getFileName(const std::string& file_path) const {
    size_t pos = file_path.find_last_of('/');
    if (pos == std::string::npos) {
        return file_path;
    }
    return file_path.substr(pos + 1);
}

std::string CHTLCompilerDispatcher::getFileDirectory(const std::string& file_path) const {
    size_t pos = file_path.find_last_of('/');
    if (pos == std::string::npos) {
        return "";
    }
    return file_path.substr(0, pos);
}

bool CHTLCompilerDispatcher::fileExists(const std::string& file_path) const {
    std::ifstream file(file_path);
    return file.good();
}

bool CHTLCompilerDispatcher::createDirectory(const std::string& directory) const {
    try {
        std::filesystem::create_directories(directory);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> CHTLCompilerDispatcher::findFiles(const std::string& directory, const std::vector<std::string>& extensions) const {
    std::vector<std::string> files;
    
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string file_path = entry.path().string();
                std::string extension = getFileExtension(file_path);
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                    files.push_back(file_path);
                }
            }
        }
    } catch (...) {
        // Ignore errors
    }
    
    return files;
}

} // namespace CHTL