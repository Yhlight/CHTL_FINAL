//! Import and namespace processor for CHTL

use crate::ast_enum::*;
use crate::error::{ChtlError, Result};
use std::collections::HashMap;
use std::path::{Path, PathBuf};

/// Import and namespace processor
pub struct ImportNamespaceProcessor {
    imports: HashMap<String, ImportDefinition>,
    namespaces: HashMap<String, NamespaceDefinition>,
    module_resolver: ModuleResolver,
}

/// Import definition
#[derive(Debug, Clone)]
pub struct ImportDefinition {
    pub path: String,
    pub import_type: ImportType,
    pub imports: Vec<ImportItem>,
    pub alias: Option<String>,
    pub is_wildcard: bool,
    pub is_precise: bool,
    pub metadata: ImportMetadata,
}

/// Import type
#[derive(Debug, Clone, PartialEq)]
pub enum ImportType {
    Chtl,
    Html,
    Css,
    JavaScript,
    Module,
}

/// Import item
#[derive(Debug, Clone)]
pub struct ImportItem {
    pub name: String,
    pub alias: Option<String>,
    pub is_default: bool,
    pub is_namespace: bool,
    pub item_type: ImportItemType,
}

/// Import item type
#[derive(Debug, Clone, PartialEq)]
pub enum ImportItemType {
    Function,
    Variable,
    Class,
    Module,
    Template,
    Custom,
    Style,
    Script,
}

/// Import metadata
#[derive(Debug, Clone)]
pub struct ImportMetadata {
    pub resolved_path: Option<PathBuf>,
    pub file_size: Option<u64>,
    pub last_modified: Option<String>,
    pub checksum: Option<String>,
}

/// Namespace definition
#[derive(Debug, Clone)]
pub struct NamespaceDefinition {
    pub name: String,
    pub content: Vec<AstNode>,
    pub imports: Vec<String>,
    pub exports: Vec<String>,
    pub parent: Option<String>,
    pub children: Vec<String>,
    pub metadata: NamespaceMetadata,
}

/// Namespace metadata
#[derive(Debug, Clone)]
pub struct NamespaceMetadata {
    pub created_at: String,
    pub updated_at: String,
    pub version: String,
    pub description: Option<String>,
}

/// Module resolver
#[derive(Debug, Clone)]
pub struct ModuleResolver {
    search_paths: Vec<PathBuf>,
    module_cache: HashMap<String, ResolvedModule>,
    alias_map: HashMap<String, String>,
}

/// Resolved module
#[derive(Debug, Clone)]
pub struct ResolvedModule {
    pub path: PathBuf,
    pub module_type: ModuleType,
    pub exports: Vec<String>,
    pub dependencies: Vec<String>,
    pub metadata: ModuleMetadata,
}

/// Module type
#[derive(Debug, Clone, PartialEq)]
pub enum ModuleType {
    Chtl,
    Html,
    Css,
    JavaScript,
    Cmod,
    Cjmod,
}

/// Module metadata
#[derive(Debug, Clone)]
pub struct ModuleMetadata {
    pub name: String,
    pub version: String,
    pub author: Option<String>,
    pub description: Option<String>,
    pub license: Option<String>,
    pub created_at: String,
    pub updated_at: String,
}

impl ImportNamespaceProcessor {
    pub fn new() -> Self {
        Self {
            imports: HashMap::new(),
            namespaces: HashMap::new(),
            module_resolver: ModuleResolver::new(),
        }
    }
    
    /// Process import statement
    pub fn process_import(&mut self, import: &ImportNode) -> Result<ImportDefinition> {
        let import_def = self.parse_import_node(import)?;
        self.imports.insert(import_def.path.clone(), import_def.clone());
        Ok(import_def)
    }
    
    /// Parse import node
    fn parse_import_node(&self, import: &ImportNode) -> Result<ImportDefinition> {
        let import_type = self.detect_import_type(&import.path)?;
        let imports = self.parse_import_items(&import.imports)?;
        let is_wildcard = import.imports.iter().any(|item| item == "*");
        let is_precise = !is_wildcard && !import.imports.is_empty();
        
        let metadata = ImportMetadata {
            resolved_path: None,
            file_size: None,
            last_modified: None,
            checksum: None,
        };
        
        Ok(ImportDefinition {
            path: import.path.clone(),
            import_type,
            imports,
            alias: import.alias.clone(),
            is_wildcard,
            is_precise,
            metadata,
        })
    }
    
