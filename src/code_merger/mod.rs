//! Code merger
//! 
//! This module provides code merging functionality for CHTL,
//! including merging different types of code and managing code integration.

pub mod code_merger;
pub mod html_merger;
pub mod css_merger;
pub mod javascript_merger;

// Re-export main components
pub use code_merger::*;
pub use html_merger::*;
pub use css_merger::*;
pub use javascript_merger::*;