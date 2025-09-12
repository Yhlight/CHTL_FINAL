#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <any>

namespace CHTL {

/**
 * 官方模块类型
 */
enum class OfficialModuleType {
    CHTHOLLY_CMOD,      // Chtholly珂朵莉模块CMOD部分
    CHTHOLLY_CJMOD,     // Chtholly珂朵莉模块CJMOD部分
    YUIGAHAMA_CMOD,     // Yuigahama由比滨结衣模块CMOD部分
    YUIGAHAMA_CJMOD     // Yuigahama由比滨结衣模块CJMOD部分
};

/**
 * Chtholly珂朵莉模块CMOD组件
 */
enum class ChthollyCMODComponent {
    ACCORDION,              // 手风琴
    FOUR_LEAF_GALLERY,      // 四叶窗相册
    MEMO,                   // 备忘录
    WARM_NOTE,              // 暖色笔记
    CHERRY_BLOSSOM_RAIN,    // 樱花雨
    MOUSE_EFFECT,           // 鼠标特效
    MOUSE_TRAIL,            // 鼠标拖尾
    PARALLAX_BACKGROUND,    // 视差滚动背景
    RIGHT_CLICK_MENU,       // 右键菜单栏
    PROGRESS_BAR            // 进度条
};

/**
 * Chtholly珂朵莉模块CJMOD组件
 */
enum class ChthollyCJMODComponent {
    PRINT_MY_LOVE,          // printMylove
    I_NEVER_AWAY,           // iNeverAway
    UTIL_THEN_EXPRESSION    // util...then表达式
};

/**
 * Yuigahama由比滨结衣模块组件
 */
enum class YuigahamaComponent {
    MUSIC_PLAYER,           // 音乐播放器
    ACCORDION,              // 手风琴
    FOUR_LEAF_GALLERY,      // 四叶窗相册
    MEMO,                   // 备忘录
    WARM_NOTE,              // 暖色笔记
    CHERRY_BLOSSOM_RAIN,    // 樱花雨
    MOUSE_EFFECT            // 鼠标特效
};

/**
 * 官方模块组件基类
 */
class OfficialModuleComponent {
public:
    OfficialModuleComponent(const std::string& name, const std::string& description);
    virtual ~OfficialModuleComponent() = default;
    
    // 基本信息
    const std::string& getName() const;
    const std::string& getDescription() const;
    
    // 组件操作
    virtual std::string generateCode() const = 0;
    virtual std::string generateHTML() const = 0;
    virtual std::string generateCSS() const = 0;
    virtual std::string generateJS() const = 0;
    
    // 配置
    virtual void setConfiguration(const std::map<std::string, std::any>& config);
    virtual std::map<std::string, std::any> getConfiguration() const;
    
    // 验证
    virtual bool validateConfiguration() const;
    virtual std::vector<std::string> getValidationErrors() const;
    
protected:
    std::string name_;
    std::string description_;
    std::map<std::string, std::any> configuration_;
    
    // 辅助函数
    virtual std::string processTemplate(const std::string& template_str) const;
    virtual std::string replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const;
};

/**
 * Chtholly珂朵莉模块CMOD组件实现
 */
class ChthollyCMODComponentImpl : public OfficialModuleComponent {
public:
    ChthollyCMODComponentImpl(ChthollyCMODComponent component);
    ~ChthollyCMODComponentImpl() override = default;
    
