use pest::Parser;
use pest_derive::Parser;
use crate::ast::CssProperty;

#[derive(Parser)]
#[grammar = "src/lexer/css.pest"]
pub struct CssParser;

pub fn parse(source: &str) -> Result<Vec<CssProperty>, pest::error::Error<Rule>> {
    let pairs = CssParser::parse(Rule::properties, source)?.next().unwrap().into_inner();

    let properties = pairs
        .filter(|pair| pair.as_rule() == Rule::property)
        .map(|pair| {
            let mut inner = pair.into_inner();
            let key = inner.next().unwrap().as_str();
            let value = inner.next().unwrap().as_str().trim();
            CssProperty { key, value }
        })
        .collect();

    Ok(properties)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_css_properties() {
        let source = r#"
            color: red;
            font-size: 16px;
            /* comment */
            margin-top: 1em;
        "#;
        let props = parse(source).unwrap();

        assert_eq!(props.len(), 3);
        assert_eq!(props[0], CssProperty { key: "color", value: "red" });
        assert_eq!(props[1], CssProperty { key: "font-size", value: "16px" });
        assert_eq!(props[2], CssProperty { key: "margin-top", value: "1em" });
    }

    #[test]
    fn test_parse_no_semicolon_at_end() {
        let source = "color: blue";
        let props = parse(source).unwrap();
        assert_eq!(props.len(), 1);
        assert_eq!(props[0], CssProperty { key: "color", value: "blue" });
    }
}
