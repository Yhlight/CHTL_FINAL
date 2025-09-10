// This is the CHTL Node module.
// It will define the structs and enums for the Abstract Syntax Tree (AST).

#[derive(Debug, PartialEq)]
pub struct Document {
    pub children: Vec<Node>,
}

#[derive(Debug, PartialEq)]
pub enum Node {
    Element(ElementNode),
    Text(TextNode),
    Comment(CommentNode),
}

#[derive(Debug, PartialEq)]
pub struct ElementNode {
    pub tag_name: String,
    pub attributes: Vec<AttributeNode>,
    pub children: Vec<Node>,
}

#[derive(Debug, PartialEq)]
pub struct AttributeNode {
    pub name: String,
    pub value: String,
}

#[derive(Debug, PartialEq)]
pub struct TextNode {
    pub content: String,
}

#[derive(Debug, PartialEq)]
pub enum CommentType {
    SingleLine,
    MultiLine,
    Generator,
}

#[derive(Debug, PartialEq)]
pub struct CommentNode {
    pub content: String,
    pub comment_type: CommentType,
}