    // 组件操作
    std::string generateCode() const override;
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
    // 配置
    void setConfiguration(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfiguration() const override;
    
    // 验证
    bool validateConfiguration() const override;
    std::vector<std::string> getValidationErrors() const override;
    
private:
    ChthollyCMODComponent component_type_;
    
    // 组件特定实现
    std::string generateAccordion() const;
    std::string generateFourLeafGallery() const;
    std::string generateMemo() const;
    std::string generateWarmNote() const;
    std::string generateCherryBlossomRain() const;
    std::string generateMouseEffect() const;
    std::string generateMouseTrail() const;
    std::string generateParallaxBackground() const;
    std::string generateRightClickMenu() const;
    std::string generateProgressBar() const;
    
    // 辅助函数
    std::string generateAccordionHTML() const;
    std::string generateAccordionCSS() const;
    std::string generateAccordionJS() const;
    
    std::string generateFourLeafGalleryHTML() const;
    std::string generateFourLeafGalleryCSS() const;
    std::string generateFourLeafGalleryJS() const;
    
    std::string generateMemoHTML() const;
    std::string generateMemoCSS() const;
    std::string generateMemoJS() const;
    
    std::string generateWarmNoteHTML() const;
    std::string generateWarmNoteCSS() const;
    std::string generateWarmNoteJS() const;
    
    std::string generateCherryBlossomRainHTML() const;
    std::string generateCherryBlossomRainCSS() const;
    std::string generateCherryBlossomRainJS() const;
    
    std::string generateMouseEffectHTML() const;
    std::string generateMouseEffectCSS() const;
    std::string generateMouseEffectJS() const;
    
    std::string generateMouseTrailHTML() const;
    std::string generateMouseTrailCSS() const;
    std::string generateMouseTrailJS() const;
    
    std::string generateParallaxBackgroundHTML() const;
    std::string generateParallaxBackgroundCSS() const;
    std::string generateParallaxBackgroundJS() const;
    
    std::string generateRightClickMenuHTML() const;
    std::string generateRightClickMenuCSS() const;
    std::string generateRightClickMenuJS() const;
    
    std::string generateProgressBarHTML() const;
    std::string generateProgressBarCSS() const;
    std::string generateProgressBarJS() const;
};

/**
 * Chtholly珂朵莉模块CJMOD组件实现
 */
class ChthollyCJMODComponentImpl : public OfficialModuleComponent {
public:
    ChthollyCJMODComponentImpl(ChthollyCJMODComponent component);
    ~ChthollyCJMODComponentImpl() override = default;
    
    // 组件操作
    std::string generateCode() const override;
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
    // 配置
    void setConfiguration(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfiguration() const override;
    
    // 验证
    bool validateConfiguration() const override;
    std::vector<std::string> getValidationErrors() const override;
    
private:
    ChthollyCJMODComponent component_type_;
    
    // 组件特定实现
    std::string generatePrintMyLove() const;
    std::string generateINeverAway() const;
    std::string generateUtilThenExpression() const;
    
    // 辅助函数
    std::string generatePrintMyLoveJS() const;
    std::string generateINeverAwayJS() const;
    std::string generateUtilThenExpressionJS() const;
    
    // printMylove特定函数
    std::string processImageToASCII(const std::string& url, const std::string& mode, 
                                   const std::string& width, const std::string& height, 
                                   const std::string& scale) const;
    
    // iNeverAway特定函数
    std::string generateVirtualObject(const std::string& name, 
                                     const std::map<std::string, std::string>& functions) const;
    
    // util...then表达式特定函数
    std::string generateUtilExpression(const std::string& condition, 
                                      const std::string& change, 
                                      const std::string& then) const;
};

/**
 * Yuigahama由比滨结衣模块组件实现
 */
class YuigahamaComponentImpl : public OfficialModuleComponent {
public:
    YuigahamaComponentImpl(YuigahamaComponent component);
    ~YuigahamaComponentImpl() override = default;
    
    // 组件操作
    std::string generateCode() const override;
    std::string generateHTML() const override;
    std::string generateCSS() const override;
    std::string generateJS() const override;
    
    // 配置
    void setConfiguration(const std::map<std::string, std::any>& config) override;
    std::map<std::string, std::any> getConfiguration() const override;
    
    // 验证
    bool validateConfiguration() const override;
    std::vector<std::string> getValidationErrors() const override;
    
private:
    YuigahamaComponent component_type_;
    
    // 组件特定实现
    std::string generateMusicPlayer() const;
    std::string generateAccordion() const;
    std::string generateFourLeafGallery() const;
    std::string generateMemo() const;
    std::string generateWarmNote() const;
    std::string generateCherryBlossomRain() const;
    std::string generateMouseEffect() const;
    
    // 辅助函数
    std::string generateMusicPlayerHTML() const;
    std::string generateMusicPlayerCSS() const;
    std::string generateMusicPlayerJS() const;
    
    // 其他组件的辅助函数...
};

/**
 * 官方模块管理器
 */
class OfficialModuleManager {
public:
    OfficialModuleManager();
    ~OfficialModuleManager();
    
    // 模块管理
    void registerComponent(OfficialModuleType type, const std::string& name, 
                          std::shared_ptr<OfficialModuleComponent> component);
    void unregisterComponent(OfficialModuleType type, const std::string& name);
    std::shared_ptr<OfficialModuleComponent> getComponent(OfficialModuleType type, const std::string& name) const;
    
