#include "CHTL/VSCodeExtension.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>
#include <thread>
#include <chrono>

// 简化的JSON替代实现
namespace SimpleJSON {
    class Value {
    public:
        Value() = default;
        Value(const std::string& str) : value_(str) {}
        
        std::string toString() const {
            return value_;
        }
        
    private:
        std::string value_;
    };
    
    class StreamWriter {
    public:
        std::string write(const Value& value) {
            return value.toString();
        }
    };
    
    class StreamWriterBuilder {
    public:
        std::unique_ptr<StreamWriter> newStreamWriter() {
            return std::make_unique<StreamWriter>();
        }
    };
}

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/wait.h>
#elif defined(__APPLE__)
#include <unistd.h>
#include <sys/wait.h>
#endif

namespace CHTL {

// AutoModuleUnpacker 实现
AutoModuleUnpacker::AutoModuleUnpacker() 
    : caching_enabled_(true) {
}

AutoModuleUnpacker::~AutoModuleUnpacker() = default;

ModuleUnpackInfo AutoModuleUnpacker::unpackCMODModule(const std::string& module_path) const {
    ModuleUnpackInfo info;
    info.module_path = module_path;
    info.module_type = "CMOD";
    info.module_name = extractModuleName(module_path);
    
    std::string content = readFileContent(module_path);
    if (content.empty()) {
        return info;
    }
    
    // 提取[Export]块
    auto export_blocks = extractExportBlocks(content);
    auto parsed_exports = parseExportBlocks(export_blocks);
    
    for (const auto& pair : parsed_exports) {
        info.exported_items.push_back(pair.first);
        info.item_descriptions[pair.first] = pair.second;
        info.item_types[pair.first] = "export";
    }
    
    // 生成JSON表
    info.json_table_path = generateJSONTable(info);
    
    return info;
}

ModuleUnpackInfo AutoModuleUnpacker::unpackCJMODModule(const std::string& module_path) const {
    ModuleUnpackInfo info;
    info.module_path = module_path;
    info.module_type = "CJMOD";
    info.module_name = extractModuleName(module_path);
    
    std::string content = readFileContent(module_path);
    if (content.empty()) {
        return info;
    }
    
    // 提取CJMOD函数
    auto functions = extractCJMODFunctions(content);
    auto analyzed_functions = analyzeCJMODFunctions(functions);
    
    for (const auto& pair : analyzed_functions) {
        info.exported_items.push_back(pair.first);
        info.item_descriptions[pair.first] = pair.second;
        info.item_types[pair.first] = "function";
    }
    
    // 生成JSON表
    info.json_table_path = generateJSONTable(info);
    
    return info;
}

ModuleUnpackInfo AutoModuleUnpacker::unpackModule(const std::string& module_path) const {
    if (isCMODFile(module_path)) {
        return unpackCMODModule(module_path);
    } else if (isCJMODFile(module_path)) {
        return unpackCJMODModule(module_path);
    }
    
    return ModuleUnpackInfo();
}

std::vector<ModuleUnpackInfo> AutoModuleUnpacker::unpackAllModules(const std::string& modules_directory) const {
    std::vector<ModuleUnpackInfo> results;
    
    // 这里应该实现目录扫描逻辑
    // 简化实现，返回空结果
    return results;
}

std::vector<ModuleUnpackInfo> AutoModuleUnpacker::unpackModulesByPattern(const std::string& pattern) const {
    std::vector<ModuleUnpackInfo> results;
    
    // 这里应该实现模式匹配逻辑
    // 简化实现，返回空结果
    return results;
}

void AutoModuleUnpacker::addUnpackInfo(const ModuleUnpackInfo& info) {
    if (caching_enabled_) {
        unpack_info_cache_[info.module_name] = info;
    }
}

void AutoModuleUnpacker::removeUnpackInfo(const std::string& module_name) {
    unpack_info_cache_.erase(module_name);
}

ModuleUnpackInfo AutoModuleUnpacker::getUnpackInfo(const std::string& module_name) const {
    auto it = unpack_info_cache_.find(module_name);
    if (it != unpack_info_cache_.end()) {
        return it->second;
    }
    return ModuleUnpackInfo();
}

std::vector<ModuleUnpackInfo> AutoModuleUnpacker::getAllUnpackInfo() const {
    std::vector<ModuleUnpackInfo> results;
    for (const auto& pair : unpack_info_cache_) {
        results.push_back(pair.second);
    }
    return results;
}

std::vector<std::string> AutoModuleUnpacker::extractExportBlocks(const std::string& module_content) const {
    std::vector<std::string> blocks;
    
    std::regex export_regex(R"(\[Export\][\s\S]*?\[/Export\])");
    std::sregex_iterator begin(module_content.begin(), module_content.end(), export_regex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        blocks.push_back(it->str());
    }
    
    return blocks;
}

std::map<std::string, std::string> AutoModuleUnpacker::parseExportBlocks(const std::vector<std::string>& blocks) const {
    std::map<std::string, std::string> exports;
    
    for (const auto& block : blocks) {
        // 简化实现，提取基本的导出项
        std::regex item_regex(R"(\w+\s*=\s*[^;]+)");
        std::sregex_iterator begin(block.begin(), block.end(), item_regex);
        std::sregex_iterator end;
        
        for (auto it = begin; it != end; ++it) {
            std::string item = it->str();
            size_t eq_pos = item.find('=');
            if (eq_pos != std::string::npos) {
                std::string name = item.substr(0, eq_pos);
                std::string value = item.substr(eq_pos + 1);
                
                // 去除空白字符
                name.erase(0, name.find_first_not_of(" \t"));
                name.erase(name.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);
                
                exports[name] = value;
            }
        }
    }
    
    return exports;
}

std::vector<std::string> AutoModuleUnpacker::extractCJMODFunctions(const std::string& module_content) const {
    std::vector<std::string> functions;
    
    std::regex function_regex(R"(function\s+\w+\s*\([^)]*\)\s*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\})");
    std::sregex_iterator begin(module_content.begin(), module_content.end(), function_regex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        functions.push_back(it->str());
    }
    
    return functions;
}

std::map<std::string, std::string> AutoModuleUnpacker::analyzeCJMODFunctions(const std::vector<std::string>& functions) const {
    std::map<std::string, std::string> analyzed;
    
    for (const auto& func : functions) {
        std::regex name_regex(R"(function\s+(\w+)\s*\()");
        std::smatch match;
        if (std::regex_search(func, match, name_regex)) {
            std::string name = match[1].str();
            analyzed[name] = func;
        }
    }
    
    return analyzed;
}

void AutoModuleUnpacker::enableCaching(bool enabled) {
    caching_enabled_ = enabled;
}

bool AutoModuleUnpacker::isCachingEnabled() const {
    return caching_enabled_;
}

void AutoModuleUnpacker::clearCache() {
    unpack_info_cache_.clear();
}

std::string AutoModuleUnpacker::readFileContent(const std::string& file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << file.rdbuf();
    file.close();
    
    return oss.str();
}

bool AutoModuleUnpacker::isCMODFile(const std::string& file_path) const {
    return file_path.length() >= 5 && file_path.substr(file_path.length() - 5) == ".cmod";
}

bool AutoModuleUnpacker::isCJMODFile(const std::string& file_path) const {
    return file_path.length() >= 6 && file_path.substr(file_path.length() - 6) == ".cjmod";
}

std::string AutoModuleUnpacker::generateJSONTable(const ModuleUnpackInfo& info) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"module_name\": \"" << info.module_name << "\",\n";
    oss << "  \"module_type\": \"" << info.module_type << "\",\n";
    oss << "  \"module_path\": \"" << info.module_path << "\",\n";
    oss << "  \"exported_items\": [\n";
    
