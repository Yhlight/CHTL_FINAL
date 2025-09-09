use std::collections::HashMap;

// The root of a CHTL JS program will be a list of these nodes.
#[derive(Debug, PartialEq, Clone)]
pub enum Node {
    FunctionCall(FunctionCallNode),
    // Other node types like variable declarations might be needed later.
}

// Represents a CHTL JS declarative function call, e.g., `listen { ... }`
#[derive(Debug, PartialEq, Clone)]
pub struct FunctionCallNode {
    pub name: String,
    pub arguments: HashMap<String, Value>,
}

// Represents the different types of values in CHTL JS
#[derive(Debug, PartialEq, Clone)]
pub enum Value {
    String(String),
    Number(f64),
    Object(HashMap<String, Value>),
    Array(Vec<Value>),
    Identifier(String), // For things like function names passed as args
    // A raw function body for callbacks, to be passed to the JS output as-is.
    RawJavaScript(String),
}
