//! CJMOD system
//! 
//! This module provides CJMOD (CHTL JS Module) system functionality for CHTL,
//! including CJMOD parsing, validation, and management.

use super::*;
use std::collections::HashMap;
use std::path::PathBuf;

/// CJMOD system
#[derive(Debug, Clone)]
pub struct CJMODSystem {
    /// CJMOD modules
    modules: HashMap<String, CJMODModule>,
    /// CJMOD parser
    parser: CJMODParser,
    /// CJMOD validator
    validator: CJMODValidator,
    /// CJMOD generator
    generator: CJMODGenerator,
}

/// CJMOD module
#[derive(Debug, Clone)]
pub struct CJMODModule {
    /// Module name
    pub name: String,
    /// Module version
    pub version: String,
    /// Module path
    pub path: PathBuf,
    /// Module info
    pub info: CJMODInfo,
    /// Module exports
    pub exports: Vec<CJMODExport>,
    /// Module content
    pub content: String,
    /// Module dependencies
    pub dependencies: Vec<String>,
}

/// CJMOD info
#[derive(Debug, Clone)]
pub struct CJMODInfo {
    /// Module name
    pub name: String,
    /// Module version
    pub version: String,
    /// Module description
    pub description: String,
    /// Module author
    pub author: String,
    /// Module license
    pub license: String,
    /// Module homepage
    pub homepage: String,
    /// Module repository
    pub repository: String,
    /// Module keywords
    pub keywords: Vec<String>,
    /// Module categories
    pub categories: Vec<String>,
}

/// CJMOD export
#[derive(Debug, Clone)]
pub struct CJMODExport {
    /// Export name
    pub name: String,
    /// Export type
    pub export_type: CJMODExportType,
    /// Export description
    pub description: String,
    /// Export parameters
    pub parameters: Vec<String>,
    /// Export return type
    pub return_type: String,
    /// Export implementation
    pub implementation: String,
}

/// CJMOD export type
#[derive(Debug, Clone, PartialEq)]
pub enum CJMODExportType {
    Function,
    Class,
    Variable,
    Constant,
    Type,
    Interface,
    Namespace,
    Module,
}

/// CJMOD parser
#[derive(Debug, Clone)]
pub struct CJMODParser {
    /// Parser options
    options: CJMODParserOptions,
}

/// CJMOD parser options
#[derive(Debug, Clone)]
pub struct CJMODParserOptions {
    /// Strict mode
    pub strict: bool,
    /// Validate syntax
    pub validate_syntax: bool,
    /// Parse comments
    pub parse_comments: bool,
    /// Parse metadata
    pub parse_metadata: bool,
}

/// CJMOD validator
#[derive(Debug, Clone)]
pub struct CJMODValidator {
    /// Validation rules
    rules: Vec<CJMODValidationRule>,
}

/// CJMOD validation rule
#[derive(Debug, Clone)]
pub struct CJMODValidationRule {
    /// Rule name
    pub name: String,
    /// Rule description
    pub description: String,
    /// Rule function
    pub rule_function: fn(&CJMODModule) -> bool,
}

/// CJMOD generator
#[derive(Debug, Clone)]
pub struct CJMODGenerator {
    /// Generator options
    options: CJMODGeneratorOptions,
}

/// CJMOD generator options
#[derive(Debug, Clone)]
pub struct CJMODGeneratorOptions {
    /// Include metadata
    pub include_metadata: bool,
    /// Include comments
    pub include_comments: bool,
    /// Format output
    pub format: bool,
    /// Minify output
    pub minify: bool,
}

/// CJMOD parsing result
#[derive(Debug, Clone)]
pub struct CJMODParsingResult {
    /// Success status
    pub success: bool,
    /// Parsed module
    pub module: Option<CJMODModule>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
}

/// CJMOD validation result
#[derive(Debug, Clone)]
pub struct CJMODValidationResult {
    /// Success status
    pub success: bool,
    /// Validation errors
    pub errors: Vec<String>,
    /// Validation warnings
    pub warnings: Vec<String>,
}

