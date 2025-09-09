//! CHTL JS processor for advanced JavaScript features

use crate::ast_enum::*;
use crate::error::{ChtlError, Result};
use std::collections::HashMap;

/// CHTL JS processor for handling advanced JavaScript features
pub struct ChtlJsProcessor {
    virtual_objects: HashMap<String, VirtualObject>,
    event_delegates: HashMap<String, EventDelegate>,
    animations: HashMap<String, Animation>,
    routers: Vec<Router>,
    file_loaders: Vec<FileLoader>,
}

/// Virtual object definition
#[derive(Debug, Clone)]
pub struct VirtualObject {
    pub name: String,
    pub functions: HashMap<String, VirtualFunction>,
    pub properties: HashMap<String, VirtualProperty>,
    pub metadata: VirtualObjectMetadata,
}

/// Virtual function
#[derive(Debug, Clone)]
pub struct VirtualFunction {
    pub name: String,
    pub parameters: Vec<FunctionParameter>,
    pub return_type: Option<String>,
    pub body: String,
    pub is_async: bool,
    pub is_generator: bool,
}

/// Function parameter
#[derive(Debug, Clone)]
pub struct FunctionParameter {
    pub name: String,
    pub parameter_type: Option<String>,
    pub default_value: Option<String>,
    pub is_rest: bool,
}

/// Virtual property
#[derive(Debug, Clone)]
pub struct VirtualProperty {
    pub name: String,
    pub property_type: String,
    pub getter: Option<String>,
    pub setter: Option<String>,
    pub is_readonly: bool,
}

/// Virtual object metadata
#[derive(Debug, Clone)]
pub struct VirtualObjectMetadata {
    pub created_at: String,
    pub updated_at: String,
    pub version: String,
    pub description: Option<String>,
}

/// Event delegate
#[derive(Debug, Clone)]
pub struct EventDelegate {
    pub parent_selector: String,
    pub target_selectors: Vec<String>,
    pub events: HashMap<String, EventHandler>,
    pub options: EventDelegateOptions,
}

/// Event handler
#[derive(Debug, Clone)]
pub struct EventHandler {
    pub event_type: String,
    pub handler: String,
    pub is_once: bool,
    pub is_passive: bool,
    pub is_capture: bool,
}

/// Event delegate options
#[derive(Debug, Clone)]
pub struct EventDelegateOptions {
    pub auto_cleanup: bool,
    pub debounce_delay: Option<u32>,
    pub throttle_delay: Option<u32>,
}

/// Animation definition
#[derive(Debug, Clone)]
pub struct Animation {
    pub name: String,
    pub target: AnimationTarget,
    pub duration: u32,
    pub easing: EasingFunction,
    pub keyframes: Vec<Keyframe>,
    pub options: AnimationOptions,
}

/// Animation target
#[derive(Debug, Clone)]
pub enum AnimationTarget {
    Selector(String),
    Selectors(Vec<String>),
    Element(String), // Reference to element
}

/// Easing function
#[derive(Debug, Clone)]
pub enum EasingFunction {
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,
    Custom(String),
}

/// Keyframe
#[derive(Debug, Clone)]
pub struct Keyframe {
    pub offset: f32,
    pub properties: HashMap<String, String>,
    pub easing: Option<EasingFunction>,
}

/// Animation options
#[derive(Debug, Clone)]
pub struct AnimationOptions {
    pub delay: u32,
    pub iterations: Option<u32>, // None means infinite
    pub direction: AnimationDirection,
    pub fill_mode: FillMode,
    pub auto_play: bool,
}

/// Animation direction
#[derive(Debug, Clone)]
pub enum AnimationDirection {
    Normal,
    Reverse,
    Alternate,
    AlternateReverse,
}

/// Fill mode
#[derive(Debug, Clone)]
pub enum FillMode {
    None,
    Forwards,
    Backwards,
    Both,
}

/// Router definition
#[derive(Debug, Clone)]
pub struct Router {
    pub routes: Vec<Route>,
    pub mode: RouterMode,
    pub root: Option<String>,
    pub options: RouterOptions,
}

