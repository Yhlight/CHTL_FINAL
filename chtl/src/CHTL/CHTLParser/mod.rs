use crate::CHTL::CHTLLexer::{CHTLParser, Rule};
use crate::CHTL::CHTLNode::{AttributeNode, Document, ElementNode, Node, TextNode};
use pest::iterators::Pair;
use pest::Parser;

/// Parses a CHTL source string into a Document AST.
pub fn parse_chtl(source: &str) -> Result<Document, pest::error::Error<Rule>> {
    let pairs = CHTLParser::parse(Rule::file, source)?;
    let file_pair = pairs.into_iter().next().unwrap();

    let mut children = Vec::new();
    for pair in file_pair.into_inner() {
        match pair.as_rule() {
            Rule::element => children.push(Node::Element(build_element_node(pair))),
            Rule::COMMENT | Rule::EOI => (),
            _ => unreachable!(),
        }
    }

    Ok(Document { children })
}

fn build_element_node(pair: Pair<Rule>) -> ElementNode {
    let mut inner_pairs = pair.into_inner();
    let tag_name = inner_pairs.next().unwrap().as_str().to_string();

    let mut attributes = Vec::new();
    let mut children = Vec::new();

    // The rest of the pairs are `content_item`s
    for p in inner_pairs {
        // Each `p` is a `content_item`, we need to get the actual element/attribute/text_block from inside it.
        let inner_p = p.into_inner().next().unwrap();
        match inner_p.as_rule() {
            Rule::attribute => {
                let mut attr_peek = inner_p.clone().into_inner();
                let attr_name = attr_peek.next().unwrap().as_str();
                if attr_name == "text" {
                    let value_pair = attr_peek.next().unwrap();
                    let content = get_value_from_pair(value_pair);
                    children.push(Node::Text(TextNode { content }));
                } else {
                    attributes.push(build_attribute_node(inner_p));
                }
            },
            Rule::element => children.push(Node::Element(build_element_node(inner_p))),
            Rule::text_block => {
                let value_pair = inner_p.into_inner().next().unwrap();
                let content = get_value_from_pair(value_pair);
                children.push(Node::Text(TextNode { content }));
            }
            _ => unreachable!("Unexpected rule inside content_item: {:?}", inner_p.as_rule()),
        }
    }

    ElementNode {
        tag_name,
        attributes,
        children,
    }
}

fn build_attribute_node(pair: Pair<Rule>) -> AttributeNode {
    let mut inner_pairs = pair.into_inner();
    let name = inner_pairs.next().unwrap().as_str().to_string();
    let value_pair = inner_pairs.next().unwrap();
    let value = get_value_from_pair(value_pair);
    AttributeNode { name, value }
}

/// Extracts the string value from a literal pair, removing quotes if necessary.
fn get_value_from_pair(pair: Pair<Rule>) -> String {
    match pair.as_rule() {
        Rule::string_literal => {
            let quoted_pair = pair.into_inner().next().unwrap();
            let raw_str = quoted_pair.as_str();
            raw_str[1..raw_str.len() - 1].to_string()
        },
        Rule::unquoted_literal => pair.as_str().to_string(),
        _ => unreachable!("Expected a literal, but found {:?}", pair.as_rule())
    }
}
