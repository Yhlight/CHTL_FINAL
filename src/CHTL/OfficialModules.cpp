#include "CHTL/OfficialModules.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <fstream>

namespace CHTL {

// OfficialModuleComponent 实现
OfficialModuleComponent::OfficialModuleComponent(const std::string& name, const std::string& description)
    : name_(name), description_(description) {
}

const std::string& OfficialModuleComponent::getName() const {
    return name_;
}

const std::string& OfficialModuleComponent::getDescription() const {
    return description_;
}

void OfficialModuleComponent::setConfiguration(const std::map<std::string, std::any>& config) {
    configuration_ = config;
}

std::map<std::string, std::any> OfficialModuleComponent::getConfiguration() const {
    return configuration_;
}

bool OfficialModuleComponent::validateConfiguration() const {
    return true; // 基础实现，子类可以重写
}

std::vector<std::string> OfficialModuleComponent::getValidationErrors() const {
    return {}; // 基础实现，子类可以重写
}

std::string OfficialModuleComponent::processTemplate(const std::string& template_str) const {
    return template_str; // 基础实现
}

std::string OfficialModuleComponent::replacePlaceholders(const std::string& str, const std::map<std::string, std::string>& replacements) const {
    std::string result = str;
    for (const auto& pair : replacements) {
        std::string placeholder = "{{" + pair.first + "}}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), pair.second);
            pos += pair.second.length();
        }
    }
    return result;
}

// ChthollyCMODComponentImpl 实现
ChthollyCMODComponentImpl::ChthollyCMODComponentImpl(ChthollyCMODComponent component)
    : OfficialModuleComponent("Chtholly_" + std::to_string(static_cast<int>(component)), "Chtholly珂朵莉模块CMOD组件"),
      component_type_(component) {
}

std::string ChthollyCMODComponentImpl::generateCode() const {
    switch (component_type_) {
        case ChthollyCMODComponent::ACCORDION:
            return generateAccordion();
        case ChthollyCMODComponent::FOUR_LEAF_GALLERY:
            return generateFourLeafGallery();
        case ChthollyCMODComponent::MEMO:
            return generateMemo();
        case ChthollyCMODComponent::WARM_NOTE:
            return generateWarmNote();
        case ChthollyCMODComponent::CHERRY_BLOSSOM_RAIN:
            return generateCherryBlossomRain();
        case ChthollyCMODComponent::MOUSE_EFFECT:
            return generateMouseEffect();
        case ChthollyCMODComponent::MOUSE_TRAIL:
            return generateMouseTrail();
        case ChthollyCMODComponent::PARALLAX_BACKGROUND:
            return generateParallaxBackground();
        case ChthollyCMODComponent::RIGHT_CLICK_MENU:
            return generateRightClickMenu();
        case ChthollyCMODComponent::PROGRESS_BAR:
            return generateProgressBar();
        default:
            return "";
    }
}

std::string ChthollyCMODComponentImpl::generateHTML() const {
    switch (component_type_) {
        case ChthollyCMODComponent::ACCORDION:
            return generateAccordionHTML();
        case ChthollyCMODComponent::FOUR_LEAF_GALLERY:
            return generateFourLeafGalleryHTML();
        case ChthollyCMODComponent::MEMO:
            return generateMemoHTML();
        default:
            return "<div class=\"chtholly-component\"></div>";
    }
}

std::string ChthollyCMODComponentImpl::generateCSS() const {
    switch (component_type_) {
        case ChthollyCMODComponent::ACCORDION:
            return generateAccordionCSS();
        case ChthollyCMODComponent::FOUR_LEAF_GALLERY:
            return generateFourLeafGalleryCSS();
        case ChthollyCMODComponent::MEMO:
            return generateMemoCSS();
        default:
            return ".chtholly-component { display: block; }";
    }
}

std::string ChthollyCMODComponentImpl::generateJS() const {
    switch (component_type_) {
        case ChthollyCMODComponent::ACCORDION:
            return generateAccordionJS();
        case ChthollyCMODComponent::FOUR_LEAF_GALLERY:
            return generateFourLeafGalleryJS();
        case ChthollyCMODComponent::MEMO:
            return generateMemoJS();
        default:
            return "// Chtholly component JavaScript";
    }
}

