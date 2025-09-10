#ifndef CHTL_CHTHOLLY_MODULE_H
#define CHTL_CHTHOLLY_MODULE_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "../CHTL/CMODSystem/CMODModule.h"
#include "../CHTL_JS/CJMODSystem/CJMODModule.h"

namespace CHTL {
namespace Official {

// Chtholly核心模块
class ChthollyCoreModule : public CMODModule {
public:
    ChthollyCoreModule();
    ~ChthollyCoreModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 核心功能
    std::string generateHTML(const std::string& content) const;
    std::string generateCSS(const std::string& content) const;
    std::string generateJS(const std::string& content) const;
    std::string processElement(const std::string& element) const;
    std::string processAttribute(const std::string& attribute) const;
    std::string processText(const std::string& text) const;
    
private:
    bool initialized;
};

// Chtholly模块 - 官方核心模块
class ChthollyModule : public CMODModule {
public:
    ChthollyModule();
    ~ChthollyModule() = default;
    
    // 模块信息
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    // 模块功能
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // Chtholly特有功能
    std::string generateHTML(const std::string& content) const;
    std::string generateCSS(const std::string& content) const;
    std::string generateJS(const std::string& content) const;
    
    // 子模块管理
    void addSubModule(std::shared_ptr<CMODModule> subModule);
    std::vector<std::shared_ptr<CMODModule>> getSubModules() const;
    std::shared_ptr<CMODModule> getSubModule(const std::string& name) const;
    
    // 配置管理
    void setConfiguration(const std::string& key, const std::string& value);
    std::string getConfiguration(const std::string& key) const;
    std::map<std::string, std::string> getAllConfigurations() const;
    
private:
    bool initialized;
    std::map<std::string, std::string> configurations;
    std::vector<std::shared_ptr<CMODModule>> subModules;
    
    void initializeDefaultConfigurations();
    void initializeSubModules();
};

// Chtholly CMOD子模块 - 手风琴
class ChthollyAccordionModule : public CMODModule {
public:
    ChthollyAccordionModule();
    ~ChthollyAccordionModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 手风琴功能
    std::string generateAccordion(const std::string& title, const std::string& content) const;
    std::string generateAccordionGroup(const std::vector<std::pair<std::string, std::string>>& items) const;
    
private:
    bool initialized;
};

// Chtholly CMOD子模块 - 四叶窗相册
class ChthollyGalleryModule : public CMODModule {
public:
    ChthollyGalleryModule();
    ~ChthollyGalleryModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 四叶窗相册功能
    std::string generateGallery(const std::vector<std::string>& images) const;
    std::string generateGalleryItem(const std::string& imageUrl, const std::string& caption) const;
    
private:
    bool initialized;
};

// Chtholly CMOD子模块 - 备忘录
class ChthollyMemoModule : public CMODModule {
public:
    ChthollyMemoModule();
    ~ChthollyMemoModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 备忘录功能
    std::string generateMemo(const std::string& title, const std::string& content) const;
    std::string generateMemoList(const std::vector<std::pair<std::string, std::string>>& memos) const;
    
private:
    bool initialized;
};

// Chtholly CMOD子模块 - 暖色笔记
class ChthollyNoteModule : public CMODModule {
public:
    ChthollyNoteModule();
    ~ChthollyNoteModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 暖色笔记功能
    std::string generateNote(const std::string& content, const std::string& color = "warm") const;
    std::string generateNoteBoard(const std::vector<std::string>& notes) const;
    
private:
    bool initialized;
};

// Chtholly CMOD子模块 - 樱花雨
class ChthollySakuraModule : public CMODModule {
public:
    ChthollySakuraModule();
    ~ChthollySakuraModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 樱花雨功能
    std::string generateSakuraRain(int count = 50) const;
    std::string generateSakuraEffect(const std::string& intensity = "medium") const;
    
private:
    bool initialized;
};

// Chtholly CMOD子模块 - 鼠标特效
class ChthollyMouseModule : public CMODModule {
public:
    ChthollyMouseModule();
    ~ChthollyMouseModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 鼠标特效功能
    std::string generateMouseTrail(const std::string& color = "pink") const;
    std::string generateMouseEffect(const std::string& type = "sparkle") const;
    
private:
    bool initialized;
};

// Chtholly CMOD子模块 - 进度条
class ChthollyProgressModule : public CMODModule {
public:
    ChthollyProgressModule();
    ~ChthollyProgressModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 进度条功能
    std::string generateProgressBar(int value, int max = 100) const;
    std::string generateCircularProgress(int value, int max = 100) const;
    
private:
    bool initialized;
};

// Chtholly CJMOD子模块 - printMylove
class ChthollyPrintMyloveModule : public CHTL_JS::CJMODModule {
public:
    ChthollyPrintMyloveModule();
    ~ChthollyPrintMyloveModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // printMylove功能
    std::string printMylove(const std::string& url, const std::string& mode = "ASCII", 
                           const std::string& width = "100", const std::string& height = "100", 
                           const std::string& scale = "1.0") const;
    
private:
    bool initialized;
};

// Chtholly CJMOD子模块 - iNeverAway
class ChthollyINeverAwayModule : public CHTL_JS::CJMODModule {
public:
    ChthollyINeverAwayModule();
    ~ChthollyINeverAwayModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // iNeverAway功能
    std::string createVirtualObject(const std::string& name, const std::map<std::string, std::string>& functions) const;
    std::string generateVirtualFunction(const std::string& objectName, const std::string& functionName, 
                                       const std::string& parameters, const std::string& body) const;
    
private:
    bool initialized;
};

// Chtholly CJMOD子模块 - util...then表达式
class ChthollyUtilThenModule : public CHTL_JS::CJMODModule {
public:
    ChthollyUtilThenModule();
    ~ChthollyUtilThenModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // util...then功能
    std::string generateUtilThen(const std::string& condition, const std::string& changeCode, 
                                const std::string& thenCode) const;
    std::string generateUtilThenBlock(const std::string& condition, const std::vector<std::string>& changeCode, 
                                     const std::vector<std::string>& thenCode) const;
    
private:
    bool initialized;
};

// Chtholly样式子模块
class ChthollyStyleModule : public CMODModule {
public:
    ChthollyStyleModule();
    ~ChthollyStyleModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 样式功能
    std::string processStyle(const std::string& style) const;
    std::string generateClass(const std::string& className) const;
    std::string generateId(const std::string& idName) const;
    
private:
    bool initialized;
};

// Chtholly脚本子模块
class ChthollyScriptModule : public CMODModule {
public:
    ChthollyScriptModule();
    ~ChthollyScriptModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 脚本功能
    std::string processScript(const std::string& script) const;
    std::string generateEvent(const std::string& eventName) const;
    std::string generateFunction(const std::string& functionName) const;
    
private:
    bool initialized;
};

// Chtholly模板子模块
class ChthollyTemplateModule : public CMODModule {
public:
    ChthollyTemplateModule();
    ~ChthollyTemplateModule() = default;
    
    std::string getModuleName() const;
    std::string getVersion() const;
    std::string getDescription() const;
    std::string getAuthor() const;
    
    bool initialize();
    bool isInitialized() const;
    void cleanup();
    
    // 模板功能
    std::string processTemplate(const std::string& templateStr) const;
    std::string generateTemplate(const std::string& templateName) const;
    std::string processVariable(const std::string& variable) const;
    
private:
    bool initialized;
};

} // namespace Official
} // namespace CHTL

#endif // CHTL_CHTHOLLY_MODULE_H