//! CMOD system
//! 
//! This module provides CMOD (CHTL Module) system functionality for CHTL,
//! including CMOD parsing, validation, and management.

use super::*;
use std::collections::HashMap;
use std::path::PathBuf;

/// CMOD system
#[derive(Debug, Clone)]
pub struct CMODSystem {
    /// CMOD modules
    modules: HashMap<String, CMODModule>,
    /// CMOD parser
    parser: CMODParser,
    /// CMOD validator
    validator: CMODValidator,
    /// CMOD generator
    generator: CMODGenerator,
}

/// CMOD module
#[derive(Debug, Clone)]
pub struct CMODModule {
    /// Module name
    pub name: String,
    /// Module version
    pub version: String,
    /// Module path
    pub path: PathBuf,
    /// Module info
    pub info: CMODInfo,
    /// Module exports
    pub exports: Vec<CMODExport>,
    /// Module content
    pub content: String,
    /// Module dependencies
    pub dependencies: Vec<String>,
}

/// CMOD info
#[derive(Debug, Clone)]
pub struct CMODInfo {
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

/// CMOD export
#[derive(Debug, Clone)]
pub struct CMODExport {
    /// Export name
    pub name: String,
    /// Export type
    pub export_type: CMODExportType,
    /// Export description
    pub description: String,
    /// Export parameters
    pub parameters: Vec<String>,
    /// Export return type
    pub return_type: String,
    /// Export implementation
    pub implementation: String,
}

/// CMOD export type
#[derive(Debug, Clone, PartialEq)]
pub enum CMODExportType {
    Function,
    Class,
    Variable,
    Constant,
    Type,
    Interface,
    Namespace,
}

/// CMOD parser
#[derive(Debug, Clone)]
pub struct CMODParser {
    /// Parser options
    options: CMODParserOptions,
}

/// CMOD parser options
#[derive(Debug, Clone)]
pub struct CMODParserOptions {
    /// Strict mode
    pub strict: bool,
    /// Validate syntax
    pub validate_syntax: bool,
    /// Parse comments
    pub parse_comments: bool,
    /// Parse metadata
    pub parse_metadata: bool,
}

/// CMOD validator
#[derive(Debug, Clone)]
pub struct CMODValidator {
    /// Validation rules
    rules: Vec<CMODValidationRule>,
}

/// CMOD validation rule
#[derive(Debug, Clone)]
pub struct CMODValidationRule {
    /// Rule name
    pub name: String,
    /// Rule description
    pub description: String,
    /// Rule function
    pub rule_function: fn(&CMODModule) -> bool,
}

/// CMOD generator
#[derive(Debug, Clone)]
pub struct CMODGenerator {
    /// Generator options
    options: CMODGeneratorOptions,
}

/// CMOD generator options
#[derive(Debug, Clone)]
pub struct CMODGeneratorOptions {
    /// Include metadata
    pub include_metadata: bool,
    /// Include comments
    pub include_comments: bool,
    /// Format output
    pub format: bool,
    /// Minify output
    pub minify: bool,
}

/// CMOD parsing result
#[derive(Debug, Clone)]
pub struct CMODParsingResult {
    /// Success status
    pub success: bool,
    /// Parsed module
    pub module: Option<CMODModule>,
    /// Warnings
    pub warnings: Vec<String>,
    /// Errors
    pub errors: Vec<String>,
}

/// CMOD validation result
#[derive(Debug, Clone)]
pub struct CMODValidationResult {
    /// Success status
    pub success: bool,
    /// Validation errors
    pub errors: Vec<String>,
    /// Validation warnings
    pub warnings: Vec<String>,
}

/// CMOD generation result
#[derive(Debug, Clone)]
pub struct CMODGenerationResult {
    /// Generated CMOD
    pub cmod: String,
    /// Generation warnings
    pub warnings: Vec<String>,
    /// Generation errors
    pub errors: Vec<String>,
}

/// CMOD error
#[derive(Debug, Clone)]
pub struct CMODError {
    pub message: String,
    pub module_name: String,
    pub suggestion: Option<String>,
}

impl CMODSystem {
    /// Create a new CMOD system
    pub fn new() -> Self {
        Self {
            modules: HashMap::new(),
            parser: CMODParser::new(),
            validator: CMODValidator::new(),
            generator: CMODGenerator::new(),
        }
    }
    
