//! This module defines the HTML Generator, which walks the AST produced by the
//! parser and generates an HTML string.

use crate::ast::{CommentNode, ElementNode, Expression, Node, Program, TextNode};

pub fn generate_program(program: &Program) -> String {
    let mut html = String::new();
    for node in &program.nodes {
        html.push_str(&generate_node(node, 0));
    }
    html
}

fn generate_node(node: &Node, indent: usize) -> String {
    match node {
        Node::Element(el) => generate_element_node(el, indent),
        Node::Text(txt) => generate_text_node(txt, indent),
        Node::Comment(comment) => generate_comment_node(comment, indent),
        Node::Style(_) => String::new(), // Style nodes are handled by their parent element
    }
}

fn generate_element_node(element: &ElementNode, indent: usize) -> String {
    let self_closing_tags: &[&str] = &[
        "area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param",
        "source", "track", "wbr",
    ];

    let mut tag = format!("{}{}<{}", " ".repeat(indent), "\n", element.tag_name);

    // Handle normal attributes
    for attr in &element.attributes {
        let value = match &attr.value {
            Expression::Ident(s) => s,
            Expression::StringLiteral(s) => s,
        };
        tag.push_str(&format!(" {}=\"{}\"", attr.name, value));
    }

    // Collect and handle inline styles from style nodes
    let mut style_properties = Vec::new();
    for child in &element.children {
        if let Node::Style(style_node) = child {
            for prop in &style_node.properties {
                style_properties.push(format!("{}: {};", prop.name, prop.value));
            }
        }
    }
    if !style_properties.is_empty() {
        tag.push_str(&format!(" style=\"{}\"", style_properties.join(" ")));
    }


    if self_closing_tags.contains(&element.tag_name.as_str()) {
        tag.push('>');
        return tag;
    }
    tag.push('>');

    let mut children_html = String::new();
    for child in &element.children {
        // Generate HTML for non-style children
        if !matches!(child, Node::Style(_)) {
            children_html.push_str(&generate_node(child, indent + 2));
        }
    }

    if children_html.is_empty() {
         format!("{}</{}>", tag, element.tag_name)
    } else {
        format!("{}{}{}</{}>", tag, children_html, "\n", element.tag_name)
    }
}

fn generate_text_node(text: &TextNode, indent: usize) -> String {
    format!("{}{}{}", " ".repeat(indent), "\n", text.value)
}

fn generate_comment_node(comment: &CommentNode, indent: usize) -> String {
    format!("{}{}\n<!-- {} -->", " ".repeat(indent), "\n", comment.value)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;
    use crate::parser::Parser;

    #[test]
    fn test_html_generation_with_style() {
        let input = r#"
            div {
                id: "app";
                style {
                    width: 100px;
                    color: red;
                }
                h1 { text { "Styled Box" } }
            }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(parser.errors().is_empty(), "{:?}", parser.errors());

        let html = generate_program(&program);

        let expected_html = r#"
            <div id="app" style="width: 100px; color: red;">
              <h1>
                Styled Box
              </h1>
            </div>
        "#;

        let normalize = |s: &str| s.lines().map(|l| l.trim()).filter(|l| !l.is_empty()).collect::<String>();

        assert_eq!(normalize(html.as_str()), normalize(expected_html));
    }
}
