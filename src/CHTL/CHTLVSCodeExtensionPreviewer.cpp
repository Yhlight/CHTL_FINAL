#include "CHTL/CHTLVSCodeExtension.h"
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

namespace CHTL {

// PagePreviewer 实现
PagePreviewer::PagePreviewer(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_previewMode("html"), m_autoRefresh(false), m_refreshInterval(1000), m_isLivePreviewActive(false) {
}

std::string PagePreviewer::generatePreview(const std::string& chtlCode) {
    if (chtlCode.empty()) {
        return "";
    }
    
    if (m_previewMode == "html") {
        return generateHTMLPreview(chtlCode);
    } else if (m_previewMode == "css") {
        return generateCSSPreview(chtlCode);
    } else if (m_previewMode == "javascript") {
        return generateJavaScriptPreview(chtlCode);
    }
    
    return generateHTMLPreview(chtlCode);
}

std::string PagePreviewer::generatePreviewFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        m_context->addError("Cannot open file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string chtlCode = buffer.str();
    
    return generatePreview(chtlCode);
}

void PagePreviewer::startLivePreview(const std::string& filePath) {
    if (m_isLivePreviewActive) {
        stopLivePreview();
    }
    
    m_isLivePreviewActive = true;
    
    // 在后台线程中运行实时预览
    std::thread previewThread([this, filePath]() {
        while (m_isLivePreviewActive) {
            std::string preview = generatePreviewFromFile(filePath);
            if (!preview.empty()) {
                notifyPreviewListeners(preview);
            }
            
            if (m_autoRefresh) {
                std::this_thread::sleep_for(std::chrono::milliseconds(m_refreshInterval));
            } else {
                break;
            }
        }
    });
    
    previewThread.detach();
}

void PagePreviewer::stopLivePreview() {
    m_isLivePreviewActive = false;
}

bool PagePreviewer::isLivePreviewActive() const {
    return m_isLivePreviewActive;
}

void PagePreviewer::addPreviewListener(std::function<void(const std::string&)> listener) {
    m_previewListeners.push_back(listener);
}

void PagePreviewer::removePreviewListener(std::function<void(const std::string&)> listener) {
    // 简化实现，实际应该支持精确的监听器移除
    m_previewListeners.clear();
}

void PagePreviewer::clearPreviewListeners() {
    m_previewListeners.clear();
}

std::string PagePreviewer::generateHTMLPreview(const std::string& chtlCode) const {
    // 这里需要实际的 CHTL 编译逻辑
    // 简化实现，返回基本的 HTML 结构
    
    std::stringstream html;
    html << "<!DOCTYPE html>\n";
    html << "<html>\n";
    html << "<head>\n";
    html << "  <meta charset=\"UTF-8\">\n";
    html << "  <title>CHTL Preview</title>\n";
    html << "  <style>\n";
    html << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    html << "    .preview-container { border: 1px solid #ccc; padding: 20px; }\n";
    html << "  </style>\n";
    html << "</head>\n";
    html << "<body>\n";
    html << "  <div class=\"preview-container\">\n";
    html << "    <h1>CHTL Preview</h1>\n";
    html << "    <p>This is a preview of your CHTL code.</p>\n";
    html << "    <pre>" << chtlCode << "</pre>\n";
    html << "  </div>\n";
    html << "</body>\n";
    html << "</html>\n";
    
    return html.str();
}

std::string PagePreviewer::generateCSSPreview(const std::string& chtlCode) const {
    std::stringstream css;
    css << "/* CHTL CSS Preview */\n";
    css << ".preview-container {\n";
    css << "  font-family: Arial, sans-serif;\n";
    css << "  margin: 20px;\n";
    css << "  padding: 20px;\n";
    css << "  border: 1px solid #ccc;\n";
    css << "}\n";
    css << "\n";
    css << "/* Generated from CHTL code */\n";
    css << chtlCode << "\n";
    
    return css.str();
}

std::string PagePreviewer::generateJavaScriptPreview(const std::string& chtlCode) const {
    std::stringstream js;
    js << "// CHTL JavaScript Preview\n";
    js << "console.log('CHTL JavaScript Preview');\n";
    js << "\n";
    js << "// Generated from CHTL code\n";
    js << chtlCode << "\n";
    
    return js.str();
}

void PagePreviewer::notifyPreviewListeners(const std::string& preview) const {
    for (const auto& listener : m_previewListeners) {
        listener(preview);
    }
}

// Exporter 实现
Exporter::Exporter(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_exportFormat("html"), m_minifyOutput(false), m_includeSourceMaps(false) {
}

void Exporter::exportToHTML(const std::string& chtlCode, const std::string& outputPath) {
    std::string html = generateHTMLPreview(chtlCode);
    
    if (m_minifyOutput) {
        html = minifyHTML(html);
    }
    
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        m_context->addError("Cannot create HTML file: " + outputPath);
        return;
    }
    
    file << html;
    file.close();
}

void Exporter::exportToCSS(const std::string& chtlCode, const std::string& outputPath) {
    std::string css = generateCSSPreview(chtlCode);
    
    if (m_minifyOutput) {
        css = minifyCSS(css);
    }
    
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        m_context->addError("Cannot create CSS file: " + outputPath);
        return;
    }
    
    file << css;
    file.close();
}

void Exporter::exportToJavaScript(const std::string& chtlCode, const std::string& outputPath) {
    std::string js = generateJavaScriptPreview(chtlCode);
    
    if (m_minifyOutput) {
        js = minifyJavaScript(js);
    }
    
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        m_context->addError("Cannot create JavaScript file: " + outputPath);
        return;
    }
    
