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
        // Style and Script blocks are handled by the dispatcher, not by the HTML generator.
        Node::StyleBlock(_) => {}
        Node::ScriptBlock(_) => {}
    }
}

fn generate_element(html: &mut String, element: &Element) {
    html.push_str(&format!("<{}>", element.tag_name));

    for child in &element.children {
        generate_node(html, child);
    }

    html.push_str(&format!("</{}>\n", element.tag_name));
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{Document, Element, Node};

    #[test]
    fn test_generate_nested_elements_and_text() {
        let doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    children: vec![
                        Node::Element(Element {
                            tag_name: "h1",
                            children: vec![
                                Node::Text("Hello")
                            ],
                        }),
                        Node::Element(Element {
                            tag_name: "p",
                            children: vec![
                                Node::Text("World")
                            ],
                        }),
                    ],
                }),
            ],
        };

        let expected_html = "<div><h1>Hello</h1>\n<p>World</p>\n</div>\n";

        let generated_html = generate(&doc);
        assert_eq!(generated_html, expected_html);
    }
}
