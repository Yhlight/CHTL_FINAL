//! Module processor for CHTL modules (CMOD) and CHTL JS modules (CJMOD)

use crate::ast_enum::*;
use crate::error::{ChtlError, Result};
use crate::chtl_js_processor::VirtualObject;
use std::collections::HashMap;
use std::path::{Path, PathBuf};

/// Module processor for handling CMOD and CJMOD
pub struct ModuleProcessor {
    modules: HashMap<String, ModuleDefinition>,
    module_paths: HashMap<String, PathBuf>,
    export_tables: HashMap<String, ExportTable>,
}

/// Module definition
#[derive(Debug, Clone)]
pub struct ModuleDefinition {
    pub name: String,
    pub module_type: ModuleType,
    pub content: ModuleContent,
    pub info: ModuleInfo,
    pub exports: ExportTable,
    pub dependencies: Vec<ModuleDependency>,
    pub path: PathBuf,
}

/// Module type
#[derive(Debug, Clone, PartialEq)]
pub enum ModuleType {
    Cmod,
    Cjmod,
}

/// Module content
#[derive(Debug, Clone)]
pub enum ModuleContent {
    Cmod(Vec<AstNode>),
    Cjmod(CjmodContent),
}

/// CJMOD content
#[derive(Debug, Clone)]
pub struct CjmodContent {
    pub syntax: SyntaxClass,
    pub arg: ArgClass,
    pub scanner: CjmodScanner,
    pub generator: CjmodGenerator,
    pub virtual_objects: HashMap<String, VirtualObject>,
    pub functions: HashMap<String, ChtlJsFunction>,
}

/// Syntax class for CJMOD
#[derive(Debug, Clone)]
pub struct SyntaxClass {
    pub methods: HashMap<String, SyntaxMethod>,
}

/// Syntax method
#[derive(Debug, Clone)]
pub struct SyntaxMethod {
    pub name: String,
    pub parameters: Vec<FunctionParameter>,
    pub return_type: String,
    pub body: String,
    pub is_static: bool,
}

/// Arg class for CJMOD
#[derive(Debug, Clone)]
pub struct ArgClass {
    pub methods: HashMap<String, ArgMethod>,
}

/// Arg method
#[derive(Debug, Clone)]
pub struct ArgMethod {
    pub name: String,
    pub parameters: Vec<FunctionParameter>,
    pub return_type: String,
    pub body: String,
    pub is_static: bool,
}

/// CJMOD scanner
#[derive(Debug, Clone)]
pub struct CjmodScanner {
    pub scan_methods: HashMap<String, ScanMethod>,
}

/// Scan method
#[derive(Debug, Clone)]
pub struct ScanMethod {
    pub name: String,
    pub parameters: Vec<FunctionParameter>,
    pub return_type: String,
    pub body: String,
}

/// CJMOD generator
#[derive(Debug, Clone)]
pub struct CjmodGenerator {
    pub export_methods: HashMap<String, ExportMethod>,
}

/// Export method
#[derive(Debug, Clone)]
pub struct ExportMethod {
    pub name: String,
    pub parameters: Vec<FunctionParameter>,
    pub return_type: String,
    pub body: String,
}

