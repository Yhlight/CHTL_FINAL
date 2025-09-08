use pest::iterators::Pair;
use pest::Parser;
use pest_derive::Parser;

use crate::ast::{Attribute, ChildSelector, CssProperty, CssValue, CustomDefinition, CustomElementGroup, CustomStyleGroup, CustomVarGroup, Document, Element, ElementSpecialization, Node, StyleContent, TemplateDefinition, TemplateElementGroup, TemplateStyleGroup, TemplateVarGroup, TemplateVariable, TopLevelDefinition};
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
                Rule::element | Rule::text_node | Rule::style_block | Rule::script_block | Rule::element_template_usage => {
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
            let content = parts.next().unwrap().into_inner().filter(|p| p.as_rule() != Rule::COMMENT).flat_map(build_style_template_content_items).collect();
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
            let content = parts.next().unwrap().into_inner().filter(|p| p.as_rule() != Rule::COMMENT).flat_map(build_style_template_content_items).collect();
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

fn build_style_template_content_items(pair: Pair<Rule>) -> Vec<StyleContent> {
    match pair.as_rule() {
        Rule::valueless_property_list => {
            pair.into_inner().map(|p| {
                StyleContent::Property(CssProperty { key: p.as_str(), value: None })
            }).collect()
        }
        Rule::property => {
            let mut p_inner = pair.into_inner();
            let key = p_inner.next().unwrap().as_str();
            let value = p_inner.next().unwrap().as_str().trim().trim_matches(|c| c == '"' || c == '\'');
            vec![StyleContent::Property(CssProperty { key, value: Some(CssValue::Literal(value)) })]
        }
        Rule::style_template_usage => {
            let name = pair.into_inner().next().unwrap().as_str();
            vec![StyleContent::StyleTemplateUsage { name }]
        }
        Rule::inherit_style => {
            let name = pair.into_inner().next().unwrap().as_str();
            vec![StyleContent::InheritStyleTemplate { name }]
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
            let mut inner = pair.into_inner();
            let name = inner.next().unwrap().as_str();
            let mut specializations = Vec::new();

            if let Some(spec_block) = inner.next() { // It's an optional specialization_block
                for spec_rule in spec_block.into_inner() {
                    if spec_rule.as_rule() == Rule::specialization_rule {
                        let mut rule_parts = spec_rule.into_inner();
                        let selector_pair = rule_parts.next().unwrap();
                        let modifications_pair = rule_parts.next().unwrap();

                        let mut selector_parts = selector_pair.into_inner();
                        let tag_name = selector_parts.next().unwrap().as_str();
                        let mut index = None;
                        if let Some(index_group_pair) = selector_parts.next() {
                            if index_group_pair.as_rule() == Rule::index_group {
                                let index_val_pair = index_group_pair.into_inner().next().unwrap();
                                index = Some(index_val_pair.as_str().parse::<usize>().unwrap());
                            }
                        }

                        let modifications = modifications_pair.into_inner()
                            .filter(|p| p.as_rule() != Rule::COMMENT)
                            .map(build_node).collect();

                        specializations.push(ElementSpecialization {
                            selector: ChildSelector { tag_name, index },
                            modifications,
                        });
                    }
                }
            }
            Node::ElementTemplateUsage { name, specializations }
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
                assert_eq!(st.content[0], StyleContent::Property(CssProperty { key: "color", value: Some(CssValue::Literal("black")) }));
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

    #[test]
    fn test_parse_valueless_properties() {
        let source = r#"
            [Custom] @Style Valueless {
                color;
            }
        "#;
        let doc = parse(source).unwrap();
        match &doc.definitions[0] {
            TopLevelDefinition::Custom(CustomDefinition::Style(cs)) => {
                assert_eq!(cs.content.len(), 1);
                assert_eq!(cs.content[0], StyleContent::Property(CssProperty { key: "color", value: None }));
            }
            _ => panic!("Expected a custom style"),
        }
    }

    #[test]
    fn test_parse_script_and_style_blocks() {
        let source = r#"
            div {
                style { color: red; }
            }
        "#;

        let expected_doc = Document {
            definitions: vec![],
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![],
                    children: vec![
                        Node::StyleBlock(vec![
                            StyleContent::Property(CssProperty { key: "color", value: Some(CssValue::Literal("red")) })
                        ]),
                    ],
                }),
            ],
        };

        let parsed_doc = parse(source).unwrap();
        assert_eq!(parsed_doc, expected_doc);
    }

    #[test]
    fn test_parse_element_specialization() {
        let source = r#"
            body {
                @Element Box {
                    div[0] {
                        style { color: red; }
                    }
                }
            }
        "#;
        let doc = parse(source).unwrap();
        let body = doc.children.get(0).unwrap();
        if let Node::Element(body_elem) = body {
            let usage = body_elem.children.get(0).unwrap();
            if let Node::ElementTemplateUsage { name, specializations } = usage {
                assert_eq!(*name, "Box");
                assert_eq!(specializations.len(), 1);
                let spec = &specializations[0];
                assert_eq!(spec.selector.tag_name, "div");
                assert_eq!(spec.selector.index, Some(0));
                assert_eq!(spec.modifications.len(), 1);
            } else {
                panic!("Expected ElementTemplateUsage");
            }
        } else {
            panic!("Expected body element");
        }
    }
}
