//! Compiler Dispatcher
//! 
//! This module dispatches code fragments to appropriate compilers.

use anyhow::Result;
use crate::scanner::{CodeFragment, FragmentType};
use crate::chtl::ChtlCompiler;
use crate::chtl_js::ChtlJSCompiler;

/// Compiler dispatcher
pub struct CompilerDispatcher {
    chtl_compiler: ChtlCompiler,
    chtl_js_compiler: ChtlJSCompiler,
}

impl CompilerDispatcher {
    /// Create a new compiler dispatcher
    pub fn new() -> Self {
        Self {
            chtl_compiler: ChtlCompiler::new(),
            chtl_js_compiler: ChtlJSCompiler::new(),
        }
    }
    
    /// Dispatch fragments to appropriate compilers
    pub fn dispatch(&self, fragments: Vec<CodeFragment>) -> Result<CompilationResult> {
        let mut html_output = String::new();
        let mut css_output = String::new();
        let mut js_output = String::new();
        
        for fragment in fragments {
            match fragment.fragment_type {
                FragmentType::Chtl => {
                    let html = self.chtl_compiler.compile_source(&fragment.content)?;
                    html_output.push_str(&html);
                }
                FragmentType::ChtlJS => {
                    let js = self.chtl_js_compiler.compile_source(&fragment.content)?;
                    js_output.push_str(&js);
                }
                FragmentType::CSS => {
                    css_output.push_str(&fragment.content);
                }
                FragmentType::JS => {
                    js_output.push_str(&fragment.content);
                }
            }
        }
        
        Ok(CompilationResult {
            html: html_output,
            css: css_output,
            js: js_output,
        })
    }
}

/// Compilation result
#[derive(Debug, Clone)]
pub struct CompilationResult {
    pub html: String,
    pub css: String,
    pub js: String,
}