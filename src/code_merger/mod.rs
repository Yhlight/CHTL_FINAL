//! Code Merger
//! 
//! This module merges compiled code from different compilers into final output.

use anyhow::Result;
use crate::compiler_dispatcher::CompilationResult;

/// Code merger for combining compilation results
pub struct CodeMerger {
    /// HTML template
    html_template: String,
}

impl CodeMerger {
    /// Create a new code merger
    pub fn new() -> Self {
        Self {
            html_template: String::new(),
        }
    }
    
    /// Merge compilation results into final HTML
    pub fn merge(&self, result: CompilationResult) -> Result<String> {
        let mut html = result.html;
        
        // Add CSS if present
        if !result.css.is_empty() {
            let css_block = format!("<style>\n{}\n</style>", result.css);
            html.push_str(&css_block);
        }
        
        // Add JavaScript if present
        if !result.js.is_empty() {
            let js_block = format!("<script>\n{}\n</script>", result.js);
            html.push_str(&js_block);
        }
        
        Ok(html)
    }
    
    /// Set HTML template
    pub fn set_template(&mut self, template: String) {
        self.html_template = template;
    }
    
    /// Merge with custom template
    pub fn merge_with_template(&self, result: CompilationResult, template: &str) -> Result<String> {
        let mut html = template.to_string();
        
        // Replace placeholders
        html = html.replace("{{HTML}}", &result.html);
        html = html.replace("{{CSS}}", &result.css);
        html = html.replace("{{JS}}", &result.js);
        
        Ok(html)
    }
}