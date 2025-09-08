use crate::ast::{Document, Element, Node, StyleContent, CssValue, CssProperty};
use crate::context::Context;
use std::collections::{HashSet, HashMap};

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
        Node::ElementTemplateUsage { name, specializations: _ } => {
            if let Some(template) = context.element_templates.get(name) {
                for child_node in &template.children {
                    generate_node(html, child_node, context);
                }
            }
        }
        Node::StyleBlock(_) => {}
        Node::ScriptBlock(_) => {}
    }
}

fn get_css_value<'a>(value: &'a Option<CssValue<'a>>, context: &Context<'a>) -> Option<String> {
    match value {
        Some(CssValue::Literal(s)) => Some(s.to_string()),
        Some(CssValue::Variable(usage)) => {
            context.var_templates.get(usage.group_name)
                .and_then(|group| group.variables.iter().find(|v| v.key == usage.var_name))
                .map(|var| var.value.to_string())
        }
        None => None,
    }
}

fn apply_style_template<'a>(name: &'a str, context: &Context<'a>, props: &mut Vec<CssProperty<'a>>, stack: &mut HashSet<&'a str>) {
    if !stack.insert(name) { return; }

    if let Some(template) = context.style_templates.get(name) {
        for item in &template.content {
            match item {
                StyleContent::Property(prop) => props.push(prop.clone()),
                StyleContent::StyleTemplateUsage { name: inherited_name } | StyleContent::InheritStyleTemplate { name: inherited_name } => {
                    apply_style_template(inherited_name, context, props, stack);
                }
                _ => {}
            }
        }
    }

    stack.remove(name);
}

fn generate_element(html: &mut String, element: &Element, context: &Context) {
    let mut final_properties: HashMap<&str, String> = HashMap::new();
    let mut keys_to_delete = HashSet::new();
    let mut auto_classes = HashSet::new();
    let mut auto_id = None;

    // --- Pass 1: Collect all properties, deletions, and auto-selectors ---
    for node in &element.children {
        if let Node::StyleBlock(style_contents) = node {
            for sc in style_contents {
                match sc {
                    StyleContent::Property(prop) => {
                        if let Some(value) = get_css_value(&prop.value, context) {
                            final_properties.insert(prop.key, value);
                        }
                    }
                    StyleContent::StyleTemplateUsage { name } => {
                        let mut inherited_props = Vec::new();
                        apply_style_template(name, context, &mut inherited_props, &mut HashSet::new());
                        for prop in inherited_props {
                             if let Some(value) = get_css_value(&prop.value, context) {
                                final_properties.insert(prop.key, value);
                             }
                        }
                    }
                    StyleContent::CustomStyleUsage { name: _, properties: _ } => { /* ... to be implemented ... */ }
                    StyleContent::Delete(keys) => {
                        for key in keys {
                            keys_to_delete.insert(*key);
                        }
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

    // --- Pass 2: Apply deletions ---
    for key in keys_to_delete {
        final_properties.remove(key);
    }

    let style_string: String = final_properties.iter().map(|(k, v)| format!("{}:{};", k, v)).collect();

    // --- Attribute Generation ---
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

    // --- HTML Generation ---
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
    fn test_delete_property() {
        let base_template = TemplateStyleGroup {
            name: "Base",
            content: vec![
                StyleContent::Property(CssProperty { key: "color", value: Some(CssValue::Literal("blue"))}),
                StyleContent::Property(CssProperty { key: "font-size", value: Some(CssValue::Literal("16px"))}),
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
                            StyleContent::StyleTemplateUsage { name: "Base" },
                            StyleContent::Delete(vec!["color"]),
                        ]),
                    ],
                }),
            ],
        };

        let mut context = Context::new();
        context.style_templates.insert("Base", &base_template);

        let generated_html = generate(&doc, &context);
        assert!(generated_html.contains("style=\"font-size:16px;\""));
        assert!(!generated_html.contains("color"));
    }
}
