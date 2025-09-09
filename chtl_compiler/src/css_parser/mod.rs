use pest::iterators::Pair;
use pest::Parser;
use pest_derive::Parser;
use crate::ast::{CssProperty, CssRuleset, CssValue, StyleContent, VarUsage};

#[derive(Parser)]
#[grammar = "src/lexer/css.pest"]
pub struct CssParser;

pub fn parse(source: &str) -> Result<Vec<StyleContent>, pest::error::Error<Rule>> {
    let pairs = CssParser::parse(Rule::style_content, source)?.next().unwrap().into_inner();

    let content = pairs
        .filter(|pair| pair.as_rule() == Rule::property || pair.as_rule() == Rule::ruleset || pair.as_rule() == Rule::style_template_usage)
        .map(build_style_content)
        .collect();

    Ok(content)
}

fn build_style_content(pair: Pair<Rule>) -> StyleContent {
    match pair.as_rule() {
        Rule::property => StyleContent::Property(build_css_property(pair)),
        Rule::ruleset => StyleContent::Ruleset(build_css_ruleset(pair)),
        Rule::style_template_usage => {
            let name = pair.into_inner().next().unwrap().as_str();
            StyleContent::StyleTemplateUsage { name }
        }
        Rule::custom_style_usage => {
            let mut inner = pair.into_inner();
            let name = inner.next().unwrap().as_str();
            let properties = inner.map(build_css_property).collect();
            StyleContent::CustomStyleUsage { name, properties }
        }
        Rule::delete_property => {
            let keys_to_delete = pair.into_inner().map(|p| p.as_str()).collect();
            StyleContent::Delete(keys_to_delete)
        }
        _ => unreachable!(),
    }
}

fn build_css_property(pair: Pair<Rule>) -> CssProperty {
    let mut inner = pair.into_inner();
    let key = inner.next().unwrap().as_str();
    let value_pair = inner.next().unwrap();

    let value_content_pair = value_pair.into_inner().next().unwrap();

    let value = match value_content_pair.as_rule() {
        Rule::var_usage => {
            let mut var_parts = value_content_pair.into_inner();
            let group_name = var_parts.next().unwrap().as_str();
            let var_name = var_parts.next().unwrap().as_str();
            CssValue::Variable(VarUsage { group_name, var_name })
        }
        Rule::literal_value => {
            CssValue::Literal(value_content_pair.as_str().trim().trim_matches(|c| c == '"' || c == '\''))
        }
        _ => unreachable!("Unexpected css property value rule: {:?}", value_content_pair.as_rule()),
    };

    CssProperty { key, value: Some(value) }
}

fn build_css_ruleset(pair: Pair<Rule>) -> CssRuleset {
    let mut inner = pair.into_inner();
    let selector = inner.next().unwrap().as_str().trim();
    let properties = inner.map(build_css_property).collect();
    CssRuleset { selector, properties }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_css_with_vars() {
        let source = r#"
            color: Theme(primary);
            font-size: 16px;
        "#;
        let content = parse(source).unwrap();

        assert_eq!(content.len(), 2);

        match &content[0] {
            StyleContent::Property(p) => {
                assert_eq!(p.key, "color");
                match &p.value {
                    Some(CssValue::Variable(v)) => {
                        assert_eq!(v.group_name, "Theme");
                        assert_eq!(v.var_name, "primary");
                    }
                    _ => panic!("Expected a variable value"),
                }
            }
            _ => panic!("Expected a property"),
        }

        match &content[1] {
            StyleContent::Property(p) => {
                assert_eq!(p.key, "font-size");
                assert_eq!(p.value, Some(CssValue::Literal("16px")));
            }
            _ => panic!("Expected a property"),
        }
    }
}