/// Route
#[derive(Debug, Clone)]
pub struct Route {
    pub path: String,
    pub component: String,
    pub name: Option<String>,
    pub meta: HashMap<String, String>,
    pub before_enter: Option<String>,
    pub before_leave: Option<String>,
}

/// Router mode
#[derive(Debug, Clone)]
pub enum RouterMode {
    History,
    Hash,
}

/// Router options
#[derive(Debug, Clone)]
pub struct RouterOptions {
    pub base: String,
    pub link_active_class: String,
    pub link_exact_active_class: String,
    pub scroll_behavior: ScrollBehavior,
}

/// Scroll behavior
#[derive(Debug, Clone)]
pub enum ScrollBehavior {
    Auto,
    Smooth,
    Instant,
}

/// File loader
#[derive(Debug, Clone)]
pub struct FileLoader {
    pub name: String,
    pub files: Vec<FileLoadEntry>,
    pub options: FileLoaderOptions,
}

/// File load entry
#[derive(Debug, Clone)]
pub struct FileLoadEntry {
    pub path: String,
    pub file_type: FileType,
    pub dependencies: Vec<String>,
    pub condition: Option<String>,
}

/// File type
#[derive(Debug, Clone)]
pub enum FileType {
    JavaScript,
    ChtlJs,
    Css,
    Html,
    Json,
}

/// File loader options
#[derive(Debug, Clone)]
pub struct FileLoaderOptions {
    pub async: bool,
    pub defer: bool,
    pub cache: bool,
    pub minify: bool,
}

impl ChtlJsProcessor {
    pub fn new() -> Self {
        Self {
            virtual_objects: HashMap::new(),
            event_delegates: HashMap::new(),
            animations: HashMap::new(),
            routers: Vec::new(),
            file_loaders: Vec::new(),
        }
    }
    
    /// Process CHTL JS script
    pub fn process_script(&mut self, script: &ScriptNode) -> Result<ProcessedScript> {
        let mut processed = ProcessedScript {
            javascript: String::new(),
            virtual_objects: Vec::new(),
            event_delegates: Vec::new(),
            animations: Vec::new(),
            routers: Vec::new(),
            file_loaders: Vec::new(),
        };
        
        if script.is_chtl_js {
            let chtl_js_content = self.parse_chtl_js(&script.content)?;
            processed = self.process_chtl_js_content(chtl_js_content)?;
        } else {
            processed.javascript = script.content.clone();
        }
        
        Ok(processed)
    }
    
    /// Parse CHTL JS content
    fn parse_chtl_js(&self, content: &str) -> Result<ChtlJsContent> {
        let mut chtl_js = ChtlJsContent {
            virtual_objects: Vec::new(),
            event_delegates: Vec::new(),
            animations: Vec::new(),
            routers: Vec::new(),
            file_loaders: Vec::new(),
            expressions: Vec::new(),
        };
        
        // Parse different CHTL JS constructs
        let lines: Vec<&str> = content.lines().collect();
        let mut i = 0;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line.starts_with("vir ") {
                let (virtual_obj, consumed_lines) = self.parse_virtual_object(&lines, i)?;
                chtl_js.virtual_objects.push(virtual_obj);
                i += consumed_lines;
            } else if line.starts_with("listen ") {
                let (event_delegate, consumed_lines) = self.parse_listen(&lines, i)?;
                chtl_js.event_delegates.push(event_delegate);
                i += consumed_lines;
            } else if line.starts_with("delegate ") {
                let (event_delegate, consumed_lines) = self.parse_delegate(&lines, i)?;
                chtl_js.event_delegates.push(event_delegate);
                i += consumed_lines;
            } else if line.starts_with("animate ") {
                let (animation, consumed_lines) = self.parse_animate(&lines, i)?;
                chtl_js.animations.push(animation);
                i += consumed_lines;
            } else if line.starts_with("router ") {
                let (router, consumed_lines) = self.parse_router(&lines, i)?;
                chtl_js.routers.push(router);
                i += consumed_lines;
            } else if line.starts_with("fileloader ") {
                let (file_loader, consumed_lines) = self.parse_fileloader(&lines, i)?;
                chtl_js.file_loaders.push(file_loader);
                i += consumed_lines;
            } else if line.starts_with("util ") {
                let (expression, consumed_lines) = self.parse_util_then(&lines, i)?;
                chtl_js.expressions.push(expression);
                i += consumed_lines;
            } else {
                i += 1;
            }
        }
        
