use std::collections::HashMap;

// The top-level container for a CHTL document
pub struct Program {
    pub children: Vec<Node>,
    pub global_styles: Vec<String>,
}

// A generic enum that can be any of the specific node types
#[derive(Debug, PartialEq, Clone)]
pub enum Node {
    Element(ElementNode),
    Text(TextNode),
    Comment(String),
    Origin(OriginNode),
}

// Represents an [Origin] block for raw code embedding
#[derive(Debug, PartialEq, Clone)]
pub struct OriginNode {
    pub origin_type: String,
    pub content: String,
}

// Represents an element like `div { ... }`
#[derive(Debug, PartialEq, Clone)]
pub struct ElementNode {
    pub tag_name: String,
    pub attributes: HashMap<String, String>,
    pub children: Vec<Node>,
}

// Represents a text block like `text { "..." }`
#[derive(Debug, PartialEq, Clone)]
pub struct TextNode {
    pub value: String,
}