    /// Detect import type from path
    fn detect_import_type(&self, path: &str) -> Result<ImportType> {
        if path.ends_with(".chtl") {
            Ok(ImportType::Chtl)
        } else if path.ends_with(".html") {
            Ok(ImportType::Html)
        } else if path.ends_with(".css") {
            Ok(ImportType::Css)
        } else if path.ends_with(".js") || path.ends_with(".cjjs") {
            Ok(ImportType::JavaScript)
        } else if path.ends_with(".cmod") || path.ends_with(".cjmod") {
            Ok(ImportType::Module)
        } else {
            // Try to resolve by content
            Ok(ImportType::Chtl)
        }
    }
    
    /// Parse import items
    fn parse_import_items(&self, items: &[String]) -> Result<Vec<ImportItem>> {
        let mut import_items = Vec::new();
        
        for item in items {
            let import_item = self.parse_import_item(item)?;
            import_items.push(import_item);
        }
        
        Ok(import_items)
    }
    
    /// Parse single import item
    fn parse_import_item(&self, item: &str) -> Result<ImportItem> {
        let (name, alias) = if let Some(alias_pos) = item.find(" as ") {
            let name = item[..alias_pos].trim().to_string();
            let alias = item[alias_pos + 4..].trim().to_string();
            (name, Some(alias))
        } else {
            (item.trim().to_string(), None)
        };
        
        let is_default = name == "default";
        let is_namespace = name == "*";
        let item_type = self.infer_import_item_type(&name);
        
        Ok(ImportItem {
            name,
            alias,
            is_default,
            is_namespace,
            item_type,
        })
    }
    
    /// Infer import item type
    fn infer_import_item_type(&self, name: &str) -> ImportItemType {
        if name.starts_with("template_") {
            ImportItemType::Template
        } else if name.starts_with("custom_") {
            ImportItemType::Custom
        } else if name.starts_with("style_") {
            ImportItemType::Style
        } else if name.starts_with("script_") {
            ImportItemType::Script
        } else if name.chars().next().map_or(false, |c| c.is_uppercase()) {
            ImportItemType::Class
        } else if name.contains('(') {
            ImportItemType::Function
        } else {
            ImportItemType::Variable
        }
    }
    
    /// Process namespace statement
    pub fn process_namespace(&mut self, namespace: &NamespaceNode) -> Result<NamespaceDefinition> {
        let namespace_def = self.parse_namespace_node(namespace)?;
        self.namespaces.insert(namespace_def.name.clone(), namespace_def.clone());
        Ok(namespace_def)
    }
    
    /// Parse namespace node
    fn parse_namespace_node(&self, namespace: &NamespaceNode) -> Result<NamespaceDefinition> {
        let imports = self.extract_namespace_imports(&namespace.content)?;
        let exports = self.extract_namespace_exports(&namespace.content)?;
        
        let metadata = NamespaceMetadata {
            created_at: chrono::Utc::now().to_rfc3339(),
            updated_at: chrono::Utc::now().to_rfc3339(),
            version: "1.0.0".to_string(),
            description: None,
        };
        
        Ok(NamespaceDefinition {
            name: namespace.name.clone(),
            content: namespace.content.clone(),
            imports,
            exports,
            parent: None,
            children: Vec::new(),
            metadata,
        })
    }
    
    /// Extract imports from namespace content
    fn extract_namespace_imports(&self, content: &[AstNode]) -> Result<Vec<String>> {
        let mut imports = Vec::new();
        
        for node in content {
            if let AstNode::Import(import) = node {
                imports.push(import.path.clone());
            }
        }
        
        Ok(imports)
    }
    
    /// Extract exports from namespace content
    fn extract_namespace_exports(&self, content: &[AstNode]) -> Result<Vec<String>> {
        let mut exports = Vec::new();
        
        for node in content {
            match node {
                AstNode::Template(template) => {
                    exports.push(template.name.clone());
                }
                AstNode::Custom(custom) => {
                    exports.push(custom.name.clone());
                }
                AstNode::Style(style) => {
                    // Extract style names from selectors
                    for selector in &style.selectors {
                        if let StyleSelector::Class(name) = selector {
                            exports.push(format!("style_{}", name));
                        } else if let StyleSelector::Id(name) = selector {
                            exports.push(format!("style_{}", name));
                        }
                    }
                }
                AstNode::Script(script) => {
                    exports.push("script".to_string());
                }
                _ => {}
            }
        }
        
        Ok(exports)
    }
    
