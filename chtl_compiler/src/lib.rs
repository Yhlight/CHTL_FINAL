pub mod ast;
pub mod chtl_js;
pub mod css_parser;
pub mod dispatcher;
pub mod generator;
pub mod lexer;
pub mod parser;

// The main public entry point for the compiler library.
pub use dispatcher::compile;
