//! CHTL unified scanner
//! 
//! This module provides the unified scanning functionality for CHTL,
//! including code separation, placeholder management, and boundary detection.

pub mod unified_scanner;
pub mod code_separator;
pub mod placeholder_manager;
pub mod boundary_detector;

// Re-export main components
pub use unified_scanner::*;
pub use code_separator::*;
pub use placeholder_manager::*;
pub use boundary_detector::*;