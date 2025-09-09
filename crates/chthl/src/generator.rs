//! This module defines the HTML Generator, which walks the AST produced by the
//! parser and generates an HTML string.

use crate::ast::{CommentNode, ElementNode, Expression, Node, Program};
use std::collections::HashSet;

pub struct Generator {
    global_css: String,
}

impl Generator {
    pub fn new() -> Self {
        Generator {
            global_css: String::new(),
        }
    }

    pub fn generate_program(&mut self, program: &Program) -> String {
        let mut body_html = String::new();
        for node in &program.nodes {
            body_html.push_str(&self.generate_node(node, 0));
        }

        if self.global_css.is_empty() {
            return body_html;
        }

        // Simple and naive wrapping for now. A more robust solution would
        // find/create the head tag.
        format!(
            "<head>\n  <style>\n{}\n  </style>\n</head>\n{}",
            self.global_css, body_html
        )
    }

    fn generate_node(&mut self, node: &Node, indent: usize) -> String {
        match node {
            Node::Element(el) => self.generate_element_node(el, indent),
            Node::Text(txt) => self.generate_text_node(txt, indent),
            Node::Comment(comment) => self.generate_comment_node(comment, indent),
            Node::Style(_) => String::new(), // Handled by parent element
        }
    }

    fn generate_element_node(&mut self, element: &ElementNode, indent: usize) -> String {
        let self_closing_tags: &[&str] = &[
            "area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param",
            "source", "track", "wbr",
        ];

        let mut attributes = Vec::new();
        let mut classes = HashSet::new();
        let mut id = None;
        let mut inline_styles = Vec::new();

        // Process explicit attributes
        for attr in &element.attributes {
            let value_str = match &attr.value {
                Expression::Ident(s) => s,
                Expression::StringLiteral(s) => s,
            };

            match attr.name.as_str() {
                "class" => {
                    classes.extend(value_str.split_whitespace().map(String::from));
                }
                "id" => {
                    id = Some(value_str.clone());
                }
                _ => {
                    attributes.push(format!("{}=\"{}\"", attr.name, value_str));
                }
            }
        }

        // Process style nodes for inline styles, global rules, and automatic attributes
        for child in &element.children {
            if let Node::Style(style_node) = child {
                // Collect inline properties
                for prop in &style_node.inline_properties {
                    inline_styles.push(format!("{}: {};", prop.name, prop.value));
                }
                // Collect global rules and auto-apply classes/ids
                for rule in &style_node.rules {
                    let selector = rule.selector.trim();
                    if let Some(class_name) = selector.strip_prefix('.') {
                        classes.insert(class_name.to_string());
                    } else if let Some(id_name) = selector.strip_prefix('#') {
                        id = Some(id_name.to_string());
                    }

                    let properties_str = rule.properties.iter()
                        .map(|p| format!("  {}: {};\n", p.name, p.value))
                        .collect::<String>();
                    self.global_css.push_str(&format!("{} {{\n{}}}\n", selector, properties_str));
                }
            }
        }

        let mut tag = format!("<{}", element.tag_name);

        // Assemble final attributes
        if let Some(id_val) = id {
            tag.push_str(&format!(" id=\"{}\"", id_val));
        }
        if !classes.is_empty() {
            let class_list = classes.into_iter().collect::<Vec<_>>().join(" ");
            tag.push_str(&format!(" class=\"{}\"", class_list));
        }
        for attr in attributes {
            tag.push_str(&format!(" {}", attr));
        }
        if !inline_styles.is_empty() {
            tag.push_str(&format!(" style=\"{}\"", inline_styles.join(" ")));
        }

        if self_closing_tags.contains(&element.tag_name.as_str()) {
            tag.push('>');
            return tag;
        }
        tag.push('>');

        let mut children_html = String::new();
        for child in &element.children {
            if !matches!(child, Node::Style(_)) {
                children_html.push_str(&self.generate_node(child, indent + 2));
            }
        }

        format!("{}{}{}{}</{}>", " ".repeat(indent), tag, children_html, "\n".repeat(indent), element.tag_name)
    }

    fn generate_text_node(&self, text: &crate::ast::TextNode, indent: usize) -> String {
        format!("{}{}", " ".repeat(indent), text.value)
    }

    fn generate_comment_node(&self, comment: &CommentNode, indent: usize) -> String {
        format!("{}<!-- {} -->", " ".repeat(indent), comment.value)
    }
}


#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;
    use crate::parser::Parser;

    #[test]
    fn test_auto_class_id_and_global_style_generation() {
        let input = r#"
            div {
                style {
                    .container {
                        border: 1px solid black;
                    }
                    color: red;
                }
                p {
                    style {
                        #main-text {
                            font-size: 16px;
                        }
                    }
                    text { "Hello" }
                }
            }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert!(parser.errors().is_empty(), "{:?}", parser.errors());

        let mut generator = Generator::new();
        let html = generator.generate_program(&program);

        let expected_html = r#"
<head>
  <style>
.container {
  border: 1px solid black;
}
#main-text {
  font-size: 16px;
}
  </style>
</head>
<div class="container" style="color: red;"><p id="main-text">  Hello</p>
</div>
        "#;

        let normalize = |s: &str| s.replace(['\n', ' '], "");

        assert_eq!(normalize(&html), normalize(expected_html));
    }
}
