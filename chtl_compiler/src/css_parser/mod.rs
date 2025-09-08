use pest::iterators::Pair;
use pest::Parser;
use pest_derive::Parser;
use crate::ast::{CssProperty, CssRuleset, StyleContent};

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
        _ => unreachable!(),
    }
}

fn build_css_property(pair: Pair<Rule>) -> CssProperty {
    let mut inner = pair.into_inner();
    let key = inner.next().unwrap().as_str();
    let value = inner.next().unwrap().as_str().trim();
    CssProperty { key, value }
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
    fn test_parse_mixed_css_content() {
        let source = r#"
            color: red;
            .my-class {
                font-weight: bold;
                text-decoration: underline;
            }
            font-size: 16px;
        "#;
        let content = parse(source).unwrap();

        assert_eq!(content.len(), 3);

        // Check first property
        match &content[0] {
            StyleContent::Property(p) => assert_eq!(p, &CssProperty { key: "color", value: "red" }),
            _ => panic!("Expected a property"),
        }

        // Check ruleset
        match &content[1] {
            StyleContent::Ruleset(r) => {
                assert_eq!(r.selector, ".my-class");
                assert_eq!(r.properties.len(), 2);
                assert_eq!(r.properties[0], CssProperty { key: "font-weight", value: "bold" });
                assert_eq!(r.properties[1], CssProperty { key: "text-decoration", value: "underline" });
            }
            _ => panic!("Expected a ruleset"),
        }

        // Check second property
        match &content[2] {
            StyleContent::Property(p) => assert_eq!(p, &CssProperty { key: "font-size", value: "16px" }),
            _ => panic!("Expected a property"),
        }
    }
}
