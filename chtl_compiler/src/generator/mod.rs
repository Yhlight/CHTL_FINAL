use crate::ast::{Document, Element, Node, StyleContent};
use std::collections::HashSet;

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
    // --- Attribute and Style Processing ---
    let mut style_string = String::new();
    let mut auto_classes = HashSet::new();
    let mut auto_id = None;

    // 1. Find style blocks and extract their inline props and auto-gen selectors
    for node in &element.children {
        if let Node::StyleBlock(style_contents) = node {
            for sc in style_contents {
                match sc {
                    StyleContent::Property(prop) => {
                        style_string.push_str(&format!("{}:{};", prop.key, prop.value));
                    }
                    StyleContent::Ruleset(ruleset) => {
                        let selector = ruleset.selector.trim();
                        if let Some(class_name) = selector.strip_prefix('.') {
                            // Take the first word as the class name
                            auto_classes.insert(class_name.split_whitespace().next().unwrap_or("").to_string());
                        } else if let Some(id_name) = selector.strip_prefix('#') {
                            if auto_id.is_none() { // Only apply the first ID found
                                auto_id = Some(id_name.split_whitespace().next().unwrap_or("").to_string());
                            }
                        }
                    }
                }
            }
        }
    }

    // 2. Build the attributes string
    let mut formatted_attrs = String::new();
    let mut has_id_attr = false;

    // Start with existing attributes
    for attr in &element.attributes {
        if attr.key == "class" {
            // Add existing classes to our set
            for cls in attr.value.split_whitespace() {
                auto_classes.insert(cls.to_string());
            }
        } else {
             if attr.key == "id" {
                has_id_attr = true;
             }
            formatted_attrs.push_str(&format!(" {}=\"{}\"", attr.key, attr.value));
        }
    }

    // Add auto-generated id if no id attribute exists
    if !has_id_attr {
        if let Some(id_name) = auto_id {
             formatted_attrs.push_str(&format!(" id=\"{}\"", id_name));
        }
    }

    // Consolidate and add the class attribute
    if !auto_classes.is_empty() {
        let all_classes = auto_classes.into_iter().collect::<Vec<_>>().join(" ");
        formatted_attrs.push_str(&format!(" class=\"{}\"", all_classes));
    }

    // Add the inline style attribute
    if !style_string.is_empty() {
        formatted_attrs.push_str(&format!(" style=\"{}\"", style_string));
    }

    // --- HTML Generation ---
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
    use crate::ast::{Attribute, CssRuleset, Document, Element, Node, StyleContent};

    #[test]
    fn test_auto_class_generation() {
        let doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![], // No initial class
                    children: vec![
                        Node::StyleBlock(vec![
                            StyleContent::Ruleset(CssRuleset {
                                selector: ".box",
                                properties: vec![]
                            }),
                        ]),
                    ],
                }),
            ],
        };
        let expected_html = "<div class=\"box\"></div>\n";
        let generated_html = generate(&doc);
        assert_eq!(generated_html.trim(), expected_html.trim());
    }

    #[test]
    fn test_auto_id_generation() {
        let doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "p",
                    attributes: vec![], // No initial id
                    children: vec![
                        Node::StyleBlock(vec![
                            StyleContent::Ruleset(CssRuleset {
                                selector: "#main-text",
                                properties: vec![]
                            }),
                        ]),
                    ],
                }),
            ],
        };
        let expected_html = "<p id=\"main-text\"></p>\n";
        let generated_html = generate(&doc);
        assert_eq!(generated_html, expected_html);
    }

    #[test]
    fn test_auto_class_append() {
        let doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![Attribute { key: "class", value: "existing" }],
                    children: vec![
                        Node::StyleBlock(vec![
                            StyleContent::Ruleset(CssRuleset {
                                selector: ".new-class",
                                properties: vec![]
                            }),
                        ]),
                    ],
                }),
            ],
        };
        // Use contains because HashSet order is not guaranteed
        let generated_html = generate(&doc);
        assert!(generated_html.contains(" class=\""));
        assert!(generated_html.contains("existing"));
        assert!(generated_html.contains("new-class"));
    }

     #[test]
    fn test_id_not_overwritten() {
        let doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![Attribute { key: "id", value: "original-id" }],
                    children: vec![
                        Node::StyleBlock(vec![
                            StyleContent::Ruleset(CssRuleset {
                                selector: "#new-id",
                                properties: vec![]
                            }),
                        ]),
                    ],
                }),
            ],
        };
        let expected_html = "<div id=\"original-id\"></div>\n";
        let generated_html = generate(&doc);
        assert_eq!(generated_html.trim(), expected_html.trim());
    }
}
