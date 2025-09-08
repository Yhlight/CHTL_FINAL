use crate::ast::{Document, Element, Node, StyleContent, CssValue, CssProperty, ElementGroup};
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
        Node::ElementTemplateUsage { name, specializations } => {
            let template: Option<&dyn ElementGroup> = context.element_templates.get(name).map(|&t| t as &dyn ElementGroup)
                .or_else(|| context.custom_element_templates.get(name).map(|&c| c as &dyn ElementGroup));

            if let Some(t) = template {
                let mut specialized_children = t.children().clone();

                for spec in specializations {
                    let mut current_index = 0;
                    for child in &mut specialized_children {
                        if let Node::Element(elem) = child {
                            if elem.tag_name == spec.selector.tag_name {
                                let index_matches = spec.selector.index.is_none() || spec.selector.index == Some(current_index);
                                if index_matches {
                                    elem.children.extend(spec.modifications.clone());
                                    if spec.selector.index.is_some() { break; }
                                }
                                current_index += 1;
                            }
                        }
                    }
                }

                for child_node in &specialized_children {
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
                    StyleContent::CustomStyleUsage { name: _, properties: _ } => {}
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

    for key in keys_to_delete {
        final_properties.remove(key);
    }

    let style_string: String = final_properties.iter().map(|(k, v)| format!("{}:{};", k, v)).collect();

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
    use crate::ast::{CustomElementGroup, CssProperty, CssValue, Document, Element, Node, StyleContent, ElementSpecialization, ChildSelector};

    #[test]
    fn test_element_specialization() {
        let template = CustomElementGroup {
            name: "Box",
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![],
                    children: vec![ Node::Text("First") ],
                }),
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![],
                    children: vec![ Node::Text("Second") ],
                }),
            ]
        };

        let doc = Document {
            definitions: vec![],
            children: vec![
                Node::ElementTemplateUsage {
                    name: "Box",
                    specializations: vec![
                        ElementSpecialization {
                            selector: ChildSelector { tag_name: "div", index: Some(1) },
                            modifications: vec![
                                Node::StyleBlock(vec![
                                    StyleContent::Property(CssProperty { key: "color", value: Some(CssValue::Literal("red")) })
                                ])
                            ]
                        }
                    ]
                }
            ],
        };

        let mut context = Context::new();
        context.custom_element_templates.insert("Box", &template);

        let expected_html = "<div>First</div>\n<div style=\"color:red;\">Second</div>\n";
        let generated_html = generate(&doc, &context);
        assert_eq!(generated_html, expected_html);
    }
}
