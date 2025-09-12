#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
// #include <json/json.h> // 暂时移除jsoncpp依赖

namespace CHTL {

/**
 * VSCode扩展功能类型
 */
enum class VSCodeFeatureType {
    CODE_HIGHLIGHTING,      // 代码高亮
    CODE_FORMATTING,        // 代码格式化
    CODE_COMPLETION,        // 代码提示
    PAGE_PREVIEW,           // 页面预览
    RIGHT_CLICK_ACTIONS,    // 右键操作
    REAL_TIME_PREVIEW,      // 实时预览
    BUILTIN_COMPILER,       // 内置编译器
    OFFICIAL_MODULES,       // 官方模块
    AUTO_MODULE_UNPACK,     // 自动模块解包
    JSON_QUERY_TABLE,       // JSON查询表
    MODULE_SYNTAX_HINTS,    // 模块语法提示
    BRACKET_COMPLETION,     // 括号自动补全
    COMPILATION_MONITOR     // 编译监视
};

/**
 * 模块解包信息
 */
struct ModuleUnpackInfo {
    std::string module_name;
    std::string module_type;  // "CMOD" or "CJMOD"
    std::string module_path;
    std::vector<std::string> exported_items;
    std::map<std::string, std::string> item_descriptions;
    std::map<std::string, std::string> item_types;
    std::map<std::string, std::vector<std::string>> item_parameters;
    std::string json_table_path;
    
    ModuleUnpackInfo() = default;
};

/**
 * 语法提示信息
 */
struct SyntaxHintInfo {
    std::string hint_text;
    std::string hint_type;    // "function", "variable", "module", "template"
    std::string description;
    std::string return_type;
    std::vector<std::string> parameters;
    std::string documentation_url;
    int priority;
    
    SyntaxHintInfo() : priority(0) {}
};

/**
. 代码格式化配置
 */
struct CodeFormattingConfig {
    bool enable_chtl_formatting;
    bool enable_js_formatting;
    bool enable_css_formatting;
    bool enable_html_formatting;
    std::string cpp_style_config;
    std::string js_style_config;
    int indent_size;
    bool use_tabs;
    bool auto_format_on_save;
    
    CodeFormattingConfig() 
        : enable_chtl_formatting(true)
        , enable_js_formatting(true)
        , enable_css_formatting(true)
        , enable_html_formatting(true)
        , indent_size(4)
        , use_tabs(false)
        , auto_format_on_save(true) {
    }
};

/**
 * VSCode扩展配置
 */
struct VSCodeExtensionConfig {
    bool enable_all_features;
    std::vector<VSCodeFeatureType> enabled_features;
    CodeFormattingConfig formatting_config;
    std::string workspace_root;
    std::string chtl_compiler_path;
    std::string output_directory;
    bool enable_auto_compilation;
    bool enable_real_time_preview;
    std::string preview_server_url;
    int preview_server_port;
    
    VSCodeExtensionConfig() 
        : enable_all_features(true)
        , workspace_root(".")
        , chtl_compiler_path("chtl")
        , output_directory("./dist")
        , enable_auto_compilation(true)
        , enable_real_time_preview(true)
        , preview_server_url("http://localhost")
        , preview_server_port(3000) {
    }
};

/**
 * 自动模块解包器
 */
class AutoModuleUnpacker {
public:
    AutoModuleUnpacker();
    ~AutoModuleUnpacker();
    
    // 模块解包
    ModuleUnpackInfo unpackCMODModule(const std::string& module_path) const;
    ModuleUnpackInfo unpackCJMODModule(const std::string& module_path) const;
    ModuleUnpackInfo unpackModule(const std::string& module_path) const;
    
    // 批量解包
    std::vector<ModuleUnpackInfo> unpackAllModules(const std::string& modules_directory) const;
    std::vector<ModuleUnpackInfo> unpackModulesByPattern(const std::string& pattern) const;
    
    // 解包信息管理
    void addUnpackInfo(const ModuleUnpackInfo& info);
    void removeUnpackInfo(const std::string& module_name);
    ModuleUnpackInfo getUnpackInfo(const std::string& module_name) const;
    std::vector<ModuleUnpackInfo> getAllUnpackInfo() const;
    
