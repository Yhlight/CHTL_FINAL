use pest::iterators::Pair;
use pest::Parser;
use pest_derive::Parser;

use crate::ast::{Attribute, Document, Element, Node};
use crate::css_parser;

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
        .filter(|pair| pair.as_rule() != Rule::COMMENT && pair.as_rule() != Rule::attribute)
        .map(build_node)
        .collect();

    Ok(Document { children })
}

fn build_node(pair: Pair<Rule>) -> Node {
    match pair.as_rule() {
        Rule::element => {
            let mut inner_pairs = pair.into_inner();
            let tag_name = inner_pairs.next().unwrap().as_str();
            let inner_content = inner_pairs.next().unwrap();

            let mut attributes = Vec::new();
            let mut children = Vec::new();

            for content_pair in inner_content.into_inner() {
                match content_pair.as_rule() {
                    Rule::attribute => {
                        let mut attr_parts = content_pair.into_inner();
                        let key = attr_parts.next().unwrap().as_str();
                        let value_pair = attr_parts.next().unwrap();
                        let value = value_pair.as_str().trim_matches(|c| c == '"' || c == '\'');
                        attributes.push(Attribute { key, value });
                    }
                    Rule::COMMENT => { /* ignore */ }
                    _ => { // Any other rule is a child node
                        children.push(build_node(content_pair));
                    }
                }
            }

            Node::Element(Element {
                tag_name,
                attributes,
                children,
            })
        }
        Rule::text_node => {
            let text = pair.into_inner().next().unwrap().as_str();
            let trimmed_text = text.trim_matches(|c| c == '"' || c == '\'');
            Node::Text(trimmed_text)
        }
        Rule::style_block => {
            let content = pair.into_inner().next().unwrap().as_str();
            // Sub-parse the content of the style block
            let properties = css_parser::parse(content).unwrap_or_else(|_| vec![]);
            Node::StyleBlock(properties)
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
    use crate::ast::{Attribute, CssProperty, Document, Element, Node, StyleContent};

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
                    attributes: vec![],
                    children: vec![],
                }),
                Node::Element(Element {
                    tag_name: "html",
                    attributes: vec![],
                    children: vec![],
                }),
            ],
        };

        let parsed_doc = parse(source).unwrap();
        assert_eq!(parsed_doc, expected_doc);
    }

    #[test]
    fn test_parse_attributes() {
        let source = r#"
            div {
                id: "app";
                class: container;
            }
        "#;

        let expected_doc = Document {
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![
                        Attribute { key: "id", value: "app" },
                        Attribute { key: "class", value: "container" },
                    ],
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
                    attributes: vec![],
                    children: vec![
                        Node::StyleBlock(vec![
                            StyleContent::Property(CssProperty { key: "color", value: "red" })
                        ]),
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
                    attributes: vec![],
                    children: vec![
                        Node::Element(Element {
                            tag_name: "h1",
                            attributes: vec![],
                            children: vec![
                                Node::Text("Hello, World!")
                            ],
                        }),
                        Node::Element(Element {
                            tag_name: "p",
                            attributes: vec![],
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