/// CHTL JS function
#[derive(Debug, Clone)]
pub struct ChtlJsFunction {
    pub name: String,
    pub parameters: Vec<FunctionParameter>,
    pub return_type: String,
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

/// Module info
#[derive(Debug, Clone)]
pub struct ModuleInfo {
    pub name: String,
    pub version: String,
    pub author: Option<String>,
    pub description: Option<String>,
    pub license: Option<String>,
    pub homepage: Option<String>,
    pub repository: Option<String>,
    pub keywords: Vec<String>,
    pub created_at: String,
    pub updated_at: String,
}

/// Export table
#[derive(Debug, Clone)]
pub struct ExportTable {
    pub functions: HashMap<String, ExportFunction>,
    pub variables: HashMap<String, ExportVariable>,
    pub classes: HashMap<String, ExportClass>,
    pub modules: HashMap<String, ExportModule>,
}

/// Export function
#[derive(Debug, Clone)]
pub struct ExportFunction {
    pub name: String,
    pub parameters: Vec<FunctionParameter>,
    pub return_type: String,
    pub description: Option<String>,
    pub is_async: bool,
    pub is_generator: bool,
}

/// Export variable
#[derive(Debug, Clone)]
pub struct ExportVariable {
    pub name: String,
    pub variable_type: String,
    pub value: Option<String>,
    pub description: Option<String>,
    pub is_constant: bool,
}

/// Export class
#[derive(Debug, Clone)]
pub struct ExportClass {
    pub name: String,
    pub methods: HashMap<String, ExportFunction>,
    pub properties: HashMap<String, ExportVariable>,
    pub description: Option<String>,
    pub is_abstract: bool,
}

/// Export module
#[derive(Debug, Clone)]
pub struct ExportModule {
    pub name: String,
    pub module_type: ModuleType,
    pub description: Option<String>,
    pub exports: ExportTable,
}

/// Module dependency
#[derive(Debug, Clone)]
pub struct ModuleDependency {
    pub name: String,
    pub version: String,
    pub dependency_type: DependencyType,
    pub path: Option<String>,
}

/// Dependency type
#[derive(Debug, Clone, PartialEq)]
pub enum DependencyType {
    Official,
    User,
    Wildcard,
}

impl ModuleProcessor {
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            module_paths: HashMap::new(),
            export_tables: HashMap::new(),
        }
    }
    
    /// Load a module from file
    pub fn load_module(&mut self, path: &Path) -> Result<ModuleDefinition> {
        let path_buf = path.to_path_buf();
        let module_name = self.extract_module_name(&path_buf)?;
        
        if let Some(existing) = self.modules.get(&module_name) {
            return Ok(existing.clone());
        }
        
        let module_type = self.detect_module_type(&path_buf)?;
        let content = self.load_module_content(&path_buf, module_type)?;
        let info = self.extract_module_info(&content)?;
        let exports = self.extract_exports(&content)?;
        let dependencies = self.extract_dependencies(&content)?;
        
        let module = ModuleDefinition {
            name: module_name.clone(),
            module_type,
            content,
            info,
            exports,
            dependencies,
            path: path_buf.clone(),
        };
        
        self.modules.insert(module_name, module.clone());
        self.module_paths.insert(module.name.clone(), path_buf);
        
        Ok(module)
    }
    
    /// Extract module name from path
    fn extract_module_name(&self, path: &Path) -> Result<String> {
        let file_name = path.file_stem()
            .ok_or_else(|| ChtlError::semantic("Invalid module path"))?
            .to_string_lossy()
            .to_string();
        
        Ok(file_name)
    }
    
    /// Detect module type from path
    fn detect_module_type(&self, path: &Path) -> Result<ModuleType> {
        let extension = path.extension()
            .ok_or_else(|| ChtlError::semantic("No file extension"))?
            .to_string_lossy();
        
        match extension.as_ref() {
            "cmod" => Ok(ModuleType::Cmod),
            "cjmod" => Ok(ModuleType::Cjmod),
            _ => Err(ChtlError::semantic("Unknown module type")),
        }
    }
    
    /// Load module content
    fn load_module_content(&self, path: &Path, module_type: ModuleType) -> Result<ModuleContent> {
        let content = std::fs::read_to_string(path)
            .map_err(|e| ChtlError::io(&format!("Failed to read module file: {}", e)))?;
        
        match module_type {
            ModuleType::Cmod => {
                let ast = self.parse_cmod_content(&content)?;
                Ok(ModuleContent::Cmod(ast))
            }
            ModuleType::Cjmod => {
                let cjmod_content = self.parse_cjmod_content(&content)?;
                Ok(ModuleContent::Cjmod(cjmod_content))
            }
        }
    }
    
    /// Parse CMOD content
    fn parse_cmod_content(&self, content: &str) -> Result<Vec<AstNode>> {
        // This would use the main CHTL parser
        // For now, return a placeholder
        Ok(vec![])
    }
    
    /// Parse CJMOD content
    fn parse_cjmod_content(&self, content: &str) -> Result<CjmodContent> {
        let mut cjmod = CjmodContent {
            syntax: SyntaxClass { methods: HashMap::new() },
            arg: ArgClass { methods: HashMap::new() },
            scanner: CjmodScanner { scan_methods: HashMap::new() },
            generator: CjmodGenerator { export_methods: HashMap::new() },
            virtual_objects: HashMap::new(),
            functions: HashMap::new(),
        };
        
        // Parse CJMOD-specific constructs
        let lines: Vec<&str> = content.lines().collect();
        let mut i = 0;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line.starts_with("class Syntax") {
                let (syntax_class, consumed) = self.parse_syntax_class(&lines, i)?;
                cjmod.syntax = syntax_class;
                i += consumed;
            } else if line.starts_with("class Arg") {
                let (arg_class, consumed) = self.parse_arg_class(&lines, i)?;
                cjmod.arg = arg_class;
                i += consumed;
            } else if line.starts_with("class CJMODScanner") {
                let (scanner, consumed) = self.parse_cjmod_scanner(&lines, i)?;
                cjmod.scanner = scanner;
                i += consumed;
            } else if line.starts_with("class CJMODGenerator") {
                let (generator, consumed) = self.parse_cjmod_generator(&lines, i)?;
                cjmod.generator = generator;
                i += consumed;
            } else if line.starts_with("function ") {
                let (func, consumed) = self.parse_function(&lines, i)?;
                cjmod.functions.insert(func.name.clone(), func);
                i += consumed;
            } else {
                i += 1;
            }
        }
        
        Ok(cjmod)
    }
    
    /// Parse Syntax class
    fn parse_syntax_class(&self, lines: &[&str], start: usize) -> Result<(SyntaxClass, usize)> {
        let mut methods = HashMap::new();
        let mut i = start + 1;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("function ") || line.starts_with("static ") {
                let (method, consumed) = self.parse_syntax_method(&lines, i)?;
                methods.insert(method.name.clone(), method);
                i += consumed;
            } else {
                i += 1;
            }
        }
        
        Ok((SyntaxClass { methods }, i - start + 1))
    }
    
    /// Parse Syntax method
    fn parse_syntax_method(&self, lines: &[&str], start: usize) -> Result<(SyntaxMethod, usize)> {
        let line = lines[start].trim();
        let is_static = line.starts_with("static ");
        
        let func_line = if is_static {
            line.strip_prefix("static ").unwrap()
        } else {
            line
        };
        
        let name = func_line.strip_prefix("function ")
            .ok_or_else(|| ChtlError::semantic("Invalid function syntax"))?
            .split('(')
            .next()
            .unwrap()
            .trim()
            .to_string();
        
        let params_start = func_line.find('(').unwrap();
        let params_end = func_line.find(')').unwrap();
        let params_str = &func_line[params_start + 1..params_end];
        
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
        
        let body_start = func_line.find('{').unwrap();
        let body = func_line[body_start..].to_string();
        
        let method = SyntaxMethod {
            name,
            parameters,
            return_type: "any".to_string(),
            body,
            is_static,
        };
        
        Ok((method, 1))
    }
    
    /// Parse Arg class
    fn parse_arg_class(&self, lines: &[&str], start: usize) -> Result<(ArgClass, usize)> {
        let mut methods = HashMap::new();
        let mut i = start + 1;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("function ") || line.starts_with("static ") {
                let (method, consumed) = self.parse_arg_method(&lines, i)?;
                methods.insert(method.name.clone(), method);
                i += consumed;
            } else {
                i += 1;
            }
        }
        
        Ok((ArgClass { methods }, i - start + 1))
    }
    
    /// Parse Arg method
    fn parse_arg_method(&self, lines: &[&str], start: usize) -> Result<(ArgMethod, usize)> {
        let line = lines[start].trim();
        let is_static = line.starts_with("static ");
        
        let func_line = if is_static {
            line.strip_prefix("static ").unwrap()
        } else {
            line
        };
        
        let name = func_line.strip_prefix("function ")
            .ok_or_else(|| ChtlError::semantic("Invalid function syntax"))?
            .split('(')
            .next()
            .unwrap()
            .trim()
            .to_string();
        
        let params_start = func_line.find('(').unwrap();
        let params_end = func_line.find(')').unwrap();
        let params_str = &func_line[params_start + 1..params_end];
        
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
        
        let body_start = func_line.find('{').unwrap();
        let body = func_line[body_start..].to_string();
        
        let method = ArgMethod {
            name,
            parameters,
            return_type: "any".to_string(),
            body,
            is_static,
        };
        
        Ok((method, 1))
    }
    
    /// Parse CJMOD scanner
    fn parse_cjmod_scanner(&self, lines: &[&str], start: usize) -> Result<(CjmodScanner, usize)> {
        let mut scan_methods = HashMap::new();
        let mut i = start + 1;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("function ") {
                let (method, consumed) = self.parse_scan_method(&lines, i)?;
                scan_methods.insert(method.name.clone(), method);
                i += consumed;
            } else {
                i += 1;
            }
        }
        
        Ok((CjmodScanner { scan_methods }, i - start + 1))
    }
    
    /// Parse scan method
    fn parse_scan_method(&self, lines: &[&str], start: usize) -> Result<(ScanMethod, usize)> {
        let line = lines[start].trim();
        
        let name = line.strip_prefix("function ")
            .ok_or_else(|| ChtlError::semantic("Invalid function syntax"))?
            .split('(')
            .next()
            .unwrap()
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
        
        let method = ScanMethod {
            name,
            parameters,
            return_type: "any".to_string(),
            body,
        };
        
        Ok((method, 1))
    }
    
    /// Parse CJMOD generator
    fn parse_cjmod_generator(&self, lines: &[&str], start: usize) -> Result<(CjmodGenerator, usize)> {
        let mut export_methods = HashMap::new();
        let mut i = start + 1;
        
        while i < lines.len() {
            let line = lines[i].trim();
            
            if line == "}" {
                break;
            }
            
            if line.starts_with("function ") {
                let (method, consumed) = self.parse_export_method(&lines, i)?;
                export_methods.insert(method.name.clone(), method);
                i += consumed;
            } else {
                i += 1;
            }
        }
        
        Ok((CjmodGenerator { export_methods }, i - start + 1))
    }
    
    /// Parse export method
    fn parse_export_method(&self, lines: &[&str], start: usize) -> Result<(ExportMethod, usize)> {
        let line = lines[start].trim();
        
        let name = line.strip_prefix("function ")
            .ok_or_else(|| ChtlError::semantic("Invalid function syntax"))?
            .split('(')
            .next()
            .unwrap()
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
        
        let method = ExportMethod {
            name,
            parameters,
            return_type: "any".to_string(),
            body,
        };
        
        Ok((method, 1))
    }
    
    /// Parse function
    fn parse_function(&self, lines: &[&str], start: usize) -> Result<(ChtlJsFunction, usize)> {
        let line = lines[start].trim();
        
        let name = line.strip_prefix("function ")
            .ok_or_else(|| ChtlError::semantic("Invalid function syntax"))?
            .split('(')
            .next()
            .unwrap()
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
        
        let function = ChtlJsFunction {
            name,
            parameters,
            return_type: "any".to_string(),
            body,
            is_async: line.contains("async"),
            is_generator: line.contains("function*"),
        };
        
        Ok((function, 1))
    }
    
    /// Extract module info
    fn extract_module_info(&self, content: &ModuleContent) -> Result<ModuleInfo> {
        match content {
            ModuleContent::Cmod(ast) => {
                self.extract_info_from_ast(ast)
            }
            ModuleContent::Cjmod(cjmod) => {
                // Extract info from CJMOD content
                Ok(ModuleInfo {
                    name: "unknown".to_string(),
                    version: "1.0.0".to_string(),
                    author: None,
                    description: None,
                    license: None,
                    homepage: None,
                    repository: None,
                    keywords: vec![],
                    created_at: chrono::Utc::now().to_rfc3339(),
                    updated_at: chrono::Utc::now().to_rfc3339(),
                })
            }
        }
    }
    
    /// Extract info from AST
    fn extract_info_from_ast(&self, ast: &[AstNode]) -> Result<ModuleInfo> {
        let mut info = ModuleInfo {
            name: "unknown".to_string(),
            version: "1.0.0".to_string(),
            author: None,
            description: None,
            license: None,
            homepage: None,
            repository: None,
            keywords: vec![],
            created_at: chrono::Utc::now().to_rfc3339(),
            updated_at: chrono::Utc::now().to_rfc3339(),
        };
        
        for node in ast {
            if let AstNode::Configuration(config) = node {
                for name_config in config.name_config.iter() {
                    match name_config.name.as_str() {
                                "name" => info.name = name_config.value.clone(),
                                "version" => info.version = name_config.value.clone(),
                                "author" => info.author = Some(name_config.value.clone()),
                                "description" => info.description = Some(name_config.value.clone()),
                                "license" => info.license = Some(name_config.value.clone()),
                                "homepage" => info.homepage = Some(name_config.value.clone()),
                                "repository" => info.repository = Some(name_config.value.clone()),
                                _ => {}
                            }
                }
            }
        }
        
        Ok(info)
    }
    
    /// Extract exports
    fn extract_exports(&self, content: &ModuleContent) -> Result<ExportTable> {
        let mut exports = ExportTable {
            functions: HashMap::new(),
            variables: HashMap::new(),
            classes: HashMap::new(),
            modules: HashMap::new(),
        };
        
        match content {
            ModuleContent::Cmod(ast) => {
                self.extract_exports_from_ast(ast, &mut exports)?;
            }
            ModuleContent::Cjmod(cjmod) => {
                self.extract_exports_from_cjmod(cjmod, &mut exports)?;
            }
        }
        
        Ok(exports)
    }
    
    /// Extract exports from AST
    fn extract_exports_from_ast(&self, ast: &[AstNode], exports: &mut ExportTable) -> Result<()> {
        for node in ast {
            if let AstNode::Template(template) = node {
                let export_func = ExportFunction {
                    name: template.name.clone(),
                    parameters: template.parameters.clone(),
                    return_type: "Template".to_string(),
                    description: None,
                    is_async: false,
                    is_generator: false,
                };
                exports.functions.insert(template.name.clone(), export_func);
            } else if let AstNode::Custom(custom) = node {
                let export_func = ExportFunction {
                    name: custom.name.clone(),
                    parameters: vec![],
                    return_type: "Custom".to_string(),
                    description: None,
                    is_async: false,
                    is_generator: false,
                };
                exports.functions.insert(custom.name.clone(), export_func);
            }
        }
        
        Ok(())
    }
    
    /// Extract exports from CJMOD
    fn extract_exports_from_cjmod(&self, cjmod: &CjmodContent, exports: &mut ExportTable) -> Result<()> {
        // Export Syntax class methods
        for (name, method) in &cjmod.syntax.methods {
            let export_func = ExportFunction {
                name: name.clone(),
                parameters: method.parameters.clone(),
                return_type: method.return_type.clone(),
                description: None,
                is_async: false,
                is_generator: false,
            };
            exports.functions.insert(name.clone(), export_func);
        }
        
        // Export Arg class methods
        for (name, method) in &cjmod.arg.methods {
            let export_func = ExportFunction {
                name: name.clone(),
                parameters: method.parameters.clone(),
                return_type: method.return_type.clone(),
                description: None,
                is_async: false,
                is_generator: false,
            };
            exports.functions.insert(name.clone(), export_func);
        }
        
        // Export functions
        for (name, function) in &cjmod.functions {
            let export_func = ExportFunction {
                name: name.clone(),
                parameters: function.parameters.clone(),
                return_type: function.return_type.clone(),
                description: None,
                is_async: function.is_async,
                is_generator: function.is_generator,
            };
            exports.functions.insert(name.clone(), export_func);
        }
        
        Ok(())
    }
    
    /// Extract dependencies
    fn extract_dependencies(&self, content: &ModuleContent) -> Result<Vec<ModuleDependency>> {
        let mut dependencies = Vec::new();
        
        match content {
            ModuleContent::Cmod(ast) => {
                self.extract_dependencies_from_ast(ast, &mut dependencies)?;
            }
            ModuleContent::Cjmod(_) => {
                // CJMOD dependencies would be extracted from the content
            }
        }
        
        Ok(dependencies)
    }
    
    /// Extract dependencies from AST
    fn extract_dependencies_from_ast(&self, ast: &[AstNode], dependencies: &mut Vec<ModuleDependency>) -> Result<()> {
        for node in ast {
            if let AstNode::Import(import) = node {
                let dependency = ModuleDependency {
                    name: import.path.clone(),
                    version: "latest".to_string(),
                    dependency_type: DependencyType::User,
                    path: Some(import.path.clone()),
                };
                dependencies.push(dependency);
            }
        }
        
        Ok(())
    }
    
    /// Get module by name
    pub fn get_module(&self, name: &str) -> Option<&ModuleDefinition> {
        self.modules.get(name)
    }
    
    /// Get module path
    pub fn get_module_path(&self, name: &str) -> Option<&PathBuf> {
        self.module_paths.get(name)
    }
    
    /// Get export table
    pub fn get_export_table(&self, name: &str) -> Option<&ExportTable> {
        self.export_tables.get(name)
    }
    
    /// List all modules
    pub fn list_modules(&self) -> Vec<&ModuleDefinition> {
        self.modules.values().collect()
    }
    
    /// Resolve module dependencies
    pub fn resolve_dependencies(&mut self, module_name: &str) -> Result<Vec<ModuleDefinition>> {
        let mut resolved = Vec::new();
        let mut to_resolve = vec![module_name.to_string()];
        let mut resolved_names = std::collections::HashSet::new();
        
        while let Some(name) = to_resolve.pop() {
            if resolved_names.contains(&name) {
                continue;
            }
            
            let module = self.get_module(&name)
                .ok_or_else(|| ChtlError::semantic(&format!("Module '{}' not found", name)))?;
            
            resolved.push(module.clone());
            resolved_names.insert(name.clone());
            
            for dependency in &module.dependencies {
                if !resolved_names.contains(&dependency.name) {
                    to_resolve.push(dependency.name.clone());
                }
            }
        }
        
        Ok(resolved)
    }
    
    /// Generate module bundle
    pub fn generate_bundle(&self, module_name: &str) -> Result<String> {
        let module = self.get_module(module_name)
            .ok_or_else(|| ChtlError::semantic(&format!("Module '{}' not found", module_name)))?;
        
        let mut bundle = String::new();
        
        // Add module header
        bundle.push_str(&format!("// Module: {}\n", module.name));
        bundle.push_str(&format!("// Version: {}\n", module.info.version));
        if let Some(description) = &module.info.description {
            bundle.push_str(&format!("// Description: {}\n", description));
        }
        bundle.push_str("\n");
        
        // Add module content
        match &module.content {
            ModuleContent::Cmod(ast) => {
                bundle.push_str("// CMOD Content\n");
                // Generate content from AST
                for node in ast {
                    bundle.push_str(&format!("// {}\n", node));
                }
            }
            ModuleContent::Cjmod(cjmod) => {
                bundle.push_str("// CJMOD Content\n");
                bundle.push_str("// Syntax class\n");
                for (name, method) in &cjmod.syntax.methods {
                    bundle.push_str(&format!("// {}\n", method.body));
                }
                bundle.push_str("// Arg class\n");
                for (name, method) in &cjmod.arg.methods {
                    bundle.push_str(&format!("// {}\n", method.body));
                }
                bundle.push_str("// Functions\n");
                for (name, function) in &cjmod.functions {
                    bundle.push_str(&format!("// {}\n", function.body));
                }
            }
        }
        
        // Add exports
        bundle.push_str("\n// Exports\n");
        for (name, func) in &module.exports.functions {
            bundle.push_str(&format!("export function {}() {{}}\n", name));
        }
        
        Ok(bundle)
    }
}