    file << js;
    file.close();
}

void Exporter::exportToFiles(const std::string& chtlCode, const std::string& outputDirectory) {
    try {
        std::filesystem::create_directories(outputDirectory);
        
        exportToHTML(chtlCode, outputDirectory + "/index.html");
        exportToCSS(chtlCode, outputDirectory + "/styles.css");
        exportToJavaScript(chtlCode, outputDirectory + "/script.js");
        
    } catch (const std::exception& e) {
        m_context->addError("Error creating output directory: " + std::string(e.what()));
    }
}

void Exporter::exportDirectory(const std::string& inputDirectory, const std::string& outputDirectory) {
    try {
        std::filesystem::create_directories(outputDirectory);
        
        for (const auto& entry : std::filesystem::recursive_directory_iterator(inputDirectory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".chtl") {
                std::string inputFile = entry.path().string();
                std::string relativePath = std::filesystem::relative(inputFile, inputDirectory).string();
                std::string outputFile = outputDirectory + "/" + relativePath;
                
                // 创建输出目录
                std::filesystem::path outputPath(outputFile);
                std::filesystem::create_directories(outputPath.parent_path());
                
                // 读取输入文件
                std::ifstream input(inputFile);
                if (input.is_open()) {
                    std::stringstream buffer;
                    buffer << input.rdbuf();
                    std::string chtlCode = buffer.str();
                    
                    // 导出文件
                    exportToFiles(chtlCode, outputFile);
                }
            }
        }
        
    } catch (const std::exception& e) {
        m_context->addError("Error exporting directory: " + std::string(e.what()));
    }
}

void Exporter::exportProject(const std::string& projectPath, const std::string& outputPath) {
    exportDirectory(projectPath, outputPath);
}

