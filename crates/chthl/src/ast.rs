//! This module defines the Abstract Syntax Tree (AST) nodes for the CHTL language.
//! The parser will produce a tree of these nodes.

/// The root of a CHTL document's AST. It's simply a collection of top-level nodes.
#[derive(Debug, PartialEq, Clone)]
pub struct Program {
    pub nodes: Vec<Node>,
}

/// A unifying enum to represent any kind of node in the AST.
/// This allows for heterogeneous lists of children in elements.
#[derive(Debug, PartialEq, Clone)]
pub enum Node {
    Element(ElementNode),
    Text(TextNode),
    Comment(CommentNode),
    Style(StyleNode),
    // More node types will be added here in later phases, e.g., Template, Custom.
}

/// Represents a local style block, e.g., `style { ... }`.
#[derive(Debug, PartialEq, Clone)]
pub struct StyleNode {
    pub inline_properties: Vec<StyleProperty>,
    pub rules: Vec<CssRuleNode>,
}

/// Represents a CSS rule block, e.g., `.class { ... }`.
#[derive(Debug, PartialEq, Clone)]
pub struct CssRuleNode {
    pub selector: String,
    pub properties: Vec<StyleProperty>,
}

/// Represents a style property, e.g., `width: 100px`.
#[derive(Debug, PartialEq, Clone)]
pub struct StyleProperty {
    pub name: String,
    // For now, the value is a single string. This will be expanded later for expressions.
    pub value: String,
}

/// Represents an expression. For now, this is used for attribute values, which can
/// be identifiers (unquoted literals) or strings (quoted literals).
#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Ident(String),
    StringLiteral(String),
}

/// Represents a key-value attribute on an element, e.g., `id: "main"`.
#[derive(Debug, PartialEq, Clone)]
pub struct AttributeNode {
    pub name: String,
    pub value: Expression,
}

/// Represents an element node, e.g., `div { ... }`.
#[derive(Debug, PartialEq, Clone)]
pub struct ElementNode {
    pub tag_name: String,
    pub attributes: Vec<AttributeNode>,
    pub children: Vec<Node>,
}

/// Represents a text node, e.g., `text { "hello" }`.
#[derive(Debug, PartialEq, Clone)]
pub struct TextNode {
    pub value: String,
}

/// Represents a generator-aware comment, e.g., `-- a comment`.
/// Normal comments (`//`, `/* */`) are discarded by the lexer.
#[derive(Debug, PartialEq, Clone)]
pub struct CommentNode {
    pub value: String,
}
