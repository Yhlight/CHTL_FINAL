#ifndef CHTL_JS_FEATURES_H
#define CHTL_JS_FEATURES_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include "CHTLJSParser/CHTLJSParser.h"

namespace CHTL {

// File loader for AMD-style module loading
class CHTLFileLoader {
public:
    CHTLFileLoader();
    ~CHTLFileLoader();
    
    // Module loading
    bool loadModule(const std::string& module_name, const std::string& path = "");
    std::string getModuleContent(const std::string& module_name) const;
    bool isModuleLoaded(const std::string& module_name) const;
    
    // Module management
    void unloadModule(const std::string& module_name);
    void clearAllModules();
    std::vector<std::string> getLoadedModules() const;
    
private:
    std::map<std::string, std::string> loaded_modules;
    std::string resolveModulePath(const std::string& module_name, const std::string& base_path = "");
    std::string readFile(const std::string& path);
};

// Enhanced selector for CSS-like selection
class CHTLEnhancedSelector {
public:
    CHTLEnhancedSelector();
    ~CHTLEnhancedSelector();
    
    // Selector parsing
    std::vector<std::string> parseSelector(const std::string& selector);
    bool isValidSelector(const std::string& selector) const;
    
    // Element selection
    std::vector<std::string> selectElements(const std::string& selector, const std::string& context = "");
    std::string selectFirstElement(const std::string& selector, const std::string& context = "");
    
    // Selector operations
    std::string combineSelectors(const std::vector<std::string>& selectors);
    std::string optimizeSelector(const std::string& selector);
    
private:
    std::vector<std::string> parseSelectorParts(const std::string& selector) const;
    bool isElementSelector(const std::string& part) const;
    bool isClassSelector(const std::string& part) const;
    bool isIdSelector(const std::string& part) const;
    bool isAttributeSelector(const std::string& part) const;
    bool isPseudoSelector(const std::string& part) const;
};

// Enhanced listener for event handling
class CHTLEnhancedListener {
public:
    CHTLEnhancedListener();
    ~CHTLEnhancedListener();
    
    // Event listener management
    bool addEventListener(const std::string& selector, const std::string& event, const std::string& handler);
    bool removeEventListener(const std::string& selector, const std::string& event);
    bool hasEventListener(const std::string& selector, const std::string& event) const;
    
    // Event handling
    void handleEvent(const std::string& event, const std::string& target);
    void triggerEvent(const std::string& event, const std::string& target, const std::map<std::string, std::string>& data = {});
    
    // Listener queries
    std::vector<std::string> getListeners(const std::string& selector) const;
    std::vector<std::string> getEvents(const std::string& selector) const;
    
private:
    std::map<std::string, std::map<std::string, std::string>> listeners;
    std::string generateHandlerId(const std::string& selector, const std::string& event);
};

// Event delegation system
class CHTLEventDelegation {
public:
    CHTLEventDelegation();
    ~CHTLEventDelegation();
    
    // Delegation setup
    bool setupDelegation(const std::string& parent_selector, const std::string& child_selector, const std::string& event, const std::string& handler);
    bool removeDelegation(const std::string& parent_selector, const std::string& child_selector, const std::string& event);
    
    // Event delegation handling
    void handleDelegatedEvent(const std::string& event, const std::string& target);
    bool isDelegatedEvent(const std::string& event, const std::string& target) const;
    
    // Delegation queries
    std::vector<std::string> getDelegatedEvents(const std::string& parent_selector) const;
    std::vector<std::string> getDelegationHandlers(const std::string& parent_selector, const std::string& child_selector) const;
    
private:
    struct DelegationRule {
        std::string parent_selector;
        std::string child_selector;
        std::string event;
        std::string handler;
    };
    
    std::vector<DelegationRule> delegation_rules;
    bool matchesSelector(const std::string& element, const std::string& selector) const;
};

// Animation system
class CHTLAnimation {
public:
    CHTLAnimation();
    ~CHTLAnimation();
    
    // Animation management
    bool createAnimation(const std::string& name, const std::map<std::string, std::string>& properties);
    bool startAnimation(const std::string& name, const std::string& target, int duration = 1000);
    bool stopAnimation(const std::string& name, const std::string& target);
    bool pauseAnimation(const std::string& name, const std::string& target);
    bool resumeAnimation(const std::string& name, const std::string& target);
    
    // Animation queries
    bool isAnimationRunning(const std::string& name, const std::string& target) const;
    std::vector<std::string> getRunningAnimations(const std::string& target) const;
    std::map<std::string, std::string> getAnimationProperties(const std::string& name) const;
    
    // Animation effects
    std::string generateCSSAnimation(const std::string& name, const std::map<std::string, std::string>& properties);
    std::string generateKeyframes(const std::string& name, const std::map<std::string, std::string>& properties);
    
private:
    struct AnimationDefinition {
        std::string name;
        std::map<std::string, std::string> properties;
        int duration;
        std::string easing;
    };
    
