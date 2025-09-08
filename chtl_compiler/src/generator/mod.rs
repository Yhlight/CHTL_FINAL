use crate::ast::{Document, Element, Node};

pub fn generate(document: &Document) -> String {
    let mut html = String::new();
    for node in &document.children {
        generate_node(&mut html, node);
    }
    html
}

fn generate_node(html: &mut String, node: &Node) {
    match node {
        Node::Element(element) => {
            generate_element(html, element);
        }
        Node::Text(text_content) => {
            html.push_str(text_content);
        }
        Node::StyleBlock(_) => { /* Handled by the parent element generator */ }
        Node::ScriptBlock(_) => { /* Handled by the dispatcher */ }
    }
}

fn generate_element(html: &mut String, element: &Element) {
    // 1. Format regular attributes
    let mut formatted_attrs = String::new();
    for attr in &element.attributes {
        formatted_attrs.push_str(&format!(" {}=\"{}\"", attr.key, attr.value));
    }

    // 2. Find style blocks and format them into an inline style attribute
    let mut style_string = String::new();
    for node in &element.children {
        if let Node::StyleBlock(properties) = node {
            for prop in properties {
                style_string.push_str(&format!("{}:{};", prop.key, prop.value));
            }
        }
    }
    if !style_string.is_empty() {
        formatted_attrs.push_str(&format!(" style=\"{}\"", style_string));
    }

    // 3. Generate the opening tag
    html.push_str(&format!("<{}{}>", element.tag_name, formatted_attrs));

    // 4. Generate child nodes
    for child in &element.children {
        generate_node(html, child);
    }

    // 5. Generate the closing tag
    html.push_str(&format!("</{}>\n", element.tag_name));
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{Attribute, CssProperty, Document, Element, Node};

    #[test]
    fn test_generate_with_attributes_and_inline_style() {
        let doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![
                        Attribute { key: "id", value: "app" },
                    ],
                    children: vec![
                        Node::StyleBlock(vec![
                            CssProperty { key: "color", value: "red" },
                            CssProperty { key: "font-size", value: "16px" },
                        ]),
                        Node::Element(Element {
                            tag_name: "p",
                            attributes: vec![],
                            children: vec![
                                Node::Text("Hello")
                            ],
                        }),
                    ],
                }),
            ],
        };

        let expected_html = "<div id=\"app\" style=\"color:red;font-size:16px;\"><p>Hello</p>\n</div>\n";

        let generated_html = generate(&doc);
        assert_eq!(generated_html, expected_html);
    }
}
