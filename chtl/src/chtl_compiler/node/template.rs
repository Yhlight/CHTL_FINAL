use super::{Node, StyleProperty};

#[derive(Debug, PartialEq, Clone)]
pub enum TemplateKind {
    Style,
    Element,
    Var,
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateDefinitionNode {
    pub name: String,
    pub kind: TemplateKind,
    pub body: TemplateBody,
}

#[derive(Debug, PartialEq, Clone)]
pub enum TemplateBody {
    Style(Vec<StyleProperty>),
    Element(Vec<Node>),
    // Vars will be similar to Style properties
    Var(Vec<StyleProperty>),
}

#[derive(Debug, PartialEq, Clone)]
pub struct TemplateUsageNode {
    pub name: String,
    pub kind: TemplateKind,
}