std::string Exporter::minifyHTML(const std::string& html) const {
    std::string result = html;
    
    // 移除注释
    std::regex commentRegex(R"(<!--[\s\S]*?-->)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 移除多余的空格
    std::regex whitespaceRegex(R"(\s+)");
    result = std::regex_replace(result, whitespaceRegex, " ");
    
    // 移除标签间的空格
    std::regex tagSpaceRegex(R"(>\s+<)");
    result = std::regex_replace(result, tagSpaceRegex, "><");
    
    return result;
}

std::string Exporter::minifyCSS(const std::string& css) const {
    std::string result = css;
    
    // 移除注释
    std::regex commentRegex(R"(/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 移除多余的空格
    std::regex whitespaceRegex(R"(\s+)");
    result = std::regex_replace(result, whitespaceRegex, " ");
    
    // 移除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    result = std::regex_replace(result, semicolonRegex, ";");
    
    return result;
}

std::string Exporter::minifyJavaScript(const std::string& javascript) const {
    std::string result = javascript;
    
    // 移除注释
    std::regex commentRegex(R"(//.*$|/\*[\s\S]*?\*/)");
    result = std::regex_replace(result, commentRegex, "");
    
    // 移除多余的空格
    std::regex whitespaceRegex(R"(\s+)");
    result = std::regex_replace(result, whitespaceRegex, " ");
    
    // 移除分号前的空格
    std::regex semicolonRegex(R"(\s*;\s*)");
    result = std::regex_replace(result, semicolonRegex, ";");
    
    return result;
}

std::string Exporter::generateSourceMap(const std::string& source, const std::string& output) const {
    // 简化实现，实际应该生成完整的 source map
    std::stringstream sourceMap;
    sourceMap << "{\n";
    sourceMap << "  \"version\": 3,\n";
    sourceMap << "  \"sources\": [\"" << source << "\"],\n";
    sourceMap << "  \"names\": [],\n";
    sourceMap << "  \"mappings\": \"\"\n";
    sourceMap << "}\n";
    
    return sourceMap.str();
}

// BuiltinCompiler 实现
BuiltinCompiler::BuiltinCompiler(std::shared_ptr<CHTLContext> context)
    : m_context(context), m_compilationMode("release"), m_optimizationLevel(1), m_debugMode(false), m_isCompiling(false) {
}

std::string BuiltinCompiler::compile(const std::string& chtlCode) {
    if (chtlCode.empty()) {
        return "";
    }
    
    m_isCompiling = true;
    clearCompilationState();
    
    try {
        // 这里需要实际的 CHTL 编译逻辑
        // 简化实现，返回基本的编译结果
        
        std::string result = chtlCode;
        
        if (m_optimizationLevel > 0) {
            result = optimizeCode(result);
        }
        
        m_isCompiling = false;
        return result;
        
    } catch (const std::exception& e) {
        addCompilationError(e.what());
        m_isCompiling = false;
        return "";
    }
}

std::string BuiltinCompiler::compileFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addCompilationError("Cannot open file: " + filePath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string chtlCode = buffer.str();
    
    return compile(chtlCode);
}

void BuiltinCompiler::compileToFile(const std::string& chtlCode, const std::string& outputPath) {
    std::string compiledCode = compile(chtlCode);
    if (compiledCode.empty()) {
        return;
    }
    
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        addCompilationError("Cannot create output file: " + outputPath);
        return;
    }
    
    file << compiledCode;
    file.close();
}

void BuiltinCompiler::compileFileToFile(const std::string& inputPath, const std::string& outputPath) {
    std::string compiledCode = compileFile(inputPath);
    if (compiledCode.empty()) {
        return;
    }
    
    compileToFile(compiledCode, outputPath);
}

void BuiltinCompiler::clearCompilationState() {
    m_compilationErrors.clear();
    m_compilationWarnings.clear();
}

void BuiltinCompiler::addCompilationError(const std::string& error) {
    m_compilationErrors.push_back(error);
}

void BuiltinCompiler::addCompilationWarning(const std::string& warning) {
    m_compilationWarnings.push_back(warning);
}

std::string BuiltinCompiler::optimizeCode(const std::string& code) const {
    std::string result = code;
    
    // 基本的代码优化
    // 移除多余的空行
    std::regex multipleNewlines(R"(\n\s*\n\s*\n)");
    result = std::regex_replace(result, multipleNewlines, "\n\n");
    
    // 移除多余的空格
    std::regex multipleSpaces(R"(\s{2,})");
    result = std::regex_replace(result, multipleSpaces, " ");
    
    return result;
}

} // namespace CHTL