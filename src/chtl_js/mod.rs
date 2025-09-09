//! CHTL JS system
//! 
//! This module provides CHTL JS functionality,
//! including JavaScript generation, virtual objects, and enhanced features.

pub mod context;
pub mod generator;
pub mod lexer;
pub mod loader;
pub mod manage;
pub mod node;
pub mod parser;
pub mod state;
pub mod io_stream;
pub mod cjmod_system;

// Re-export main components
pub use context::*;
pub use generator::*;
pub use lexer::*;
pub use loader::*;
pub use manage::*;
pub use node::*;
pub use parser::*;
pub use state::*;
pub use io_stream::*;
pub use cjmod_system::*;