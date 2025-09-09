//! CHTL JS context
//! 
//! This module provides the context for CHTL JS compilation,
//! including virtual objects, enhanced selectors, and JavaScript-specific features.

use std::collections::HashMap;
use serde::{Deserialize, Serialize};

/// CHTL JS context
#[derive(Debug, Clone)]
pub struct CHTLJSContext {
    /// Virtual objects
    pub virtual_objects: HashMap<String, VirtualObject>,
    /// Enhanced selectors
    pub enhanced_selectors: HashMap<String, EnhancedSelector>,
    /// Event listeners
    pub event_listeners: Vec<EventListener>,
    /// Event delegates
    pub event_delegates: Vec<EventDelegate>,
    /// Animations
    pub animations: Vec<Animation>,
    /// Routes
    pub routes: Vec<Route>,
    /// File loaders
    pub file_loaders: Vec<FileLoader>,
    /// Global scripts
    pub global_scripts: Vec<GlobalScript>,
    /// Configuration
    pub config: CHTLJSConfiguration,
}

/// Virtual object
#[derive(Debug, Clone)]
pub struct VirtualObject {
    /// Object name
    pub name: String,
    /// Object properties
    pub properties: HashMap<String, VirtualProperty>,
    /// Object methods
    pub methods: HashMap<String, VirtualMethod>,
    /// Object location
    pub location: SourceLocation,
}

/// Virtual property
#[derive(Debug, Clone)]
pub struct VirtualProperty {
    /// Property name
    pub name: String,
    /// Property type
    pub property_type: VirtualPropertyType,
    /// Property value
    pub value: Option<String>,
    /// Property location
    pub location: SourceLocation,
}

/// Virtual property type
#[derive(Debug, Clone, PartialEq)]
pub enum VirtualPropertyType {
    String,
    Number,
    Boolean,
    Object,
    Array,
    Function,
}

/// Virtual method
#[derive(Debug, Clone)]
pub struct VirtualMethod {
    /// Method name
    pub name: String,
    /// Method parameters
    pub parameters: Vec<VirtualParameter>,
    /// Method body
    pub body: String,
    /// Method location
    pub location: SourceLocation,
}

/// Virtual parameter
#[derive(Debug, Clone)]
pub struct VirtualParameter {
    /// Parameter name
    pub name: String,
    /// Parameter type
    pub parameter_type: VirtualParameterType,
    /// Parameter default value
    pub default_value: Option<String>,
    /// Parameter location
    pub location: SourceLocation,
}

/// Virtual parameter type
#[derive(Debug, Clone, PartialEq)]
pub enum VirtualParameterType {
    String,
    Number,
    Boolean,
    Object,
    Array,
    Function,
    Any,
}

/// Enhanced selector
#[derive(Debug, Clone)]
pub struct EnhancedSelector {
    /// Selector name
    pub name: String,
    /// Selector pattern
    pub pattern: String,
    /// Selector type
    pub selector_type: EnhancedSelectorType,
    /// Selector location
    pub location: SourceLocation,
}

/// Enhanced selector type
#[derive(Debug, Clone, PartialEq)]
pub enum EnhancedSelectorType {
    Class,
    Id,
    Tag,
    Descendant,
    Child,
    Sibling,
    PseudoClass,
    PseudoElement,
}

/// Event listener
#[derive(Debug, Clone)]
pub struct EventListener {
    /// Event type
    pub event_type: String,
    /// Event handler
    pub handler: String,
    /// Event target
    pub target: String,
    /// Event location
    pub location: SourceLocation,
}

/// Event delegate
#[derive(Debug, Clone)]
pub struct EventDelegate {
    /// Parent selector
    pub parent_selector: String,
    /// Target selectors
    pub target_selectors: Vec<String>,
    /// Event handlers
    pub handlers: HashMap<String, String>,
    /// Event location
    pub location: SourceLocation,
}

/// Animation
#[derive(Debug, Clone)]
pub struct Animation {
    /// Animation name
    pub name: String,
    /// Animation target
    pub target: String,
    /// Animation duration
    pub duration: u32,
    /// Animation easing
    pub easing: String,
    /// Animation keyframes
    pub keyframes: Vec<Keyframe>,
    /// Animation options
    pub options: AnimationOptions,
    /// Animation location
    pub location: SourceLocation,
}

/// Keyframe
#[derive(Debug, Clone)]
pub struct Keyframe {
    /// Keyframe time
    pub time: f32,
    /// Keyframe properties
    pub properties: HashMap<String, String>,
}

/// Animation options
#[derive(Debug, Clone)]
pub struct AnimationOptions {
    /// Loop count
    pub loop_count: i32,
    /// Direction
    pub direction: AnimationDirection,
    /// Delay
    pub delay: u32,
    /// Callback
    pub callback: Option<String>,
}

/// Animation direction
#[derive(Debug, Clone, PartialEq)]
pub enum AnimationDirection {
    Normal,
    Reverse,
    Alternate,
    AlternateReverse,
}

/// Route
#[derive(Debug, Clone)]
pub struct Route {
    /// Route URL
    pub url: String,
    /// Route page
    pub page: String,
    /// Route options
    pub options: RouteOptions,
    /// Route location
    pub location: SourceLocation,
}

/// Route options
#[derive(Debug, Clone)]
pub struct RouteOptions {
    /// Route mode
    pub mode: RouteMode,
    /// Route root
    pub root: Option<String>,
}

/// Route mode
#[derive(Debug, Clone, PartialEq)]
pub enum RouteMode {
    History,
    Hash,
}

