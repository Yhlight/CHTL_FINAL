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

// The `indent` parameter is for pretty-printing, which can be useful for debugging.
fn generate_node(node: &Node, indent: usize) -> String {
    match node {
        Node::Element(el) => generate_element_node(el, indent),
        Node::Text(txt) => generate_text_node(txt, indent),
        Node::Comment(comment) => generate_comment_node(comment, indent),
    }
}

fn generate_element_node(element: &ElementNode, indent: usize) -> String {
    let self_closing_tags: &[&str] = &[
        "area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param",
        "source", "track", "wbr",
    ];

    let mut tag = format!("{}{}<{}", " ".repeat(indent), "\n", element.tag_name);
    for attr in &element.attributes {
        let value = match &attr.value {
            Expression::Ident(s) => s,
            Expression::StringLiteral(s) => s,
        };
        tag.push_str(&format!(" {}=\"{}\"", attr.name, value));
    }

    if self_closing_tags.contains(&element.tag_name.as_str()) {
        tag.push('>');
        return tag;
    }
    tag.push('>');

    let mut children_html = String::new();
    for child in &element.children {
        children_html.push_str(&generate_node(child, indent + 2));
    }

    if children_html.is_empty() {
         format!("{}</{}>", tag, element.tag_name)
    } else {
        format!("{}{}{}</{}>", tag, children_html, "\n", element.tag_name)
    }
}

fn generate_text_node(text: &TextNode, indent: usize) -> String {
    // In a real implementation, we would handle HTML escaping here.
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
    fn test_html_generation() {
        let input = r#"
            html {
                head {
                    title { text { "My CHTL Page" } }
                }
                body {
                    id: "app";
                    -- main content
                    div {
                        class: "container";
                        h1 { text { "Welcome!" } }
                        img { src: "/logo.png"; }
                    }
                }
            }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(parser.errors().is_empty());

        let html = generate_program(&program);

        // Note: The expected output is based on the pretty-printing logic (indents and newlines)
        // in the generator. This makes the test specific but easier to read.
        let expected_html = r#"
<html>
  <head>
    <title>
      My CHTL Page
    </title>
  </head>
  <body id="app">
    <!-- main content -->
    <div class="container">
      <h1>
        Welcome!
      </h1>
      <img src="/logo.png">
    </div>
  </body>
</html>"#;

        // A simple normalization to make comparison less brittle regarding whitespace.
        let normalize = |s: &str| s.lines().map(|l| l.trim()).filter(|l| !l.is_empty()).collect::<String>();

        assert_eq!(normalize(html.as_str()), normalize(expected_html));
    }
}
