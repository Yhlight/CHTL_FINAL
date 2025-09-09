use crate::ast::{Document, Element, Node, StyleContent, CssValue, CssProperty, Specialization};
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
            let mut children_to_render = Vec::new();
            let mut template_found = false;

            if let Some(template) = context.element_templates.get(name) {
                children_to_render = template.children.clone();
                template_found = true;
            } else if let Some(template) = context.custom_element_templates.get(name) {
                children_to_render = template.children.clone();
                template_found = true;
            }

            if template_found {
                apply_specializations(&mut children_to_render, specializations);
                for child_node in &children_to_render {
                    generate_node(html, child_node, context);
                }
            }
        }
        Node::StyleBlock(_) => {}
        Node::ScriptBlock(_) => {}
    }
}

fn apply_specializations<'a>(children: &mut Vec<Node<'a>>, specializations: &[Specialization<'a>]) {
    for spec in specializations {
        match spec {
            Specialization::Modify(ms) => {
                let mut current_index = 0;
                for child in children.iter_mut() {
                    if let Node::Element(elem) = child {
                        if elem.tag_name == ms.selector.tag_name {
                            let should_modify =
                                ms.selector.index.is_none() || ms.selector.index == Some(current_index);
                            if should_modify {
                                // Replace the children of the matched element
                                elem.children = ms.modifications.clone();
                                // If we were targeting a specific index, we're done with this rule
                                if ms.selector.index.is_some() {
                                    break;
                                }
                            }
                            current_index += 1;
                        }
                    }
                }
            }
            Specialization::Delete(ds) => {
                let mut current_index = 0;
                children.retain(|child| {
                    if let Node::Element(elem) = child {
                        if elem.tag_name == ds.selector.tag_name {
                            let should_delete = ds.selector.index.is_none() || ds.selector.index == Some(current_index);
                            current_index += 1;
                            return !should_delete;
                        }
                    }
                    true
                });
            }
            Specialization::Insert(is) => {
                let mut current_index = 0;
                let mut target_pos = None;

                for (i, child) in children.iter().enumerate() {
                    if let Node::Element(elem) = child {
                        if elem.tag_name == is.target_selector.tag_name {
                            if is.target_selector.index.is_none() || is.target_selector.index == Some(current_index) {
                                target_pos = Some(i);
                                break;
                            }
                            current_index += 1;
                        }
                    }
                }

                if let Some(pos) = target_pos {
                    match is.position {
                        crate::ast::InsertPosition::Before => {
                            let mut i = 0;
                            for node in is.nodes_to_insert.clone() {
                                children.insert(pos + i, node);
                                i += 1;
                            }
                        }
                        crate::ast::InsertPosition::After => {
                            let mut i = 1;
                             for node in is.nodes_to_insert.clone() {
                                children.insert(pos + i, node);
                                i += 1;
                            }
                        }
                        crate::ast::InsertPosition::Replace => {
                            children.remove(pos);
                            let mut i = 0;
                            for node in is.nodes_to_insert.clone() {
                                children.insert(pos + i, node);
                                i += 1;
                            }
                        }
                        _ => {} // AtTop and AtBottom not handled for specific selectors yet
                    }
                } else if is.position == crate::ast::InsertPosition::AtTop {
                     let mut i = 0;
                    for node in is.nodes_to_insert.clone() {
                        children.insert(i, node);
                        i += 1;
                    }
                } else if is.position == crate::ast::InsertPosition::AtBottom {
                    children.extend(is.nodes_to_insert.clone());
                }
            }
        }
    }

    // Recurse into child elements
    for child in children.iter_mut() {
        if let Node::Element(elem) = child {
            apply_specializations(&mut elem.children, specializations);
        }
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
    use crate::ast::{CustomElementGroup, Document, Element, Node, Specialization, ChildSelector, DeleteSpec, InsertSpec, ModifySpec};

    #[test]
    fn test_element_modify_specialization() {
        let template = CustomElementGroup {
            name: "Box",
            children: vec![
                Node::Element(Element {
                    tag_name: "div",
                    attributes: vec![],
                    children: vec![Node::Text("Original Content")]
                }),
            ]
        };

        let doc = Document {
            definitions: vec![],
            children: vec![
                Node::ElementTemplateUsage {
                    name: "Box",
                    specializations: vec![
                        Specialization::Modify(ModifySpec {
                            selector: ChildSelector { tag_name: "div", index: None },
                            modifications: vec![Node::Text("New Content")]
                        })
                    ]
                }
            ],
        };

        let mut context = Context::new();
        context.custom_element_templates.insert("Box", &template);

        let expected_html = "<div>New Content</div>\n";
        let generated_html = generate(&doc, &context);
        assert_eq!(generated_html, expected_html);
    }

    #[test]
    fn test_element_delete_specialization() {
        let template = CustomElementGroup {
            name: "Box",
            children: vec![
                Node::Element(Element { tag_name: "p", attributes: vec![], children: vec![Node::Text("One")] }),
                Node::Element(Element { tag_name: "span", attributes: vec![], children: vec![] }),
                Node::Element(Element { tag_name: "p", attributes: vec![], children: vec![Node::Text("Two")] }),
            ]
        };

        let doc = Document {
            definitions: vec![],
            children: vec![
                Node::ElementTemplateUsage {
                    name: "Box",
                    specializations: vec![
                        Specialization::Delete(DeleteSpec {
                            selector: ChildSelector { tag_name: "span", index: None },
                        })
                    ]
                }
            ],
        };

        let mut context = Context::new();
        context.custom_element_templates.insert("Box", &template);

        let expected_html = "<p>One</p>\n<p>Two</p>\n";
        let generated_html = generate(&doc, &context);
        assert_eq!(generated_html, expected_html);
    }

    #[test]
    fn test_element_insert_specialization() {
        let template = CustomElementGroup {
            name: "Box",
            children: vec![
                Node::Element(Element { tag_name: "p", attributes: vec![], children: vec![Node::Text("One")] }),
            ]
        };

        let doc = Document {
            definitions: vec![],
            children: vec![
                Node::ElementTemplateUsage {
                    name: "Box",
                    specializations: vec![
                        Specialization::Insert(InsertSpec {
                            position: crate::ast::InsertPosition::After,
                            target_selector: ChildSelector { tag_name: "p", index: Some(0) },
                            nodes_to_insert: vec![
                                Node::Element(Element { tag_name: "hr", attributes: vec![], children: vec![] })
                            ]
                        })
                    ]
                }
            ],
        };

        let mut context = Context::new();
        context.custom_element_templates.insert("Box", &template);

        let expected_html = "<p>One</p>\n<hr></hr>\n";
        let generated_html = generate(&doc, &context);
        assert_eq!(generated_html, expected_html);
    }
}
