//! Compiler dispatcher
//! 
//! This module provides the compiler dispatcher functionality for CHTL,
//! including coordinating different compilers and managing compilation pipeline.

pub mod dispatcher;
pub mod pipeline;
pub mod coordinator;

// Re-export main components
pub use dispatcher::*;
pub use pipeline::*;
pub use coordinator::*;