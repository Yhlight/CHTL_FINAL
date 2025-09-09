use crate::chtl_compiler::node::{Attribute, Node, ElementNode, TextNode, CommentNode};

pub struct Generator;

impl Generator {
    pub fn new() -> Self {
        Generator
    }

    pub fn generate(&self, program: Vec<Node>) -> String {
        let mut output = String::new();
        for node in program {
            output.push_str(&self.generate_node(node));
        }
        output
    }

    // The generator needs access to the template definitions.
    // A better design would be to pass the context explicitly.
    // For now, we'll assume the expansion happens in the parser or a transformation pass.
    // The below implementation is a placeholder for how it *would* work if expansion was done here.
    // However, my current parser expands @Style templates immediately.
    // I will implement @Element expansion in a similar immediate way in the parser.
    // This means the generator doesn't actually need to be changed for now.
    // I will refactor this later if needed.

    fn generate_node(&self, node: Node) -> String {
        match node {
            Node::Element(element) => self.generate_element(element),
            Node::Text(text) => self.generate_text(text),
            Node::Comment(comment) => self.generate_comment(comment),
            Node::Style(_) => {
                // Style nodes are handled within generate_element and should not be rendered directly.
                String::new()
            }
            Node::TemplateDefinition(_) => {
                // Template definitions are not rendered.
                String::new()
            }
            Node::TemplateUsage(usage) => {
                // This part of the generator will not be reached if expansion happens in the parser.
                // If it were to be reached, it would look up the template and generate its content.
                format!("<!-- UNEXPANDED TEMPLATE: {} -->", usage.name)
            }
            Node::Import(import) => {
                format!("<!-- CHTL: Import '{}' -->", import.path)
            }
        }
    }

    fn generate_element(&self, mut element: ElementNode) -> String {
        let mut output = String::new();
        let mut inner_content = String::new();
        let mut attributes_str = String::new();

        // Extract inline styles from any StyleNode children
        let mut inline_styles = String::new();
        element.children.retain(|child| {
            if let Node::Style(style_node) = child {
                for content_item in &style_node.content {
                    // The expander should have already handled TemplateUsages,
                    // so we only expect to see properties here.
                    if let crate::chtl_compiler::node::StyleContent::Property(prop) = content_item {
                        let value_str = match &prop.value {
                            crate::chtl_compiler::node::PropertyValue::Literal(s) => s.clone(),
                            crate::chtl_compiler::node::PropertyValue::Conditional(_) => {
                                "/* CHTL: conditional expression not evaluated */".to_string()
                            }
                        };
                        inline_styles.push_str(&format!("{}: {};", prop.name, value_str));
                    }
                }
                false // Remove the style node from children
            } else {
                true // Keep other nodes
            }
        });

        // Separate text attribute and existing style attribute from others
        let mut text_attr_value: Option<String> = None;
        let mut other_attributes = Vec::new();
        let mut existing_style_attr: Option<String> = None;

        for attr in element.attributes {
            if attr.name == "text" {
                text_attr_value = Some(attr.value);
            } else if attr.name == "style" {
                existing_style_attr = Some(attr.value);
            }
            else {
                other_attributes.push(attr);
            }
        }

        // Combine existing style attribute with new inline styles
        if let Some(mut existing) = existing_style_attr {
            if !existing.ends_with(';') {
                existing.push(';');
            }
            inline_styles = format!("{}{}", existing, inline_styles);
        }

        if !inline_styles.is_empty() {
            other_attributes.push(Attribute { name: "style".to_string(), value: inline_styles });
        }


        // Generate children content
        for child in element.children {
            inner_content.push_str(&self.generate_node(child));
        }

        // If there was a text attribute, it becomes the inner content
        if let Some(text) = text_attr_value {
            inner_content.push_str(&text);
        }

        // Generate attributes string
        if !other_attributes.is_empty() {
            let attrs: Vec<String> = other_attributes
                .into_iter()
                .map(|a| format!(r#"{}="{}""#, a.name, a.value))
                .collect();
            attributes_str = format!(" {}", attrs.join(" "));
        }

        // Handle single-tag elements (though CHTL doc doesn't specify, it's good practice)
        let single_tags = ["area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"];
        if single_tags.contains(&element.tag_name.as_str()) && inner_content.is_empty() {
             output.push_str(&format!("<{}{}>", element.tag_name, attributes_str));
        } else {
            output.push_str(&format!("<{}{}>{}</{}>", element.tag_name, attributes_str, inner_content, element.tag_name));
        }

        output
    }

    fn generate_text(&self, text: TextNode) -> String {
        text.content
    }

    fn generate_comment(&self, comment: CommentNode) -> String {
        if comment.is_generatable {
            format!("<!-- {} -->", comment.content)
        } else {
            String::new()
        }
    }
}
