use pest::iterators::Pair;
use pest::Parser;
use pest_derive::Parser;

use crate::ast::{Document, Element, Node};

#[derive(Parser)]
#[grammar = "src/lexer/chtl.pest"]
pub struct ChtlParser;

pub fn parse(source: &str) -> Result<Document, pest::error::Error<Rule>> {
    let mut pairs = ChtlParser::parse(Rule::chtl_file, source)?;
    let file_pair = pairs.next().unwrap();

    let children = file_pair.into_inner()
        .find(|pair| pair.as_rule() == Rule::inner_content)
        .unwrap()
        .into_inner()
        .filter(|pair| pair.as_rule() != Rule::COMMENT)
        .map(build_node)
        .collect();

    Ok(Document { children })
}

fn build_node(pair: Pair<Rule>) -> Node {
    match pair.as_rule() {
        Rule::element => {
            let mut inner_pairs = pair.into_inner();
            let tag_name = inner_pairs.next().unwrap().as_str();

            let children = inner_pairs
                .next()
                .unwrap() // inner_content
                .into_inner()
                .filter(|pair| pair.as_rule() != Rule::COMMENT)
                .map(build_node)
                .collect();

            Node::Element(Element {
                tag_name,
                children,
            })
        }
        Rule::text_node => {
            let text = pair.into_inner().next().unwrap().as_str();
            // The text literal includes quotes, which we need to remove.
            let trimmed_text = text.trim_matches(|c| c == '"' || c == '\'');
            Node::Text(trimmed_text)
        }
        Rule::style_block => {
            let content = pair.into_inner().next().unwrap().as_str();
            Node::StyleBlock(content)
        }
        Rule::script_block => {
            let content = pair.into_inner().next().unwrap().as_str();
            Node::ScriptBlock(content)
        }
        _ => unreachable!("Unexpected rule: {:?}", pair.as_rule()),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::*;

    #[test]
    fn test_parse_simple_elements() {
        let source = r#"
            div {}
            html {}
        "#;

        let expected_doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    children: vec![],
                }),
                Node::Element(Element {
                    tag_name: "html",
                    children: vec![],
                }),
            ],
        };

        let parsed_doc = parse(source).unwrap();
        assert_eq!(parsed_doc, expected_doc);
    }

    #[test]
    fn test_parse_script_and_style_blocks() {
        let source = r#"
            div {
                style { color: red; }
                script { const a = 1; }
            }
        "#;

        let expected_doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    children: vec![
                        Node::StyleBlock("color: red; "),
                        Node::ScriptBlock("const a = 1; "),
                    ],
                }),
            ],
        };

        let parsed_doc = parse(source).unwrap();
        assert_eq!(parsed_doc, expected_doc);
    }

    #[test]
    fn test_parse_nested_elements_and_text() {
        let source = r#"
            div {
                h1 {
                    text { "Hello, World!" }
                }
                p {
                    text { 'This is a test.' }
                }
            }
        "#;

        let expected_doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    children: vec![
                        Node::Element(Element {
                            tag_name: "h1",
                            children: vec![
                                Node::Text("Hello, World!")
                            ],
                        }),
                        Node::Element(Element {
                            tag_name: "p",
                            children: vec![
                                Node::Text("This is a test.")
                            ],
                        }),
                    ],
                }),
            ],
        };

        let parsed_doc = parse(source).unwrap();
        assert_eq!(parsed_doc, expected_doc);
    }
}