    /// Create a new CMOD system with options
    pub fn with_options(parser_options: CMODParserOptions, generator_options: CMODGeneratorOptions) -> Self {
        Self {
            modules: HashMap::new(),
            parser: CMODParser::with_options(parser_options),
            validator: CMODValidator::new(),
            generator: CMODGenerator::with_options(generator_options),
        }
    }
    
    /// Parse CMOD from file
    pub fn parse_file(&mut self, file_path: &PathBuf) -> Result<CMODParsingResult, CMODError> {
        let content = std::fs::read_to_string(file_path)
            .map_err(|e| CMODError {
                message: format!("Failed to read file: {}", e),
                module_name: file_path.to_string_lossy().to_string(),
                suggestion: Some("Check file path and permissions".to_string()),
            })?;
        
        self.parse_content(&content, file_path)
    }
    
    /// Parse CMOD from content
    pub fn parse_content(&mut self, content: &str, file_path: &PathBuf) -> Result<CMODParsingResult, CMODError> {
        let mut result = CMODParsingResult {
            success: false,
            module: None,
            warnings: Vec::new(),
            errors: Vec::new(),
        };
        
        // Parse CMOD content
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
    
    /// Generate CMOD from module
    pub fn generate_cmod(&self, module: &CMODModule) -> Result<CMODGenerationResult, CMODError> {
        self.generator.generate(module)
    }
    
    /// Add CMOD module
    pub fn add_module(&mut self, module: CMODModule) {
        self.modules.insert(module.name.clone(), module);
    }
    
    /// Get CMOD module
    pub fn get_module(&self, name: &str) -> Option<&CMODModule> {
        self.modules.get(name)
    }
    
    /// Get all CMOD modules
    pub fn get_modules(&self) -> &HashMap<String, CMODModule> {
        &self.modules
    }
    
    /// Check if CMOD module exists
    pub fn has_module(&self, name: &str) -> bool {
        self.modules.contains_key(name)
    }
    
    /// Remove CMOD module
    pub fn remove_module(&mut self, name: &str) -> Option<CMODModule> {
        self.modules.remove(name)
    }
    
    /// Clear all CMOD modules
    pub fn clear_modules(&mut self) {
        self.modules.clear();
    }
}

impl CMODParser {
    /// Create a new CMOD parser
    pub fn new() -> Self {
        Self {
            options: CMODParserOptions::default(),
        }
    }
    
    /// Create a new CMOD parser with options
    pub fn with_options(options: CMODParserOptions) -> Self {
        Self { options }
    }
    