    /// Resolve import path
    pub fn resolve_import_path(&mut self, import_path: &str) -> Result<PathBuf> {
        self.module_resolver.resolve_module(import_path)
    }
    
    /// Get import definition
    pub fn get_import(&self, path: &str) -> Option<&ImportDefinition> {
        self.imports.get(path)
    }
    
    /// Get namespace definition
    pub fn get_namespace(&self, name: &str) -> Option<&NamespaceDefinition> {
        self.namespaces.get(name)
    }
    
    /// List all imports
    pub fn list_imports(&self) -> Vec<&ImportDefinition> {
        self.imports.values().collect()
    }
    
    /// List all namespaces
    pub fn list_namespaces(&self) -> Vec<&NamespaceDefinition> {
        self.namespaces.values().collect()
    }
    
    /// Generate import statements
    pub fn generate_imports(&self, imports: &[String]) -> Result<String> {
        let mut output = String::new();
        
        for import_path in imports {
            if let Some(import_def) = self.get_import(import_path) {
                let import_stmt = self.generate_import_statement(import_def)?;
                output.push_str(&import_stmt);
                output.push('\n');
            }
        }
        
        Ok(output)
    }
    
    /// Generate import statement
    fn generate_import_statement(&self, import_def: &ImportDefinition) -> Result<String> {
        let mut stmt = String::new();
        
        match import_def.import_type {
            ImportType::Chtl => {
                stmt.push_str("import {");
                for (i, item) in import_def.imports.iter().enumerate() {
                    if i > 0 {
                        stmt.push_str(", ");
                    }
                    stmt.push_str(&item.name);
                    if let Some(alias) = &item.alias {
                        stmt.push_str(&format!(" as {}", alias));
                    }
                }
                stmt.push_str(&format!("}} from '{}'", import_def.path));
            }
            ImportType::Html => {
                stmt.push_str(&format!("import '{}'", import_def.path));
            }
            ImportType::Css => {
                stmt.push_str(&format!("import '{}'", import_def.path));
            }
            ImportType::JavaScript => {
                if import_def.is_wildcard {
                    stmt.push_str(&format!("import * as {} from '{}'", 
                        import_def.alias.as_ref().unwrap_or(&"module".to_string()), 
                        import_def.path));
                } else {
                    stmt.push_str("import {");
                    for (i, item) in import_def.imports.iter().enumerate() {
                        if i > 0 {
                            stmt.push_str(", ");
                        }
                        stmt.push_str(&item.name);
                        if let Some(alias) = &item.alias {
                            stmt.push_str(&format!(" as {}", alias));
                        }
                    }
                    stmt.push_str(&format!("}} from '{}'", import_def.path));
                }
            }
            ImportType::Module => {
                stmt.push_str(&format!("import {} from '{}'", 
                    import_def.alias.as_ref().unwrap_or(&"module".to_string()), 
                    import_def.path));
            }
        }
        
        Ok(stmt)
    }
    
    /// Generate namespace wrapper
    pub fn generate_namespace_wrapper(&self, namespace_name: &str, content: &str) -> Result<String> {
        let mut output = String::new();
        
        output.push_str(&format!("// Namespace: {}\n", namespace_name));
        output.push_str(&format!("(function({}) {{\n", namespace_name));
        output.push_str("    'use strict';\n\n");
        output.push_str(content);
        output.push_str(&format!("\n}})({});\n", namespace_name));
        
        Ok(output)
    }
    
    /// Check for import conflicts
    pub fn check_import_conflicts(&self) -> Result<Vec<ImportConflict>> {
        let mut conflicts = Vec::new();
        let mut name_map: HashMap<String, Vec<String>> = HashMap::new();
        
        for (path, import_def) in &self.imports {
            for item in &import_def.imports {
                let name = item.alias.as_ref().unwrap_or(&item.name);
                name_map.entry(name.clone()).or_insert_with(Vec::new).push(path.clone());
            }
        }
        
        for (name, paths) in name_map {
            if paths.len() > 1 {
                conflicts.push(ImportConflict {
                    name,
                    paths,
                    conflict_type: ConflictType::NameCollision,
                });
            }
        }
        
        Ok(conflicts)
    }
    