    struct RunningAnimation {
        std::string name;
        std::string target;
        int start_time;
        int duration;
        bool paused;
    };
    
    std::map<std::string, AnimationDefinition> animations;
    std::vector<RunningAnimation> running_animations;
    std::string generateAnimationId(const std::string& name, const std::string& target);
};

// Virtual object system
class CHTLVirtualObject {
public:
    CHTLVirtualObject();
    ~CHTLVirtualObject();
    
    // Virtual object management
    bool createVirtualObject(const std::string& name, const std::map<std::string, std::string>& properties);
    bool updateVirtualObject(const std::string& name, const std::map<std::string, std::string>& properties);
    bool deleteVirtualObject(const std::string& name);
    
    // Virtual object operations
    std::string getVirtualObjectProperty(const std::string& name, const std::string& property) const;
    bool setVirtualObjectProperty(const std::string& name, const std::string& property, const std::string& value);
    std::map<std::string, std::string> getVirtualObjectProperties(const std::string& name) const;
    
    // Virtual object queries
    bool virtualObjectExists(const std::string& name) const;
    std::vector<std::string> getVirtualObjectNames() const;
    std::vector<std::string> getVirtualObjectPropertyNames(const std::string& name) const;
    
private:
    std::map<std::string, std::map<std::string, std::string>> virtual_objects;
    std::string generateVirtualObjectId(const std::string& name);
};

// Router system
class CHTLRouter {
public:
    CHTLRouter();
    ~CHTLRouter();
    
    // Route management
    bool addRoute(const std::string& path, const std::string& handler);
    bool removeRoute(const std::string& path);
    bool updateRoute(const std::string& path, const std::string& handler);
    
    // Route handling
    std::string handleRoute(const std::string& path) const;
    bool routeExists(const std::string& path) const;
    std::vector<std::string> getRoutes() const;
    
    // Route matching
    std::string matchRoute(const std::string& path) const;
    std::map<std::string, std::string> extractRouteParams(const std::string& path, const std::string& route) const;
    
    // Navigation
    void navigateTo(const std::string& path);
    std::string getCurrentPath() const;
    void setCurrentPath(const std::string& path);
    
private:
    std::map<std::string, std::string> routes;
    std::string current_path;
    bool isRouteMatch(const std::string& path, const std::string& route) const;
    std::vector<std::string> parseRouteSegments(const std::string& route) const;
};

// Local script system
class CHTLLocalScript {
public:
    CHTLLocalScript();
    ~CHTLLocalScript();
    
    // Script management
    bool addScript(const std::string& name, const std::string& content);
    bool removeScript(const std::string& name);
    bool updateScript(const std::string& name, const std::string& content);
    
    // Script execution
    std::string executeScript(const std::string& name, const std::map<std::string, std::string>& context = {});
    bool isScriptLoaded(const std::string& name) const;
    std::string getScriptContent(const std::string& name) const;
    
    // Script queries
    std::vector<std::string> getScriptNames() const;
    std::map<std::string, std::string> getAllScripts() const;
    
private:
    std::map<std::string, std::string> scripts;
    std::string processScriptContent(const std::string& content, const std::map<std::string, std::string>& context);
};

// CHTL JS features manager
class CHTLJSFeaturesManager {
public:
    CHTLJSFeaturesManager();
    ~CHTLJSFeaturesManager();
    
    // Feature initialization
    bool initializeFeatures();
    void cleanupFeatures();
    
    // Feature access
    CHTLFileLoader& getFileLoader();
    CHTLEnhancedSelector& getEnhancedSelector();
    CHTLEnhancedListener& getEnhancedListener();
    CHTLEventDelegation& getEventDelegation();
    CHTLAnimation& getAnimation();
    CHTLVirtualObject& getVirtualObject();
    CHTLRouter& getRouter();
    CHTLLocalScript& getLocalScript();
    
    // Feature management
    bool isFeatureEnabled(const std::string& feature_name) const;
    void enableFeature(const std::string& feature_name);
    void disableFeature(const std::string& feature_name);
    
    // Feature queries
    std::vector<std::string> getEnabledFeatures() const;
    std::vector<std::string> getAvailableFeatures() const;
    
private:
    CHTLFileLoader file_loader;
    CHTLEnhancedSelector enhanced_selector;
    CHTLEnhancedListener enhanced_listener;
    CHTLEventDelegation event_delegation;
    CHTLAnimation animation;
    CHTLVirtualObject virtual_object;
    CHTLRouter router;
    CHTLLocalScript local_script;
    
    std::map<std::string, bool> feature_states;
    void initializeFeatureStates();
};

} // namespace CHTL

#endif // CHTL_JS_FEATURES_H