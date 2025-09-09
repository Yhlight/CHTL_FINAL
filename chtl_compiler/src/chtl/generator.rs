use crate::chtl::node::{Program, Node, ElementNode, TextNode, OriginNode};

pub struct Generator {}

impl Generator {
    pub fn new() -> Self { Generator {} }

    pub fn generate(&self, program: &Program) -> String {
        let mut output = String::new();
        if !program.global_styles.is_empty() {
            output.push_str("<style>\n");
            for style_rule in &program.global_styles {
                output.push_str(&format!("  {}\n", style_rule));
            }
            output.push_str("</style>\n");
        }
        for node in &program.children {
            output.push_str(&self.generate_node(node, 0));
        }
        output
    }

    fn generate_node(&self, node: &Node, indent: usize) -> String {
        match node {
            Node::Element(n) => self.generate_element(n, indent),
            Node::Text(n) => self.generate_text(n, indent),
            Node::Comment(c) => self.generate_comment(c, indent),
            Node::Origin(n) => self.generate_origin(n),
        }
    }

    fn generate_origin(&self, origin: &OriginNode) -> String {
        origin.content.clone()
    }

    fn generate_element(&self, element: &ElementNode, indent: usize) -> String {
        let mut output = String::new();
        let indentation = " ".repeat(indent * 2);
        output.push_str(&format!("{}<{}", indentation, element.tag_name));
        for (key, value) in &element.attributes {
            output.push_str(&format!(" {}=\"{}\"", key, value));
        }
        output.push_str(">\n");
        for child in &element.children {
            output.push_str(&self.generate_node(child, indent + 1));
        }
        output.push_str(&format!("{}</{}>\n", indentation, element.tag_name));
        output
    }

    fn generate_text(&self, text: &TextNode, indent: usize) -> String {
        " ".repeat(indent * 2) + &text.value + "\n"
    }

    fn generate_comment(&self, comment: &String, indent: usize) -> String {
        " ".repeat(indent * 2) + &format!("<!-- {} -->\n", comment)
    }
}

#[cfg(test)]
mod tests {
    // THIS IS THE FIX ->
    use crate::chtl::lexer::Lexer;
    use crate::chtl::parser::Parser;
    use super::*;

    #[test]
    fn test_generate_html() {
        // ... test implementation ...
    }

    #[test]
    fn test_generate_origin_block() {
        // ... test implementation ...
    }
}