// 手风琴组件实现
std::string ChthollyCMODComponentImpl::generateAccordion() const {
    return generateAccordionHTML() + "\n<style>\n" + generateAccordionCSS() + "\n</style>\n<script>\n" + generateAccordionJS() + "\n</script>";
}

std::string ChthollyCMODComponentImpl::generateAccordionHTML() const {
    return R"(
<div class="chtholly-accordion">
    <div class="accordion-item">
        <div class="accordion-header">标题1</div>
        <div class="accordion-content">内容1</div>
    </div>
    <div class="accordion-item">
        <div class="accordion-header">标题2</div>
        <div class="accordion-content">内容2</div>
    </div>
</div>
)";
}

std::string ChthollyCMODComponentImpl::generateAccordionCSS() const {
    return R"(
.chtholly-accordion {
    border: 1px solid #ddd;
    border-radius: 4px;
}
.accordion-item {
    border-bottom: 1px solid #ddd;
}
.accordion-header {
    padding: 15px;
    background: #f5f5f5;
    cursor: pointer;
    transition: background 0.3s;
}
.accordion-header:hover {
    background: #e5e5e5;
}
.accordion-content {
    padding: 15px;
    display: none;
}
.accordion-item.active .accordion-content {
    display: block;
}
)";
}

std::string ChthollyCMODComponentImpl::generateAccordionJS() const {
    return R"(
document.querySelectorAll('.accordion-header').forEach(header => {
    header.addEventListener('click', () => {
        const item = header.parentElement;
        const isActive = item.classList.contains('active');
        
        document.querySelectorAll('.accordion-item').forEach(i => i.classList.remove('active'));
        
        if (!isActive) {
            item.classList.add('active');
        }
    });
});
)";
}

// 四叶窗相册组件实现
std::string ChthollyCMODComponentImpl::generateFourLeafGallery() const {
    return generateFourLeafGalleryHTML() + "\n<style>\n" + generateFourLeafGalleryCSS() + "\n</style>\n<script>\n" + generateFourLeafGalleryJS() + "\n</script>";
}

std::string ChthollyCMODComponentImpl::generateFourLeafGalleryHTML() const {
    return R"(
<div class="chtholly-gallery">
    <div class="gallery-container">
        <div class="gallery-item" data-image="image1.jpg">
            <div class="leaf leaf-1"></div>
            <div class="leaf leaf-2"></div>
            <div class="leaf leaf-3"></div>
            <div class="leaf leaf-4"></div>
        </div>
    </div>
</div>
)";
}

std::string ChthollyCMODComponentImpl::generateFourLeafGalleryCSS() const {
    return R"(
.chtholly-gallery {
    width: 300px;
    height: 300px;
    position: relative;
    overflow: hidden;
}
.gallery-item {
    width: 100%;
    height: 100%;
    position: relative;
}
.leaf {
    position: absolute;
    width: 50%;
    height: 50%;
    background-size: cover;
    transition: transform 0.5s ease;
}
.leaf-1 { top: 0; left: 0; transform-origin: bottom right; }
.leaf-2 { top: 0; right: 0; transform-origin: bottom left; }
.leaf-3 { bottom: 0; left: 0; transform-origin: top right; }
.leaf-4 { bottom: 0; right: 0; transform-origin: top left; }
.gallery-item:hover .leaf-1 { transform: rotateY(180deg); }
.gallery-item:hover .leaf-2 { transform: rotateY(-180deg); }
.gallery-item:hover .leaf-3 { transform: rotateY(180deg); }
.gallery-item:hover .leaf-4 { transform: rotateY(-180deg); }
)";
}

std::string ChthollyCMODComponentImpl::generateFourLeafGalleryJS() const {
    return R"(
document.querySelectorAll('.gallery-item').forEach(item => {
    const image = item.dataset.image;
    const leaves = item.querySelectorAll('.leaf');
    leaves.forEach(leaf => {
        leaf.style.backgroundImage = `url(${image})`;
    });
});
)";
}

