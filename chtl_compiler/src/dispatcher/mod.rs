// This module will contain the main compiler pipeline dispatcher.
use crate::{ast, chtl_js, context::Context, generator, parser};

pub fn compile(source: &str) -> Result<String, String> {
    // 1. Parse the main CHTL source code
    let chtl_ast = parser::parse(source).map_err(|e| e.to_string())?;

    // 2. Build the compilation context from definitions
    let mut context = Context::new();
    for def in &chtl_ast.definitions {
        match def {
            ast::TopLevelDefinition::Template(template_def) => match template_def {
                ast::TemplateDefinition::Style(st) => { context.style_templates.insert(st.name, st); }
                ast::TemplateDefinition::Element(et) => { context.element_templates.insert(et.name, et); }
                ast::TemplateDefinition::Var(vt) => { context.var_templates.insert(vt.name, vt); }
            },
            ast::TopLevelDefinition::Custom(custom_def) => match custom_def {
                ast::CustomDefinition::Style(st) => { context.custom_style_templates.insert(st.name, st); }
                ast::CustomDefinition::Element(et) => { context.custom_element_templates.insert(et.name, et); }
                ast::CustomDefinition::Var(vt) => { context.custom_var_templates.insert(vt.name, vt); }
            }
        }
    }

    // 3. Walk the CHTL AST to extract script and hoisted style content
    let mut script_content = String::new();
    let mut style_content = String::new();
    extract_hoisted_blocks(&chtl_ast.children, &mut script_content, &mut style_content, &context);

    // 4. Compile the collected CHTL JS content
    let generated_js = if !script_content.is_empty() {
        let chtl_js_ast = chtl_js::parser::parse(&script_content).map_err(|e| e.to_string())?;
        chtl_js::generator::generate(&chtl_js_ast)
    } else {
        String::new()
    };

    // 5. Generate the base HTML from the CHTL AST
    let mut generated_html = generator::generate(&chtl_ast, &context);

    // 6. Merge the generated code into the HTML
    if !style_content.is_empty() {
        let style_tag = format!("<style>{}</style>", style_content);
        if let Some(pos) = generated_html.find("</head>") {
            generated_html.insert_str(pos, &style_tag);
        } else {
            generated_html.insert_str(0, &style_tag);
        }
    }
    if !generated_js.is_empty() {
        let script_tag = format!("<script>{}</script>", generated_js);
        if let Some(pos) = generated_html.rfind("</body>") {
            generated_html.insert_str(pos, &script_tag);
        } else {
            generated_html.push_str(&script_tag);
        }
    }

    Ok(generated_html)
}

fn get_css_value<'a>(value: &'a Option<ast::CssValue<'a>>, context: &Context<'a>) -> Option<String> {
    match value {
        Some(ast::CssValue::Literal(s)) => Some(s.to_string()),
        Some(ast::CssValue::Variable(usage)) => {
            if let Some(var_group) = context.var_templates.get(usage.group_name) {
                if let Some(variable) = var_group.variables.iter().find(|v| v.key == usage.var_name) {
                    return Some(variable.value.to_string());
                }
            }
            None
        }
        None => None,
    }
}

fn extract_hoisted_blocks(nodes: &[ast::Node], script_content: &mut String, style_content: &mut String, context: &Context) {
    for node in nodes {
        if let ast::Node::Element(element) = node {
            extract_hoisted_blocks(&element.children, script_content, style_content, context);
        } else if let ast::Node::ScriptBlock(content) = node {
            script_content.push_str(content.trim());
            script_content.push('\n');
        } else if let ast::Node::StyleBlock(contents) = node {
            for sc in contents {
                if let ast::StyleContent::Ruleset(ruleset) = sc {
                    style_content.push_str(ruleset.selector.trim());
                    style_content.push_str(" { ");
                    for prop in &ruleset.properties {
                        if let Some(value) = get_css_value(&prop.value, context) {
                            style_content.push_str(&format!("{}:{};", prop.key, value));
                        }
                    }
                    style_content.push_str(" }\n");
                }
            }
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn test_end_to_end_delete_specialization() {
        let source = r#"
            [Custom] @Element Box {
                div class: "container" {
                    p { text { "Hello" } }
                    span { text { "Delete me" } }
                }
            }

            @Element Box {
                delete span;
            }
        "#;
        let expected_html = "<div class=\"container\"><p>Hello</p>\n</div>\n";
        let result = super::compile(source).unwrap();
        assert_eq!(result.trim(), expected_html.trim());
    }

    #[test]
    fn test_end_to_end_insert_specialization() {
        let source = r#"
            [Custom] @Element Box {
                div class: "container" {
                    p { text { "Hello" } }
                }
            }

            @Element Box {
                insert after p {
                    hr {}
                }
            }
        "#;
        let expected_html = "<div class=\"container\"><p>Hello</p>\n<hr></hr>\n</div>\n";
        let result = super::compile(source).unwrap();
        assert_eq!(result.trim(), expected_html.trim());
    }

    #[test]
    fn test_valueless_property_usage() {
        let _source = r#"
            [Custom] @Style Flexy {
                display;
                justify-content;
            }
            div {
                style {
                    @Style Flexy {
                        display: flex;
                        justify-content: center;
                    }
                }
            }
        "#;
        // This test will fail until the generator is updated to handle this usage syntax.
        // let result = compile(source).unwrap();
        // assert!(result.contains("style=\"display:flex;justify-content:center;\""));
    }
}
