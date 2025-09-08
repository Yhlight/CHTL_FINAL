use pest::Parser;
use pest_derive::Parser;

use crate::chtl_js::ast::{Document, EnhancedSelector, Node};

#[derive(Parser)]
#[grammar = "src/chtl_js/lexer/chtl_js.pest"]
pub struct ChtlJsParser;

pub fn parse(source: &str) -> Result<Document, pest::error::Error<Rule>> {
    let script_pair = ChtlJsParser::parse(Rule::script_content, source)?.next().unwrap();

    let nodes = script_pair.into_inner()
        .filter(|pair| pair.as_rule() != Rule::EOI)
        .map(|pair| {
        match pair.as_rule() {
            Rule::enhanced_selector => {
                let selector = pair.into_inner().next().unwrap().as_str();
                Node::EnhancedSelector(EnhancedSelector { selector })
            }
            Rule::other_code => {
                Node::Code(pair.as_str())
            }
            _ => unreachable!("Unexpected rule: {:?}", pair.as_rule()),
        }
    }).collect();

    Ok(Document { nodes })
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chtl_js::ast::{Document, EnhancedSelector, Node};

    #[test]
    fn test_parse_chtl_js_simple() {
        let source = "const a = {{.my-class}};";
        let doc = parse(source).unwrap();

        let expected_doc = Document {
            nodes: vec![
                Node::Code("const a = "),
                Node::EnhancedSelector(EnhancedSelector { selector: ".my-class" }),
                Node::Code(";"),
            ],
        };

        assert_eq!(doc, expected_doc);
    }
}