    /// Resolve import conflicts
    pub fn resolve_import_conflicts(&mut self, conflicts: &[ImportConflict]) -> Result<()> {
        for conflict in conflicts {
            match conflict.conflict_type {
                ConflictType::NameCollision => {
                    // Add path prefix to resolve collision
                    for (i, path) in conflict.paths.iter().enumerate() {
                        if let Some(import_def) = self.imports.get_mut(path) {
                            for item in &mut import_def.imports {
                                if item.name == conflict.name {
                                    let prefix = self.generate_path_prefix(path);
                                    item.alias = Some(format!("{}_{}", prefix, conflict.name));
                                }
                            }
                        }
                    }
                }
            }
        }
        
        Ok(())
    }
    
    /// Generate path prefix
    fn generate_path_prefix(&self, path: &str) -> String {
        path.replace('/', "_")
            .replace('.', "_")
            .replace('-', "_")
            .replace('\\', "_")
    }
}

impl ModuleResolver {
    pub fn new() -> Self {
        Self {
            search_paths: vec![
                PathBuf::from("./"),
                PathBuf::from("./src/"),
                PathBuf::from("./modules/"),
                PathBuf::from("./node_modules/"),
            ],
            module_cache: HashMap::new(),
            alias_map: HashMap::new(),
        }
    }
    
    /// Add search path
    pub fn add_search_path(&mut self, path: PathBuf) {
        self.search_paths.push(path);
    }
    
    /// Add alias
    pub fn add_alias(&mut self, alias: String, path: String) {
        self.alias_map.insert(alias, path);
    }
    
    /// Resolve module
    pub fn resolve_module(&mut self, import_path: &str) -> Result<PathBuf> {
        // Check cache first
        if let Some(resolved) = self.module_cache.get(import_path) {
            return Ok(resolved.path.clone());
        }
        
        // Check aliases
        if let Some(aliased_path) = self.alias_map.get(import_path) {
            return self.resolve_module(aliased_path);
        }
        
        // Try different file extensions
        let extensions = [".chtl", ".html", ".css", ".js", ".cjjs", ".cmod", ".cjmod"];
        
        for search_path in &self.search_paths {
            for ext in &extensions {
                let full_path = search_path.join(format!("{}{}", import_path, ext));
                if full_path.exists() {
                    let resolved = ResolvedModule {
                        path: full_path.clone(),
                        module_type: self.detect_module_type(&full_path)?,
                        exports: Vec::new(),
                        dependencies: Vec::new(),
                        metadata: ModuleMetadata {
                            name: import_path.to_string(),
                            version: "1.0.0".to_string(),
                            author: None,
                            description: None,
                            license: None,
                            created_at: chrono::Utc::now().to_rfc3339(),
                            updated_at: chrono::Utc::now().to_rfc3339(),
                        },
                    };
                    
                    self.module_cache.insert(import_path.to_string(), resolved);
                    return Ok(full_path);
                }
            }
        }
        
        Err(ChtlError::semantic(&format!("Module '{}' not found", import_path)))
    }
    
    /// Detect module type from path
    fn detect_module_type(&self, path: &Path) -> Result<ModuleType> {
        let extension = path.extension()
            .ok_or_else(|| ChtlError::semantic("No file extension"))?
            .to_string_lossy();
        
        match extension.as_ref() {
            "chtl" => Ok(ModuleType::Chtl),
            "html" => Ok(ModuleType::Html),
            "css" => Ok(ModuleType::Css),
            "js" => Ok(ModuleType::JavaScript),
            "cjjs" => Ok(ModuleType::JavaScript),
            "cmod" => Ok(ModuleType::Cmod),
            "cjmod" => Ok(ModuleType::Cjmod),
            _ => Err(ChtlError::semantic("Unknown module type")),
        }
    }
    
    /// Get resolved module
    pub fn get_resolved_module(&self, import_path: &str) -> Option<&ResolvedModule> {
        self.module_cache.get(import_path)
    }
    
    /// Clear cache
    pub fn clear_cache(&mut self) {
        self.module_cache.clear();
    }
}

/// Import conflict
#[derive(Debug, Clone)]
pub struct ImportConflict {
    pub name: String,
    pub paths: Vec<String>,
    pub conflict_type: ConflictType,
}

/// Conflict type
#[derive(Debug, Clone, PartialEq)]
pub enum ConflictType {
    NameCollision,
    TypeMismatch,
    VersionConflict,
}