    // 导出内容提取
    std::vector<std::string> extractExportBlocks(const std::string& module_content) const;
    std::map<std::string, std::string> parseExportBlocks(const std::vector<std::string>& blocks) const;
    
    // CJMOD函数分析
    std::vector<std::string> extractCJMODFunctions(const std::string& module_content) const;
    std::map<std::string, std::string> analyzeCJMODFunctions(const std::vector<std::string>& functions) const;
    
    // 性能优化
    void enableCaching(bool enabled);
    bool isCachingEnabled() const;
    void clearCache();
    
private:
    std::map<std::string, ModuleUnpackInfo> unpack_info_cache_;
    bool caching_enabled_;
    
    // 辅助函数
    std::string readFileContent(const std::string& file_path) const;
    bool isCMODFile(const std::string& file_path) const;
    bool isCJMODFile(const std::string& file_path) const;
    std::string generateJSONTable(const ModuleUnpackInfo& info) const;
    std::string extractModuleName(const std::string& module_path) const;
};

/**
 * JSON查询表生成器
 */
class JSONQueryTableGenerator {
public:
    JSONQueryTableGenerator();
    ~JSONQueryTableGenerator();
    
    // 查询表生成
    std::string generateModuleQueryTable(const ModuleUnpackInfo& info) const;
    std::string generateGlobalQueryTable(const std::vector<ModuleUnpackInfo>& modules) const;
    std::string generateSyntaxQueryTable(const std::vector<SyntaxHintInfo>& hints) const;
    
    // 查询表管理
    void saveQueryTable(const std::string& table_name, const std::string& content);
    std::string loadQueryTable(const std::string& table_name) const;
    void deleteQueryTable(const std::string& table_name);
    std::vector<std::string> listQueryTables() const;
    
    // 查询操作
    std::vector<std::string> queryModules(const std::string& query) const;
    std::vector<SyntaxHintInfo> querySyntaxHints(const std::string& query) const;
    std::vector<std::string> queryExportedItems(const std::string& module_name) const;
    
    // 索引管理
    void buildIndex(const std::vector<ModuleUnpackInfo>& modules);
    void rebuildIndex();
    bool isIndexBuilt() const;
    
    // 性能优化
    void enableCaching(bool enabled);
    bool isCachingEnabled() const;
    void clearCache();
    
private:
    std::map<std::string, std::string> query_tables_;
    std::map<std::string, std::vector<std::string>> module_index_;
    std::map<std::string, std::vector<SyntaxHintInfo>> syntax_index_;
    bool caching_enabled_;
    bool index_built_;
    
    // 辅助函数
    std::string createModuleJSON(const ModuleUnpackInfo& info) const;
    std::string createSyntaxJSON(const std::vector<SyntaxHintInfo>& hints) const;
    std::vector<std::string> searchInIndex(const std::string& query, const std::map<std::string, std::vector<std::string>>& index) const;
    void addToIndex(const std::string& key, const std::string& value);
};

/**
 * 模块语法提示器
 */
class ModuleSyntaxHinter {
public:
    ModuleSyntaxHinter();
    ~ModuleSyntaxHinter();
    
    // 语法提示生成
    std::vector<SyntaxHintInfo> generateHints(const std::string& context, const std::string& input) const;
    std::vector<SyntaxHintInfo> generateModuleHints(const std::string& module_name) const;
    std::vector<SyntaxHintInfo> generateFunctionHints(const std::string& function_name) const;
    std::vector<SyntaxHintInfo> generateTemplateHints(const std::string& template_name) const;
    
    // 提示管理
    void addHint(const SyntaxHintInfo& hint);
    void removeHint(const std::string& hint_text);
    void updateHint(const std::string& hint_text, const SyntaxHintInfo& new_hint);
    std::vector<SyntaxHintInfo> getAllHints() const;
    
    // 上下文分析
    std::string analyzeContext(const std::string& code, int cursor_position) const;
    std::string detectInputType(const std::string& input) const;
    std::vector<std::string> extractKeywords(const std::string& code) const;
    
    // 智能提示
    std::vector<SyntaxHintInfo> getSmartHints(const std::string& context, const std::string& input) const;
    std::vector<SyntaxHintInfo> getContextualHints(const std::string& code, int cursor_position) const;
    std::vector<SyntaxHintInfo> getCompletionHints(const std::string& partial_input) const;
    