    for (size_t i = 0; i < info.exported_items.size(); ++i) {
        const auto& item = info.exported_items[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << item << "\",\n";
        oss << "      \"description\": \"" << info.item_descriptions.at(item) << "\",\n";
        oss << "      \"type\": \"" << info.item_types.at(item) << "\"\n";
        oss << "    }";
        if (i < info.exported_items.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}";
    return oss.str();
}

std::string AutoModuleUnpacker::extractModuleName(const std::string& module_path) const {
    size_t last_slash = module_path.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? module_path : module_path.substr(last_slash + 1);
    
    size_t last_dot = filename.find_last_of('.');
    if (last_dot != std::string::npos) {
        filename = filename.substr(0, last_dot);
    }
    
    return filename;
}

// JSONQueryTableGenerator 实现
JSONQueryTableGenerator::JSONQueryTableGenerator() 
    : caching_enabled_(true)
    , index_built_(false) {
}

JSONQueryTableGenerator::~JSONQueryTableGenerator() = default;

std::string JSONQueryTableGenerator::generateModuleQueryTable(const ModuleUnpackInfo& info) const {
    return createModuleJSON(info);
}

std::string JSONQueryTableGenerator::generateGlobalQueryTable(const std::vector<ModuleUnpackInfo>& modules) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"type\": \"global_query_table\",\n";
    oss << "  \"total_modules\": " << modules.size() << ",\n";
    oss << "  \"modules\": [\n";
    
    for (size_t i = 0; i < modules.size(); ++i) {
        const auto& module = modules[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << module.module_name << "\",\n";
        oss << "      \"type\": \"" << module.module_type << "\",\n";
        oss << "      \"path\": \"" << module.module_path << "\",\n";
        oss << "      \"exported_items_count\": " << module.exported_items.size() << "\n";
        oss << "    }";
        if (i < modules.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}";
    return oss.str();
}

std::string JSONQueryTableGenerator::generateSyntaxQueryTable(const std::vector<SyntaxHintInfo>& hints) const {
    return createSyntaxJSON(hints);
}

void JSONQueryTableGenerator::saveQueryTable(const std::string& table_name, const std::string& content) {
    if (caching_enabled_) {
        query_tables_[table_name] = content;
    }
    
    std::string filename = table_name + ".json";
    std::ofstream file(filename);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

std::string JSONQueryTableGenerator::loadQueryTable(const std::string& table_name) const {
    if (caching_enabled_) {
        auto it = query_tables_.find(table_name);
        if (it != query_tables_.end()) {
            return it->second;
        }
    }
    
    std::string filename = table_name + ".json";
    std::ifstream file(filename);
    if (file.is_open()) {
        std::ostringstream oss;
        oss << file.rdbuf();
        file.close();
        return oss.str();
    }
    
    return "";
}

void JSONQueryTableGenerator::deleteQueryTable(const std::string& table_name) {
    query_tables_.erase(table_name);
    
    std::string filename = table_name + ".json";
    std::remove(filename.c_str());
}

std::vector<std::string> JSONQueryTableGenerator::listQueryTables() const {
    std::vector<std::string> tables;
    for (const auto& pair : query_tables_) {
        tables.push_back(pair.first);
    }
    return tables;
}

std::vector<std::string> JSONQueryTableGenerator::queryModules(const std::string& query) const {
    if (!index_built_) {
        return {};
    }
    
    return searchInIndex(query, module_index_);
}

std::vector<SyntaxHintInfo> JSONQueryTableGenerator::querySyntaxHints(const std::string& query) const {
    if (!index_built_) {
        return {};
    }
    
    std::vector<SyntaxHintInfo> results;
    auto it = syntax_index_.find(query);
    if (it != syntax_index_.end()) {
        results = it->second;
    }
    
    return results;
}

std::vector<std::string> JSONQueryTableGenerator::queryExportedItems(const std::string& module_name) const {
    std::vector<std::string> results;
    
    auto it = module_index_.find(module_name);
    if (it != module_index_.end()) {
        results = it->second;
    }
    
    return results;
}

void JSONQueryTableGenerator::buildIndex(const std::vector<ModuleUnpackInfo>& modules) {
    module_index_.clear();
    syntax_index_.clear();
    
    for (const auto& module : modules) {
        addToIndex(module.module_name, module.module_name);
        
        for (const auto& item : module.exported_items) {
            addToIndex(module.module_name, item);
        }
    }
    
    index_built_ = true;
}

void JSONQueryTableGenerator::rebuildIndex() {
    index_built_ = false;
    module_index_.clear();
    syntax_index_.clear();
}

bool JSONQueryTableGenerator::isIndexBuilt() const {
    return index_built_;
}

void JSONQueryTableGenerator::enableCaching(bool enabled) {
    caching_enabled_ = enabled;
}

bool JSONQueryTableGenerator::isCachingEnabled() const {
    return caching_enabled_;
}

void JSONQueryTableGenerator::clearCache() {
    query_tables_.clear();
    module_index_.clear();
    syntax_index_.clear();
    index_built_ = false;
}

std::string JSONQueryTableGenerator::createModuleJSON(const ModuleUnpackInfo& info) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"module_name\": \"" << info.module_name << "\",\n";
    oss << "  \"module_type\": \"" << info.module_type << "\",\n";
    oss << "  \"module_path\": \"" << info.module_path << "\",\n";
    oss << "  \"exported_items\": [\n";
    
    for (size_t i = 0; i < info.exported_items.size(); ++i) {
        const auto& item = info.exported_items[i];
        oss << "    {\n";
        oss << "      \"name\": \"" << item << "\",\n";
        oss << "      \"description\": \"" << info.item_descriptions.at(item) << "\",\n";
        oss << "      \"type\": \"" << info.item_types.at(item) << "\"\n";
        oss << "    }";
        if (i < info.exported_items.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}";
    return oss.str();
}

std::string JSONQueryTableGenerator::createSyntaxJSON(const std::vector<SyntaxHintInfo>& hints) const {
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"type\": \"syntax_query_table\",\n";
    oss << "  \"total_hints\": " << hints.size() << ",\n";
    oss << "  \"hints\": [\n";
    
    for (size_t i = 0; i < hints.size(); ++i) {
        const auto& hint = hints[i];
        oss << "    {\n";
        oss << "      \"text\": \"" << hint.hint_text << "\",\n";
        oss << "      \"type\": \"" << hint.hint_type << "\",\n";
        oss << "      \"description\": \"" << hint.description << "\",\n";
        oss << "      \"return_type\": \"" << hint.return_type << "\",\n";
        oss << "      \"priority\": " << hint.priority << ",\n";
        oss << "      \"parameters\": [";
        
        for (size_t j = 0; j < hint.parameters.size(); ++j) {
            oss << "\"" << hint.parameters[j] << "\"";
            if (j < hint.parameters.size() - 1) oss << ",";
        }
        
        oss << "]\n";
        oss << "    }";
        if (i < hints.size() - 1) oss << ",";
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}";
    return oss.str();
}

std::vector<std::string> JSONQueryTableGenerator::searchInIndex(const std::string& query, const std::map<std::string, std::vector<std::string>>& index) const {
    std::vector<std::string> results;
    
    for (const auto& pair : index) {
        for (const auto& item : pair.second) {
            if (item.find(query) != std::string::npos) {
                results.push_back(item);
            }
        }
    }
    
    return results;
}

void JSONQueryTableGenerator::addToIndex(const std::string& key, const std::string& value) {
    module_index_[key].push_back(value);
}

// ModuleSyntaxHinter 实现
ModuleSyntaxHinter::ModuleSyntaxHinter() {
    updateHintIndex();
}

ModuleSyntaxHinter::~ModuleSyntaxHinter() = default;

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::generateHints(const std::string& context, const std::string& input) const {
    std::vector<SyntaxHintInfo> results;
    
    for (const auto& hint : hints_) {
        if (matchesInput(hint, input)) {
            results.push_back(hint);
        }
    }
    
    sortHintsByRelevance(results, input);
    return results;
}

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::generateModuleHints(const std::string& module_name) const {
    std::vector<SyntaxHintInfo> results;
    
    for (const auto& hint : hints_) {
        if (hint.hint_type == "module" && hint.hint_text.find(module_name) != std::string::npos) {
            results.push_back(hint);
        }
    }
    
    return results;
}

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::generateFunctionHints(const std::string& function_name) const {
    std::vector<SyntaxHintInfo> results;
    
    for (const auto& hint : hints_) {
        if (hint.hint_type == "function" && hint.hint_text.find(function_name) != std::string::npos) {
            results.push_back(hint);
        }
    }
    
    return results;
}

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::generateTemplateHints(const std::string& template_name) const {
    std::vector<SyntaxHintInfo> results;
    
    for (const auto& hint : hints_) {
        if (hint.hint_type == "template" && hint.hint_text.find(template_name) != std::string::npos) {
            results.push_back(hint);
        }
    }
    
    return results;
}

void ModuleSyntaxHinter::addHint(const SyntaxHintInfo& hint) {
    hints_.push_back(hint);
    updateHintIndex();
}

void ModuleSyntaxHinter::removeHint(const std::string& hint_text) {
    hints_.erase(
        std::remove_if(hints_.begin(), hints_.end(),
            [&hint_text](const SyntaxHintInfo& hint) {
                return hint.hint_text == hint_text;
            }),
        hints_.end()
    );
    updateHintIndex();
}

void ModuleSyntaxHinter::updateHint(const std::string& hint_text, const SyntaxHintInfo& new_hint) {
    for (auto& hint : hints_) {
        if (hint.hint_text == hint_text) {
            hint = new_hint;
            break;
        }
    }
    updateHintIndex();
}

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::getAllHints() const {
    return hints_;
}

std::string ModuleSyntaxHinter::analyzeContext(const std::string& code, int cursor_position) const {
    if (cursor_position < 0 || cursor_position >= static_cast<int>(code.length())) {
        return "";
    }
    
    // 简化实现，返回光标前的文本
    return code.substr(0, cursor_position);
}

std::string ModuleSyntaxHinter::detectInputType(const std::string& input) const {
    if (input.empty()) {
        return "empty";
    }
    
    if (input.find('(') != std::string::npos) {
        return "function_call";
    }
    
    if (input.find('.') != std::string::npos) {
        return "property_access";
    }
    
    if (input.find('[') != std::string::npos) {
        return "array_access";
    }
    
    return "identifier";
}

std::vector<std::string> ModuleSyntaxHinter::extractKeywords(const std::string& code) const {
    std::vector<std::string> keywords;
    
    std::regex word_regex(R"(\b\w+\b)");
    std::sregex_iterator begin(code.begin(), code.end(), word_regex);
    std::sregex_iterator end;
    
    for (auto it = begin; it != end; ++it) {
        keywords.push_back(it->str());
    }
    
    return keywords;
}

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::getSmartHints(const std::string& context, const std::string& input) const {
    std::vector<SyntaxHintInfo> results = generateHints(context, input);
    
    // 根据上下文智能排序
    sortHintsByRelevance(results, input);
    
    return results;
}

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::getContextualHints(const std::string& code, int cursor_position) const {
    std::string context = analyzeContext(code, cursor_position);
    std::string input = extractHintPrefix(context);
    
    return getSmartHints(context, input);
}

std::vector<SyntaxHintInfo> ModuleSyntaxHinter::getCompletionHints(const std::string& partial_input) const {
    std::vector<SyntaxHintInfo> results;
    
    for (const auto& hint : hints_) {
        if (hint.hint_text.find(partial_input) == 0) {
            results.push_back(hint);
        }
    }
    
    sortHintsByRelevance(results, partial_input);
    return results;
}

void ModuleSyntaxHinter::sortHintsByRelevance(std::vector<SyntaxHintInfo>& hints, const std::string& input) const {
    std::sort(hints.begin(), hints.end(),
        [this, &input](const SyntaxHintInfo& a, const SyntaxHintInfo& b) {
            double relevance_a = calculateRelevance(a, input);
            double relevance_b = calculateRelevance(b, input);
            return relevance_a > relevance_b;
        });
}

void ModuleSyntaxHinter::sortHintsByPriority(std::vector<SyntaxHintInfo>& hints) const {
    std::sort(hints.begin(), hints.end(),
        [](const SyntaxHintInfo& a, const SyntaxHintInfo& b) {
            return a.priority > b.priority;
        });
}

void ModuleSyntaxHinter::sortHintsByType(std::vector<SyntaxHintInfo>& hints) const {
    std::sort(hints.begin(), hints.end(),
        [](const SyntaxHintInfo& a, const SyntaxHintInfo& b) {
            return a.hint_type < b.hint_type;
        });
}

double ModuleSyntaxHinter::calculateRelevance(const SyntaxHintInfo& hint, const std::string& input) const {
    if (input.empty()) {
        return 0.0;
    }
    
    double relevance = 0.0;
    
    // 完全匹配
    if (hint.hint_text == input) {
        relevance += 100.0;
    }
    
    // 前缀匹配
    if (hint.hint_text.find(input) == 0) {
        relevance += 50.0;
    }
    
    // 包含匹配
    if (hint.hint_text.find(input) != std::string::npos) {
        relevance += 25.0;
    }
    
    // 优先级加成
    relevance += hint.priority;
    
    return relevance;
}

bool ModuleSyntaxHinter::matchesInput(const SyntaxHintInfo& hint, const std::string& input) const {
    if (input.empty()) {
        return true;
    }
    
    return hint.hint_text.find(input) != std::string::npos;
}

std::string ModuleSyntaxHinter::extractHintPrefix(const std::string& input) const {
    size_t last_space = input.find_last_of(" \t\n\r");
    if (last_space != std::string::npos) {
        return input.substr(last_space + 1);
    }
    return input;
}

void ModuleSyntaxHinter::updateHintIndex() {
    hint_index_.clear();
    
    for (const auto& hint : hints_) {
        hint_index_[hint.hint_type].push_back(hint);
    }
}

// CodeHighlighter 实现
CodeHighlighter::CodeHighlighter() 
    : current_theme_("default") {
    initializeThemes();
}

CodeHighlighter::~CodeHighlighter() = default;

std::string CodeHighlighter::highlightCHTL(const std::string& code) const {
    return applyTheme(tokenizeCHTL(code), "chtl");
}

std::string CodeHighlighter::highlightCHTLJS(const std::string& code) const {
    return applyTheme(tokenizeCHTLJS(code), "chtljs");
}

std::string CodeHighlighter::highlightCSS(const std::string& code) const {
    return applyTheme(code, "css");
}

std::string CodeHighlighter::highlightHTML(const std::string& code) const {
    return applyTheme(code, "html");
}

std::string CodeHighlighter::detectLanguage(const std::string& code) const {
    if (isCHTLCode(code)) {
        return "chtl";
    } else if (isCHTLJSCode(code)) {
        return "chtljs";
    } else if (code.find("<html") != std::string::npos || code.find("<div") != std::string::npos) {
        return "html";
    } else if (code.find("{") != std::string::npos && code.find("}") != std::string::npos) {
        return "css";
    }
    
    return "text";
}

bool CodeHighlighter::isCHTLCode(const std::string& code) const {
    return code.find("[Template]") != std::string::npos || 
           code.find("[Custom]") != std::string::npos ||
           code.find("[Import]") != std::string::npos;
}

bool CodeHighlighter::isCHTLJSCode(const std::string& code) const {
    return code.find("function") != std::string::npos && 
           code.find("->") != std::string::npos;
}

void CodeHighlighter::setTheme(const std::string& theme_name) {
    current_theme_ = theme_name;
}

std::string CodeHighlighter::getCurrentTheme() const {
    return current_theme_;
}

std::vector<std::string> CodeHighlighter::getAvailableThemes() const {
    std::vector<std::string> themes;
    for (const auto& pair : theme_configs_) {
        themes.push_back(pair.first);
    }
    return themes;
}

void CodeHighlighter::setHighlightingConfig(const std::map<std::string, std::string>& config) {
    highlighting_config_ = config;
}

std::map<std::string, std::string> CodeHighlighter::getHighlightingConfig() const {
    return highlighting_config_;
}

void CodeHighlighter::initializeThemes() {
    // 默认主题
    theme_configs_["default"] = {
        {"keyword", "color: #0000FF"},
        {"string", "color: #008000"},
        {"comment", "color: #808080"},
        {"function", "color: #800080"},
        {"variable", "color: #000000"}
    };
    
    // 暗色主题
    theme_configs_["dark"] = {
        {"keyword", "color: #569CD6"},
        {"string", "color: #CE9178"},
        {"comment", "color: #6A9955"},
        {"function", "color: #DCDCAA"},
        {"variable", "color: #D4D4D4"}
    };
    
    // 高对比度主题
    theme_configs_["high-contrast"] = {
        {"keyword", "color: #000000; background-color: #FFFF00"},
        {"string", "color: #008000; background-color: #FFFFFF"},
        {"comment", "color: #808080; background-color: #FFFFFF"},
        {"function", "color: #800080; background-color: #FFFFFF"},
        {"variable", "color: #000000; background-color: #FFFFFF"}
    };
}

std::string CodeHighlighter::applyTheme(const std::string& code, const std::string& language) const {
    auto theme_it = theme_configs_.find(current_theme_);
    if (theme_it == theme_configs_.end()) {
        return code;
    }
    
    const auto& theme = theme_it->second;
    std::string result = code;
    
    // 应用主题样式
    for (const auto& style : theme) {
        // 简化实现，直接返回原代码
        // 实际实现应该应用HTML/CSS样式
    }
    
    return result;
}

std::string CodeHighlighter::tokenizeCHTL(const std::string& code) const {
    // 简化实现，返回原代码
    return code;
}

std::string CodeHighlighter::tokenizeCHTLJS(const std::string& code) const {
    // 简化实现，返回原代码
    return code;
}

// CodeFormatter 实现
CodeFormatter::CodeFormatter(const CodeFormattingConfig& config) 
    : config_(config) {
}

CodeFormatter::~CodeFormatter() = default;

std::string CodeFormatter::formatCHTL(const std::string& code) const {
    if (!config_.enable_chtl_formatting) {
        return code;
    }
    
    return formatCHTLWithRules(code);
}

std::string CodeFormatter::formatCHTLJS(const std::string& code) const {
    if (!config_.enable_js_formatting) {
        return code;
    }
    
    return formatJSWithRules(code);
}

std::string CodeFormatter::formatCSS(const std::string& code) const {
    if (!config_.enable_css_formatting) {
        return code;
    }
    
    return formatCSSWithRules(code);
}

std::string CodeFormatter::formatHTML(const std::string& code) const {
    if (!config_.enable_html_formatting) {
        return code;
    }
    
    return formatHTMLWithRules(code);
}

void CodeFormatter::updateConfig(const CodeFormattingConfig& config) {
    config_ = config;
}

CodeFormattingConfig CodeFormatter::getConfig() const {
    return config_;
}

void CodeFormatter::setIndentSize(int size) {
    config_.indent_size = size;
}

void CodeFormatter::setUseTabs(bool use_tabs) {
    config_.use_tabs = use_tabs;
}

void CodeFormatter::setAutoFormatOnSave(bool enabled) {
    config_.auto_format_on_save = enabled;
}

std::string CodeFormatter::formatWithIndent(const std::string& code, int indent_size, bool use_tabs) const {
    std::string result = code;
    std::string indent = use_tabs ? "\t" : std::string(indent_size, ' ');
    
    // 简化实现，添加基本缩进
    std::istringstream iss(code);
    std::ostringstream oss;
    std::string line;
    int current_indent = 0;
    
    while (std::getline(iss, line)) {
        // 去除前导空白
        line.erase(0, line.find_first_not_of(" \t"));
        
        // 添加缩进
        for (int i = 0; i < current_indent; ++i) {
            oss << indent;
        }
        oss << line << "\n";
        
        // 调整缩进级别
        if (line.find('{') != std::string::npos) {
            current_indent++;
        }
        if (line.find('}') != std::string::npos) {
            current_indent = std::max(0, current_indent - 1);
        }
    }
    
    return oss.str();
}

std::string CodeFormatter::formatCHTLWithRules(const std::string& code) const {
    return formatWithIndent(code, config_.indent_size, config_.use_tabs);
}

std::string CodeFormatter::formatJSWithRules(const std::string& code) const {
    return formatWithIndent(code, config_.indent_size, config_.use_tabs);
}

std::string CodeFormatter::formatCSSWithRules(const std::string& code) const {
    return formatWithIndent(code, config_.indent_size, config_.use_tabs);
}

std::string CodeFormatter::formatHTMLWithRules(const std::string& code) const {
    return formatWithIndent(code, config_.indent_size, config_.use_tabs);
}

// PagePreviewer 实现
PagePreviewer::PagePreviewer() 
    : preview_server_running_(false)
    , real_time_preview_enabled_(false)
    , preview_port_(3000) {
}

PagePreviewer::~PagePreviewer() {
    stopPreviewServer();
}

void PagePreviewer::startPreviewServer(int port) {
    if (preview_server_running_) {
        return;
    }
    
    preview_port_ = port;
    preview_server_running_ = true;
    
    // 启动HTTP服务器
    startHTTPServer(port);
}

void PagePreviewer::stopPreviewServer() {
    if (!preview_server_running_) {
        return;
    }
    
    preview_server_running_ = false;
    stopHTTPServer();
}

bool PagePreviewer::isPreviewServerRunning() const {
    return preview_server_running_;
}

std::string PagePreviewer::generatePreviewHTML(const std::string& chtl_code) const {
    std::string compiled_html = compileForPreview(chtl_code);
    return generateServerHTML(compiled_html);
}

std::string PagePreviewer::generatePreviewURL(const std::string& file_path) const {
    auto it = preview_config_.find("base_url");
    if (it != preview_config_.end()) {
        return it->second + "/preview?file=" + file_path;
    }
    return "http://localhost:8080/preview?file=" + file_path;
}

void PagePreviewer::openInBrowser(const std::string& url) const {
#ifdef _WIN32
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__)
    std::string command = "xdg-open " + url;
    system(command.c_str());
#elif defined(__APPLE__)
    std::string command = "open " + url;
    system(command.c_str());
#endif
}

void PagePreviewer::enableRealTimePreview(bool enabled) {
    real_time_preview_enabled_ = enabled;
}

bool PagePreviewer::isRealTimePreviewEnabled() const {
    return real_time_preview_enabled_;
}

void PagePreviewer::updatePreview(const std::string& file_path) const {
    if (!real_time_preview_enabled_) {
        return;
    }
    
    // 实现实时预览更新逻辑
    // 简化实现
}

void PagePreviewer::setPreviewConfig(const std::map<std::string, std::string>& config) {
    preview_config_ = config;
}

std::map<std::string, std::string> PagePreviewer::getPreviewConfig() const {
    return preview_config_;
}

std::string PagePreviewer::compileForPreview(const std::string& chtl_code) const {
    // 简化实现，返回原代码
    return chtl_code;
}

void PagePreviewer::startHTTPServer(int port) {
    // 简化实现，实际应该启动HTTP服务器
}

void PagePreviewer::stopHTTPServer() {
    // 简化实现，实际应该停止HTTP服务器
}

std::string PagePreviewer::generateServerHTML(const std::string& content) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n";
    oss << "<html>\n";
    oss << "<head>\n";
    oss << "    <title>CHTL Preview</title>\n";
    oss << "</head>\n";
    oss << "<body>\n";
    oss << content << "\n";
    oss << "</body>\n";
    oss << "</html>\n";
    
    return oss.str();
}

// RightClickActionHandler 实现
RightClickActionHandler::RightClickActionHandler() {
    // 初始化默认配置
    action_config_["browser_command"] = "default";
    action_config_["documentation_url"] = "https://chtl.dev/docs";
}

RightClickActionHandler::~RightClickActionHandler() = default;

void RightClickActionHandler::openInBrowser(const std::string& file_path) const {
    std::string command = getBrowserCommand();
    if (command == "default") {
        command = "file://" + file_path;
    }
    
    executeCommand(command);
}

void RightClickActionHandler::openDocumentation(const std::string& topic) const {
    std::string url = getDocumentationURL(topic);
    executeCommand(url);
}

void RightClickActionHandler::exportToHTML(const std::string& file_path) const {
    // 实现HTML导出逻辑
    // 简化实现
}

void RightClickActionHandler::exportToCSS(const std::string& file_path) const {
    // 实现CSS导出逻辑
    // 简化实现
}

void RightClickActionHandler::exportToJS(const std::string& file_path) const {
    // 实现JS导出逻辑
    // 简化实现
}

void RightClickActionHandler::compileFile(const std::string& file_path) const {
    // 实现文件编译逻辑
    // 简化实现
}

void RightClickActionHandler::setActionConfig(const std::map<std::string, std::string>& config) {
    action_config_ = config;
}

std::map<std::string, std::string> RightClickActionHandler::getActionConfig() const {
    return action_config_;
}

void RightClickActionHandler::addCustomAction(const std::string& action_name, std::function<void(const std::string&)> action) {
    custom_actions_[action_name] = action;
}

void RightClickActionHandler::removeCustomAction(const std::string& action_name) {
    custom_actions_.erase(action_name);
}

std::vector<std::string> RightClickActionHandler::getAvailableActions() const {
    std::vector<std::string> actions;
    
    // 添加默认操作
    actions.push_back("open_in_browser");
    actions.push_back("open_documentation");
    actions.push_back("export_to_html");
    actions.push_back("export_to_css");
    actions.push_back("export_to_js");
    actions.push_back("compile_file");
    
    // 添加自定义操作
    for (const auto& pair : custom_actions_) {
        actions.push_back(pair.first);
    }
    
    return actions;
}

std::string RightClickActionHandler::getBrowserCommand() const {
    auto it = action_config_.find("browser_command");
    if (it != action_config_.end()) {
        return it->second;
    }
    return "default";
}

std::string RightClickActionHandler::getDocumentationURL(const std::string& topic) const {
    auto it = action_config_.find("documentation_url");
    if (it != action_config_.end()) {
        return it->second + "/" + topic;
    }
    return "https://chtl.dev/docs/" + topic;
}

void RightClickActionHandler::executeCommand(const std::string& command) const {
#ifdef _WIN32
    ShellExecuteA(NULL, "open", command.c_str(), NULL, NULL, SW_SHOWNORMAL);
#elif defined(__linux__)
    std::string cmd = "xdg-open " + command;
    system(cmd.c_str());
#elif defined(__APPLE__)
    std::string cmd = "open " + command;
    system(cmd.c_str());
#endif
}

// BracketCompletionHandler 实现
BracketCompletionHandler::BracketCompletionHandler() {
    initializeBracketPairs();
}

BracketCompletionHandler::~BracketCompletionHandler() = default;

std::string BracketCompletionHandler::handleBracketCompletion(const std::string& code, int cursor_position, char bracket) const {
    if (cursor_position < 0 || cursor_position >= static_cast<int>(code.length())) {
        return code;
    }
    
    char closing_bracket = getClosingBracket(bracket);
    if (closing_bracket == '\0') {
        return code;
    }
    
    std::string result = code;
    result.insert(cursor_position, std::string(1, closing_bracket));
    
    return result;
}

std::string BracketCompletionHandler::handleAutoCompletion(const std::string& code, int cursor_position) const {
    if (cursor_position < 0 || cursor_position >= static_cast<int>(code.length())) {
        return code;
    }
    
    char current_char = code[cursor_position];
    if (isOpeningBracket(current_char)) {
        return handleBracketCompletion(code, cursor_position + 1, current_char);
    }
    
    return code;
}

void BracketCompletionHandler::setCompletionConfig(const std::map<std::string, bool>& config) {
    completion_config_ = config;
}

std::map<std::string, bool> BracketCompletionHandler::getCompletionConfig() const {
    return completion_config_;
}

bool BracketCompletionHandler::isBracketMatched(const std::string& code) const {
    std::stack<char> bracket_stack;
    
    for (char c : code) {
        if (isOpeningBracket(c)) {
            bracket_stack.push(c);
        } else if (isClosingBracket(c)) {
            if (bracket_stack.empty()) {
                return false;
            }
            
            char opening = bracket_stack.top();
            bracket_stack.pop();
            
            if (getClosingBracket(opening) != c) {
                return false;
            }
        }
    }
    
    return bracket_stack.empty();
}

std::vector<int> BracketCompletionHandler::findUnmatchedBrackets(const std::string& code) const {
    std::vector<int> unmatched_positions;
    std::stack<std::pair<char, int>> bracket_stack;
    
    for (int i = 0; i < static_cast<int>(code.length()); ++i) {
        char c = code[i];
        
        if (isOpeningBracket(c)) {
            bracket_stack.push({c, i});
        } else if (isClosingBracket(c)) {
            if (bracket_stack.empty()) {
                unmatched_positions.push_back(i);
            } else {
                char opening = bracket_stack.top().first;
                bracket_stack.pop();
                
                if (getClosingBracket(opening) != c) {
                    unmatched_positions.push_back(i);
                }
            }
        }
    }
    
    // 添加未匹配的开始括号
    while (!bracket_stack.empty()) {
        unmatched_positions.push_back(bracket_stack.top().second);
        bracket_stack.pop();
    }
    
    return unmatched_positions;
}

void BracketCompletionHandler::initializeBracketPairs() {
    bracket_pairs_['('] = ')';
    bracket_pairs_['['] = ']';
    bracket_pairs_['{'] = '}';
    bracket_pairs_['<'] = '>';
    bracket_pairs_['"'] = '"';
    bracket_pairs_['\''] = '\'';
}

char BracketCompletionHandler::getClosingBracket(char opening_bracket) const {
    auto it = bracket_pairs_.find(opening_bracket);
    if (it != bracket_pairs_.end()) {
        return it->second;
    }
    return '\0';
}

bool BracketCompletionHandler::isOpeningBracket(char c) const {
    return bracket_pairs_.find(c) != bracket_pairs_.end();
}

bool BracketCompletionHandler::isClosingBracket(char c) const {
    for (const auto& pair : bracket_pairs_) {
        if (pair.second == c) {
            return true;
        }
    }
    return false;
}

// VSCodeExtension 实现
VSCodeExtension::VSCodeExtension(const VSCodeExtensionConfig& config) 
    : config_(config)
    , is_active_(false) {
    initializeComponents();
}

VSCodeExtension::~VSCodeExtension() {
    deactivate();
}

void VSCodeExtension::initialize() {
    validateConfig();
    setupFeatureDependencies();
}

void VSCodeExtension::activate() {
    if (is_active_) {
        return;
    }
    
    is_active_ = true;
    
    // 激活所有启用的功能
    for (auto feature : enabled_features_) {
        enableFeature(feature);
    }
}

void VSCodeExtension::deactivate() {
    if (!is_active_) {
        return;
    }
    
    is_active_ = false;
    
    // 停用所有功能
    for (auto feature : enabled_features_) {
        disableFeature(feature);
    }
}

bool VSCodeExtension::isActive() const {
    return is_active_;
}

void VSCodeExtension::enableFeature(VSCodeFeatureType feature) {
    if (std::find(enabled_features_.begin(), enabled_features_.end(), feature) == enabled_features_.end()) {
        enabled_features_.push_back(feature);
    }
}

void VSCodeExtension::disableFeature(VSCodeFeatureType feature) {
    enabled_features_.erase(
        std::remove(enabled_features_.begin(), enabled_features_.end(), feature),
        enabled_features_.end()
    );
}

bool VSCodeExtension::isFeatureEnabled(VSCodeFeatureType feature) const {
    return std::find(enabled_features_.begin(), enabled_features_.end(), feature) != enabled_features_.end();
}

void VSCodeExtension::updateConfig(const VSCodeExtensionConfig& config) {
    config_ = config;
    validateConfig();
}

VSCodeExtensionConfig VSCodeExtension::getConfig() const {
    return config_;
}

AutoModuleUnpacker& VSCodeExtension::getModuleUnpacker() {
    return *module_unpacker_;
}

JSONQueryTableGenerator& VSCodeExtension::getQueryTableGenerator() {
    return *query_table_generator_;
}

ModuleSyntaxHinter& VSCodeExtension::getSyntaxHinter() {
    return *syntax_hinter_;
}

CodeHighlighter& VSCodeExtension::getCodeHighlighter() {
    return *code_highlighter_;
}

CodeFormatter& VSCodeExtension::getCodeFormatter() {
    return *code_formatter_;
}

PagePreviewer& VSCodeExtension::getPagePreviewer() {
    return *page_previewer_;
}

RightClickActionHandler& VSCodeExtension::getRightClickHandler() {
    return *right_click_handler_;
}

BracketCompletionHandler& VSCodeExtension::getBracketCompletionHandler() {
    return *bracket_completion_handler_;
}

void VSCodeExtension::processFile(const std::string& file_path) {
    if (!is_active_) {
        return;
    }
    
    // 处理文件
    // 简化实现
}

void VSCodeExtension::processWorkspace(const std::string& workspace_path) {
    if (!is_active_) {
        return;
    }
    
    // 处理工作区
    // 简化实现
}

std::string VSCodeExtension::getExtensionStatus() const {
    std::ostringstream oss;
    oss << "VSCode Extension Status:\n";
    oss << "Active: " << (is_active_ ? "Yes" : "No") << "\n";
    oss << "Enabled Features: " << enabled_features_.size() << "\n";
    oss << "Workspace Root: " << config_.workspace_root << "\n";
    oss << "Compiler Path: " << config_.chtl_compiler_path << "\n";
    
    return oss.str();
}

void VSCodeExtension::initializeComponents() {
    module_unpacker_ = std::make_unique<AutoModuleUnpacker>();
    query_table_generator_ = std::make_unique<JSONQueryTableGenerator>();
    syntax_hinter_ = std::make_unique<ModuleSyntaxHinter>();
    code_highlighter_ = std::make_unique<CodeHighlighter>();
    code_formatter_ = std::make_unique<CodeFormatter>(config_.formatting_config);
    page_previewer_ = std::make_unique<PagePreviewer>();
    right_click_handler_ = std::make_unique<RightClickActionHandler>();
    bracket_completion_handler_ = std::make_unique<BracketCompletionHandler>();
}

void VSCodeExtension::setupFeatureDependencies() {
    // 设置功能依赖关系
    // 简化实现
}

void VSCodeExtension::validateConfig() const {
    if (config_.workspace_root.empty()) {
        throw std::invalid_argument("Workspace root cannot be empty");
    }
    
    if (config_.chtl_compiler_path.empty()) {
        throw std::invalid_argument("CHTL compiler path cannot be empty");
    }
}

} // namespace CHTL