    /// Parse CMOD content
    pub fn parse(&self, content: &str, file_path: &PathBuf) -> Result<CMODModule, CMODError> {
        let mut module = CMODModule {
            name: String::new(),
            version: String::new(),
            path: file_path.clone(),
            info: CMODInfo::default(),
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
    fn parse_module_info(&self, module: &mut CMODModule, content: &str) -> Result<(), CMODError> {
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
    fn parse_info_section(&self, module: &mut CMODModule, content: &str) -> Result<(), CMODError> {
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
    fn parse_exports(&self, module: &mut CMODModule, content: &str) -> Result<(), CMODError> {
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
    fn parse_dependencies(&self, module: &mut CMODModule, content: &str) -> Result<(), CMODError> {
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
    fn parse_export_line(&self, line: &str) -> Option<CMODExport> {
        let parts: Vec<&str> = line.split_whitespace().collect();
        if parts.len() >= 2 {
            let name = parts[1].to_string();
            let export_type = self.detect_export_type(line);
            
            return Some(CMODExport {
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
    fn detect_export_type(&self, line: &str) -> CMODExportType {
        if line.contains("function") {
            CMODExportType::Function
        } else if line.contains("class") {
            CMODExportType::Class
        } else if line.contains("const") {
            CMODExportType::Constant
        } else if line.contains("type") {
            CMODExportType::Type
        } else if line.contains("interface") {
            CMODExportType::Interface
        } else if line.contains("namespace") {
            CMODExportType::Namespace
        } else {
            CMODExportType::Variable
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

impl CMODValidator {
    /// Create a new CMOD validator
    pub fn new() -> Self {
        let mut validator = Self {
            rules: Vec::new(),
        };
        
        // Add default validation rules
        validator.add_rule(CMODValidationRule {
            name: "name_required".to_string(),
            description: "Module name is required".to_string(),
            rule_function: |module| !module.info.name.is_empty(),
        });
        
        validator.add_rule(CMODValidationRule {
            name: "version_required".to_string(),
            description: "Module version is required".to_string(),
            rule_function: |module| !module.info.version.is_empty(),
        });
        
        validator.add_rule(CMODValidationRule {
            name: "exports_required".to_string(),
            description: "Module must have at least one export".to_string(),
            rule_function: |module| !module.exports.is_empty(),
        });
        
        validator
    }
    
    /// Add validation rule
    pub fn add_rule(&mut self, rule: CMODValidationRule) {
        self.rules.push(rule);
    }
    
    /// Validate CMOD module
    pub fn validate(&self, module: &CMODModule) -> CMODValidationResult {
        let mut errors = Vec::new();
        let mut warnings = Vec::new();
        
        for rule in &self.rules {
            if !(rule.rule_function)(module) {
                errors.push(format!("Validation failed: {}", rule.description));
            }
        }
        
        CMODValidationResult {
            success: errors.is_empty(),
            errors,
            warnings,
        }
    }
}

impl CMODGenerator {
    /// Create a new CMOD generator
    pub fn new() -> Self {
        Self {
            options: CMODGeneratorOptions::default(),
        }
    }
    
    /// Create a new CMOD generator with options
    pub fn with_options(options: CMODGeneratorOptions) -> Self {
        Self { options }
    }
    
    /// Generate CMOD from module
    pub fn generate(&self, module: &CMODModule) -> Result<CMODGenerationResult, CMODError> {
        let mut cmod = String::new();
        
        // Add module info
        if self.options.include_metadata {
            cmod.push_str("[Info]\n");
            cmod.push_str(&format!("name = {}\n", module.info.name));
            cmod.push_str(&format!("version = {}\n", module.info.version));
            cmod.push_str(&format!("description = {}\n", module.info.description));
            cmod.push_str(&format!("author = {}\n", module.info.author));
            cmod.push_str(&format!("license = {}\n", module.info.license));
            cmod.push_str(&format!("homepage = {}\n", module.info.homepage));
            cmod.push_str(&format!("repository = {}\n", module.info.repository));
            cmod.push_str(&format!("keywords = {}\n", module.info.keywords.join(", ")));
            cmod.push_str(&format!("categories = {}\n", module.info.categories.join(", ")));
            cmod.push_str("\n");
        }
        
        // Add exports
        if !module.exports.is_empty() {
            cmod.push_str("[Export]\n");
            for export in &module.exports {
                cmod.push_str(&format!("export {} {}\n", export.export_type, export.name));
            }
            cmod.push_str("\n");
        }
        
        // Add dependencies
        if !module.dependencies.is_empty() {
            for dependency in &module.dependencies {
                cmod.push_str(&format!("import {}\n", dependency));
            }
            cmod.push_str("\n");
        }
        
        // Add module content
        cmod.push_str(&module.content);
        
        Ok(CMODGenerationResult {
            cmod,
            warnings: Vec::new(),
            errors: Vec::new(),
        })
    }
}

impl Default for CMODSystem {
    fn default() -> Self {
        Self::new()
    }
}

impl Default for CMODInfo {
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

impl Default for CMODParserOptions {
    fn default() -> Self {
        Self {
            strict: false,
            validate_syntax: true,
            parse_comments: true,
            parse_metadata: true,
        }
    }
}

impl Default for CMODGeneratorOptions {
    fn default() -> Self {
        Self {
            include_metadata: true,
            include_comments: true,
            format: true,
            minify: false,
        }
    }
}