    // 提示排序
    void sortHintsByRelevance(std::vector<SyntaxHintInfo>& hints, const std::string& input) const;
    void sortHintsByPriority(std::vector<SyntaxHintInfo>& hints) const;
    void sortHintsByType(std::vector<SyntaxHintInfo>& hints) const;
    
private:
    std::vector<SyntaxHintInfo> hints_;
    std::map<std::string, std::vector<SyntaxHintInfo>> hint_index_;
    
    // 辅助函数
    double calculateRelevance(const SyntaxHintInfo& hint, const std::string& input) const;
    bool matchesInput(const SyntaxHintInfo& hint, const std::string& input) const;
    std::string extractHintPrefix(const std::string& input) const;
    void updateHintIndex();
};

/**
 * 代码高亮器
 */
class CodeHighlighter {
public:
    CodeHighlighter();
    ~CodeHighlighter();
    
    // 代码高亮
    std::string highlightCHTL(const std::string& code) const;
    std::string highlightCHTLJS(const std::string& code) const;
    std::string highlightCSS(const std::string& code) const;
    std::string highlightHTML(const std::string& code) const;
    
    // 语法检测
    std::string detectLanguage(const std::string& code) const;
    bool isCHTLCode(const std::string& code) const;
    bool isCHTLJSCode(const std::string& code) const;
    
    // 主题管理
    void setTheme(const std::string& theme_name);
    std::string getCurrentTheme() const;
    std::vector<std::string> getAvailableThemes() const;
    
    // 高亮配置
    void setHighlightingConfig(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getHighlightingConfig() const;
    
private:
    std::string current_theme_;
    std::map<std::string, std::string> highlighting_config_;
    std::map<std::string, std::map<std::string, std::string>> theme_configs_;
    
    // 辅助函数
    void initializeThemes();
    std::string applyTheme(const std::string& code, const std::string& language) const;
    std::string tokenizeCHTL(const std::string& code) const;
    std::string tokenizeCHTLJS(const std::string& code) const;
};

/**
 * 代码格式化器
 */
class CodeFormatter {
public:
    CodeFormatter(const CodeFormattingConfig& config);
    ~CodeFormatter();
    
    // 代码格式化
    std::string formatCHTL(const std::string& code) const;
    std::string formatCHTLJS(const std::string& code) const;
    std::string formatCSS(const std::string& code) const;
    std::string formatHTML(const std::string& code) const;
    
    // 格式化配置
    void updateConfig(const CodeFormattingConfig& config);
    CodeFormattingConfig getConfig() const;
    
    // 格式化选项
    void setIndentSize(int size);
    void setUseTabs(bool use_tabs);
    void setAutoFormatOnSave(bool enabled);
    
private:
    CodeFormattingConfig config_;
    
    // 辅助函数
    std::string formatWithIndent(const std::string& code, int indent_size, bool use_tabs) const;
    std::string formatCHTLWithRules(const std::string& code) const;
    std::string formatJSWithRules(const std::string& code) const;
    std::string formatCSSWithRules(const std::string& code) const;
    std::string formatHTMLWithRules(const std::string& code) const;
};

/**
 * 页面预览器
 */
class PagePreviewer {
public:
    PagePreviewer();
    ~PagePreviewer();
    
    // 预览功能
    void startPreviewServer(int port = 3000);
    void stopPreviewServer();
    bool isPreviewServerRunning() const;
    
    // 页面预览
    std::string generatePreviewHTML(const std::string& chtl_code) const;
    std::string generatePreviewURL(const std::string& file_path) const;
    void openInBrowser(const std::string& url) const;
    
    // 实时预览
    void enableRealTimePreview(bool enabled);
    bool isRealTimePreviewEnabled() const;
    void updatePreview(const std::string& file_path) const;
    
    // 预览配置
    void setPreviewConfig(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getPreviewConfig() const;
    
private:
    bool preview_server_running_;
    bool real_time_preview_enabled_;
    int preview_port_;
    std::map<std::string, std::string> preview_config_;
    
    // 辅助函数
    std::string compileForPreview(const std::string& chtl_code) const;
    void startHTTPServer(int port);
    void stopHTTPServer();
    std::string generateServerHTML(const std::string& content) const;
};

/**
 * 右键操作处理器
 */
class RightClickActionHandler {
public:
    RightClickActionHandler();
    ~RightClickActionHandler();
    
