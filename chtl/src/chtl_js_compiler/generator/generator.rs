use crate::chtl_js_compiler::node::{CHTLJSNode, EnhancedSelectorNode};

pub struct Generator;

impl Generator {
    pub fn new() -> Self {
        Generator
    }

    pub fn generate(&self, program: Vec<CHTLJSNode>) -> String {
        let mut output = String::new();
        for node in program {
            output.push_str(&self.generate_node(node));
            output.push_str(";\n"); // Add semicolon after each statement
        }
        output
    }

    fn generate_node(&self, node: CHTLJSNode) -> String {
        match node {
            CHTLJSNode::EnhancedSelector(selector) => self.generate_enhanced_selector(selector),
        }
    }

    fn generate_enhanced_selector(&self, selector: EnhancedSelectorNode) -> String {
        // CHTL JS spec says {{box}} should try tag, then id, then class.
        // And {{.box}} is class, {{#box}} is id.
        // For this basic generator, we will keep it simple.
        // A more advanced version would need to handle the different selector types.
        format!("document.querySelector('{}')", selector.selector)
    }
}