    // 组件查询
    std::vector<std::string> getComponentNames(OfficialModuleType type) const;
    std::vector<std::shared_ptr<OfficialModuleComponent>> getAllComponents(OfficialModuleType type) const;
    bool hasComponent(OfficialModuleType type, const std::string& name) const;
    
    // 代码生成
    std::string generateModuleCode(OfficialModuleType type, const std::string& name) const;
    std::string generateModuleHTML(OfficialModuleType type, const std::string& name) const;
    std::string generateModuleCSS(OfficialModuleType type, const std::string& name) const;
    std::string generateModuleJS(OfficialModuleType type, const std::string& name) const;
    
    // 模块信息
    std::string getModuleDescription(OfficialModuleType type) const;
    std::vector<std::string> getModuleFeatures(OfficialModuleType type) const;
    
    // 初始化
    void initializeDefaultModules();
    
    // 清理
    void clear();
    void clearModule(OfficialModuleType type);
    
private:
    std::map<OfficialModuleType, std::map<std::string, std::shared_ptr<OfficialModuleComponent>>> modules_;
    
    // 辅助函数
    void initializeChthollyCMOD();
    void initializeChthollyCJMOD();
    void initializeYuigahamaCMOD();
    void initializeYuigahamaCJMOD();
    
    std::string getModuleTypeName(OfficialModuleType type) const;
    std::string getComponentTypeName(OfficialModuleType type, const std::string& component) const;
};

/**
 * 官方模块导入器
 */
class OfficialModuleImporter {
public:
    OfficialModuleImporter();
    ~OfficialModuleImporter();
    
    // 导入操作
    std::string importModule(const std::string& modulePath) const;
    std::string importChthollyModule(const std::string& component) const;
    std::string importYuigahamaModule(const std::string& component) const;
    
    // 通配符导入
    std::vector<std::string> importWildcard(const std::string& pattern) const;
    std::vector<std::string> importAllCMOD(const std::string& path) const;
    std::vector<std::string> importAllCJMOD(const std::string& path) const;
    std::vector<std::string> importAllCHTL(const std::string& path) const;
    
    // 子模块导入
    std::vector<std::string> importSubmodules(const std::string& moduleName) const;
    std::vector<std::string> importSpecificSubmodule(const std::string& moduleName, const std::string& submodule) const;
    
    // 路径解析
    std::string resolveModulePath(const std::string& modulePath) const;
    std::vector<std::string> searchModulePaths(const std::string& pattern) const;
    
    // 配置
    void setModuleManager(std::shared_ptr<OfficialModuleManager> manager);
    std::shared_ptr<OfficialModuleManager> getModuleManager() const;
    
private:
    std::shared_ptr<OfficialModuleManager> module_manager_;
    
    // 辅助函数
    std::string processModulePath(const std::string& path) const;
    std::vector<std::string> expandWildcard(const std::string& pattern) const;
    bool isOfficialModule(const std::string& moduleName) const;
    std::string getOfficialModulePrefix() const;
};

/**
 * 官方模块分类器
 */
class OfficialModuleClassifier {
public:
    OfficialModuleClassifier();
    ~OfficialModuleClassifier();
    
    // 分类操作
    std::string classifyModule(const std::string& modulePath) const;
    std::string getModuleType(const std::string& modulePath) const;
    std::string getModuleCategory(const std::string& modulePath) const;
    
    // 分类结构
    std::string createClassificationStructure(const std::string& basePath) const;
    std::string createCMODStructure(const std::string& basePath) const;
    std::string createCJMODStructure(const std::string& basePath) const;
    std::string createCHTLStructure(const std::string& basePath) const;
    
    // 分类验证
    bool validateClassification(const std::string& path) const;
    std::vector<std::string> getClassificationErrors(const std::string& path) const;
    
    // 配置
    void setClassificationRules(const std::map<std::string, std::string>& rules);
    std::map<std::string, std::string> getClassificationRules() const;
    
private:
    std::map<std::string, std::string> classification_rules_;
    
    // 辅助函数
    std::string determineModuleType(const std::string& filePath) const;
    std::string getFileExtension(const std::string& filePath) const;
    bool isCMODFile(const std::string& filePath) const;
    bool isCJMODFile(const std::string& filePath) const;
    bool isCHTLFile(const std::string& filePath) const;
};

} // namespace CHTL