    // 右键操作
    void openInBrowser(const std::string& file_path) const;
    void openDocumentation(const std::string& topic) const;
    void exportToHTML(const std::string& file_path) const;
    void exportToCSS(const std::string& file_path) const;
    void exportToJS(const std::string& file_path) const;
    void compileFile(const std::string& file_path) const;
    
    // 操作配置
    void setActionConfig(const std::map<std::string, std::string>& config);
    std::map<std::string, std::string> getActionConfig() const;
    
    // 自定义操作
    void addCustomAction(const std::string& action_name, std::function<void(const std::string&)> action);
    void removeCustomAction(const std::string& action_name);
    std::vector<std::string> getAvailableActions() const;
    
private:
    std::map<std::string, std::string> action_config_;
    std::map<std::string, std::function<void(const std::string&)>> custom_actions_;
    
    // 辅助函数
    std::string getBrowserCommand() const;
    std::string getDocumentationURL(const std::string& topic) const;
    void executeCommand(const std::string& command) const;
};

/**
 * 括号自动补全器
 */
class BracketCompletionHandler {
public:
    BracketCompletionHandler();
    ~BracketCompletionHandler();
    
    // 括号补全
    std::string handleBracketCompletion(const std::string& code, int cursor_position, char bracket) const;
    std::string handleAutoCompletion(const std::string& code, int cursor_position) const;
    
    // 补全配置
    void setCompletionConfig(const std::map<std::string, bool>& config);
    std::map<std::string, bool> getCompletionConfig() const;
    
    // 括号匹配
    bool isBracketMatched(const std::string& code) const;
    std::vector<int> findUnmatchedBrackets(const std::string& code) const;
    
private:
    std::map<std::string, bool> completion_config_;
    std::map<char, char> bracket_pairs_;
    
    // 辅助函数
    void initializeBracketPairs();
    char getClosingBracket(char opening_bracket) const;
    bool isOpeningBracket(char c) const;
    bool isClosingBracket(char c) const;
};

/**
 * VSCode扩展主类
 */
class VSCodeExtension {
public:
    VSCodeExtension(const VSCodeExtensionConfig& config);
    ~VSCodeExtension();
    
    // 扩展管理
    void initialize();
    void activate();
    void deactivate();
    bool isActive() const;
    
    // 功能管理
    void enableFeature(VSCodeFeatureType feature);
    void disableFeature(VSCodeFeatureType feature);
    bool isFeatureEnabled(VSCodeFeatureType feature) const;
    
    // 配置管理
    void updateConfig(const VSCodeExtensionConfig& config);
    VSCodeExtensionConfig getConfig() const;
    
    // 组件访问
    AutoModuleUnpacker& getModuleUnpacker();
    JSONQueryTableGenerator& getQueryTableGenerator();
    ModuleSyntaxHinter& getSyntaxHinter();
    CodeHighlighter& getCodeHighlighter();
    CodeFormatter& getCodeFormatter();
    PagePreviewer& getPagePreviewer();
    RightClickActionHandler& getRightClickHandler();
    BracketCompletionHandler& getBracketCompletionHandler();
    
    // 集成功能
    void processFile(const std::string& file_path);
    void processWorkspace(const std::string& workspace_path);
    std::string getExtensionStatus() const;
    
private:
    VSCodeExtensionConfig config_;
    bool is_active_;
    std::vector<VSCodeFeatureType> enabled_features_;
    
    // 组件
    std::unique_ptr<AutoModuleUnpacker> module_unpacker_;
    std::unique_ptr<JSONQueryTableGenerator> query_table_generator_;
    std::unique_ptr<ModuleSyntaxHinter> syntax_hinter_;
    std::unique_ptr<CodeHighlighter> code_highlighter_;
    std::unique_ptr<CodeFormatter> code_formatter_;
    std::unique_ptr<PagePreviewer> page_previewer_;
    std::unique_ptr<RightClickActionHandler> right_click_handler_;
    std::unique_ptr<BracketCompletionHandler> bracket_completion_handler_;
    
    // 辅助函数
    void initializeComponents();
    void setupFeatureDependencies();
    void validateConfig() const;
};

} // namespace CHTL