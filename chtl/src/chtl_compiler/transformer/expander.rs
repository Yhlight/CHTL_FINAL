use std::collections::HashMap;
use crate::chtl_compiler::node::{Node, TemplateDefinitionNode, TemplateBody, ElementNode, StyleNode, StyleContent};

pub struct Expander<'a> {
    templates: &'a HashMap<String, TemplateDefinitionNode>,
}

impl<'a> Expander<'a> {
    pub fn new(templates: &'a HashMap<String, TemplateDefinitionNode>) -> Self {
        Expander { templates }
    }

    pub fn expand_program(&self, program: Vec<Node>) -> Vec<Node> {
        program.into_iter().flat_map(|node| self.expand_node(node)).collect()
    }

    fn expand_node(&self, node: Node) -> Vec<Node> {
        match node {
            Node::Element(element) => {
                let expanded_children = element.children.into_iter().flat_map(|child| self.expand_node(child)).collect();
                vec![Node::Element(ElementNode {
                    children: expanded_children,
                    ..element
                })]
            }
            Node::Style(style_node) => {
                let mut expanded_content = Vec::new();
                for content in style_node.content {
                    match content {
                        StyleContent::TemplateUsage(usage) => {
                            if let Some(template) = self.templates.get(&usage.name) {
                                if let TemplateBody::Style(properties) = &template.body {
                                    for prop in properties {
                                        expanded_content.push(StyleContent::Property(prop.clone()));
                                    }
                                }
                            }
                        }
                        _ => expanded_content.push(content),
                    }
                }
                vec![Node::Style(StyleNode { content: expanded_content })]
            }
            Node::TemplateUsage(usage) => {
                if let Some(template) = self.templates.get(&usage.name) {
                    if let TemplateBody::Element(nodes) = &template.body {
                        // Recursively expand nodes from the template body
                        return nodes.clone().into_iter().flat_map(|n| self.expand_node(n)).collect();
                    }
                }
                // If template not found or wrong type, it will just disappear.
                // A real implementation should error here.
                vec![]
            }
            // Other nodes are returned as is
            _ => vec![node],
        }
    }
}