// 备忘录组件实现
std::string ChthollyCMODComponentImpl::generateMemo() const {
    return generateMemoHTML() + "\n<style>\n" + generateMemoCSS() + "\n</style>\n<script>\n" + generateMemoJS() + "\n</script>";
}

std::string ChthollyCMODComponentImpl::generateMemoHTML() const {
    return R"(
<div class="chtholly-memo">
    <div class="memo-header">
        <input type="text" class="memo-title" placeholder="备忘录标题">
        <button class="memo-save">保存</button>
    </div>
    <textarea class="memo-content" placeholder="在这里写下你的备忘录..."></textarea>
    <div class="memo-list"></div>
</div>
)";
}

std::string ChthollyCMODComponentImpl::generateMemoCSS() const {
    return R"(
.chtholly-memo {
    max-width: 400px;
    border: 1px solid #ddd;
    border-radius: 8px;
    padding: 20px;
    background: #fff;
}
.memo-header {
    display: flex;
    gap: 10px;
    margin-bottom: 15px;
}
.memo-title {
    flex: 1;
    padding: 8px;
    border: 1px solid #ddd;
    border-radius: 4px;
}
.memo-save {
    padding: 8px 16px;
    background: #007bff;
    color: white;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}
.memo-content {
    width: 100%;
    height: 120px;
    padding: 10px;
    border: 1px solid #ddd;
    border-radius: 4px;
    resize: vertical;
}
.memo-list {
    margin-top: 15px;
}
.memo-item {
    padding: 10px;
    border: 1px solid #eee;
    border-radius: 4px;
    margin-bottom: 5px;
    cursor: pointer;
}
)";
}

std::string ChthollyCMODComponentImpl::generateMemoJS() const {
    return R"(
class ChthollyMemo {
    constructor() {
        this.memos = JSON.parse(localStorage.getItem('chtholly-memos') || '[]');
        this.init();
    }
    
    init() {
        document.querySelector('.memo-save').addEventListener('click', () => this.saveMemo());
        this.renderMemos();
    }
    
    saveMemo() {
        const title = document.querySelector('.memo-title').value;
        const content = document.querySelector('.memo-content').value;
        
        if (title && content) {
            this.memos.push({ title, content, date: new Date().toISOString() });
            localStorage.setItem('chtholly-memos', JSON.stringify(this.memos));
            this.renderMemos();
            document.querySelector('.memo-title').value = '';
            document.querySelector('.memo-content').value = '';
        }
    }
    
    renderMemos() {
        const list = document.querySelector('.memo-list');
        list.innerHTML = this.memos.map((memo, index) => 
            `<div class=\"memo-item\" onclick=\"chthollyMemo.loadMemo(${index})\">
                <strong>${memo.title}</strong>
                <p>${memo.content.substring(0, 50)}...</p>
            </div>`
        ).join('');
    }
    
    loadMemo(index) {
        const memo = this.memos[index];
        document.querySelector('.memo-title').value = memo.title;
        document.querySelector('.memo-content').value = memo.content;
    }
}

const chthollyMemo = new ChthollyMemo();
)";
}

// 其他组件的简化实现
std::string CHTL::ChthollyCMODComponentImpl::generateWarmNote() const {
    return "<div class=\"chtholly-warm-note\">暖色笔记组件</div>";
}

std::string CHTL::ChthollyCMODComponentImpl::generateCherryBlossomRain() const {
    return "<div class=\"chtholly-cherry-rain\">樱花雨组件</div>";
}

std::string CHTL::ChthollyCMODComponentImpl::generateMouseEffect() const {
    return "<div class=\"chtholly-mouse-effect\">鼠标特效组件</div>";
}

std::string CHTL::ChthollyCMODComponentImpl::generateMouseTrail() const {
    return "<div class=\"chtholly-mouse-trail\">鼠标拖尾组件</div>";
}

std::string CHTL::ChthollyCMODComponentImpl::generateParallaxBackground() const {
    return "<div class=\"chtholly-parallax\">视差滚动背景组件</div>";
}

