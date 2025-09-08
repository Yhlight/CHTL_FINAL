use crate::ast::{Document, Element, Node, StyleContent, CssValue};
use crate::context::Context;
use std::collections::HashSet;

pub fn generate<'a>(document: &'a Document, context: &Context<'a>) -> String {
    let mut html = String::new();
    for node in &document.children {
        generate_node(&mut html, node, context);
    }
    html
}

fn generate_node(html: &mut String, node: &Node, context: &Context) {
    match node {
        Node::Element(element) => {
            generate_element(html, element, context);
        }
        Node::Text(text_content) => {
            html.push_str(text_content);
        }
        Node::ElementTemplateUsage { name } => {
            if let Some(template) = context.element_templates.get(name) {
                for child_node in &template.children {
                    generate_node(html, child_node, context);
                }
            }
        }
        Node::StyleBlock(_) => { /* Handled by parent */ }
        Node::ScriptBlock(_) => { /* Handled by dispatcher */ }
    }
}

fn get_css_value<'a>(value: &'a CssValue<'a>, context: &Context<'a>) -> String {
    match value {
        CssValue::Literal(s) => s.to_string(),
        CssValue::Variable(usage) => {
            if let Some(var_group) = context.var_templates.get(usage.group_name) {
                if let Some(variable) = var_group.variables.iter().find(|v| v.key == usage.var_name) {
                    return variable.value.to_string();
                }
            }
            String::new() // Return empty string if var not found
        }
    }
}

fn apply_style_template<'a>(name: &'a str, context: &Context<'a>, style_string: &mut String, stack: &mut HashSet<&'a str>) {
    if !stack.insert(name) { return; }

    if let Some(template) = context.style_templates.get(name) {
        for item in &template.content {
            match item {
                StyleContent::Property(prop) => {
                    let value = get_css_value(&prop.value, context);
                    style_string.push_str(&format!("{}:{};", prop.key, value));
                }
                StyleContent::StyleTemplateUsage { name: inherited_name } | StyleContent::InheritStyleTemplate { name: inherited_name } => {
                    apply_style_template(inherited_name, context, style_string, stack);
                }
                _ => {}
            }
        }
    }

    stack.remove(name);
}

fn generate_element(html: &mut String, element: &Element, context: &Context) {
    let mut style_string = String::new();
    let mut auto_classes = HashSet::new();
    let mut auto_id = None;

    for node in &element.children {
        if let Node::StyleBlock(style_contents) = node {
            for sc in style_contents {
                match sc {
                    StyleContent::Property(prop) => {
                         let value = get_css_value(&prop.value, context);
                         style_string.push_str(&format!("{}:{};", prop.key, value));
                    }
                    StyleContent::StyleTemplateUsage { name } => {
                        let mut stack = HashSet::new();
                        apply_style_template(name, context, &mut style_string, &mut stack);
                    }
                    StyleContent::Ruleset(ruleset) => {
                        let selector = ruleset.selector.trim();
                        if let Some(class_name) = selector.strip_prefix('.') {
                            auto_classes.insert(class_name.split_whitespace().next().unwrap_or("").to_string());
                        } else if let Some(id_name) = selector.strip_prefix('#') {
                            if auto_id.is_none() { auto_id = Some(id_name.split_whitespace().next().unwrap_or("").to_string()); }
                        }
                    }
                    _ => {}
                }
            }
        }
    }

    let mut formatted_attrs = String::new();
    let mut has_id_attr = false;
    for attr in &element.attributes {
        if attr.key == "class" {
            for cls in attr.value.split_whitespace() { auto_classes.insert(cls.to_string()); }
        } else {
             if attr.key == "id" { has_id_attr = true; }
            formatted_attrs.push_str(&format!(" {}=\"{}\"", attr.key, attr.value));
        }
    }
    if !has_id_attr {
        if let Some(id_name) = auto_id {
             formatted_attrs.push_str(&format!(" id=\"{}\"", id_name));
        }
    }
    if !auto_classes.is_empty() {
        let all_classes = auto_classes.into_iter().collect::<Vec<_>>().join(" ");
        formatted_attrs.push_str(&format!(" class=\"{}\"", all_classes));
    }
    if !style_string.is_empty() {
        formatted_attrs.push_str(&format!(" style=\"{}\"", style_string));
    }

    html.push_str(&format!("<{}{}>", element.tag_name, formatted_attrs));
    for child in &element.children {
        generate_node(html, child, context);
    }
    html.push_str(&format!("</{}>\n", element.tag_name));
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{CssProperty, CssValue, Document, Element, Node, TemplateStyleGroup, StyleContent};

    #[test]
    fn test_style_template_inheritance() {
        let base_template = TemplateStyleGroup {
            name: "Base",
            content: vec![
                StyleContent::Property(CssProperty { key: "font-family", value: CssValue::Literal("sans-serif")}),
                StyleContent::Property(CssProperty { key: "color", value: CssValue::Literal("black")}),
            ]
        };
        let theme_template = TemplateStyleGroup {
            name: "Theme",
            content: vec![
                StyleContent::StyleTemplateUsage { name: "Base" },
                StyleContent::Property(CssProperty { key: "color", value: CssValue::Literal("blue")}),
            ]
        };

        let doc = Document {
            definitions: vec![],
            children: vec![
                Node::Element(Element {
                    tag_name: "p",
                    attributes: vec![],
                    children: vec![
                        Node::StyleBlock(vec![
                            StyleContent::StyleTemplateUsage { name: "Theme" },
                        ]),
                    ],
                }),
            ],
        };

        let mut context = Context::new();
        context.style_templates.insert("Base", &base_template);
        context.style_templates.insert("Theme", &theme_template);

        let expected_html = "<p style=\"font-family:sans-serif;color:black;color:blue;\"></p>\n";
        let generated_html = generate(&doc, &context);
        assert_eq!(generated_html, expected_html);
    }
}
