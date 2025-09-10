use crate::CHTL::CHTLNode::{Document, ElementNode, Node, TextNode};

/// Generates an HTML string from a Document AST.
pub fn generate_html(doc: &Document) -> String {
    let mut output = String::new();
    for node in &doc.children {
        output.push_str(&generate_node(node, 0));
    }
    output
}

fn generate_node(node: &Node, indent_level: usize) -> String {
    match node {
        Node::Element(element_node) => generate_element_node(element_node, indent_level),
        Node::Text(text_node) => generate_text_node(text_node, indent_level),
        Node::Comment(_) => String::new(),
    }
}

fn is_self_closing(tag_name: &str) -> bool {
    matches!(tag_name, "area" | "base" | "br" | "col" | "embed" | "hr" | "img" | "input" | "link" | "meta" | "param" | "source" | "track" | "wbr")
}

fn generate_element_node(element_node: &ElementNode, indent_level: usize) -> String {
    let mut output = String::new();
    let indent = "  ".repeat(indent_level);

    output.push_str(&format!("{}<{}", indent, element_node.tag_name));

    for attr in &element_node.attributes {
        output.push_str(&format!(" {}=\"{}\"", attr.name, attr.value));
    }

    if element_node.children.is_empty() && is_self_closing(&element_node.tag_name) {
        output.push_str(" />\n");
    } else {
        output.push_str(">\n");

        for child in &element_node.children {
            output.push_str(&generate_node(child, indent_level + 1));
        }

        output.push_str(&format!("{}</{}>\n", indent, element_node.tag_name));
    }

    output
}

fn generate_text_node(text_node: &TextNode, indent_level: usize) -> String {
    let indent = "  ".repeat(indent_level);
    format!("{}{}\n", indent, text_node.content)
}