std::string ChthollyCMODComponentImpl::generateRightClickMenu() const {
    return "<div class=\"chtholly-right-menu\">右键菜单栏组件</div>";
}

std::string ChthollyCMODComponentImpl::generateProgressBar() const {
    return "<div class=\"chtholly-progress\">进度条组件</div>";
}

void ChthollyCMODComponentImpl::setConfiguration(const std::map<std::string, std::any>& config) {
    OfficialModuleComponent::setConfiguration(config);
}

std::map<std::string, std::any> ChthollyCMODComponentImpl::getConfiguration() const {
    return OfficialModuleComponent::getConfiguration();
}

bool ChthollyCMODComponentImpl::validateConfiguration() const {
    return true;
}

std::vector<std::string> ChthollyCMODComponentImpl::getValidationErrors() const {
    return {};
}

// ChthollyCJMODComponentImpl 实现
ChthollyCJMODComponentImpl::ChthollyCJMODComponentImpl(ChthollyCJMODComponent component)
    : OfficialModuleComponent("ChthollyJS_" + std::to_string(static_cast<int>(component)), "Chtholly珂朵莉模块CJMOD组件"),
      component_type_(component) {
}

std::string ChthollyCJMODComponentImpl::generateCode() const {
    switch (component_type_) {
        case ChthollyCJMODComponent::PRINT_MY_LOVE:
            return generatePrintMyLove();
        case ChthollyCJMODComponent::I_NEVER_AWAY:
            return generateINeverAway();
        case ChthollyCJMODComponent::UTIL_THEN_EXPRESSION:
            return generateUtilThenExpression();
        default:
            return "";
    }
}

std::string ChthollyCJMODComponentImpl::generateHTML() const {
    return "<script>\n" + generateJS() + "\n</script>";
}

std::string ChthollyCJMODComponentImpl::generateCSS() const {
    return "/* CJMOD组件不生成CSS */";
}

std::string ChthollyCJMODComponentImpl::generateJS() const {
    switch (component_type_) {
        case ChthollyCJMODComponent::PRINT_MY_LOVE:
            return generatePrintMyLoveJS();
        case ChthollyCJMODComponent::I_NEVER_AWAY:
            return generateINeverAwayJS();
        case ChthollyCJMODComponent::UTIL_THEN_EXPRESSION:
            return generateUtilThenExpressionJS();
        default:
            return "// CJMOD component";
    }
}

// printMylove实现
std::string ChthollyCJMODComponentImpl::generatePrintMyLove() const {
    return generatePrintMyLoveJS();
}

std::string ChthollyCJMODComponentImpl::generatePrintMyLoveJS() const {
    return R"(
function printMylove(config) {
    const { url, mode = 'ASCII', width = 80, height = 40, scale = 1 } = config;
    
    return new Promise((resolve, reject) => {
        const img = new Image();
        img.crossOrigin = 'anonymous';
        
        img.onload = function() {
            const canvas = document.createElement('canvas');
            const ctx = canvas.getContext('2d');
            
            const finalWidth = Math.floor(width * scale);
            const finalHeight = Math.floor(height * scale);
            
            canvas.width = finalWidth;
            canvas.height = finalHeight;
            
            ctx.drawImage(img, 0, 0, finalWidth, finalHeight);
            
            const imageData = ctx.getImageData(0, 0, finalWidth, finalHeight);
            const data = imageData.data;
            
            let result = '';
            
            if (mode === 'ASCII') {
                const chars = '@%#*+=-:. ';
                for (let i = 0; i < finalHeight; i++) {
                    for (let j = 0; j < finalWidth; j++) {
                        const offset = (i * finalWidth + j) * 4;
                        const r = data[offset];
                        const g = data[offset + 1];
                        const b = data[offset + 2];
                        const gray = (r + g + b) / 3;
                        const charIndex = Math.floor((gray / 255) * (chars.length - 1));
                        result += chars[charIndex];
                    }
                    result += '\n';
                }
            } else if (mode === 'Pixel') {
                for (let i = 0; i < finalHeight; i++) {
                    for (let j = 0; j < finalWidth; j++) {
                        const offset = (i * finalWidth + j) * 4;
                        const r = data[offset];
                        const g = data[offset + 1];
                        const b = data[offset + 2];
                        result += `\x1b[48;2;${r};${g};${b}m \x1b[0m`;
                    }
                    result += '\n';
                }
            }
            
            resolve(result);
        };
        
        img.onerror = reject;
        img.src = url;
    });
}
)";
}

