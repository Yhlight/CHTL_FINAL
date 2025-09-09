use super::{ElementNode, TextNode, CommentNode, StyleNode, TemplateDefinitionNode, TemplateUsageNode, ImportNode};

#[derive(Debug, PartialEq, Clone)]
pub enum Node {
    Element(ElementNode),
    Text(TextNode),
    Comment(CommentNode),
    Style(StyleNode),
    TemplateDefinition(TemplateDefinitionNode),
    TemplateUsage(TemplateUsageNode),
    Import(ImportNode),
    // Will be expanded with more node types like Custom, etc. in the future.
}

#[derive(Debug, PartialEq, Clone)]
pub struct Attribute {
    pub name: String,
    pub value: String,
}