        Ok(chtl_js)
    }
    
    /// Parse virtual object
    fn parse_virtual_object(&self, lines: &[&str], start: usize) -> Result<(VirtualObject, usize)> {
        let mut i = start;
        let line = lines[i].trim();
        
        // Extract name
        let name = line.strip_prefix("vir ")
            .ok_or_else(|| ChtlError::semantic("Invalid virtual object syntax"))?
            .trim()
            .to_string();
        
        let mut functions = HashMap::new();
        let mut properties = HashMap::new();
        
        i += 1;
        
        // Parse object body
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.contains(":") && !line.contains("(") {
                // Property
                let (prop_name, prop_value) = self.parse_property(line)?;
                properties.insert(prop_name, prop_value);
            } else if line.contains("(") && line.contains(")") {
                // Function
                let (func_name, func) = self.parse_function(line)?;
                functions.insert(func_name, func);
            }
            
            i += 1;
        }
        
        let virtual_obj = VirtualObject {
            name,
            functions,
            properties,
            metadata: VirtualObjectMetadata {
                created_at: chrono::Utc::now().to_rfc3339(),
                updated_at: chrono::Utc::now().to_rfc3339(),
                version: "1.0.0".to_string(),
                description: None,
            },
        };
        
        Ok((virtual_obj, i - start + 1))
    }
    
    /// Parse property
    fn parse_property(&self, line: &str) -> Result<(String, VirtualProperty)> {
        let parts: Vec<&str> = line.split(':').collect();
        if parts.len() != 2 {
            return Err(ChtlError::semantic("Invalid property syntax"));
        }
        
        let name = parts[0].trim().to_string();
        let value = parts[1].trim();
        
        let property = VirtualProperty {
            name: name.clone(),
            property_type: self.infer_property_type(value),
            getter: None,
            setter: None,
            is_readonly: false,
        };
        
        Ok((name, property))
    }
    
    /// Parse function
    fn parse_function(&self, line: &str) -> Result<(String, VirtualFunction)> {
        let func_name = line.split('(').next()
            .ok_or_else(|| ChtlError::semantic("Invalid function syntax"))?
            .trim()
            .to_string();
        
        let params_start = line.find('(').unwrap();
        let params_end = line.find(')').unwrap();
        let params_str = &line[params_start + 1..params_end];
        
        let parameters = if params_str.trim().is_empty() {
            Vec::new()
        } else {
            params_str.split(',')
                .map(|p| FunctionParameter {
                    name: p.trim().to_string(),
                    parameter_type: None,
                    default_value: None,
                    is_rest: false,
                })
                .collect()
        };
        
        let body_start = line.find('{').unwrap();
        let body = line[body_start..].to_string();
        
        let function = VirtualFunction {
            name: func_name.clone(),
            parameters,
            return_type: None,
            body,
            is_async: line.contains("async"),
            is_generator: line.contains("function*"),
        };
        
        Ok((func_name, function))
    }
    
    /// Parse listen construct
    fn parse_listen(&self, lines: &[&str], start: usize) -> Result<(EventDelegate, usize)> {
        let mut i = start;
        let line = lines[i].trim();
        
        // Extract selector
        let selector = line.strip_prefix("listen ")
            .ok_or_else(|| ChtlError::semantic("Invalid listen syntax"))?
            .trim()
            .to_string();
        
        let mut events = HashMap::new();
        
        i += 1;
        
        // Parse event handlers
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.contains(":") {
                let (event_type, handler) = self.parse_event_handler(line)?;
                events.insert(event_type, handler);
            }
            
            i += 1;
        }
        
        let event_delegate = EventDelegate {
            parent_selector: selector,
            target_selectors: vec![],
            events,
            options: EventDelegateOptions {
                auto_cleanup: true,
                debounce_delay: None,
                throttle_delay: None,
            },
        };
        
        Ok((event_delegate, i - start + 1))
    }
    
    /// Parse delegate construct
    fn parse_delegate(&self, lines: &[&str], start: usize) -> Result<(EventDelegate, usize)> {
        let mut i = start;
        let line = lines[i].trim();
        
        // Extract parent selector
        let parent_selector = line.strip_prefix("delegate ")
            .ok_or_else(|| ChtlError::semantic("Invalid delegate syntax"))?
            .trim()
            .to_string();
        
        let mut target_selectors = Vec::new();
        let mut events = HashMap::new();
        
        i += 1;
        
        // Parse delegate configuration
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("target:") {
                let targets = line.strip_prefix("target:")
                    .unwrap()
                    .trim()
                    .split(',')
                    .map(|s| s.trim().to_string())
                    .collect();
                target_selectors = targets;
            } else if line.contains(":") {
                let (event_type, handler) = self.parse_event_handler(line)?;
                events.insert(event_type, handler);
            }
            
            i += 1;
        }
        
        let event_delegate = EventDelegate {
            parent_selector,
            target_selectors,
            events,
            options: EventDelegateOptions {
                auto_cleanup: true,
                debounce_delay: None,
                throttle_delay: None,
            },
        };
        
        Ok((event_delegate, i - start + 1))
    }
    
    /// Parse event handler
    fn parse_event_handler(&self, line: &str) -> Result<(String, EventHandler)> {
        let parts: Vec<&str> = line.split(':').collect();
        if parts.len() != 2 {
            return Err(ChtlError::semantic("Invalid event handler syntax"));
        }
        
        let event_type = parts[0].trim().to_string();
        let handler = parts[1].trim().to_string();
        
        let event_handler = EventHandler {
            event_type: event_type.clone(),
            handler,
            is_once: false,
            is_passive: false,
            is_capture: false,
        };
        
        Ok((event_type, event_handler))
    }
    
    /// Parse animate construct
    fn parse_animate(&self, lines: &[&str], start: usize) -> Result<(Animation, usize)> {
        let mut i = start;
        let line = lines[i].trim();
        
        // Extract animation name
        let name = line.strip_prefix("animate ")
            .ok_or_else(|| ChtlError::semantic("Invalid animate syntax"))?
            .trim()
            .to_string();
        
        let mut target = AnimationTarget::Selector("".to_string());
        let mut duration = 1000;
        let mut easing = EasingFunction::EaseInOut;
        let mut keyframes = Vec::new();
        let mut options = AnimationOptions {
            delay: 0,
            iterations: Some(1),
            direction: AnimationDirection::Normal,
            fill_mode: FillMode::Both,
            auto_play: true,
        };
        
        i += 1;
        
        // Parse animation configuration
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("target:") {
                let target_str = line.strip_prefix("target:").unwrap().trim();
                target = AnimationTarget::Selector(target_str.to_string());
            } else if line.starts_with("duration:") {
                duration = line.strip_prefix("duration:").unwrap().trim().parse()
                    .map_err(|_| ChtlError::semantic("Invalid duration value"))?;
            } else if line.starts_with("easing:") {
                let easing_str = line.strip_prefix("easing:").unwrap().trim();
                easing = self.parse_easing_function(easing_str)?;
            } else if line.starts_with("delay:") {
                options.delay = line.strip_prefix("delay:").unwrap().trim().parse()
                    .map_err(|_| ChtlError::semantic("Invalid delay value"))?;
            } else if line.starts_with("loop:") {
                let loop_str = line.strip_prefix("loop:").unwrap().trim();
                if loop_str == "-1" {
                    options.iterations = None;
                } else {
                    options.iterations = Some(loop_str.parse()
                        .map_err(|_| ChtlError::semantic("Invalid loop value"))?);
                }
            } else if line.starts_with("begin:") {
                // Parse begin keyframe
                let begin_props = self.parse_keyframe_properties(&lines, i)?;
                keyframes.push(Keyframe {
                    offset: 0.0,
                    properties: begin_props,
                    easing: None,
                });
            } else if line.starts_with("end:") {
                // Parse end keyframe
                let end_props = self.parse_keyframe_properties(&lines, i)?;
                keyframes.push(Keyframe {
                    offset: 1.0,
                    properties: end_props,
                    easing: None,
                });
            } else if line.starts_with("when:") {
                // Parse when keyframes
                let when_keyframes = self.parse_when_keyframes(&lines, i)?;
                keyframes.extend(when_keyframes);
            }
            
            i += 1;
        }
        
        let animation = Animation {
            name,
            target,
            duration,
            easing,
            keyframes,
            options,
        };
        
        Ok((animation, i - start + 1))
    }
    
    /// Parse easing function
    fn parse_easing_function(&self, easing_str: &str) -> Result<EasingFunction> {
        match easing_str {
            "linear" => Ok(EasingFunction::Linear),
            "ease-in" => Ok(EasingFunction::EaseIn),
            "ease-out" => Ok(EasingFunction::EaseOut),
            "ease-in-out" => Ok(EasingFunction::EaseInOut),
            _ => Ok(EasingFunction::Custom(easing_str.to_string())),
        }
    }
    
    /// Parse keyframe properties
    fn parse_keyframe_properties(&self, lines: &[&str], start: usize) -> Result<HashMap<String, String>> {
        let mut properties = HashMap::new();
        let mut i = start;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.contains(":") {
                let parts: Vec<&str> = line.split(':').collect();
                if parts.len() == 2 {
                    let key = parts[0].trim().to_string();
                    let value = parts[1].trim().to_string();
                    properties.insert(key, value);
                }
            }
            
            i += 1;
        }
        
        Ok(properties)
    }
    
    /// Parse when keyframes
    fn parse_when_keyframes(&self, lines: &[&str], start: usize) -> Result<Vec<Keyframe>> {
        let mut keyframes = Vec::new();
        let mut i = start;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "]" {
                break;
            }
            
            if line.starts_with("at:") {
                let offset = line.strip_prefix("at:").unwrap().trim().parse::<f32>()
                    .map_err(|_| ChtlError::semantic("Invalid keyframe offset"))?;
                
                i += 1;
                let properties = self.parse_keyframe_properties(lines, i)?;
                
                keyframes.push(Keyframe {
                    offset,
                    properties,
                    easing: None,
                });
            }
            
            i += 1;
        }
        
        Ok(keyframes)
    }
    
    /// Parse router construct
    fn parse_router(&self, lines: &[&str], start: usize) -> Result<(Router, usize)> {
        let mut i = start;
        let line = lines[i].trim();
        
        // Extract router name
        let name = line.strip_prefix("router ")
            .ok_or_else(|| ChtlError::semantic("Invalid router syntax"))?
            .trim()
            .to_string();
        
        let mut routes = Vec::new();
        let mut mode = RouterMode::History;
        let mut root = None;
        let mut options = RouterOptions {
            base: "/".to_string(),
            link_active_class: "router-link-active".to_string(),
            link_exact_active_class: "router-link-exact-active".to_string(),
            scroll_behavior: ScrollBehavior::Auto,
        };
        
        i += 1;
        
        // Parse router configuration
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("url:") {
                let url = line.strip_prefix("url:").unwrap().trim().to_string();
                routes.push(Route {
                    path: url,
                    component: "".to_string(),
                    name: None,
                    meta: HashMap::new(),
                    before_enter: None,
                    before_leave: None,
                });
            } else if line.starts_with("page:") {
                let page = line.strip_prefix("page:").unwrap().trim().to_string();
                if let Some(route) = routes.last_mut() {
                    route.component = page;
                }
            } else if line.starts_with("mode:") {
                let mode_str = line.strip_prefix("mode:").unwrap().trim();
                mode = match mode_str {
                    "history" => RouterMode::History,
                    "hash" => RouterMode::Hash,
                    _ => return Err(ChtlError::semantic("Invalid router mode")),
                };
            } else if line.starts_with("root:") {
                root = Some(line.strip_prefix("root:").unwrap().trim().to_string());
            }
            
            i += 1;
        }
        
        let router = Router {
            routes,
            mode,
            root,
            options,
        };
        
        Ok((router, i - start + 1))
    }
    
    /// Parse fileloader construct
    fn parse_fileloader(&self, lines: &[&str], start: usize) -> Result<(FileLoader, usize)> {
        let mut i = start;
        let line = lines[i].trim();
        
        // Extract fileloader name
        let name = line.strip_prefix("fileloader ")
            .ok_or_else(|| ChtlError::semantic("Invalid fileloader syntax"))?
            .trim()
            .to_string();
        
        let mut files = Vec::new();
        let mut options = FileLoaderOptions {
            async: true,
            defer: false,
            cache: true,
            minify: false,
        };
        
        i += 1;
        
        // Parse fileloader configuration
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("load:") {
                let load_str = line.strip_prefix("load:").unwrap().trim();
                let file_entries = self.parse_load_entries(load_str)?;
                files.extend(file_entries);
            } else if line.starts_with("async:") {
                options.async = line.strip_prefix("async:").unwrap().trim() == "true";
            } else if line.starts_with("defer:") {
                options.defer = line.strip_prefix("defer:").unwrap().trim() == "true";
            } else if line.starts_with("cache:") {
                options.cache = line.strip_prefix("cache:").unwrap().trim() == "true";
            } else if line.starts_with("minify:") {
                options.minify = line.strip_prefix("minify:").unwrap().trim() == "true";
            }
            
            i += 1;
        }
        
        let file_loader = FileLoader {
            name,
            files,
            options,
        };
        
        Ok((file_loader, i - start + 1))
    }
    
    /// Parse load entries
    fn parse_load_entries(&self, load_str: &str) -> Result<Vec<FileLoadEntry>> {
        let mut entries = Vec::new();
        
        for entry_str in load_str.split(',') {
            let entry_str = entry_str.trim();
            let file_type = self.infer_file_type(entry_str);
            
            entries.push(FileLoadEntry {
                path: entry_str.to_string(),
                file_type,
                dependencies: Vec::new(),
                condition: None,
            });
        }
        
        Ok(entries)
    }
    
    /// Infer file type from path
    fn infer_file_type(&self, path: &str) -> FileType {
        if path.ends_with(".js") {
            FileType::JavaScript
        } else if path.ends_with(".cjjs") {
            FileType::ChtlJs
        } else if path.ends_with(".css") {
            FileType::Css
        } else if path.ends_with(".html") {
            FileType::Html
        } else if path.ends_with(".json") {
            FileType::Json
        } else {
            FileType::JavaScript
        }
    }
    
    /// Parse util then construct
    fn parse_util_then(&self, lines: &[&str], start: usize) -> Result<(UtilThenExpression, usize)> {
        let mut i = start;
        let line = lines[i].trim();
        
        // Extract expression
        let expression = line.strip_prefix("util ")
            .ok_or_else(|| ChtlError::semantic("Invalid util syntax"))?
            .trim()
            .to_string();
        
        let mut change_action = None;
        let mut then_action = None;
        
        i += 1;
        
        // Parse change and then actions
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line.starts_with("change ") {
                change_action = Some(line.strip_prefix("change ").unwrap().trim().to_string());
            } else if line.starts_with("then ") {
                then_action = Some(line.strip_prefix("then ").unwrap().trim().to_string());
            }
            
            i += 1;
        }
        
        let util_expr = UtilThenExpression {
            expression,
            change_action,
            then_action,
        };
        
        Ok((util_expr, i - start))
    }
    
    /// Process CHTL JS content
    fn process_chtl_js_content(&mut self, content: ChtlJsContent) -> Result<ProcessedScript> {
        let mut processed = ProcessedScript {
            javascript: String::new(),
            virtual_objects: Vec::new(),
            event_delegates: Vec::new(),
            animations: Vec::new(),
            routers: Vec::new(),
            file_loaders: Vec::new(),
        };
        
        // Process virtual objects
        for virtual_obj in content.virtual_objects {
            let js_code = self.generate_virtual_object_js(&virtual_obj)?;
            processed.javascript.push_str(&js_code);
            processed.virtual_objects.push(virtual_obj);
        }
        
        // Process event delegates
        for event_delegate in content.event_delegates {
            let js_code = self.generate_event_delegate_js(&event_delegate)?;
            processed.javascript.push_str(&js_code);
            processed.event_delegates.push(event_delegate);
        }
        
        // Process animations
        for animation in content.animations {
            let js_code = self.generate_animation_js(&animation)?;
            processed.javascript.push_str(&js_code);
            processed.animations.push(animation);
        }
        
        // Process routers
        for router in content.routers {
            let js_code = self.generate_router_js(&router)?;
            processed.javascript.push_str(&js_code);
            processed.routers.push(router);
        }
        
        // Process file loaders
        for file_loader in content.file_loaders {
            let js_code = self.generate_fileloader_js(&file_loader)?;
            processed.javascript.push_str(&js_code);
            processed.file_loaders.push(file_loader);
        }
        
        // Process expressions
        for expression in content.expressions {
            let js_code = self.generate_util_then_js(&expression)?;
            processed.javascript.push_str(&js_code);
        }
        
        Ok(processed)
    }
    
    /// Generate JavaScript for virtual object
    fn generate_virtual_object_js(&self, virtual_obj: &VirtualObject) -> Result<String> {
        let mut js = String::new();
        
        js.push_str(&format!("// Virtual object: {}\n", virtual_obj.name));
        js.push_str(&format!("const {} = {{\n", virtual_obj.name));
        
        // Generate properties
        for (prop_name, prop) in &virtual_obj.properties {
            js.push_str(&format!("    {}: {},\n", prop_name, prop.property_type));
        }
        
        // Generate functions
        for (func_name, func) in &virtual_obj.functions {
            js.push_str(&format!("    {}: function({}) {{\n", func_name, 
                func.parameters.iter().map(|p| &p.name).collect::<Vec<_>>().join(", ")));
            js.push_str(&format!("        {}\n", func.body));
            js.push_str("    },\n");
        }
        
        js.push_str("};\n\n");
        
        Ok(js)
    }
    
    /// Generate JavaScript for event delegate
    fn generate_event_delegate_js(&self, event_delegate: &EventDelegate) -> Result<String> {
        let mut js = String::new();
        
        js.push_str(&format!("// Event delegate for: {}\n", event_delegate.parent_selector));
        js.push_str(&format!("const delegate_{} = new EventDelegate('{}', {{\n", 
            event_delegate.parent_selector.replace(" ", "_"), event_delegate.parent_selector));
        
        for (event_type, handler) in &event_delegate.events {
            js.push_str(&format!("    {}: {},\n", event_type, handler));
        }
        
        js.push_str("});\n\n");
        
        Ok(js)
    }
    
    /// Generate JavaScript for animation
    fn generate_animation_js(&self, animation: &Animation) -> Result<String> {
        let mut js = String::new();
        
        js.push_str(&format!("// Animation: {}\n", animation.name));
        js.push_str(&format!("const {} = new Animation({{\n", animation.name));
        js.push_str(&format!("    target: '{}',\n", match &animation.target {
            AnimationTarget::Selector(s) => s,
            AnimationTarget::Selectors(s) => &s.join(", "),
            AnimationTarget::Element(s) => s,
        }));
        js.push_str(&format!("    duration: {},\n", animation.duration));
        js.push_str(&format!("    easing: '{}',\n", match &animation.easing {
            EasingFunction::Linear => "linear",
            EasingFunction::EaseIn => "ease-in",
            EasingFunction::EaseOut => "ease-out",
            EasingFunction::EaseInOut => "ease-in-out",
            EasingFunction::Custom(s) => s,
        }));
        
        js.push_str("    keyframes: [\n");
        for keyframe in &animation.keyframes {
            js.push_str(&format!("        {{ offset: {}, properties: {{", keyframe.offset));
            for (prop, value) in &keyframe.properties {
                js.push_str(&format!(" {}: '{}',", prop, value));
            }
            js.push_str(" } },\n");
        }
        js.push_str("    ],\n");
        
        js.push_str("});\n\n");
        
        Ok(js)
    }
    
    /// Generate JavaScript for router
    fn generate_router_js(&self, router: &Router) -> Result<String> {
        let mut js = String::new();
        
        js.push_str("// Router configuration\n");
        js.push_str("const router = new Router({\n");
        js.push_str(&format!("    mode: '{}',\n", match router.mode {
            RouterMode::History => "history",
            RouterMode::Hash => "hash",
        }));
        
        js.push_str("    routes: [\n");
        for route in &router.routes {
            js.push_str(&format!("        {{ path: '{}', component: '{}' }},\n", route.path, route.component));
        }
        js.push_str("    ],\n");
        
        js.push_str("});\n\n");
        
        Ok(js)
    }
    
    /// Generate JavaScript for file loader
    fn generate_fileloader_js(&self, file_loader: &FileLoader) -> Result<String> {
        let mut js = String::new();
        
        js.push_str(&format!("// File loader: {}\n", file_loader.name));
        js.push_str(&format!("const {} = new FileLoader([\n", file_loader.name));
        
        for file in &file_loader.files {
            js.push_str(&format!("    '{}',\n", file.path));
        }
        
        js.push_str("], {\n");
        js.push_str(&format!("    async: {},\n", file_loader.options.async));
        js.push_str(&format!("    defer: {},\n", file_loader.options.defer));
        js.push_str(&format!("    cache: {},\n", file_loader.options.cache));
        js.push_str(&format!("    minify: {},\n", file_loader.options.minify));
        js.push_str("});\n\n");
        
        Ok(js)
    }
    
    /// Generate JavaScript for util then expression
    fn generate_util_then_js(&self, expression: &UtilThenExpression) -> Result<String> {
        let mut js = String::new();
        
        js.push_str("// Util then expression\n");
        js.push_str(&format!("const utilExpr = new UtilThen('{}', {{\n", expression.expression));
        
        if let Some(change) = &expression.change_action {
            js.push_str(&format!("    change: {},\n", change));
        }
        
        if let Some(then) = &expression.then_action {
            js.push_str(&format!("    then: {},\n", then));
        }
        
        js.push_str("});\n\n");
        
        Ok(js)
    }
    
    /// Infer property type
    fn infer_property_type(&self, value: &str) -> String {
        if value.starts_with("function") {
            "function".to_string()
        } else if value.starts_with("{") {
            "object".to_string()
        } else if value.starts_with("[") {
            "array".to_string()
        } else if value == "true" || value == "false" {
            "boolean".to_string()
        } else if value.parse::<f64>().is_ok() {
            "number".to_string()
        } else {
            "string".to_string()
        }
    }
}

/// CHTL JS content
#[derive(Debug, Clone)]
pub struct ChtlJsContent {
    pub virtual_objects: Vec<VirtualObject>,
    pub event_delegates: Vec<EventDelegate>,
    pub animations: Vec<Animation>,
    pub routers: Vec<Router>,
    pub file_loaders: Vec<FileLoader>,
    pub expressions: Vec<UtilThenExpression>,
}

/// Processed script result
#[derive(Debug, Clone)]
pub struct ProcessedScript {
    pub javascript: String,
    pub virtual_objects: Vec<VirtualObject>,
    pub event_delegates: Vec<EventDelegate>,
    pub animations: Vec<Animation>,
    pub routers: Vec<Router>,
    pub file_loaders: Vec<FileLoader>,
}

/// Util then expression
#[derive(Debug, Clone)]
pub struct UtilThenExpression {
    pub expression: String,
    pub change_action: Option<String>,
    pub then_action: Option<String>,
}