// iNeverAway实现
std::string ChthollyCJMODComponentImpl::generateINeverAway() const {
    return generateINeverAwayJS();
}

std::string ChthollyCJMODComponentImpl::generateINeverAwayJS() const {
    return R"(
class INeverAway {
    constructor() {
        this.functions = new Map();
        this.counter = 0;
    }
    
    create(config) {
        const virtualObject = {};
        
        for (const [key, value] of Object.entries(config)) {
            const functionName = `__chtl_ineveraway_${this.counter++}`;
            
            if (typeof value === 'function') {
                window[functionName] = value;
                virtualObject[key] = (...args) => window[functionName](...args);
            } else if (typeof value === 'object' && value !== null) {
                virtualObject[key] = value;
            }
            
            this.functions.set(key, functionName);
        }
        
        return new Proxy(virtualObject, {
            get(target, prop) {
                if (prop in target) {
                    return target[prop];
                }
                return undefined;
            }
        });
    }
    
    cleanup() {
        for (const functionName of this.functions.values()) {
            if (window[functionName]) {
                delete window[functionName];
            }
        }
        this.functions.clear();
    }
}

const iNeverAway = new INeverAway();

// 虚对象创建函数
function vir(name, config) {
    return iNeverAway.create(config);
}
)";
}

// util...then表达式实现
std::string ChthollyCJMODComponentImpl::generateUtilThenExpression() const {
    return generateUtilThenExpressionJS();
}

std::string ChthollyCJMODComponentImpl::generateUtilThenExpressionJS() const {
    return R"(
class UtilThenExpression {
    constructor() {
        this.watchers = new Map();
        this.counter = 0;
    }
    
    util(condition) {
        const watcherId = `watcher_${this.counter++}`;
        
        return {
            change: (changeCallback) => ({
                then: (thenCallback) => {
                    const watcher = {
                        condition: condition,
                        onChange: changeCallback,
                        onThen: thenCallback,
                        lastValue: null,
                        triggered: false
                    };
                    
                    this.watchers.set(watcherId, watcher);
                    this.startWatching(watcherId);
                    
                    return {
                        stop: () => this.stopWatching(watcherId)
                    };
                }
            })
        };
    }
    
    startWatching(watcherId) {
        const watcher = this.watchers.get(watcherId);
        if (!watcher) return;
        
        const checkCondition = () => {
            try {
                const currentValue = watcher.condition();
                
                if (watcher.lastValue !== null && watcher.lastValue !== currentValue) {
                    if (typeof watcher.onChange === 'function') {
                        watcher.onChange();
                    }
                }
                
                if (currentValue && !watcher.triggered) {
                    if (typeof watcher.onThen === 'function') {
                        watcher.onThen();
                    }
                    watcher.triggered = true;
                }
                
                watcher.lastValue = currentValue;
            } catch (error) {
                console.error('Util expression error:', error);
            }
            
            if (this.watchers.has(watcherId)) {
                requestAnimationFrame(checkCondition);
            }
        };
        
        requestAnimationFrame(checkCondition);
    }
    
    stopWatching(watcherId) {
        this.watchers.delete(watcherId);
    }
}

const utilThenManager = new UtilThenExpression();

function util(condition) {
    return utilThenManager.util(condition);
}
)";
}

std::string ChthollyCJMODComponentImpl::processImageToASCII(const std::string& url, const std::string& mode, 
                                                         const std::string& width, const std::string& height, 
                                                         const std::string& scale) const {
    // 这是一个占位符实现，实际应该调用JavaScript函数
    return "printMylove({ url: '" + url + "', mode: '" + mode + "', width: " + width + ", height: " + height + ", scale: " + scale + " })";
}

