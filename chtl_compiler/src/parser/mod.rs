use pest::iterators::Pair;
use pest::Parser;
use pest_derive::Parser;

use crate::ast::{Attribute, CssProperty, CssValue, CustomDefinition, CustomElementGroup, CustomStyleGroup, CustomVarGroup, Document, Element, Node, StyleContent, TemplateDefinition, TemplateElementGroup, TemplateStyleGroup, TemplateVarGroup, TemplateVariable, TopLevelDefinition};
use crate::css_parser;

#[derive(Parser)]
#[grammar = "src/lexer/chtl.pest"]
pub struct ChtlParser;

pub fn parse(source: &str) -> Result<Document, pest::error::Error<Rule>> {
    let file_pair = ChtlParser::parse(Rule::chtl_file, source)?.next().unwrap();

    let mut definitions = Vec::new();
    let mut children = Vec::new();

    if let Some(file_content_pair) = file_pair.into_inner().find(|p| p.as_rule() == Rule::file_content) {
        for pair in file_content_pair.into_inner() {
            match pair.as_rule() {
                Rule::template_definition => {
                    definitions.push(TopLevelDefinition::Template(build_template_definition(pair)));
                }
                Rule::custom_definition => {
                    definitions.push(TopLevelDefinition::Custom(build_custom_definition(pair)));
                }
                Rule::element | Rule::text_node | Rule::style_block | Rule::script_block => {
                    children.push(build_node(pair));
                }
                Rule::COMMENT | Rule::EOI => { /* ignore */ }
                _ => unreachable!("Unexpected top-level rule: {:?}", pair.as_rule()),
            }
        }
    }

    Ok(Document { definitions, children })
}

fn build_custom_definition(pair: Pair<Rule>) -> CustomDefinition {
    let inner = pair.into_inner().next().unwrap();
    match inner.as_rule() {
        Rule::style_template => {
            let mut parts = inner.into_inner();
            let name = parts.next().unwrap().as_str();
            let content = parts.next().unwrap().into_inner().filter(|p| p.as_rule() != Rule::COMMENT).map(build_style_template_content).collect();
            CustomDefinition::Style(CustomStyleGroup { name, content })
        }
        Rule::element_template => {
            let mut parts = inner.into_inner();
            let name = parts.next().unwrap().as_str();
            let children = parts.filter(|p| p.as_rule() != Rule::COMMENT).map(build_node).collect();
            CustomDefinition::Element(CustomElementGroup { name, children })
        }
        Rule::var_template => {
            let mut parts = inner.into_inner();
            let name = parts.next().unwrap().as_str();
            let variables = parts.map(|v| {
                 let mut v_inner = v.into_inner();
                 let key = v_inner.next().unwrap().as_str();
                 let value = v_inner.next().unwrap().as_str().trim_matches(|c| c == '"' || c == '\'');
                 TemplateVariable { key, value }
            }).collect();
            CustomDefinition::Var(CustomVarGroup { name, variables })
        }
        _ => unreachable!("Unexpected custom type: {:?}", inner.as_rule()),
    }
}

fn build_template_definition(pair: Pair<Rule>) -> TemplateDefinition {
    let inner = pair.into_inner().next().unwrap();
    match inner.as_rule() {
        Rule::style_template => {
            let mut parts = inner.into_inner();
            let name = parts.next().unwrap().as_str();
            let content = parts.next().unwrap().into_inner().filter(|p| p.as_rule() != Rule::COMMENT).map(build_style_template_content).collect();
            TemplateDefinition::Style(TemplateStyleGroup { name, content })
        }
        Rule::element_template => {
            let mut parts = inner.into_inner();
            let name = parts.next().unwrap().as_str();
            let children = parts.filter(|p| p.as_rule() != Rule::COMMENT).map(build_node).collect();
            TemplateDefinition::Element(TemplateElementGroup { name, children })
        }
        Rule::var_template => {
            let mut parts = inner.into_inner();
            let name = parts.next().unwrap().as_str();
            let variables = parts.map(|v| {
                 let mut v_inner = v.into_inner();
                 let key = v_inner.next().unwrap().as_str();
                 let value = v_inner.next().unwrap().as_str().trim_matches(|c| c == '"' || c == '\'');
                 TemplateVariable { key, value }
            }).collect();
            TemplateDefinition::Var(TemplateVarGroup { name, variables })
        }
        _ => unreachable!("Unexpected template type: {:?}", inner.as_rule()),
    }
}

fn build_style_template_content(pair: Pair<Rule>) -> StyleContent {
    match pair.as_rule() {
        Rule::property => {
            let mut p_inner = pair.into_inner();
            let key = p_inner.next().unwrap().as_str();
            let value = p_inner.next().unwrap().as_str().trim().trim_matches(|c| c == '"' || c == '\'');
            StyleContent::Property(CssProperty { key, value: CssValue::Literal(value) })
        }
        Rule::style_template_usage => {
            let name = pair.into_inner().next().unwrap().as_str();
            StyleContent::StyleTemplateUsage { name }
        }
        Rule::inherit_style => {
            let name = pair.into_inner().next().unwrap().as_str();
            StyleContent::InheritStyleTemplate { name }
        }
        _ => unreachable!("Unexpected style template content: {:?}", pair.as_rule()),
    }
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
                    _ => {
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
            let style_content = css_parser::parse(content).unwrap_or_else(|_| vec![]);
            Node::StyleBlock(style_content)
        }
        Rule::script_block => {
            let content = pair.into_inner().next().unwrap().as_str();
            Node::ScriptBlock(content)
        }
        Rule::element_template_usage => {
            let name = pair.into_inner().next().unwrap().as_str();
            Node::ElementTemplateUsage { name }
        }
        _ => unreachable!("Unexpected node rule: {:?}", pair.as_rule()),
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::*;

    #[test]
    fn test_parse_definitions() {
        let source = r#"
            [Template] @Style DefaultText {
                color: "black";
            }

            [Custom] @Element Box {
                div {}
            }
        "#;

        let doc = parse(source).unwrap();
        assert!(doc.children.is_empty());
        assert_eq!(doc.definitions.len(), 2);

        // Check template
        match &doc.definitions[0] {
            TopLevelDefinition::Template(TemplateDefinition::Style(st)) => {
                assert_eq!(st.name, "DefaultText");
                assert_eq!(st.content.len(), 1);
            }
            _ => panic!("Expected a style template"),
        }

        // Check custom
        match &doc.definitions[1] {
            TopLevelDefinition::Custom(CustomDefinition::Element(et)) => {
                assert_eq!(et.name, "Box");
                assert_eq!(et.children.len(), 1);
            }
            _ => panic!("Expected a custom element"),
        }
    }
}