/// File loader
#[derive(Debug, Clone)]
pub struct FileLoader {
    /// Loader name
    pub name: String,
    /// Files to load
    pub files: Vec<String>,
    /// Loader options
    pub options: FileLoaderOptions,
    /// Loader location
    pub location: SourceLocation,
}

/// File loader options
#[derive(Debug, Clone)]
pub struct FileLoaderOptions {
    /// Load order
    pub load_order: LoadOrder,
    /// Dependencies
    pub dependencies: Vec<String>,
}

/// Load order
#[derive(Debug, Clone, PartialEq)]
pub enum LoadOrder {
    Sequential,
    Parallel,
    Dependency,
}

/// Global script
#[derive(Debug, Clone)]
pub struct GlobalScript {
    /// Script content
    pub content: String,
    /// Script type
    pub script_type: ScriptType,
    /// Script priority
    pub priority: usize,
    /// Script location
    pub location: SourceLocation,
}

/// Script type
#[derive(Debug, Clone, PartialEq)]
pub enum ScriptType {
    JavaScript,
    CHTLJS,
}

/// CHTL JS configuration
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct CHTLJSConfiguration {
    /// Enable virtual objects
    pub enable_virtual_objects: bool,
    /// Enable enhanced selectors
    pub enable_enhanced_selectors: bool,
    /// Enable event delegation
    pub enable_event_delegation: bool,
    /// Enable animations
    pub enable_animations: bool,
    /// Enable routing
    pub enable_routing: bool,
    /// Enable file loading
    pub enable_file_loading: bool,
    /// Minify output
    pub minify_output: bool,
    /// Source maps
    pub source_maps: bool,
}

/// Source location
#[derive(Debug, Clone, PartialEq)]
pub struct SourceLocation {
    pub file: String,
    pub line: usize,
    pub column: usize,
}

impl CHTLJSContext {
    /// Create a new CHTL JS context
    pub fn new() -> Self {
        Self {
            virtual_objects: HashMap::new(),
            enhanced_selectors: HashMap::new(),
            event_listeners: Vec::new(),
            event_delegates: Vec::new(),
            animations: Vec::new(),
            routes: Vec::new(),
            file_loaders: Vec::new(),
            global_scripts: Vec::new(),
            config: CHTLJSConfiguration::default(),
        }
    }
    
    /// Add a virtual object
    pub fn add_virtual_object(&mut self, name: String, virtual_object: VirtualObject) {
        self.virtual_objects.insert(name, virtual_object);
    }
    
    /// Get a virtual object
    pub fn get_virtual_object(&self, name: &str) -> Option<&VirtualObject> {
        self.virtual_objects.get(name)
    }
    
    /// Add an enhanced selector
    pub fn add_enhanced_selector(&mut self, name: String, selector: EnhancedSelector) {
        self.enhanced_selectors.insert(name, selector);
    }
    
    /// Get an enhanced selector
    pub fn get_enhanced_selector(&self, name: &str) -> Option<&EnhancedSelector> {
        self.enhanced_selectors.get(name)
    }
    
    /// Add an event listener
    pub fn add_event_listener(&mut self, listener: EventListener) {
        self.event_listeners.push(listener);
    }
    
    /// Add an event delegate
    pub fn add_event_delegate(&mut self, delegate: EventDelegate) {
        self.event_delegates.push(delegate);
    }
    
    /// Add an animation
    pub fn add_animation(&mut self, animation: Animation) {
        self.animations.push(animation);
    }
    
    /// Add a route
    pub fn add_route(&mut self, route: Route) {
        self.routes.push(route);
    }
    
    /// Add a file loader
    pub fn add_file_loader(&mut self, loader: FileLoader) {
        self.file_loaders.push(loader);
    }
    
    /// Add a global script
    pub fn add_global_script(&mut self, script: GlobalScript) {
        self.global_scripts.push(script);
    }
    
    /// Get all virtual objects
    pub fn get_virtual_objects(&self) -> &HashMap<String, VirtualObject> {
        &self.virtual_objects
    }
    
    /// Get all enhanced selectors
    pub fn get_enhanced_selectors(&self) -> &HashMap<String, EnhancedSelector> {
        &self.enhanced_selectors
    }
    
    /// Get all event listeners
    pub fn get_event_listeners(&self) -> &[EventListener] {
        &self.event_listeners
    }
    
    /// Get all event delegates
    pub fn get_event_delegates(&self) -> &[EventDelegate] {
        &self.event_delegates
    }
    
    /// Get all animations
    pub fn get_animations(&self) -> &[Animation] {
        &self.animations
    }
    
    /// Get all routes
    pub fn get_routes(&self) -> &[Route] {
        &self.routes
    }
    
    /// Get all file loaders
    pub fn get_file_loaders(&self) -> &[FileLoader] {
        &self.file_loaders
    }
    
    /// Get all global scripts
    pub fn get_global_scripts(&self) -> &[GlobalScript] {
        &self.global_scripts
    }
    
    /// Clear all data
    pub fn clear(&mut self) {
        self.virtual_objects.clear();
        self.enhanced_selectors.clear();
        self.event_listeners.clear();
        self.event_delegates.clear();
        self.animations.clear();
        self.routes.clear();
        self.file_loaders.clear();
        self.global_scripts.clear();
    }
}

impl Default for CHTLJSContext {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CHTLJSConfiguration {
    fn default() -> Self {
        Self {
            enable_virtual_objects: true,
            enable_enhanced_selectors: true,
            enable_event_delegation: true,
            enable_animations: true,
            enable_routing: true,
            enable_file_loading: true,
            minify_output: false,
            source_maps: false,
        }
    }
}