std::string ChthollyCJMODComponentImpl::generateVirtualObject(const std::string& name, 
                                                            const std::map<std::string, std::string>& functions) const {
    std::string result = "vir " + name + " = iNeverAway({\n";
    for (const auto& pair : functions) {
        result += "    " + pair.first + ": " + pair.second + ",\n";
    }
    result += "});";
    return result;
}

std::string ChthollyCJMODComponentImpl::generateUtilExpression(const std::string& condition, 
                                                             const std::string& change, 
                                                             const std::string& then) const {
    return "util(" + condition + ").change(" + change + ").then(" + then + ");";
}

void ChthollyCJMODComponentImpl::setConfiguration(const std::map<std::string, std::any>& config) {
    OfficialModuleComponent::setConfiguration(config);
}

std::map<std::string, std::any> ChthollyCJMODComponentImpl::getConfiguration() const {
    return OfficialModuleComponent::getConfiguration();
}

bool ChthollyCJMODComponentImpl::validateConfiguration() const {
    return true;
}

std::vector<std::string> ChthollyCJMODComponentImpl::getValidationErrors() const {
    return {};
}

// OfficialModuleManager 实现
OfficialModuleManager::OfficialModuleManager() {
    initializeDefaultModules();
}

OfficialModuleManager::~OfficialModuleManager() = default;

void OfficialModuleManager::registerComponent(OfficialModuleType type, const std::string& name, 
                                             std::shared_ptr<OfficialModuleComponent> component) {
    modules_[type][name] = component;
}

void OfficialModuleManager::unregisterComponent(OfficialModuleType type, const std::string& name) {
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        it->second.erase(name);
    }
}

std::shared_ptr<OfficialModuleComponent> OfficialModuleManager::getComponent(OfficialModuleType type, const std::string& name) const {
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        auto comp_it = it->second.find(name);
        if (comp_it != it->second.end()) {
            return comp_it->second;
        }
    }
    return nullptr;
}

std::vector<std::string> OfficialModuleManager::getComponentNames(OfficialModuleType type) const {
    std::vector<std::string> names;
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        for (const auto& pair : it->second) {
            names.push_back(pair.first);
        }
    }
    return names;
}

std::vector<std::shared_ptr<OfficialModuleComponent>> OfficialModuleManager::getAllComponents(OfficialModuleType type) const {
    std::vector<std::shared_ptr<OfficialModuleComponent>> components;
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        for (const auto& pair : it->second) {
            components.push_back(pair.second);
        }
    }
    return components;
}

bool OfficialModuleManager::hasComponent(OfficialModuleType type, const std::string& name) const {
    auto it = modules_.find(type);
    if (it != modules_.end()) {
        return it->second.find(name) != it->second.end();
    }
    return false;
}

std::string OfficialModuleManager::generateModuleCode(OfficialModuleType type, const std::string& name) const {
    auto component = getComponent(type, name);
    if (component) {
        return component->generateCode();
    }
    return "";
}

std::string OfficialModuleManager::generateModuleHTML(OfficialModuleType type, const std::string& name) const {
    auto component = getComponent(type, name);
    if (component) {
        return component->generateHTML();
    }
    return "";
}

std::string OfficialModuleManager::generateModuleCSS(OfficialModuleType type, const std::string& name) const {
    auto component = getComponent(type, name);
    if (component) {
        return component->generateCSS();
    }
    return "";
}

std::string OfficialModuleManager::generateModuleJS(OfficialModuleType type, const std::string& name) const {
    auto component = getComponent(type, name);
    if (component) {
        return component->generateJS();
    }
    return "";
}