/// CJMOD generation result
#[derive(Debug, Clone)]
pub struct CJMODGenerationResult {
    /// Generated CJMOD
    pub cjmod: String,
    /// Generation warnings
    pub warnings: Vec<String>,
    /// Generation errors
    pub errors: Vec<String>,
}

/// CJMOD error
#[derive(Debug, Clone)]
pub struct CJMODError {
    pub message: String,
    pub module_name: String,
    pub suggestion: Option<String>,
}

impl CJMODSystem {
    /// Create a new CJMOD system
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            parser: CJMODParser::new(),
            validator: CJMODValidator::new(),
            generator: CJMODGenerator::new(),
        }
    }
    
    /// Create a new CJMOD system with options
    pub fn with_options(parser_options: CJMODParserOptions, generator_options: CJMODGeneratorOptions) -> Self {
        Self {
            modules: HashMap::new(),
            parser: CJMODParser::with_options(parser_options),
            validator: CJMODValidator::new(),
            generator: CJMODGenerator::with_options(generator_options),
        }
    }
    
    /// Parse CJMOD from file
    pub fn parse_file(&mut self, file_path: &PathBuf) -> Result<CJMODParsingResult, CJMODError> {
        let content = std::fs::read_to_string(file_path)
            .map_err(|e| CJMODError {
                message: format!("Failed to read file: {}", e),
                module_name: file_path.to_string_lossy().to_string(),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        self.parse_content(&content, file_path)
    }
    
    /// Parse CJMOD from content
    pub fn parse_content(&mut self, content: &str, file_path: &PathBuf) -> Result<CJMODParsingResult, CJMODError> {
        let mut result = CJMODParsingResult {
            success: false,
            module: None,
            warnings: Vec::new(),
            errors: Vec::new(),
        };
        
        // Parse CJMOD content
        let module = self.parser.parse(content, file_path)?;
        
        // Validate module
        let validation_result = self.validator.validate(&module);
        if !validation_result.success {
            result.errors.extend(validation_result.errors);
            return Ok(result);
        }
        
        result.warnings.extend(validation_result.warnings);
        result.module = Some(module);
        result.success = true;
        
        Ok(result)
    }
    
    /// Generate CJMOD from module
    pub fn generate_cjmod(&self, module: &CJMODModule) -> Result<CJMODGenerationResult, CJMODError> {
        self.generator.generate(module)
    }
    
    /// Add CJMOD module
    pub fn add_module(&mut self, module: CJMODModule) {
        self.modules.insert(module.name.clone(), module);
    }
    
    /// Get CJMOD module
    pub fn get_module(&self, name: &str) -> Option<&CJMODModule> {
        self.modules.get(name)
    }
    
    /// Get all CJMOD modules
    pub fn get_modules(&self) -> &HashMap<String, CJMODModule> {
        &self.modules
    }
    
    /// Check if CJMOD module exists
    pub fn has_module(&self, name: &str) -> bool {
        self.modules.contains_key(name)
    }
    
    /// Remove CJMOD module
    pub fn remove_module(&mut self, name: &str) -> Option<CJMODModule> {
        self.modules.remove(name)
    }
    
    /// Clear all CJMOD modules
    pub fn clear_modules(&mut self) {
        self.modules.clear();
    }
}

impl CJMODParser {
    /// Create a new CJMOD parser
    pub fn new() -> Self {
        Self {
            options: CJMODParserOptions::default(),
        }
    }
    
    /// Create a new CJMOD parser with options
    pub fn with_options(options: CJMODParserOptions) -> Self {
        Self { options }
    }
    
    /// Parse CJMOD content
    pub fn parse(&self, content: &str, file_path: &PathBuf) -> Result<CJMODModule, CJMODError> {
        let mut module = CJMODModule {
            name: String::new(),
            version: String::new(),
            path: file_path.clone(),
            info: CJMODInfo::default(),
            exports: Vec::new(),
            content: content.to_string(),
            dependencies: Vec::new(),
        };
        
        // Parse module info
        self.parse_module_info(&mut module, content)?;
        
        // Parse exports
        self.parse_exports(&mut module, content)?;
        
        // Parse dependencies
        self.parse_dependencies(&mut module, content)?;
        
        Ok(module)
    }
    
    /// Parse module info
    fn parse_module_info(&self, module: &mut CJMODModule, content: &str) -> Result<(), CJMODError> {
        for line in content.lines() {
            let line = line.trim();
            
            if line.starts_with("[Info]") {
                // Parse info section
                self.parse_info_section(module, content)?;
                break;
            }
        }
        
        Ok(())
    }
    
    /// Parse info section
    fn parse_info_section(&self, module: &mut CJMODModule, content: &str) -> Result<(), CJMODError> {
        let mut in_info_section = false;
        
        for line in content.lines() {
            let line = line.trim();
            
            if line.starts_with("[Info]") {
                in_info_section = true;
                continue;
            }
            
            if line.starts_with("[") && !line.starts_with("[Info]") {
                in_info_section = false;
                continue;
            }
            
            if in_info_section {
                if let Some((key, value)) = self.parse_key_value(line) {
                    match key.as_str() {
                        "name" => module.info.name = value,
                        "version" => module.info.version = value,
                        "description" => module.info.description = value,
                        "author" => module.info.author = value,
                        "license" => module.info.license = value,
                        "homepage" => module.info.homepage = value,
                        "repository" => module.info.repository = value,
                        "keywords" => module.info.keywords = value.split(',').map(|s| s.trim().to_string()).collect(),
                        "categories" => module.info.categories = value.split(',').map(|s| s.trim().to_string()).collect(),
                        _ => {}
                    }
                }
            }
        }
        
        // Set module name and version
        module.name = module.info.name.clone();
        module.version = module.info.version.clone();
        
        Ok(())
    }
    
    /// Parse exports
    fn parse_exports(&self, module: &mut CJMODModule, content: &str) -> Result<(), CJMODError> {
        let mut in_export_section = false;
        
        for line in content.lines() {
            let line = line.trim();
            
            if line.starts_with("[Export]") {
                in_export_section = true;
                continue;
            }
            
            if line.starts_with("[") && !line.starts_with("[Export]") {
                in_export_section = false;
                continue;
            }
            
            if in_export_section {
                if let Some(export) = self.parse_export_line(line) {
                    module.exports.push(export);
                }
            }
        }
        
        Ok(())
    }
    
    /// Parse dependencies
    fn parse_dependencies(&self, module: &mut CJMODModule, content: &str) -> Result<(), CJMODError> {
        for line in content.lines() {
            let line = line.trim();
            
            if line.starts_with("import ") {
                if let Some(dependency) = self.extract_dependency(line) {
                    module.dependencies.push(dependency);
                }
            }
        }
        
        Ok(())
    }
    
    /// Parse key-value pair
    fn parse_key_value(&self, line: &str) -> Option<(String, String)> {
        if let Some(eq_pos) = line.find('=') {
            let key = line[..eq_pos].trim().to_string();
            let value = line[eq_pos + 1..].trim().to_string();
            return Some((key, value));
        }
        None
    }
    
    /// Parse export line
    fn parse_export_line(&self, line: &str) -> Option<CJMODExport> {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 2 {
            let name = parts[1].to_string();
            let export_type = self.detect_export_type(line);
            
            return Some(CJMODExport {
                name,
                export_type,
                description: String::new(),
                parameters: Vec::new(),
                return_type: String::new(),
                implementation: String::new(),
            });
        }
        None
    }
    
    /// Detect export type
    fn detect_export_type(&self, line: &str) -> CJMODExportType {
        if line.contains("function") {
            CJMODExportType::Function
        } else if line.contains("class") {
            CJMODExportType::Class
        } else if line.contains("const") {
            CJMODExportType::Constant
        } else if line.contains("type") {
            CJMODExportType::Type
        } else if line.contains("interface") {
            CJMODExportType::Interface
        } else if line.contains("namespace") {
            CJMODExportType::Namespace
        } else if line.contains("module") {
            CJMODExportType::Module
        } else {
            CJMODExportType::Variable
        }
    }
    
    /// Extract dependency
    fn extract_dependency(&self, line: &str) -> Option<String> {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 2 {
            return Some(parts[1].to_string());
        }
        None
    }
}

impl CJMODValidator {
    /// Create a new CJMOD validator
    pub fn new() -> Self {
        let mut validator = Self {
            rules: Vec::new(),
        };
        
        // Add default validation rules
        validator.add_rule(CJMODValidationRule {
            name: "name_required".to_string(),
            description: "Module name is required".to_string(),
            rule_function: |module| !module.info.name.is_empty(),
        });
        
        validator.add_rule(CJMODValidationRule {
            name: "version_required".to_string(),
            description: "Module version is required".to_string(),
            rule_function: |module| !module.info.version.is_empty(),
        });
        
        validator.add_rule(CJMODValidationRule {
            name: "exports_required".to_string(),
            description: "Module must have at least one export".to_string(),
            rule_function: |module| !module.exports.is_empty(),
        });
        
        validator
    }
    
    /// Add validation rule
    pub fn add_rule(&mut self, rule: CJMODValidationRule) {
        self.rules.push(rule);
    }
    
    /// Validate CJMOD module
    pub fn validate(&self, module: &CJMODModule) -> CJMODValidationResult {
        let mut errors = Vec::new();
        let mut warnings = Vec::new();
        
        for rule in &self.rules {
            if !(rule.rule_function)(module) {
                errors.push(format!("Validation failed: {}", rule.description));
            }
        }
        
        CJMODValidationResult {
            success: errors.is_empty(),
            errors,
            warnings,
        }
    }
}

impl CJMODGenerator {
    /// Create a new CJMOD generator
    pub fn new() -> Self {
        Self {
            options: CJMODGeneratorOptions::default(),
        }
    }
    
    /// Create a new CJMOD generator with options
    pub fn with_options(options: CJMODGeneratorOptions) -> Self {
        Self { options }
    }
    
    /// Generate CJMOD from module
    pub fn generate(&self, module: &CJMODModule) -> Result<CJMODGenerationResult, CJMODError> {
        let mut cjmod = String::new();
        
        // Add module info
        if self.options.include_metadata {
            cjmod.push_str("[Info]\n");
            cjmod.push_str(&format!("name = {}\n", module.info.name));
            cjmod.push_str(&format!("version = {}\n", module.info.version));
            cjmod.push_str(&format!("description = {}\n", module.info.description));
            cjmod.push_str(&format!("author = {}\n", module.info.author));
            cjmod.push_str(&format!("license = {}\n", module.info.license));
            cjmod.push_str(&format!("homepage = {}\n", module.info.homepage));
            cjmod.push_str(&format!("repository = {}\n", module.info.repository));
            cjmod.push_str(&format!("keywords = {}\n", module.info.keywords.join(", ")));
            cjmod.push_str(&format!("categories = {}\n", module.info.categories.join(", ")));
            cjmod.push_str("\n");
        }
        
        // Add exports
        if !module.exports.is_empty() {
            cjmod.push_str("[Export]\n");
            for export in &module.exports {
                cjmod.push_str(&format!("export {} {}\n", export.export_type, export.name));
            }
            cjmod.push_str("\n");
        }
        
        // Add dependencies
        if !module.dependencies.is_empty() {
            for dependency in &module.dependencies {
                cjmod.push_str(&format!("import {}\n", dependency));
            }
            cjmod.push_str("\n");
        }
        
        // Add module content
        cjmod.push_str(&module.content);
        
        Ok(CJMODGenerationResult {
            cjmod,
            warnings: Vec::new(),
            errors: Vec::new(),
        })
    }
}

impl Default for CJMODSystem {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CJMODInfo {
    fn default() -> Self {
        Self {
            name: String::new(),
            version: String::new(),
            description: String::new(),
            author: String::new(),
            license: String::new(),
            homepage: String::new(),
            repository: String::new(),
            keywords: Vec::new(),
            categories: Vec::new(),
        }
    }
}

impl Default for CJMODParserOptions {
    fn default() -> Self {
        Self {
            strict: false,
            validate_syntax: true,
            parse_comments: true,
            parse_metadata: true,
        }
    }
}

impl Default for CJMODGeneratorOptions {
    fn default() -> Self {
        Self {
            include_metadata: true,
            include_comments: true,
            format: true,
            minify: false,
        }
    }
}