//! CHTL JS AST Nodes
//! 
//! This module defines the Abstract Syntax Tree nodes for CHTL JS.

use serde::{Deserialize, Serialize};

/// CHTL JS AST node
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ChtlJSNode {
    pub content: String,
}