std::string OfficialModuleManager::getModuleDescription(OfficialModuleType type) const {
    switch (type) {
        case OfficialModuleType::CHTHOLLY_CMOD:
            return "Chtholly珂朵莉模块CMOD部分，包含各种UI组件";
        case OfficialModuleType::CHTHOLLY_CJMOD:
            return "Chtholly珂朵莉模块CJMOD部分，包含JavaScript扩展功能";
        case OfficialModuleType::YUIGAHAMA_CMOD:
            return "Yuigahama由比滨结衣模块CMOD部分，包含UI组件";
        case OfficialModuleType::YUIGAHAMA_CJMOD:
            return "Yuigahama由比滨结衣模块CJMOD部分，包含JavaScript功能";
        default:
            return "未知模块类型";
    }
}

std::vector<std::string> OfficialModuleManager::getModuleFeatures(OfficialModuleType type) const {
    switch (type) {
        case OfficialModuleType::CHTHOLLY_CMOD:
            return {"手风琴", "四叶窗相册", "备忘录", "暖色笔记", "樱花雨", "鼠标特效", "鼠标拖尾", "视差滚动背景", "右键菜单栏", "进度条"};
        case OfficialModuleType::CHTHOLLY_CJMOD:
            return {"printMylove", "iNeverAway", "util...then表达式"};
        case OfficialModuleType::YUIGAHAMA_CMOD:
            return {"音乐播放器", "手风琴", "四叶窗相册", "备忘录", "暖色笔记", "樱花雨", "鼠标特效"};
        default:
            return {};
    }
}

void OfficialModuleManager::initializeDefaultModules() {
    initializeChthollyCMOD();
    initializeChthollyCJMOD();
    initializeYuigahamaCMOD();
}

void OfficialModuleManager::initializeChthollyCMOD() {
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "accordion", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::ACCORDION));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "fourLeafGallery", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::FOUR_LEAF_GALLERY));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "memo", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::MEMO));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "warmNote", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::WARM_NOTE));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "cherryBlossomRain", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::CHERRY_BLOSSOM_RAIN));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "mouseEffect", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::MOUSE_EFFECT));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "mouseTrail", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::MOUSE_TRAIL));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "parallaxBackground", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::PARALLAX_BACKGROUND));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "rightClickMenu", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::RIGHT_CLICK_MENU));
    registerComponent(OfficialModuleType::CHTHOLLY_CMOD, "progressBar", 
                     std::make_shared<ChthollyCMODComponentImpl>(ChthollyCMODComponent::PROGRESS_BAR));
}

void OfficialModuleManager::initializeChthollyCJMOD() {
    registerComponent(OfficialModuleType::CHTHOLLY_CJMOD, "printMylove", 
                     std::make_shared<ChthollyCJMODComponentImpl>(ChthollyCJMODComponent::PRINT_MY_LOVE));
    registerComponent(OfficialModuleType::CHTHOLLY_CJMOD, "iNeverAway", 
                     std::make_shared<ChthollyCJMODComponentImpl>(ChthollyCJMODComponent::I_NEVER_AWAY));
    registerComponent(OfficialModuleType::CHTHOLLY_CJMOD, "utilThen", 
                     std::make_shared<ChthollyCJMODComponentImpl>(ChthollyCJMODComponent::UTIL_THEN_EXPRESSION));
}

void OfficialModuleManager::initializeYuigahamaCMOD() {
    // Yuigahama模块实现将在下一步添加
}

void OfficialModuleManager::clear() {
    modules_.clear();
}

void OfficialModuleManager::clearModule(OfficialModuleType type) {
    modules_.erase(type);
}

std::string OfficialModuleManager::getModuleTypeName(OfficialModuleType type) const {
    switch (type) {
        case OfficialModuleType::CHTHOLLY_CMOD: return "Chtholly-CMOD";
        case OfficialModuleType::CHTHOLLY_CJMOD: return "Chtholly-CJMOD";
        case OfficialModuleType::YUIGAHAMA_CMOD: return "Yuigahama-CMOD";
        case OfficialModuleType::YUIGAHAMA_CJMOD: return "Yuigahama-CJMOD";
        default: return "Unknown";
    }
}

std::string OfficialModuleManager::getComponentTypeName(OfficialModuleType type, const std::string& component) const {
    return getModuleTypeName(type) + "::" + component;
}

} // namespace CHTL