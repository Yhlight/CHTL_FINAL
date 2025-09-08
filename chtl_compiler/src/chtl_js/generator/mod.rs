use crate::chtl_js::ast::{Document, Node};

pub fn generate(doc: &Document) -> String {
    let mut js = String::new();
    for node in &doc.nodes {
        generate_node(&mut js, node);
    }
    js
}

fn generate_node(js: &mut String, node: &Node) {
    match node {
        Node::EnhancedSelector(selector) => {
            // The selector itself may contain quotes, so we need to escape them
            // or use a template literal in JS. Template literal is safer.
            js.push_str(&format!("document.querySelector(`{}`)", selector.selector));
        }
        Node::Code(code_str) => {
            js.push_str(code_str);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl_js::ast::{Document, EnhancedSelector, Node};

    #[test]
    fn test_generate_chtl_js_simple() {
        let doc = Document {
            nodes: vec![
                Node::Code("const a = "),
                Node::EnhancedSelector(EnhancedSelector { selector: ".my-class" }),
                Node::Code(";"),
            ],
        };

        let expected_js = "const a = document.querySelector(`.my-class`);";
        let generated_js = generate(&doc);

        assert_eq!(generated_js, expected_js);
    }
}
